#include "framework.h"
#include "SCPU.h"
#include "StressCPUDlg.h"
namespace SCPU
{
	// GUIコントロール用排他ロック
	std::mutex g_mtx_[D_MAX_THRD_CNT];
	// GUI制御用排他オブジェクト
	std::mutex mtx_scpu;
	// スレッド計算速度情報収集の排他オブジェクト
	std::mutex mtx_flps;
	// 単精度演算消費時間サンプリング実行中フラグ排他オブジェクト
	std::mutex mtx_flpssmpflg;
	// プログレスバー更新フラグ排他
	std::mutex mtx_prgbar;
	// 乱数表示フラグ排他
	std::mutex mtx_rnddisp;
	// 全スレッド終了フラグ
	// true=全スレッド終了, false=全スレッド稼働中
	bool ThreadEndFlg;
	/// <summary>
	/// 指定スレッドの乱数をGUIコントロールに表示する
	/// スレッド関数として動作する
	/// </summary>
	/// <param name="thridx">スレッドID</param>
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void ThreadRandVarDisp(
		int thridx,
		CStressCPUDlg* strc
	)
	{
		std::string svar = "";
		SCPU::SetThreadEnd(false);
		int stat;
		bool PrgRunFlg = true;
		for (; GetThreadEnd() == false;)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			stat = strc->hmultiThrd->GetStatus(thridx);
			if (stat == MT::e_Status::st_sleep)
			{
				// プログレスバー表示終了
				if (PrgRunFlg == false)
				{
					strc->PrgBarEnd(thridx);
					// スレッド乱数表示欄にCPUベンダー名表示
					strc->SetStVector(thridx, strc->GetCpuVend().c_str());
					PrgRunFlg = true;
				}
				continue;
			}
			else if (stat == MT::e_Status::st_running)
			{
				// 乱数表示ON
				if (strc->GetThreadRndDispFlg() == TRUE)
				{
					// 各ストレス乱数を取得
					svar = strc->hmultiThrd->GetRndStr(thridx);
					// 各乱数表示領域に表示
					strc->SetStVector(thridx, svar.c_str());
				}
				// プログレスバー更新ON
				if (strc->GetPrgBarUpdateFlg() == TRUE)
				{
					// プログレスバー表示
					if (PrgRunFlg == true)
					{
						strc->PrgBarRun(thridx);
						PrgRunFlg = false;
					}
					// プログレスバー更新
					strc->PrgBarUpdate(thridx);
				}
			}
		}
	}
	/// <summary>
	/// ストレススレッドの乱数をGUIコントロールに表示する
	/// </summary>
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void MultiThreadMgrSub(CStressCPUDlg* strc)
	{
		std::string svar = "";
		// 最大対象スレッド数取得
		int max_thrd = strc->GetMaxThrdCount();
		for (int ii = 0; ii < max_thrd; ii++)
		{
			// スレッド乱数表示用スレッド関数起動
			strc->hmultiThrd->MTM_Tbl[ii].hRndDisp = new std::thread(
				SCPU::ThreadRandVarDisp,
				ii,
				strc
			);
			// 該当スレッドの占有優先度を設定する。
			// 基準優先度を優先度クラスより2だけ低く設定します。
			auto thread_pri = THREAD_PRIORITY_LOWEST;
			SetThreadPriority(
				strc->hmultiThrd->GetThrdNativeHnd(ii),
				thread_pri
			);
		}
	}
	/// <summary>
	/// ストレススレッドの乱数をGUIコントロールに表示する
	/// スレッド監視用スレッド関数
	/// 発生した乱数を拾って表示エリアに表示
	/// </summary>
	/// <remark>CStressCPUDlgのコンストラクターから呼ばれる
	/// </remark>
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void MultiThreadMgr(CStressCPUDlg* strc)
	{
		static bool RunFlg = false;
		for (;;)
		{
			// 一括実行された
			if (RunFlg == false &&
				strc->GetExecAllFlag() == TRUE)
			{
				// ストレススレッドの乱数をGUIコントロールに表示する
				// １回のみ実行
				MultiThreadMgrSub(strc);
				RunFlg = true;
			}
			// 一括待機状態
			/// 全スレッド終了チェック
			if (GetThreadEnd() == true)
			{
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
	}
	/// <summary>
	/// スレッド停止・起動状態表示
	/// </summary>
	/// 0.5秒間隔で「スレッド起動可能」
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void ThreadStatMsg(CStressCPUDlg* strc)
	{
		// 0.5秒間隔で「スレッド起動可能」
		CString StMsg = "";
		int disp_flg = 1; // 1=表示、0=空白
		for (;;)
		{
			if (disp_flg)
			{
				StMsg = "スレッドは";
			}
			else
			{
				StMsg = "起動可能";
			}
			// スレッド状態表示設定
			strc->SetST_ThreadStatus(StMsg);
			// 表示待ち
			// 0.5秒毎に出力
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			disp_flg ^= 1; // 1/0 反転
			/// 全スレッド終了チェック
			if (GetThreadEnd() == true)
			{
				// スレッド状態表示設定
				strc->SetST_ThreadStatus("スレッドは停止中");
				break;
			}
		}
	}
	/// <summary>
	/// 全スレッド終了チェック
	/// </summary>
	/// <return>bool ThreadEndFlg</return>
	bool GetThreadEnd()
	{
		// GUI制御用排他オブジェクト
		std::lock_guard<std::mutex> lock(mtx_scpu);
		return ThreadEndFlg;
	}
	/// <summary>
	/// 全スレッド終了フラグ設定
	/// </summary>
	/// <param name="ThreadEndFlg">全スレッド終了フラグ</param>
	void SetThreadEnd(bool var)
	{
		// GUI制御用排他オブジェクト
		std::lock_guard<std::mutex> lock(mtx_scpu);
		ThreadEndFlg = var;
	}
	/// <summary>
	/// 各スレッド毎のThreadStressor()の計算処理時間の平均値を算出（時間[nano]）
	/// 1m秒毎に収集し、平均値を取得する
	/// </summary>
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void FlopsMultiThreadCollectCalculationVolume(CStressCPUDlg* strc)
	{
		const auto max_count = strc->GetMaxThrdCount();
		// 終了フラグを判定
		for (;SCPU::GetThreadEnd() == false;)
		{
			// サンプリング実行中フラグONか？
			if (strc->GetFlpsSamplingFlg() == false)
			{
				// 1m秒待ち
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}
			// 各実行中スレッドのストレス処理時間を集計して平均値を算出
			// 実行スレッド処理時間合計値
			double flpsTm = 0.0;
			for (int ii = 0; ii < max_count; ii++)
			{
				// 実行中のスレッドのみ消費時間を採取
				if (strc->hmultiThrd->GetStatus(ii) == MT::e_Status::st_running)
				{
					// スレッド処理時間取得
					flpsTm += strc->hmultiThrd->GetThreadTimes(ii);
				}
				// 1m秒待ち
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			// 実行スレッド数
			int ExecThread = strc->hmultiThrd->GetUpdateCounter();
			// 実行中スレッド数で割る
			flpsTm /= (double)ExecThread;
			// 平均値消費時間を設定
			strc->SetFlpsCalTimerEven(flpsTm);
			// サンプリング実行中フラグOFF
			strc->SetFlpsSamplingFlg(false);
		}
	}
	/// <summary>
	/// FLOPS表示スレッド
	/// 0.3秒毎に更新
	/// 全スレッドが終了するまで無限にループ
	/// </summary>
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void FlopsDispValueThread(CStressCPUDlg* strc)
	{
		float fps_max = 0.0;
		CString cfps_max = "";
		float fps_min = 0.0;
		CString cfps_min = "";
		// 実行スレッド数保持
		static int st_RunThreadCount;
		// 単精度演算消費時間サンプリング実行中フラグをON
		// FlopsMultiThreadCollectCalculationVolume()でFlpsSamplingFlgが参照され、
		// サンプリング完了後にOFFにされる
		strc->SetFlpsSamplingFlg(true);
		for (; SCPU::GetThreadEnd() == false;)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			// 単精度演算消費時間サンプリング実行中はFLOPS計算はしない
			if (strc->GetFlpsSamplingFlg() == true)
			{
				continue;
			}
			// 稼働中のスレッドがあるか
			if (strc->GetExecAllFlag() == TRUE)
			{
				//*******************************************************************
				// FLOPS計算
				//*******************************************************************
				// 実行スレッド数取得
				auto RunThreadCount = strc->hmultiThrd->GetUpdateCounter();
				// 実行スレッド数が変更されたらMAX/MINをクリア
				if (st_RunThreadCount != RunThreadCount)
				{				
					// MAX/MINをクリア
					double tmpFlops = -1.0;
					CString tmpCvar = "";
					GetMinMax(
						tmpFlops,
						tmpFlops,
						tmpFlops,
						tmpCvar,
						tmpCvar,
						tmpCvar
					);
					// 実行スレッド数保管
					st_RunThreadCount = RunThreadCount;
				}
				// 高負荷率可変数(MT::GetStressCtrl()) x 外側ループ回数 x スレッド数
				double d_inner = MT::GetStressCtrl();
				double d_outer = D_OUTER;
				double d_RunThreadCount = RunThreadCount;
				auto Flops = d_inner * d_outer * d_RunThreadCount;
				// nano(1/1,000,000,000)秒 / スレッド毎の平均処理時間
				// サンプリング処理に要した時間は微々たるため無視する
				auto nSecond = strc->GetFlpsCalTimerEven();
				// GFLOPS変換　1nano(1/1000,000,000)秒間に何回演算ができるか
				if (nSecond > 0.0)
				{
					Flops /= nSecond;
				}
				else
				{
					Flops = 0.0;
				}
				//*******************************************************************
				// MIN/MAX とそれぞれの単位を返す
				// 単位表示変更
				//*******************************************************************
				double MinVar = 0.0;
				double MaxVar = 0.0;
				CString	cFPUnit = "GFLOPS";
				CString cMinUnit = "GFLOPS";
				CString cMaxUnit = "GFLOPS";
				GetMinMax(
					Flops,
					MinVar,
					MaxVar,
					cFPUnit,
					cMinUnit,
					cMaxUnit
				);
				CString cvar = "";
				// リアルタイムFLOPS表示欄に出力
				cvar.Format("%7.2f", Flops);
				strc->SetSTFlops(cvar, strc->m_ST_Flops);
				strc->SetSTFlops(cFPUnit, strc->m_ST_FPUNIT);
				// MIN値FLOPS表示欄に出力
				cvar.Format("%7.2f", MinVar);
				strc->SetSTFlops(cvar, strc->m_ST_FlopsMIN);
				strc->SetSTFlops(cMinUnit, strc->m_ST_FPUNIT_MIN);
				// MAX値FLOPS表示欄に出力
				cvar.Format("%7.2f", MaxVar);
				strc->SetSTFlops(cvar, strc->m_ST_FlopsMAX);
				strc->SetSTFlops(cMaxUnit, strc->m_ST_FPUNIT_MAX);
			}
			// 単精度演算消費時間サンプリング実行中フラグをON
			// FlopsMultiThreadCollectCalculationVolume()でFlpsSamplingFlgが参照され、
			// サンプリング完了後にOFFにされる
			strc->SetFlpsSamplingFlg(true);
		}
	}
	/// <summary>
	/// 誤差補正処理
	/// </summary>
	/// <param name="real_flops">リアルタイムFLOPS値</param>
	/// <param name="min_max_flops">最小または最大値</param>
	/// <param name="per">誤差率（％）</param>
	/// <returns>
	/// 誤差率（％）を超えた場合はTRUE、
	/// 誤差率（％）範囲内の場合はFALSE
	/// </returns>
	BOOL IsAJustFlops(
		double real_flops,
		double min_max_flops,
		double per
	)
	{
		BOOL retvB = FALSE; // 誤差範囲の場合はFALSEを返す
		// real_flopsとmin_max_flopsの差の絶対値を取る
		auto diff = ::abs(real_flops - min_max_flops);
		// 差がreal_flopsの誤差率を超えた場合はTRUE
		auto rates = (diff / real_flops) * 100;
		retvB = (per < rates) ? TRUE : FALSE;
		return retvB;
	}
	/// <summary>
	/// FLOPS値のMIN/MAXとそれぞれの単位文字を返す
	/// リアルタイムFLOPSとMAX及びMINがかけ離れたら
	/// 一旦リアルタイムFLOPS値に設定する。
	/// リアルタイムFLOPSとの差が50%以上の場合は
	/// リアルタイムFLOPS値に設定する
	/// </summary>
	/// <param name="flops_var">取得した演算消費時間</param>
	/// <param name="min_var">MIN値</param>
	/// <param name="max_var">MAX値</param>
	/// <param name="str_real_var">リアルタイムFLOPSの単位</param>
	/// <param name="str_min_unit">MIN値単位</param>
	/// <param name="str_max_unit">MAX値単位</param>
	void GetMinMax(
		double& flops_var,
		double& min_var,
		double& max_var,
		CString& str_real_var,
		CString& str_min_unit,
		CString& str_max_unit
	)
	{
		// 誤差補正率(%)
		// リアルタイムFLOPS値とMAX値及びMIN値の誤差率が12.0%を超えた場合は強制的に
		// リアルタイムFLOPS値に設定します。
		static const double ajRate = 12.0;
		// 比較用の値
		static double MaxVar = 0.0;
		static double MinVar = 0.0;
		// 表示用の値と単位
		static double FlpsMaxVar = 0.0;
		static double FlpsMinVar = 0.0;
		static CString cMaxUnit = "GFLOPS";
		static CString cMinUnit = "GFLOPS";
		auto var = flops_var;
		// MAX/MINクリア
		if (flops_var < 0)
		{
			MaxVar = 0.0;
			MinVar = 0.0;
			FlpsMaxVar = 0.0;
			FlpsMinVar = 0.0;
			cMaxUnit = "GFLOPS";
			cMinUnit = "GFLOPS";
			return;
		}
		// 単位を求める
		CString cunit;
		cunit = "GFLOPS";
		if (var > 950.0)
		{
			cunit = "TFLOPS";
			var /= 1000.0;
		}
		if (var > 950.0)
		{
			cunit = "PFLOPS";
			var /= 1000.0;
		}
		str_real_var = cunit;
		// 初期設定
		if (MaxVar == 0.0)
		{
			MaxVar = flops_var;
			FlpsMinVar = var;
		}
		if (MinVar == 0.0)
		{
			MinVar = flops_var;
			FlpsMinVar = var;
		}
		// MAXより大きいか
		if (flops_var >= MaxVar)
		{
			MaxVar = flops_var;
			FlpsMaxVar = var;
			// 単位表示変更
			cMaxUnit = cunit;
		}
		// MINより小さいか
		if (flops_var < MinVar)
		{
			MinVar = flops_var;
			FlpsMinVar = var;
			// 単位表示変更
			cMinUnit = cunit;
		}
		// リアルタイムFLOPSとの差が50%以上の場合は
		// リアルタイムFLOPS値に設定する
		if (TRUE == IsAJustFlops(flops_var, MinVar, ajRate))
		{
			MinVar = flops_var;
			FlpsMinVar = var;
		}
		if (TRUE == IsAJustFlops(flops_var, MaxVar, ajRate))
		{
			MaxVar = flops_var;
			FlpsMaxVar = var;
		}
		// リアルタイムFLOPS
		flops_var = var;
		str_real_var = cunit;
		// MIN値FLOPS
		min_var = FlpsMinVar;
		str_min_unit = cMinUnit;
		// MAX値FLOPS
		max_var = FlpsMaxVar;
		str_max_unit = cMaxUnit;
	}
	///////////////////////////////////////////////////////////////////
	/// <summary>
	/// 一時的にイベント無効フラグを設定する
	/// </summary>
	/// <param name="lpCBtn">CButtonポインタ</param>
	/// <param name="mtime">イベント無効時間(ms)</param>
	void EventWait(
		CButton* lpCBtn,
		UINT32 mtime
	)
	{
		lpCBtn->EnableWindow(FALSE);
		std::this_thread::sleep_for(std::chrono::milliseconds(mtime));
		lpCBtn->EnableWindow(TRUE);
	}
}

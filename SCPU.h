#pragma once
/*
	■各スレッドの単精度演算消費時間の合計からFLOPSを割り出す
	　１秒間に単精度演算を実行する回数＝FLOPS（Floating-Point Operations Per Second）
	　TT＝全スレッドの単精度演算終了時間の合計（ns）
	　FC＝単精度演算ループ回数（可変）
	　FP＝4（１回当たりの単精度演算回数＝固定）
	　TC＝全スレッド数（可変）
	　GFLOPS（秒）＝(FC×FP×TC)/TT
*/
#include <mutex>
#include "StressCPUDlg.h"

namespace MT
{
	// 排他制御用オブジェクト
	extern std::mutex mtx_[D_MAX_THRD_CNT];
}
namespace SCPU
{
	// GUIコントロール用排他ロック
	extern std::mutex g_mtx_[D_MAX_THRD_CNT];
	// GUI制御用排他オブジェクト
	extern std::mutex mtx_scpu;
	extern std::mutex mtx_Flops;
	extern std::mutex mtx_FlopsVar;
	extern std::mutex mtx_FlopsMax;
	extern std::mutex mtx_FlopsMin;
	extern std::mutex mtx_BU_scpu;
	extern std::mutex mtx_ExecAllFlag;
	extern std::mutex mtx_scpu_MaxThrdCount;
	extern std::mutex mtx_ThreadEndFlg;
	// GUI制御用排他オブジェクト2
	extern std::mutex mtx_scpu2;
	// スレッド計算速度情報収集の排他オブジェクト
	extern std::mutex mtx_flps;
	// 単精度演算消費時間サンプリング実行中フラグ排他オブジェクト
	extern std::mutex mtx_flpssmpflg;
	// プログレスバー更新フラグ排他
	extern std::mutex mtx_prgbar;
	// 乱数表示フラグ排他
	extern std::mutex mtx_rnddisp;
	///////////////////////////////////////////////////////////////////
	/// <summary>
	/// 一時的にイベント無効フラグを設定する
	/// </summary>
	/// <param name="lpCBtn">CButtonポインタ</param>
	/// <param name="mtime">イベント無効時間(ms)</param>
	void EventWait(
		CButton* lpCBtn,
		UINT32 mtime
	);
	/// <summary>
	/// FLOPS値のMIN/MAXとそれぞれの単位文字を返す 
	/// </summary>
	/// <param name="var">取得した演算消費時間</param>
	/// <param name="min_var">MIN値</param>
	/// <param name="max_var">MAX値</param>
	/// <param name="cunit_var">リアルタイムFLOPSの単位</param>
	/// <param name="cmin_unit">MIN値単位</param>
	/// <param name="cmax_unit">MAX値単位</param>
	void GetMinMax(
		double& flops_var,
		double& min_var,
		double& max_var,
		CString& cunit_var,
		CString& cmin_unit,
		CString& cmax_unit
	);
	/// <summary>
	/// 誤差補正処理
	/// </summary>
	/// <param name="real_flops">リアルタイムFLOPS値</param>
	/// <param name="min_max_flops">最小または最大値</param>
	/// <param name="per">誤差率（％）</param>
	/// <returns>
	/// 誤差を超えた場合はTRUE、
	/// 誤差範囲内の場合はFALSE
	/// </returns>
	BOOL IsAJustFlops(
		double real_flops,
		double min_max_flops,
		double per
	);
	/// <summary>
	/// FLOPS用のプログレスバー更新スレッド
	/// 実行中のスレッドが１つ以上あれば実行
	/// </summary>
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void FlopsProgressBarUpdate(CStressCPUDlg* strc);
	// FLOPS表示スレッド
	// 0.5秒毎に更新
	// 全スレッドが終了するまで無限にループ
	void FlopsDispValueThread(CStressCPUDlg* strc);
	/// <summary>
	/// ストレススレッドの乱数をGUIコントロールに表示する
	/// </summary>
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void MultiThreadMgrSub(CStressCPUDlg* strc);
	/// <summary>
	/// 指定スレッドの乱数をGUIコントロールに表示する
	/// スレッド関数として動作する
	/// </summary>
	/// <param name="thridx">スレッドID</param>
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void ThreadRandVarDisp(
		int thridx,
		CStressCPUDlg* strc
	);
	/// <summary>
	/// ストレススレッドの乱数をGUIコントロールに表示する
	/// スレッド監視用スレッド関数
	/// 発生した乱数を拾って表示エリアに表示
	/// </summary>
	/// <param name="strc">CStressCPUDlgポインタ</param>
	void MultiThreadMgr(CStressCPUDlg* strc);
	/// <summary>
	/// スレッド停止・起動状態表示
	/// </summary>
	/// <param name="strc"></param>
	void ThreadStatMsg(CStressCPUDlg* strc);
	/// <summary>
	/// 全スレッド終了チェック
	/// </summary>
	/// <return>bool ThreadEndFlg</return>
	bool GetThreadEnd();
	/// <summary>
	/// 全スレッド終了フラグ設定
	/// </summary>
	/// <param name="ThreadEndFlg">全スレッド終了フラグ</param>
	void SetThreadEnd(bool var);
	/// <summary>
	/// 各スレッドの計算数の平均値を収集（回数、時間[S]）
	/// 0.5秒毎に計算量を収集し、平均値を取得する
	/// </summary>
	/// <param name="strc"></param>
	void FlopsMultiThreadCollectCalculationVolume(CStressCPUDlg* strc);
}

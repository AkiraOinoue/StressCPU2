#include <mutex>
#include <chrono>
#include <random>
#include <time.h>
#include <atlstr.h>
#include <taskschd.h> // CPU Threadのスケジューラ？
#include <string>
#include "MultiThread.h"
namespace SCPU
{
	// スレッド計算速度情報収集の排他オブジェクト
	extern std::mutex mtx_flps;
}
namespace MT
{
	// 排他制御用オブジェクト
	std::mutex mtx_[D_MAX_THRD_CNT];
	// 高負荷率可変排他オブジェクト
	std::mutex mtx_strss;
	// マルチスレッド終了カウンタ
	int thrdCounter = 0;
	// 高負荷率可変数
	ULONG LoopInnerVar;
	// 高負荷率可変設定
	void SetStressCtrl(const ULONG var)
	{
		// 高負荷率可変排他オブジェクト
		std::lock_guard<std::mutex> lock(mtx_strss);
		LoopInnerVar = var;
	}
	// 高負荷率可変取得
	ULONG GetStressCtrl()
	{
		// 高負荷率可変排他オブジェクト
		std::lock_guard<std::mutex> lock(mtx_strss);
		return LoopInnerVar;
	}
	// マルチスレッド終了カウンタ取得
	int GetthrdCounter()
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_strss);
		return thrdCounter;
	}
	/// <summary>
	/// 乱数発生
	/// </summary>
	/// <param name="mrnd">乱数値</param>
	/// <returns>ULONG</returns>
	ULONG GetRnd(ULONG mrnd)
	{
		static const ULONG N = 2100000000; // [500, N-1] の乱数を発生
		return ((mrnd % N) + 500);
	}
	/// <summary>
	/// 負荷処理
	/// 倍精度演算プロセッサ＆整数演算プロセッサの負荷処理
	/// </summary>
	void ThreadStressor(void)
	{
		// 乱数生成初期化
		std::mt19937 mt;            // メルセンヌ・ツイスタの32ビット版
		std::random_device rnd;     // 非決定的な乱数生成器

		ULONG max_count = GetStressCtrl();
		for (ULONG kk = 0; kk < D_OUTER; kk++)
		{
			// 演算値を乱数発生
			mt.seed(rnd());            // シード指定
			auto var1 = mt();
			auto var2 = mt();
			auto var3 = mt();
			double d = (double)var1 * 1.987654;
			double e = (double)d * 1.012345;
			for (ULONG ii = 0; ii < max_count; ii++)
			{
				d *= e;
				d /= e;
				d += e;
				d -= e;
			}
		}
	}
	/// <summary>
	/// CPUを動作させてストレスを与える。
	/// ・倍精度浮動小数点数演算
	/// ・整数演算
	/// ・乱数を発生させる。
	/// </summary>
	/// <param name="thrd">マルチスレッド管理クラス</param>
	/// <param name="idx">マルチスレッドID</param>
	void CPU_MainStressor(
		MultiThread* thrd,
		const int idx
	)
	{
		// 乱数生成初期化
		std::mt19937 mt;            // メルセンヌ・ツイスタの32ビット版
		std::random_device rnd;     // 非決定的な乱数生成器
		// 乱数生成
		int r;
		// プログレスバーコントロールを開始
		//thrd->
		// 経過時間を初期化
		for (int stat = e_Status::st_sleep;
			stat != e_Status::st_end;
			stat = thrd->GetStatus(idx)
			)
		{
			if (stat == e_Status::st_sleep)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			// 演算値を乱数発生
			mt.seed(rnd());            // シード指定
			// 時間計測開始
			auto start = std::chrono::system_clock::now();
			// 負荷処理
			ThreadStressor();
			// 経過時間設定
			auto end = std::chrono::system_clock::now();
			// 経過時間をnano秒単位で表示
			auto diff = end - start;
			auto worktm = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count();
			// 経過時間を設定
			thrd->SetThreadTimes(idx, (double)worktm);
			// 乱数生成
			r = GetRnd(mt());
			CString cvar_rnd;
			cvar_rnd.Format("%010d", r);
			// 乱数を該当スレッドIDに保存
			std::string rndstr = cvar_rnd.GetString();
			thrd->SetRndStr(
				idx,
				rndstr
			);
		}
		// 終了フラグをON
		thrd->SetEndFlg(idx, true);
	}
	// 該当するスレッドを非同期で起動する。
	void MultiThread::ExecThread(int ID)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[ID]);
		// 非同期スレッドの生成
		this->MTM_Tbl[ID].hThread = new std::thread(
			MT::CPU_MainStressor,
			this,
			ID
		);
		// スレッドのハンドルを取り出す
		auto handle = this->MTM_Tbl[ID].hThread->native_handle();
		// CPUのスレッドに割り当てる
		SetThreadAffinityMask(
			handle,
			1 << ID
		);
	}
	// 該当するスレッドのステータスと優先度をコントロールする
	void MultiThread::CtrlThread(
		int thrdID,
		int status
	)
	{
		// スレッドのステータス設定
		this->SetStatus(thrdID, status);
		// スレッドポインタを取得
		if (this->GetThrdPointer(thrdID) == nullptr)
		{
			return;
		}
		// 指定されたスレッドの相対優先度を変更
		int thread_pri;
		if (status == e_Status::st_running)
		{
			// 活動状態
			// 基準優先度を優先度クラスより2だけ低く設定します。
			thread_pri = THREAD_PRIORITY_LOWEST;
		}
		else if (status == e_Status::st_sleep)
		{
			// 待機状態		
			// 基準優先度を優先度クラスより1だけ高く設定します。
			//THREAD_PRIORITY_LOWEST;
			thread_pri = THREAD_PRIORITY_ABOVE_NORMAL;
		}
		else
		{
			// 停止状態
			// 基準優先度を優先度クラスより2だけ低く設定します。
			thread_pri = THREAD_PRIORITY_LOWEST;
		}
		// 該当スレッドの占有優先度を設定する。
		SetThreadPriority(
			// スレッドハンドルを取得
			this->GetThrdNativeHnd(thrdID),
			thread_pri
		);
	}
	void MultiThread::SetEndFlg(int thrdID, bool var)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		this->MTM_Tbl[thrdID].endflg = var;
		MT::thrdCounter++;
	}
	// スレッドの終了フラグを取得
	bool MultiThread::GetEndFlg(int thrdID)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].endflg;
	}
	// マルチスレッドのステータス取得
	int MultiThread::GetStatus(int thrdID)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		// スレッドのステータス設定
		return this->MTM_Tbl[thrdID].status;
	}
	// ステータス反転
	int MultiThread::GetReverse(int thrdID)
	{
		// MT::e_Status::st_sleep <---> MT::e_Status::st_running
		int st = this->GetStatus(thrdID) ^ MT::e_Status::st_for_xor;
		return st;
	}
	// スレッドステータス設定取得
	void MultiThread::SetStatus(int thrdID, int var)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		this->MTM_Tbl[thrdID].status = var;
	}
	// マルチスレッドの乱数文字列を取得
	std::string MultiThread::GetRndStr(int thrdID)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].var_rnd;
	}
	// マルチスレッドの乱数文字列を設定
	void MultiThread::SetRndStr(
		int thrdID,
		std::string& rndstr
	)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		this->MTM_Tbl[thrdID].var_rnd = rndstr;
	}
	// スレッド処理時間設定
	// スレッド毎のThreadStressor()一回当たりの実行時間を設定する
	void MultiThread::SetThreadTimes(int thrdID, double var)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		this->MTM_Tbl[thrdID].caltimer = var;
	}
	// スレッド処理時間取得（ループ）
	// ThreadStressor()一回当たりの実行時間を返す
	double MultiThread::GetThreadTimes(int thrdID)
	{
		// 処理時間が設定されるまで待つ
		double caltimer = 0.0;
		for (; caltimer <= 0.0;)
		{
			caltimer = this->GetCalTimer(thrdID);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return caltimer;
	}
	// スレッド処理時間取得
	// ThreadStressor()一回当たりの実行時間を返す
	double MultiThread::GetCalTimer(int thrdID)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].caltimer;
	}
	// 現在実行中スレッドのカウントを行う
	void MultiThread::SetUpdateCounter()
	{
		// マルチスレッドテーブルのサイズを取得
		size_t max_counter = this->GetMthrSize();
		int run_counter = 0;
		for (size_t ii = 0; ii < max_counter; ii++)
		{
			if (this->GetStatus(ii) == e_Status::st_running)
			{
				run_counter++;
			}
		}
		this->m_RunningCount = run_counter;
	}
	// マルチスレッドテーブルのサイズを取得
	size_t MultiThread::GetMthrSize()
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_strss);
		size_t max_counter = this->MTM_Tbl.size();
		return max_counter;
	}
	// スレッドポインタを取得
	std::thread* MultiThread::GetThrdPointer(int thrdID)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].hThread;
	}
	// スレッドのハンドルを取得
	std::thread::native_handle_type MultiThread::GetThrdNativeHnd(int thrdID)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_[thrdID]);
		return this->MTM_Tbl[thrdID].hThread->native_handle();
	}
	// 現在実行中スレッドのカウントを設定する
	void MultiThread::SetUpdateCounter(int var)
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_strss);
		this->m_RunningCount = var;
	}
	// 現在実行中スレッドのカウントを取得
	int MultiThread::GetUpdateCounter()
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_strss);
		return this->m_RunningCount;
	}
	// 現在実行中のスレッド数をプラス
	void MultiThread::PlusCounter()
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_strss);
		this->m_RunningCount++;
	}
	// 現在実行中のスレッド数をマイナス
	void MultiThread::MinusCounter()
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_strss);
		this->m_RunningCount--;
	}
	// 現在実行中のスレッド数をゼロにする
	void MultiThread::ZeroCounter()
	{
		// 参照する前にロックを取得する
		std::lock_guard<std::mutex> lock(mtx_strss);
		this->m_RunningCount = 0;
	}
}
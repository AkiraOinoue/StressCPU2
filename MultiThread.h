#pragma once
#include <thread>
#include <map>
#include <string>
#define D_OUTER (100000)
#define D_INNER (5042) // FLOPS値調整用の値
#define D_MAX_THRD_CNT (16)
namespace MT
{
	// 高負荷率可変設定
	void SetStressCtrl(const ULONG var);
	// 高負荷率可変取得
	ULONG GetStressCtrl();
	// マルチスレッド終了カウンタ取得
	int GetthrdCounter();
	// 前方参照宣言
	class MultiThread;
	/// <summary>
	/// CPUを動作させてストレスを与える。
	/// ・倍精度浮動小数点数演算
	/// ・整数演算
	/// ・乱数を発生さえる。
	/// </summary>
	/// <param name="csCpudlg">マルチスレッド管理クラス</param>
	/// <param name="idx">マルチスレッドID</param>
	void CPU_MainStressor(
		MultiThread* thrd,
		const int idx
	);
	/// <summary>
	/// スレッドステータス定義
	/// </summary>
	enum e_Status
	{
		st_sleep = 1,	// 一時停止
		st_running = 2,	// 実行中
		st_for_xor = 3,	// 待機／復帰反転用
		st_end = 99		// 終了
	};
	/// <summary>
	/// スレッドコントロールデータ
	/// </summary>
	typedef struct _st {
		_st() :
			status(e_Status::st_sleep),
			var_rnd(""),
			endflg(false),
			caltimer(0.0),
			hRndDisp(nullptr),
			hThread(nullptr)
		{}
		std::thread* hThread;	// スレッドオブジェクト
		std::thread* hRndDisp;	// スレッド乱数表示スレッドオブジェクト
		int	status;				// 処理コントロール（0=終了、1=一時停止、2=実行中）
		bool endflg;
		std::string var_rnd;	// スレッド毎の乱数
		double caltimer;		// ストレス処理時間（nano秒=10^-9=1/1,000,000,000）
	} _T_THREAD_CTRL_DATA;
	/// <summary>
	///  マルチスレッド管理テーブル
	/// </summary>
	typedef std::map<
		int,				// スレッドID
		_T_THREAD_CTRL_DATA	// スレッドコントロールデータ
	> _T_THREAD_MGR_TBL;
	//////////////////////////////
	// マルチスレッドクラス
	//////////////////////////////
	class MultiThread
	{
	public:
		MultiThread():
			m_RunningCount(0),
			MTM_Tbl{}
		{}
	private:
		// 現在実行中のスレッド数
		int m_RunningCount;
	public:
		// 現在実行中のスレッド数をプラス
		void PlusCounter();
		// 現在実行中のスレッド数をマイナス
		void MinusCounter();
		// 現在実行中のスレッド数をゼロにする
		void ZeroCounter();
		// 現在実行中スレッドのカウントを行う
		void SetUpdateCounter();
		// 現在実行中スレッドのカウントを設定する
		void SetUpdateCounter(int var);
		// 現在実行中スレッドのカウントを取得
		int GetUpdateCounter();
		// 該当するスレッドを非同期で起動する。
		void ExecThread(int thrdID);
		// 該当するスレッドのステータスと優先度をコントロールする
		void CtrlThread(
			int thrdID,
			int status
		);
		// スレッドの終了フラグを設定
		void SetEndFlg(int thrdID, bool var);
		// スレッドの終了フラグを取得
		bool GetEndFlg(int thrdID);
		// スレッドステータス設定取得
		void SetStatus(int thrdID, int var);
		// マルチスレッドのステータス取得
		int GetStatus(int thrdID);
		// ステータス反転
		int GetReverse(int thrdID);
		// マルチスレッドテーブルのサイズを取得
		size_t GetMthrSize();
		// スレッドポインタを取得
		std::thread* GetThrdPointer(int thrdID);
		// スレッドのハンドルを取得
		std::thread::native_handle_type GetThrdNativeHnd(int thrdID);
		// マルチスレッドの乱数文字列を取得
		std::string GetRndStr(int thrdID);
		// マルチスレッドの乱数文字列を設定
		void SetRndStr(
			int thrdID,
			std::string& rndstr
		);
		// スレッド処理時間設定
		void SetThreadTimes(int thrdID, double var);
		// スレッド処理時間取得（ループ）
		double GetThreadTimes(int thrdID);
		// スレッド処理時間取得
		double GetCalTimer(int thrdID);
		// マルチスレッド管理テーブル
		_T_THREAD_MGR_TBL MTM_Tbl;
	};
}
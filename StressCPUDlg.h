// マルチスレッドCPUの全スレッドに対して
// 100%の負荷を与える。且つスレッド後のコントロールが可能
//・全スレッドを一括で（待機／復帰）指定できる。
//・スレッドを個別に（待機／復帰）切り替えるができます。
//・全スレッドを一括で（待機／復帰）を反転できる。
//・負荷スレッドは「スレッドの優先順位」を下げているので他のアプリへの負荷は軽減できる。
//・最大論理コア数は12まで対応
// StressCPUDlg.h : ヘッダー ファイル
// Designed and Implemented by Akira Oinoue
// Email:davinci1958@docomonet.jp
// 2020.6 ver 1.0.0
// ※）スレッドの割り当てについては、以下のサイトを参考にしました。
// 山本ワールドさんありがとう
// http://yamatyuu.net/computer/program/vc2013/thread4/index.html

#pragma once
#include <thread>
#include <vector>
#include <string>
#include "MultiThread.h"
#include "CPUID.h"
#include "PrgbCtrl.h"
#define D_WAIT_TIME (500)
//#define D_MAX_THRD_CNT (16)
// CStressCPUDlg ダイアログ
class CStressCPUDlg : 
	public CDialogEx
{
private:
	// CPUベンダ名
	std::string cpu_vender;
	// コンストラクション
public:
	CStressCPUDlg(CWnd* pParent = nullptr);	// 標準コンストラクター
	// CPUベンダ名
	std::string GetCpuVend();
// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STRESSCPU_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート
	// 初期起動実行フラグ
	bool m_InitExec;

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// CPUインフォメーション
	CPUID cpuid;
	// 一括実行フラグ
	BOOL m_ExecAllFlag;
	// 一括実行フラグ取得
	BOOL GetExecAllFlag();
	// 一括実行フラグ設定
	void SetExecAllFlag(BOOL var);
	// スレッド乱数ベクター
	std::vector<CStatic*>	m_StVector;
	// プログレスバーコントロールベクター
	std::vector<PrgbCtrl>	PrgBarCtl;
	// プログレスバー実行
	void PrgBarRun(int idx);
	// プログレスバー終了
	void PrgBarEnd(int idx);
	// プログレスバー更新
	void PrgBarUpdate(int idx);
	// スレッド乱数ベクター設定
	void SetStVector(int idx, const char* strRnd);
	// スレッド実行／休止の切替メソッド
	void SwitchThread(int sw);
	// スレッド監視用スレッド
	std::thread* hMultiThreadMgr;
	// マルチスレッド管理
	MT::MultiThread* hmultiThrd;
	// 論理プロセッサコア数
	int m_MaxThrdCount;
	// 物理プロセッサコア数
	int m_MaxPhysicalCore;
	// 各スレッドの実行速度測定スレッド
	std::thread* hFlpsPerThread;
	// 単精度演算消費時間サンプリング実行フラグ
	bool m_FlpsSamplingFlg;
	// 単精度演算消費時間サンプリング実行フラグ設定
	void SetFlpsSamplingFlg(bool var);
	// 単精度演算消費時間サンプリング実行フラグ取得
	bool GetFlpsSamplingFlg(void) const;
	/// 各スレッドの計算処理時間の平均値を収集（時間[nano]）
	/// 0.5秒毎に収集し、平均値を取得する
	void FlpsGetCollectCalculationVolume(void);
	// FLOPS表示スレッド
	void FlpsDispValue(void);
	// スレッド計算量の平均値
	// Max, Medium, Lowで決まる
	double FlpsCalCounter;
	// スレッド計算時間の平均値
	double FlpsCalTimerEven;
	// ストレス処理時間平均値設定
	void SetFlpsCalTimerEven(double var);
	// ストレス処理時間平均値取得
	double GetFlpsCalTimerEven(void);
	// 最大対象スレッド数取得
	int GetMaxThrdCount();
	std::vector<CButton*>	m_CH_Idles;
	/// <summary>
	/// SetCH_Idles
	/// 待機チェックのON/OFFと有効/無効
	/// </summary>
	/// <param name="idx">スレッドID</param>
	/// <param name="chk">チェックON/OFF</param>
	/// <param name="enbl">チェックボックス有効/無効</param>
	void SetCH_Idles(int idx, BOOL chk, BOOL enbl);
	// 一括復帰の有効／無効
	void SetBU_AllWake(BOOL enbl);
	// 一括待機チェックの有効／無効
	void SetBU_AllSleep(BOOL enbl);
	// 待機／復帰反転ボタンの有効／無効
	void SetBU_XorThrd(BOOL enbl);
	// スレッド状態表示設定
	void SetST_ThreadStatus(CString stmsg);
	// FLOPS表示
	void SetSTFlops(
		CString stvar,
		CStatic& cst
	);
	afx_msg void OnClickedBuAllSleep();
	afx_msg void OnClickedBuAllExec();
	// 一括実行
	CButton m_BU_ALL_Exec;
	// 一括待機
	CButton m_BU_AllSleep;
	// 一括復帰
	CButton m_BU_AllWake;
	// 乱数表示領域
	CStatic m_ST_Guid01;
	// 乱数表示領域
	CStatic m_ST_Guid02;
	// 乱数表示領域
	CStatic m_ST_Guid03;
	// 乱数表示領域
	CStatic m_ST_Guid04;
	// 乱数表示領域
	CStatic m_ST_Guid05;
	// 乱数表示領域
	CStatic m_ST_Guid06;
	// 乱数表示領域
	CStatic m_ST_Guid07;
	// 乱数表示領域
	CStatic m_ST_Guid08;
	// 乱数表示領域
	CStatic m_ST_Guid09;
	// 乱数表示領域
	CStatic m_ST_Guid10;
	// 乱数表示領域
	CStatic m_ST_Guid11;
	// 乱数表示領域
	CStatic m_ST_Guid12;
	// 乱数表示領域
	CStatic m_ST_Guid13;
	// 乱数表示領域
	CStatic m_ST_Guid14;
	// 乱数表示領域
	CStatic m_ST_Guid15;
	// 乱数表示領域
	CStatic m_ST_Guid16;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle01;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle02;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle03;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle04;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle05;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle06;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle07;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle08;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle09;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle10;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle11;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle12;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle13;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle14;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle15;
	// マルチスレッドの待機フラグ
	CButton m_CH_Idle16;
	// マルチスレッドの待機ON/OFFイベント処理
	afx_msg void OnClickedChIdle01();
	afx_msg void OnClickedChIdle02();
	afx_msg void OnClickedChIdle03();
	afx_msg void OnClickedChIdle04();
	afx_msg void OnClickedChIdle05();
	afx_msg void OnClickedChIdle06();
	afx_msg void OnClickedChIdle07();
	afx_msg void OnClickedChIdle08();
	afx_msg void OnClickedChIdle09();
	afx_msg void OnClickedChIdle10();
	afx_msg void OnClickedChIdle11();
	afx_msg void OnClickedChIdle12();
	afx_msg void OnClickedChIdle13();
	afx_msg void OnClickedChIdle14();
	afx_msg void OnClickedChIdle15();
	afx_msg void OnClickedChIdle16();
	afx_msg void OnClickedBuAllWake();
	afx_msg void OnClickedBuXorThread();
	// 待機／復帰反転ボタン
	CButton m_BU_XorThrd;
	// CPUベンダー名
	CStatic m_ST_CpuVen;
	// CPU情報表示欄
	CStatic m_ST_CpuBrand;
	// 論理CPUのスレッド数
	CStatic m_ST_CpuCnt;
	// スレッド停止・実行メッセージ
	CStatic m_ST_ThrdStat;
	afx_msg void OnBnClickedEnding();
	// 負荷率コントロール
	CComboBox m_COM_Stress;
	afx_msg void OnSelchangeComStress();
	// 最大スレッド数選択コンボボックス
	CComboBox m_COM_Thread;
	afx_msg void OnSelchangeComThread();
	afx_msg void OnClickedBuVersion();
	// CPUの速度性能を表示
	CStatic m_ST_Flops;
	// FLOPS単位表示
	CStatic m_ST_FPUNIT;
	// FLOPSMAX値
	CStatic m_ST_FlopsMAX;
	// FLOPSMIN値
	CStatic m_ST_FlopsMIN;
	// FLOPS単位MAX
	CStatic m_ST_FPUNIT_MAX;
	// FLOPSMIN単位
	CStatic m_ST_FPUNIT_MIN;
	// 負荷率選択タイトル
	CStatic m_ST_Stress;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	// プログレスバー
	CProgressCtrl Prgb01;
	CProgressCtrl Prgb10;
	CProgressCtrl Prgb11;
	CProgressCtrl Prgb12;
	CProgressCtrl Prgb13;
	CProgressCtrl Prgb14;
	CProgressCtrl Prgb15;
	CProgressCtrl Prgb16;
	CProgressCtrl Prgb02;
	CProgressCtrl Prgb03;
	CProgressCtrl Prgb04;
	CProgressCtrl Prgb05;
	CProgressCtrl Prgb06;
	CProgressCtrl Prgb07;
	CProgressCtrl Prgb08;
	CProgressCtrl Prgb09;
};

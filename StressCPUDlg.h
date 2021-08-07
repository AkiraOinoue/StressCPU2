// マルチスレッドCPUの全スレッドに対して
// 100%の負荷を与える。且つスレッド後のコントロールが可能
//・全スレッドを一括で（待機／復帰）指定できる。
//・スレッドを個別に（待機／復帰）切り替えるができます。
//・全スレッドを一括で（待機／復帰）を反転できる。
//・負荷スレッドは「スレッドの優先順位」を下げているので他のアプリへの負荷は軽減できる。
//・最大論理コア数は16まで対応
// ●gihub url
// https://github.com/AkiraOinoue/StressCPU2
// StressCPUDlg.h : ヘッダー ファイル
// Designed and Implemented by Akira Oinoue
// Email:davinci1958@docomonet.jp
// 2021.4 ver 1.2.0 FLOPS値調整
// 2021.2 ver 1.1.0 プログレスバー追加
// 2020.6 ver 1.0.0
// ※）スレッドの割り当てについては、以下のサイトを参考にしました。
// 山本ワールドさんありがとう
// http://yamatyuu.net/computer/program/vc2013/thread4/index.html

/* GITHUB .md
# StressCPU2
CPUの各スレッド毎に100%の負荷を与える事ができる。
このプログラムはその特性上PC全体を停止させたり、使用中のアプリケーションに大きな影響を与え且つ取り扱い中のデータを破壊してしまうリスクがあります。
このプログラムをご使用になる場合は他のアプリケーションを停止した状態にして下さい。
スレッド毎の負荷の掛け方は以下のサイトを参考にしました。
http://yamatyuu.net/computer/program/vc2013/thread4/index.html
山本ワールドさんありがとうございます。
開発ツール：Visual Studio 2019
プラットフォーム：Windows 10 64bit
アプリケーション：MFC Win32API 32bit アプリケーション
操作は操作マニュアル.xlsxを参考にして下さい。
*/

#pragma once
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include "MultiThread.h"
#include "CPUID.h"
#include "PrgbCtrl.h"
#define D_WAIT_TIME (509)
// ダイアログ幅拡張サイズ
#define D_PLUS_WIDTH (340)
// ダイアログ幅
#define D_DLG_WIDTH (440+40)
// ダイアログ高さ
#define D_DLG_HEIGHT (675+50)
// 部品間隔幅プラス
#define D_CTRL_PLUS_WIDTH (80)
// 部品間隔高さプラス
#define D_CTRL_PLUS_HEIGHT (80)
namespace fs = std::filesystem;
// 部品のサイズ型
typedef struct
{
	CStatic* lpST;
	RECT rect;
} _T_ST_RECT;
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
	/// <summary>
	/// スレッド表示列数選択リスト作成
	/// </summary>
	/// <param name="var">最大スレッド数</param>
	/// <returns>最小列数</returns>
	int MakeThrdCMBList(int var);
	/// <summary>
	/// 部品の初期化
	/// </summary>
	/// <param name=""></param>
	void SetCtrlInit(void);
	/// <summary>
	/// コントロールのベクターテーブル初期化
	/// </summary>
	/// <param name=""></param>
	void VectorTableInit(void);
	/// <summary>
	/// 各コントロールのツールチップを設定
	/// </summary>
	/// <param name=""></param>
	void SetCtrlToolTips(void);
	/// <summary>
	/// 各コントロールのツールチップを追加
	/// </summary>
	/// <param name="lpCtrl">コントロールのポインタ</param>
	/// <param name="tipsmsg">ツールチップ</param>
	void SetAddToolTips(CWnd* lpCtrl, LPCTSTR tipsmsg);
	HICON m_hIcon;
	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	/// <summary>
	/// コントロール部品の配置テーブルを作成
	/// </summary>
	/// <param name="r_tbl">部品の配置ベクター</param>
	/// <param name="r_rect">初期座標</param>
	/// <param name="var">配置列宇数</param>
	void MakeCordinateCtrl(
		std::vector<RECT>& r_tbl,
		const RECT r_rect,
		const int var
	);
	/// <summary>
	/// 部品のレイアウト
	/// </summary>
	/// <param name="var">列数</param>
	/// <param name="cnt">スレッド数</param>
	void SetLayOut(
		const int var,
		const int cnt
	);
	/// <summary>
	/// ダイアログサイズを設定
	/// </summary>
	/// <param name="idx">0=標準、1=横長</param>
	void SetDialogSize(int idx);
	/// <summary>
	/// FLOPS値MAXチェックON/OFF取得
	/// </summary>
	/// <returns>チェックON/OFF</returns>
	BOOL GetChkFlopsMAXLimited(void);
	/// <summary>
	/// FLOPS値MAXを設定
	/// </summary>
	/// <param name="var">MAX値</param>
	void SetFlopsMAXLimited(int var);
	/// <summary>
	/// FLOPS値MAXを取得
	/// </summary>
	/// <returns>MAX値</returns>
	int GetFlopsMAXLimited(void);
	// ツールチップ
	CToolTipCtrl m_toolTip;
	// CPUインフォメーション
	CPUID cpuid;
	// 一括実行フラグ
	BOOL m_ExecAllFlag;
	// 一括実行フラグ取得
	BOOL GetExecAllFlag();
	// 一括実行フラグ設定
	void SetExecAllFlag(BOOL var);
	// スレッドID表示欄コントロールベクター
	std::vector<CStatic*>	m_StThredID;
	// プログレスバーコントロールベクター
	std::vector<PrgbCtrl>	PrgBarCtl;
	// スレッド乱数ベクター
	std::vector<CStatic*>	m_StThrdRnd;
	// スレッド実行待機ボタンベクター
	std::vector<CButton*>	m_CH_Idles;
	// スレッドID表示サイズ
	RECT r_StThredID;
	// プログレスバーサイズ
	RECT r_PrgBarCtl;
	// スレッド乱数サイズ
	RECT r_StThrdRnd;
	// スレッド実行待機ボタンサイズ
	RECT r_CH_Idles;
	// FLOPS値リアル値のプログレスバー
	PrgbCtrl* Flops_RealBar;
	// FLOPS値MAX値のプログレスバー
	PrgbCtrl* Flops_MAXBar;
	// FLOPS値MIN値のプログレスバー
	PrgbCtrl* Flops_MINBar;
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
	// 倍精度演算消費時間サンプリング実行フラグ
	bool m_FlpsSamplingFlg;
	// 倍精度演算消費時間サンプリング実行フラグ設定
	void SetFlpsSamplingFlg(bool var);
	// 倍精度演算消費時間サンプリング実行フラグ取得
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
	/// <summary>
	/// 最大対象スレッド数設定
	/// </summary>
	/// <param name="thrdCnt">最大スレッド数</param>
	void SetMaxThrdCount(int thrdCnt);
	/// <summary>
	/// スレッド表示列数設定
	/// </summary>
	/// <param name="thrdCol">表示列数</param>
	void SetThrdDispCol(int thrdCol);
	/// <summary>
	/// スレッド表示列数取得
	/// </summary>
	/// <param name="">なし</param>
	/// <returns></returns>
	int GetThrdDispCol(void);
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
	/// <summary>
	/// スレッドステータス変更と
	/// チェックボックス変更・活性・非活性
	/// </summary>
	/// <param name="thrdStat">スレッドの状態指定
	/// st_sleep：休止
	/// st_running：実行中
	/// st_end：終了（通常は使用しない）
	/// </param>
	/// <param name="chk">TRUE:チェックON、FALSE：チェックOFF</param>
	/// <param name="enbl">TRUE:有効、FALSE：無効</param>
	void SetThrdCheckCtrl(
		MT::e_Status thrdStat,
		const BOOL chk,
		const BOOL enbl
	);
	/// <summary>
	/// スレッド乱数表示の初期化
	/// </summary>
	/// <param name="">なし</param>
	void SetRndInitial(void);
	/// <summary>
	/// コントロールを一定時間非活性
	/// </summary>
	/// <param name="lpbtnctrl">対象のコントロールポインタ</param>
	/// <param name="wait_time_ms">非活性時間(ms)：デフォルト509ms</param>
	void WaitActiveCtrl(
		CButton* lpbtnctrl,
		UINT32 wait_time_ms = D_WAIT_TIME
	);
	/// <summary>
	/// スレッド稼働フラグ設定
	/// </summary>
	/// <param name="flg">スレッド稼働フラグ
	/// true：稼働中
	/// fase：停止中
	/// </param>
	void SetInitExec(bool flg);
	/// <summary>
	/// スレッド稼働フラグ取得
	/// </summary>
	/// <param name="">なし</param>
	/// <returns>スレッド稼働フラグ</returns>
	bool GetInitExec(void);
	// プログレスバー更新フラグ
	BOOL PrgBarUpdateFlg;
	// スレッド毎の乱数表示フラグ
	BOOL ThreadRndDispFlg;
	// プログレスバー更新フラグ設定
	void SetPrgBarUpdateFlg(BOOL bval);
	// プログレスバー更新フラグ取得
	BOOL GetPrgBarUpdateFlg(void);
	// スレッド毎の乱数表示フラグ設定
	void SetThreadRndDispFlg(BOOL bval);
	// スレッド毎の乱数表示フラグ取得
	BOOL GetThreadRndDispFlg(void);
	afx_msg void OnClickedBuAllSleep();
	afx_msg void OnClickedBuAllExec();
	// スレッド準備
	CButton m_BU_ALL_Exec;
	// 一括休止
	CButton m_BU_AllSleep;
	// 一括実行
	CButton m_BU_AllWake;
	// 乱数表示領域
	CStatic m_ST_Guid01;
	CStatic m_ST_Guid02;
	CStatic m_ST_Guid03;
	CStatic m_ST_Guid04;
	CStatic m_ST_Guid05;
	CStatic m_ST_Guid06;
	CStatic m_ST_Guid07;
	CStatic m_ST_Guid08;
	CStatic m_ST_Guid09;
	CStatic m_ST_Guid10;
	CStatic m_ST_Guid11;
	CStatic m_ST_Guid12;
	CStatic m_ST_Guid13;
	CStatic m_ST_Guid14;
	CStatic m_ST_Guid15;
	CStatic m_ST_Guid16;
	CStatic m_ST_Guid17;
	CStatic m_ST_Guid18;
	CStatic m_ST_Guid19;
	CStatic m_ST_Guid20;
	CStatic m_ST_Guid21;
	CStatic m_ST_Guid22;
	CStatic m_ST_Guid23;
	CStatic m_ST_Guid24;
	CStatic m_ST_Guid25;
	CStatic m_ST_Guid26;
	CStatic m_ST_Guid27;
	CStatic m_ST_Guid28;
	CStatic m_ST_Guid29;
	CStatic m_ST_Guid30;
	CStatic m_ST_Guid31;
	CStatic m_ST_Guid32;
	// マルチスレッドの実行待機ボタン
	CButton m_CH_Idle01;
	CButton m_CH_Idle02;
	CButton m_CH_Idle03;
	CButton m_CH_Idle04;
	CButton m_CH_Idle05;
	CButton m_CH_Idle06;
	CButton m_CH_Idle07;
	CButton m_CH_Idle08;
	CButton m_CH_Idle09;
	CButton m_CH_Idle10;
	CButton m_CH_Idle11;
	CButton m_CH_Idle12;
	CButton m_CH_Idle13;
	CButton m_CH_Idle14;
	CButton m_CH_Idle15;
	CButton m_CH_Idle16;
	CButton m_CH_Idle17;
	CButton m_CH_Idle18;
	CButton m_CH_Idle19;
	CButton m_CH_Idle20;
	CButton m_CH_Idle21;
	CButton m_CH_Idle22;
	CButton m_CH_Idle23;
	CButton m_CH_Idle24;
	CButton m_CH_Idle25;
	CButton m_CH_Idle26;
	CButton m_CH_Idle27;
	CButton m_CH_Idle28;
	CButton m_CH_Idle29;
	CButton m_CH_Idle30;
	CButton m_CH_Idle31;
	CButton m_CH_Idle32;
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
	afx_msg void OnClickedChIdle17();
	afx_msg void OnClickedChIdle18();
	afx_msg void OnClickedChIdle19();
	afx_msg void OnClickedChIdle20();
	afx_msg void OnClickedChIdle21();
	afx_msg void OnClickedChIdle22();
	afx_msg void OnClickedChIdle23();
	afx_msg void OnClickedChIdle24();
	afx_msg void OnClickedChIdle25();
	afx_msg void OnClickedChIdle26();
	afx_msg void OnClickedChIdle27();
	afx_msg void OnClickedChIdle28();
	afx_msg void OnClickedChIdle29();
	afx_msg void OnClickedChIdle30();
	afx_msg void OnClickedChIdle31();
	afx_msg void OnClickedChIdle32();
	//
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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	// プログレスバー
	CProgressCtrl Prgb01;
	CProgressCtrl Prgb02;
	CProgressCtrl Prgb03;
	CProgressCtrl Prgb04;
	CProgressCtrl Prgb05;
	CProgressCtrl Prgb06;
	CProgressCtrl Prgb07;
	CProgressCtrl Prgb08;
	CProgressCtrl Prgb09;
	CProgressCtrl Prgb10;
	CProgressCtrl Prgb11;
	CProgressCtrl Prgb12;
	CProgressCtrl Prgb13;
	CProgressCtrl Prgb14;
	CProgressCtrl Prgb15;
	CProgressCtrl Prgb16;
	CProgressCtrl Prgb17;
	CProgressCtrl Prgb18;
	CProgressCtrl Prgb19;
	CProgressCtrl Prgb20;
	CProgressCtrl Prgb21;
	CProgressCtrl Prgb22;
	CProgressCtrl Prgb23;
	CProgressCtrl Prgb24;
	CProgressCtrl Prgb25;
	CProgressCtrl Prgb26;
	CProgressCtrl Prgb27;
	CProgressCtrl Prgb28;
	CProgressCtrl Prgb29;
	CProgressCtrl Prgb30;
	CProgressCtrl Prgb31;
	CProgressCtrl Prgb32;
	// プログレスバー更新有無切替
	CButton chk_PrgBar;
	// 乱数表示有無切替え
	CButton chk_RndVal;
	afx_msg void OnClickedChkPrgbar();
	afx_msg void OnClickedChkRndval();
	// CPU画像表示変数
	CStatic m_CPU_IMG;
	// FLOPS値設定
	void SetFlopsVar(double var);
	// FLOPS値取得
	double GetFlopVar(void);
	// FLOPS値MAX設定
	void SetFlopsMax(double var);
	// FLOPS値MAX取得
	double GetFlopsMax(void);
	// FLOPS値MIN設定
	void SetFlopsMin(double var);
	// FLOPS値MIN取得
	double GetFlopsMin(void);
private:
	// スレッド表示列数
	int m_ThrdDispCol;
	// ダイアログサイズのテーブル
	std::map<int, RECT> m_DialogSize;
	double m_Flops;
	double m_FlopsMax;
	double m_FlopsMin;
	int m_FlopsMAXLimited;
	// コントロールのハンドル格納マップ
	std::map<HWND, int>	m_CtrlMap;
	// CPUIDベンダー表示
	CPUID m_cpuinfo;
	// プログラムのカレントパス
	fs::path m_CurrentPath;
	// AMD CPU イメージファイル名
	fs::path m_amd_cpu_file;
	// Intel CPU イメージファイル名
	fs::path m_intel_cpu_file;
	//ピクチャーコントロールの描画エリア
	RECT m_cpu_rect;
	/// <summary>
	/// ピクチャーコントロールへの描画処理
	/// </summary>
	/// <param name="cpuid">CPUベンダー情報</param>
	void setPictureControl(CPUID& cpuid);
public:
	// ツールチップ表示イベント
	// 仮想関数を追加で作成
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClickedBuReset();
	// ストレス状態をリセットする
	CButton m_Btn_Reset;
	// 論理プロセッサ数
	CStatic m_ST_LProcCnt;
	// FLOPS値リアル値
	CProgressCtrl m_FLPSPRG01;
	// FLOPS値MAX値
	CProgressCtrl m_FLPSPRG02;
	// FLOPS値MIN値
	CProgressCtrl m_FLPSPRG03;
	// FLOPS値MAX指定チェックボックス
	CButton m_Chk_FlopsMAX;
	// FLOPS値MAX値入力欄
	CEdit m_EDT_FlopsMAX;
	afx_msg void OnChangeEdtFlopsmax();
	afx_msg void OnClickedChkFlopsmax();
	// FLOPSのMAX値表示欄
	CStatic m_ST_MaxValue;
	// スレッドID表示欄
	CStatic m_ST_THR01;
	CStatic m_ST_THR02;
	CStatic m_ST_THR03;
	CStatic m_ST_THR04;
	CStatic m_ST_THR05;
	CStatic m_ST_THR06;
	CStatic m_ST_THR07;
	CStatic m_ST_THR08;
	CStatic m_ST_THR09;
	CStatic m_ST_THR10;
	CStatic m_ST_THR11;
	CStatic m_ST_THR12;
	CStatic m_ST_THR13;
	CStatic m_ST_THR14;
	CStatic m_ST_THR15;
	CStatic m_ST_THR16;
	CStatic m_ST_THR17;
	CStatic m_ST_THR18;
	CStatic m_ST_THR19;
	CStatic m_ST_THR20;
	CStatic m_ST_THR21;
	CStatic m_ST_THR22;
	CStatic m_ST_THR23;
	CStatic m_ST_THR24;
	CStatic m_ST_THR25;
	CStatic m_ST_THR26;
	CStatic m_ST_THR27;
	CStatic m_ST_THR28;
	CStatic m_ST_THR29;
	CStatic m_ST_THR30;
	CStatic m_ST_THR31;
	CStatic m_ST_THR32;
	// スレッド表示列数の選択リスト
	CComboBox m_CMB_THRDW;
	// 表示列数
	CStatic m_ST_COL;
	afx_msg void OnSelchangeCmbThrdw();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


// StressCPUDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "StressCPU.h"
#include "StressCPUDlg.h"
#include "CVersionDlg.h"
#include "CDialogEnding.h"
#include "afxdialogex.h"
#include "MultiThread.h"
#include "SCPU.h"
#include <chrono>
#include <mutex>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
namespace MT
{
	// マルチスレッド終了カウンタ
	extern int thrdCounter;
}
// CPUベンダ名
std::string CStressCPUDlg::GetCpuVend()
{
	return this->cpu_vender;
}
// CStressCPUDlg ダイアログ
/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="pParent"></param>
/// <returns>なし</returns>
CStressCPUDlg::CStressCPUDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STRESSCPU_DIALOG, pParent),
	m_ExecAllFlag(FALSE),	// 一括実行フラグ
	m_InitExec(false),		// 初期起動実行フラグ
	m_FlpsSamplingFlg(false),	// 倍精度演算消費時間サンプリング実行フラグ
	FlpsCalCounter(0.0),
	FlpsCalTimerEven(0.0),
	hFlpsPerThread(nullptr),
	m_MaxPhysicalCore(0),
	PrgBarUpdateFlg(TRUE),
	ThreadRndDispFlg(TRUE),
	m_CurrentPath(""),
	m_amd_cpu_file("amd_cpu.jpg"),
	m_intel_cpu_file("intel_cpu.jpg"),
	m_cpuinfo{},
	m_CtrlMap{},
	m_MaxThrdCount(0)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_CPU);
	// ストレスマルチスレッド管理
	this->hmultiThrd = new MT::MultiThread;
	// スレッド数設定
	this->m_MaxThrdCount = this->m_cpuinfo.GetLogicProcessorCnt();
	if (this->m_MaxThrdCount > D_MAX_THRD_CNT)
	{
		CString cthrcnt = "";
		cthrcnt.Format(
			"お使いの論理CPUコアが%d個を超えましたので\n"
			"%d個に設定しました。",
			D_MAX_THRD_CNT,
			D_MAX_THRD_CNT
		);
		::MessageBox(
			nullptr,
			cthrcnt.GetString(),
			"スレッド最大数チェック",
			MB_ICONEXCLAMATION
		);
		this->m_MaxThrdCount = D_MAX_THRD_CNT;
	}
	// 全スレッド分の乱数・プログレスバー更新スレッド起動
	this->SetMaxThrdCount(this->m_MaxThrdCount);
	// 乱数表示用スレッド実行フラグ
	this->SetExecAllFlag(TRUE);
	// ストレススレッド監視用スレッド起動
	this->hMultiThreadMgr = new std::thread(
		SCPU::MultiThreadMgr,
		this
	);
	// 基準優先度を優先度クラスより2だけ低く設定
	const int thread_pri = THREAD_PRIORITY_LOWEST;
	SetThreadPriority(
		this->hMultiThreadMgr->native_handle(),
		thread_pri
	);
	// PCの最大のスレッド数分のスレッドを休止で実行
	const int max_count = this->GetMaxThrdCount();
	for (int ii = 0; ii < max_count; ii++)
	{
		// 非同期スレッドを起動
		this->hmultiThrd->ExecThread(ii);
		// スレッド状態をスリープ状態に設定
		this->hmultiThrd->CtrlThread(
			ii,
			MT::e_Status::st_sleep
		);
	}
	// カレントディレクトリ設定
	this->m_CurrentPath = fs::current_path();
}
void CStressCPUDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BU_ALL_EXEC, m_BU_ALL_Exec);
	DDX_Control(pDX, IDC_BU_ALL_SLEEP, m_BU_AllSleep);
	DDX_Control(pDX, IDC_BU_ALL_WAKE, m_BU_AllWake);
	DDX_Control(pDX, IDC_ST_GUID01, m_ST_Guid01);
	DDX_Control(pDX, IDC_ST_GUID02, m_ST_Guid02);
	DDX_Control(pDX, IDC_ST_GUID03, m_ST_Guid03);
	DDX_Control(pDX, IDC_ST_GUID04, m_ST_Guid04);
	DDX_Control(pDX, IDC_ST_GUID05, m_ST_Guid05);
	DDX_Control(pDX, IDC_ST_GUID06, m_ST_Guid06);
	DDX_Control(pDX, IDC_ST_GUID07, m_ST_Guid07);
	DDX_Control(pDX, IDC_ST_GUID08, m_ST_Guid08);
	DDX_Control(pDX, IDC_ST_GUID09, m_ST_Guid09);
	DDX_Control(pDX, IDC_ST_GUID10, m_ST_Guid10);
	DDX_Control(pDX, IDC_ST_GUID11, m_ST_Guid11);
	DDX_Control(pDX, IDC_ST_GUID12, m_ST_Guid12);
	DDX_Control(pDX, IDC_ST_GUID13, m_ST_Guid13);
	DDX_Control(pDX, IDC_ST_GUID14, m_ST_Guid14);
	DDX_Control(pDX, IDC_ST_GUID15, m_ST_Guid15);
	DDX_Control(pDX, IDC_ST_GUID16, m_ST_Guid16);
	DDX_Control(pDX, IDC_CH_IDLE01, m_CH_Idle01);
	DDX_Control(pDX, IDC_CH_IDLE02, m_CH_Idle02);
	DDX_Control(pDX, IDC_CH_IDLE03, m_CH_Idle03);
	DDX_Control(pDX, IDC_CH_IDLE04, m_CH_Idle04);
	DDX_Control(pDX, IDC_CH_IDLE05, m_CH_Idle05);
	DDX_Control(pDX, IDC_CH_IDLE06, m_CH_Idle06);
	DDX_Control(pDX, IDC_CH_IDLE07, m_CH_Idle07);
	DDX_Control(pDX, IDC_CH_IDLE08, m_CH_Idle08);
	DDX_Control(pDX, IDC_CH_IDLE09, m_CH_Idle09);
	DDX_Control(pDX, IDC_CH_IDLE10, m_CH_Idle10);
	DDX_Control(pDX, IDC_CH_IDLE11, m_CH_Idle11);
	DDX_Control(pDX, IDC_CH_IDLE12, m_CH_Idle12);
	DDX_Control(pDX, IDC_CH_IDLE13, m_CH_Idle13);
	DDX_Control(pDX, IDC_CH_IDLE14, m_CH_Idle14);
	DDX_Control(pDX, IDC_CH_IDLE15, m_CH_Idle15);
	DDX_Control(pDX, IDC_CH_IDLE16, m_CH_Idle16);
	DDX_Control(pDX, IDC_BU_XOR, m_BU_XorThrd);
	DDX_Control(pDX, IDC_ST_CPUCNT, m_ST_CpuCnt);
	DDX_Control(pDX, IDC_ST_CPU_BRAND, m_ST_CpuBrand);
	DDX_Control(pDX, IDC_ST_CPUVEN, m_ST_CpuVen);
	DDX_Control(pDX, IDC_ST_THRD_STAT, m_ST_ThrdStat);
	DDX_Control(pDX, IDC_COM_THREAD, m_COM_Thread);
	DDX_Control(pDX, IDC_ST_FLOPS, m_ST_Flops);
	DDX_Control(pDX, IDC_ST_FPUNIT, m_ST_FPUNIT);
	DDX_Control(pDX, IDC_ST_FLOPS_MAX, m_ST_FlopsMAX);
	DDX_Control(pDX, IDC_ST_FLOPS_MIN, m_ST_FlopsMIN);
	DDX_Control(pDX, IDC_ST_FPUNIT_MAX, m_ST_FPUNIT_MAX);
	DDX_Control(pDX, IDC_ST_FPUNIT_MIN, m_ST_FPUNIT_MIN);
	DDX_Control(pDX, IDC_PROGRESS1, Prgb01);
	DDX_Control(pDX, IDC_PROGRESS2, Prgb02);
	DDX_Control(pDX, IDC_PROGRESS3, Prgb03);
	DDX_Control(pDX, IDC_PROGRESS4, Prgb04);
	DDX_Control(pDX, IDC_PROGRESS5, Prgb05);
	DDX_Control(pDX, IDC_PROGRESS6, Prgb06);
	DDX_Control(pDX, IDC_PROGRESS7, Prgb07);
	DDX_Control(pDX, IDC_PROGRESS8, Prgb08);
	DDX_Control(pDX, IDC_PROGRESS9, Prgb09);
	DDX_Control(pDX, IDC_PROGRESS10, Prgb10);
	DDX_Control(pDX, IDC_PROGRESS11, Prgb11);
	DDX_Control(pDX, IDC_PROGRESS12, Prgb12);
	DDX_Control(pDX, IDC_PROGRESS13, Prgb13);
	DDX_Control(pDX, IDC_PROGRESS14, Prgb14);
	DDX_Control(pDX, IDC_PROGRESS15, Prgb15);
	DDX_Control(pDX, IDC_PROGRESS16, Prgb16);
	DDX_Control(pDX, IDC_CHK_PRGBAR, chk_PrgBar);
	DDX_Control(pDX, IDC_CHK_RNDVAL, chk_RndVal);
	DDX_Control(pDX, IDC_CPU_IMG, m_CPU_IMG);
	DDX_Control(pDX, ID_RESET, m_Btn_Reset);
	DDX_Control(pDX, IDC_ST_CPUCNT2, m_ST_LProcCnt);
	DDX_Control(pDX, IDC_FLPS_PROGRESS01, m_FLPSPRG01);
	DDX_Control(pDX, IDC_FLPS_PROGRESS02, m_FLPSPRG02);
	DDX_Control(pDX, IDC_FLPS_PROGRESS03, m_FLPSPRG03);
	DDX_Control(pDX, IDC_CHK_FLOPSMAX, m_Chk_FlopsMAX);
	DDX_Control(pDX, IDC_EDT_FLOPSMAX, m_EDT_FlopsMAX);
}

BEGIN_MESSAGE_MAP(CStressCPUDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BU_ALL_SLEEP, &CStressCPUDlg::OnClickedBuAllSleep)
	ON_BN_CLICKED(IDC_BU_ALL_EXEC, &CStressCPUDlg::OnClickedBuAllExec)
	ON_BN_CLICKED(IDC_CH_IDLE01, &CStressCPUDlg::OnClickedChIdle01)
	ON_BN_CLICKED(IDC_CH_IDLE02, &CStressCPUDlg::OnClickedChIdle02)
	ON_BN_CLICKED(IDC_CH_IDLE03, &CStressCPUDlg::OnClickedChIdle03)
	ON_BN_CLICKED(IDC_CH_IDLE04, &CStressCPUDlg::OnClickedChIdle04)
	ON_BN_CLICKED(IDC_CH_IDLE05, &CStressCPUDlg::OnClickedChIdle05)
	ON_BN_CLICKED(IDC_CH_IDLE06, &CStressCPUDlg::OnClickedChIdle06)
	ON_BN_CLICKED(IDC_CH_IDLE07, &CStressCPUDlg::OnClickedChIdle07)
	ON_BN_CLICKED(IDC_CH_IDLE08, &CStressCPUDlg::OnClickedChIdle08)
	ON_BN_CLICKED(IDC_CH_IDLE09, &CStressCPUDlg::OnClickedChIdle09)
	ON_BN_CLICKED(IDC_CH_IDLE10, &CStressCPUDlg::OnClickedChIdle10)
	ON_BN_CLICKED(IDC_CH_IDLE11, &CStressCPUDlg::OnClickedChIdle11)
	ON_BN_CLICKED(IDC_CH_IDLE12, &CStressCPUDlg::OnClickedChIdle12)
	ON_BN_CLICKED(IDC_CH_IDLE13, &CStressCPUDlg::OnClickedChIdle13)
	ON_BN_CLICKED(IDC_CH_IDLE14, &CStressCPUDlg::OnClickedChIdle14)
	ON_BN_CLICKED(IDC_CH_IDLE15, &CStressCPUDlg::OnClickedChIdle15)
	ON_BN_CLICKED(IDC_CH_IDLE16, &CStressCPUDlg::OnClickedChIdle16)
	ON_BN_CLICKED(IDC_BU_ALL_WAKE, &CStressCPUDlg::OnClickedBuAllWake)
	ON_BN_CLICKED(IDC_BU_XOR, &CStressCPUDlg::OnClickedBuXorThread)
	ON_BN_CLICKED(IDCANCEL, &CStressCPUDlg::OnBnClickedEnding)
	ON_CBN_SELCHANGE(IDC_COM_THREAD, &CStressCPUDlg::OnSelchangeComThread)
	ON_BN_CLICKED(IDC_BU_VERSION, &CStressCPUDlg::OnClickedBuVersion)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHK_PRGBAR, &CStressCPUDlg::OnClickedChkPrgbar)
	ON_BN_CLICKED(IDC_CHK_RNDVAL, &CStressCPUDlg::OnClickedChkRndval)
	ON_BN_CLICKED(ID_RESET, &CStressCPUDlg::OnClickedBuReset)
	ON_EN_CHANGE(IDC_EDT_FLOPSMAX, &CStressCPUDlg::OnChangeEdtFlopsmax)
	ON_BN_CLICKED(IDC_CHK_FLOPSMAX, &CStressCPUDlg::OnClickedChkFlopsmax)
END_MESSAGE_MAP()

// CStressCPUDlg メッセージ ハンドラー

BOOL CStressCPUDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// スレッド乱数表示欄のコントロールをベクター登録
	this->m_StVector.push_back(&this->m_ST_Guid01);
	this->m_StVector.push_back(&this->m_ST_Guid02);
	this->m_StVector.push_back(&this->m_ST_Guid03);
	this->m_StVector.push_back(&this->m_ST_Guid04);
	this->m_StVector.push_back(&this->m_ST_Guid05);
	this->m_StVector.push_back(&this->m_ST_Guid06);
	this->m_StVector.push_back(&this->m_ST_Guid07);
	this->m_StVector.push_back(&this->m_ST_Guid08);
	this->m_StVector.push_back(&this->m_ST_Guid09);
	this->m_StVector.push_back(&this->m_ST_Guid10);
	this->m_StVector.push_back(&this->m_ST_Guid11);
	this->m_StVector.push_back(&this->m_ST_Guid12);
	this->m_StVector.push_back(&this->m_ST_Guid13);
	this->m_StVector.push_back(&this->m_ST_Guid14);
	this->m_StVector.push_back(&this->m_ST_Guid15);
	this->m_StVector.push_back(&this->m_ST_Guid16);
	// FLOPS値リアル値のプログレスバー
	this->Flops_RealBar = new PrgbCtrl(&this->m_FLPSPRG01);
	// FLOPS値MAX値のプログレスバー
	this->Flops_MAXBar = new PrgbCtrl(&this->m_FLPSPRG02);
	// FLOPS値MIN値のプログレスバー
	this->Flops_MINBar = new PrgbCtrl(&this->m_FLPSPRG03);
	// MAX値FLOPSプログレスバーレンジ設定
	this->Flops_MAXBar->SetFlopsRange(100);
	// プログレスバーコントロールベクターの配列を生成
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb01));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb02));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb03));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb04));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb05));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb06));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb07));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb08));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb09));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb10));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb11));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb12));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb13));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb14));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb15));
	this->PrgBarCtl.push_back(PrgbCtrl(&this->Prgb16));
	// プログレスバー更新チェックボックスON
	this->chk_PrgBar.SetCheck(TRUE);
	// スレッド毎の乱数表示チェックボックスON
	this->chk_RndVal.SetCheck(TRUE);
	// ストレス対象数選択リスト初期化
	int ii = 1;
	for (const auto& stvec : this->m_StVector)
	{
		if (ii > this->m_MaxThrdCount)
		{
			break;
		}
		CString var;
		var.Format("%02d", ii);
		this->m_COM_Thread.AddString(var.GetString());
		ii++;
	}
	this->m_COM_Thread.SetCurSel(this->m_MaxThrdCount - 1);
	MT::SetStressCtrl(D_INNER);
	// 一括待機を無効
	this->m_BU_AllSleep.EnableWindow(FALSE);
	// 一括復帰を無効
	this->m_BU_AllWake.EnableWindow(FALSE);
	// 待機／復帰反転を無効
	this->m_BU_XorThrd.EnableWindow(FALSE);
	// 初期一括実行
	this->m_BU_ALL_Exec.EnableWindow(TRUE);
	// リセットボタンを非活性
	this->m_Btn_Reset.EnableWindow(FALSE);
	// 待機ボタンコントロールをベクター初期化
	this->m_CH_Idles.push_back(&this->m_CH_Idle01);
	this->m_CH_Idles.push_back(&this->m_CH_Idle02);
	this->m_CH_Idles.push_back(&this->m_CH_Idle03);
	this->m_CH_Idles.push_back(&this->m_CH_Idle04);
	this->m_CH_Idles.push_back(&this->m_CH_Idle05);
	this->m_CH_Idles.push_back(&this->m_CH_Idle06);
	this->m_CH_Idles.push_back(&this->m_CH_Idle07);
	this->m_CH_Idles.push_back(&this->m_CH_Idle08);
	this->m_CH_Idles.push_back(&this->m_CH_Idle09);
	this->m_CH_Idles.push_back(&this->m_CH_Idle10);
	this->m_CH_Idles.push_back(&this->m_CH_Idle11);
	this->m_CH_Idles.push_back(&this->m_CH_Idle12);
	this->m_CH_Idles.push_back(&this->m_CH_Idle13);
	this->m_CH_Idles.push_back(&this->m_CH_Idle14);
	this->m_CH_Idles.push_back(&this->m_CH_Idle15);
	this->m_CH_Idles.push_back(&this->m_CH_Idle16);
	// CPUIDベンダー表示
	this->cpu_vender = this->m_cpuinfo.GetVendor().c_str();
	this->m_ST_CpuVen.SetWindowTextA(this->cpu_vender.c_str());
	// CPUIDブランド表示
	CString tmpBrnd = this->m_cpuinfo.GetBrand().c_str();
	this->m_ST_CpuBrand.SetWindowTextA(tmpBrnd);
	// コア数表示
	CString tmpCpu = "";
	tmpCpu.Format("%2d", 
		this->m_cpuinfo.GetPhysCoreCnt()
	);
	this->m_ST_CpuCnt.SetWindowTextA(tmpCpu);
	// 論理プロセッサ数表示
	CString logPrc = "";
	logPrc.Format("%02d",
		this->m_cpuinfo.GetLogicProcessorCnt()
	);
	this->m_ST_LProcCnt.SetWindowTextA(logPrc);
	// FLOPS値MAXの初期値
	this->m_EDT_FlopsMAX.SetWindowTextA("1000");
	// スレッド乱数表示初期化
	for (auto var : this->m_StVector)
	{
		var->SetWindowTextA(this->cpu_vender.c_str());
	}
	// CPU画像ピクチャーコントロールの幅と高さを取得
	this->m_CPU_IMG.GetWindowRect(&this->m_cpu_rect);
	// 各コントロールのツールチップ設定
	this->SetCtrlToolTips();
	// スレッド監視プロセスは稼働中
	SCPU::SetThreadEnd(false);
	// スレッド状態表示起動
	static std::thread* StatThrdHandler =
		new std::thread(
			SCPU::ThreadStatMsg,
			this
		);
	// 基準優先度を優先度クラスより2だけ低く設定
	const int thread_pri = THREAD_PRIORITY_LOWEST;
	SetThreadPriority(
		StatThrdHandler->native_handle(),
		thread_pri
	);
	/// 各スレッドの計算処理時間の平均値を収集（時間[nano]）
	/// 0.5秒毎に収集し、平均値を取得する
	this->FlpsGetCollectCalculationVolume();
	// FLOPS表示スレッド
	// 0.5秒毎に更新
	// 全スレッドが終了するまで無限にループ
	this->FlpsDispValue();

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}
/// <summary>
/// 各コントロールのツールチップを設定
/// </summary>
/// <param name=""></param>
void CStressCPUDlg::SetCtrlToolTips(void)
{
	//////////////////////////////////////////////////////
	// ツールチップ設定
	//////////////////////////////////////////////////////
	this->m_toolTip.Create(
		this,
		TTS_ALWAYSTIP | TTS_BALLOON
	);
	// 最大スレッド数選択
	this->SetAddToolTips(&this->m_COM_Thread, "負荷を掛ける最大スレッド数を選択して下さい。");
	// スレッド準備ボタン
	this->SetAddToolTips(&this->m_BU_ALL_Exec, "このボタンでスレッド起動の準備をします。");
	// 全実行
	this->SetAddToolTips(&this->m_BU_AllWake, "全てのスレッドに負荷を掛けます。");
	// 全休止
	this->SetAddToolTips(&this->m_BU_AllSleep, "全てのスレッドを休止状態にします。");
	// 休止/実行反転
	this->SetAddToolTips(&this->m_BU_XorThrd, "全てのスレッドの実行／休止状態を反転します。");
	// リセットボタン
	this->SetAddToolTips(&this->m_Btn_Reset, "全てのスレッドを停止し、スレッド数を再選択可能にします。");
	// FLOPS値MAX入力欄
	this->SetAddToolTips(&this->m_EDT_FlopsMAX, "FLOPS値のMAXを指定出来ます。(100以上10000以下)");
	// FLOPS値MAXをプログレスバーに反映
	this->SetAddToolTips(&this->m_Chk_FlopsMAX, "FLOPS値のMAXをプログレスバーに反映ON/OFFします。");
	// 実行チェックボックス
	for (const auto& var : this->m_CH_Idles)
	{
		this->SetAddToolTips(var, "スレッドの負荷／休止を切り替える");
	}
}
/// <summary>
/// 各コントロールのツールチップを追加
/// </summary>
/// <param name="lpCtrl">コントロールのポインタ</param>
/// <param name="tipsmsg">ツールチップ</param>
void CStressCPUDlg::SetAddToolTips(
	CWnd* lpCtrl,
	LPCTSTR tipsmsg
)
{
	// コントロールにツールチップを追加登録
	this->m_toolTip.AddTool(lpCtrl, tipsmsg);
	// コントロールのハンドルを登録
	this->m_CtrlMap[(*lpCtrl).m_hWnd] = 1;
}
// ツールチップ表示イベント
// 仮想関数を追加で作成
BOOL CStressCPUDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (pMsg->message == WM_MOUSEMOVE)
	{		
		if (this->m_CtrlMap[pMsg->hwnd] == 1)
		{
			this->m_toolTip.RelayEvent(pMsg);
		}
		else
		{
			this->m_toolTip.Pop();
		}
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CStressCPUDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CStressCPUDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
////////////////////////////////////////////
// スレッド準備
void CStressCPUDlg::OnClickedBuAllExec()
{
	// リセットボタンを活性
	this->m_Btn_Reset.EnableWindow(TRUE);
	// プルダウンメニューで選択したスレッド数
	int max_count = this->GetMaxThrdCount();
	// 各スレッドの実行OFF
	int ii = 0;
	for (auto& var : this->m_CH_Idles)
	{
		// スレッド数以内のみ有効にする
		if (ii < max_count)
		{
			var->EnableWindow(TRUE);
		}
		else
		{
			break;
		}
		ii++;
	}
	// 自身を無効化する
	this->m_BU_ALL_Exec.EnableWindow(FALSE);
	// 最大スレッド数選択無効化
	this->m_COM_Thread.EnableWindow(FALSE);
	// 一括休止ボタンを有効
	this->SetBU_AllSleep(TRUE);
	// 一括実行ボタンを有効
	this->SetBU_AllWake(TRUE);
	// 休止／実行反転ボタンを有効
	this->SetBU_XorThrd(TRUE);
	// 一括実行フラグON
	this->SetExecAllFlag(TRUE);
	// CPU画像表示
	this->setPictureControl(this->m_cpuinfo);
}
// 一括休止ボタン
void CStressCPUDlg::OnClickedBuAllSleep()
{
	// スレッド稼働フラグOFF
	this->SetInitExec(false);
	// スレッドチェックボックス休止・有効
	this->SetThrdCheckCtrl(
		MT::e_Status::st_sleep,
		FALSE,
		TRUE
	);
	// 現在実行中スレッドのカウントをゼロにする
	this->hmultiThrd->ZeroCounter();
	// 一定時間クリックイベントを無効にする
	this->WaitActiveCtrl(&this->m_BU_AllSleep);
}
// 一括実行ボタン
void CStressCPUDlg::OnClickedBuAllWake()
{
	// スレッド稼働フラグON
	this->SetInitExec(true);
	// スレッドチェックボックス実行中・有効
	this->SetThrdCheckCtrl(
		MT::e_Status::st_running,
		TRUE,
		TRUE
	);
	// 一括実行フラグON
	this->SetExecAllFlag(TRUE);
	// 現在実行中スレッドのカウントを設定する
	this->hmultiThrd->SetUpdateCounter(this->GetMaxThrdCount());
	// 一定時間クリックベントを無効にする
	this->WaitActiveCtrl(&this->m_BU_AllWake);
}
// 休止／実行反転
void CStressCPUDlg::OnClickedBuXorThread()
{
	static std::thread* thrHnd = nullptr;
	int max_count = this->GetMaxThrdCount(); //this->m_MaxThrdCount;
	BOOL chkbx_sw;
	for (int ii = 0; ii < max_count; ii++)
	{
		// 現在のスレッド状態を反転
		// MT::e_Status::st_sleep <---> MT::e_Status::st_running
		auto st = this->hmultiThrd->GetReverse(ii);
		this->hmultiThrd->CtrlThread(
			ii,
			st
		);
		// 各待機チェックをOFF
		// $1=Index, $2=sleep/running(FALSE/TRUE), $3=Enable(TRUE/FALSE)
		if (st == MT::e_Status::st_sleep)
		{
			chkbx_sw = FALSE;
			// スレッド数減算
			this->hmultiThrd->MinusCounter();		
		}
		else
		{
			chkbx_sw = TRUE;
			// スレッド数加算
			this->hmultiThrd->PlusCounter();
		}
		// 待機／実行チェックボックス切替
		this->SetCH_Idles(ii, chkbx_sw, TRUE);
	}
	// wait
	std::this_thread::sleep_for(std::chrono::microseconds(17));
	// スレッド停止・起動表示切替
	if (this->hmultiThrd->GetUpdateCounter() == 0)
	{
		// スレッドは停止中
		this->SetInitExec(false);
	}
	else
	{
		// スレッドは実行中
		this->SetInitExec(true);
	}
	// 一定時間クリックベントを無効にする
	this->WaitActiveCtrl(&this->m_BU_XorThrd);
}
// プログレスバー実行
void CStressCPUDlg::PrgBarRun(int idx)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::g_mtx_[idx]);
	this->PrgBarCtl[idx].Start();
}
// プログレスバー終了
void CStressCPUDlg::PrgBarEnd(int idx)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::g_mtx_[idx]);
	this->PrgBarCtl[idx].End();
}
// プログレスバー更新
void CStressCPUDlg::PrgBarUpdate(int idx)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::g_mtx_[idx]);
	// プログレスバー更新
	auto perc_val = this->PrgBarCtl[idx].Inc();
	// バーの色変更
	this->PrgBarCtl[idx].SetBarB2RColor(perc_val);
}
/// <summary>
/// SetCH_Idles
/// 待機チェックのON/OFFと有効/無効
/// </summary>
/// <param name="idx">スレッドID</param>
/// <param name="chk">チェックON/OFF</param>
/// <param name="enbl">チェックボックス有効/無効</param>
void CStressCPUDlg::SetCH_Idles(int idx, BOOL chk, BOOL enbl)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::g_mtx_[idx]);
	this->m_CH_Idles[idx]->SetCheck(chk);
	this->m_CH_Idles[idx]->EnableWindow(enbl);
}
// 一括復帰の有効／無効
void CStressCPUDlg::SetBU_AllWake(BOOL enbl)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_BU_scpu);
	this->m_BU_AllWake.EnableWindow(enbl);
}
// 一括待機チェックの有効／無効
void CStressCPUDlg::SetBU_AllSleep(BOOL enbl)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_BU_scpu);
	this->m_BU_AllSleep.EnableWindow(enbl);
}
// 待機／復帰反転ボタンの有効／無効
void CStressCPUDlg::SetBU_XorThrd(BOOL enbl)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_BU_scpu);
	this->m_BU_XorThrd.EnableWindow(enbl);
}
// スレッド乱数ベクター設定
void CStressCPUDlg::SetStVector(int idx, const char* strRnd)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::g_mtx_[idx]);
	this->m_StVector[idx]->SetWindowTextA(strRnd);
}
// 最大対象スレッド数取得
int CStressCPUDlg::GetMaxThrdCount()
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_scpu_MaxThrdCount);
	return this->m_MaxThrdCount;
}
/// <summary>
/// 最大対象スレッド数設定
/// </summary>
/// <param name="thrdCnt">最大スレッド数</param>
void CStressCPUDlg::SetMaxThrdCount(int thrdCnt)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_scpu_MaxThrdCount);
	this->m_MaxThrdCount = thrdCnt;
}
/// <summary>
/// SwitchThread
/// スレッド実行／休止の切替メソッド
/// </summary>
/// <param name="tID">スレッドID</param>
void CStressCPUDlg::SwitchThread(int tID)
{
	// 休止設定
	if (this->m_CH_Idles[tID]->GetCheck() == FALSE)
	{
		this->hmultiThrd->CtrlThread(
			tID,
			MT::e_Status::st_sleep
		);
		// スレッド数減算
		this->hmultiThrd->MinusCounter();
	}
	// 実行設定
	else
	{
		this->hmultiThrd->CtrlThread(
			tID,
			MT::e_Status::st_running
		);
		// スレッド数加算
		this->hmultiThrd->PlusCounter();
		// スレッド監視ON
		this->SetExecAllFlag(TRUE);
	}
	// wait
	std::this_thread::sleep_for(std::chrono::microseconds(23));
	if (this->hmultiThrd->GetUpdateCounter() == 0)
	{
		// スレッド停止状態
		this->SetInitExec(false);
	}
	else
	{
		// スレッド実行中状態
		this->SetInitExec(true);
	}
	// 一定時間クリックベントを無効にする
	this->WaitActiveCtrl(this->m_CH_Idles[tID]);
}
#pragma region スレッド毎の実行ボタン
// スレッド毎の実行ボタン 01
void CStressCPUDlg::OnClickedChIdle01()
{
	int st = 0;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 02
void CStressCPUDlg::OnClickedChIdle02()
{
	int st = 1;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 03
void CStressCPUDlg::OnClickedChIdle03()
{
	int st = 2;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 04
void CStressCPUDlg::OnClickedChIdle04()
{
	int st = 3;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 05
void CStressCPUDlg::OnClickedChIdle05()
{
	int st = 4;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 06
void CStressCPUDlg::OnClickedChIdle06()
{
	int st = 5;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 07
void CStressCPUDlg::OnClickedChIdle07()
{
	int st = 6;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 08
void CStressCPUDlg::OnClickedChIdle08()
{
	int st = 7;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 09
void CStressCPUDlg::OnClickedChIdle09()
{
	int st = 8;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 10
void CStressCPUDlg::OnClickedChIdle10()
{
	int st = 9;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 11
void CStressCPUDlg::OnClickedChIdle11()
{
	int st = 10;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 12
void CStressCPUDlg::OnClickedChIdle12()
{
	int st = 11;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 13
void CStressCPUDlg::OnClickedChIdle13()
{
	int st = 12;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 14
void CStressCPUDlg::OnClickedChIdle14()
{
	int st = 13;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 15
void CStressCPUDlg::OnClickedChIdle15()
{
	int st = 14;
	SwitchThread(st);
}
// スレッド毎の実行ボタン 16
void CStressCPUDlg::OnClickedChIdle16()
{
	int st = 15;
	SwitchThread(st);
}
#pragma endregion
// 一括実行フラグ取得
BOOL CStressCPUDlg::GetExecAllFlag()
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_ExecAllFlag);
	// 一括実行フラグ
	return this->m_ExecAllFlag;
}
// 一括実行フラグ設定
void CStressCPUDlg::SetExecAllFlag(BOOL var)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_ExecAllFlag);
	// 一括実行フラグ
	this->m_ExecAllFlag = var;
}
// スレッド状態表示設定
void CStressCPUDlg::SetST_ThreadStatus(CString stmsg)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_scpu);
	this->m_ST_ThrdStat.SetWindowTextA(stmsg);
}
/// <summary>
/// エンディング処理
/// </summary>
/// すべてのスレッドを停止して閉じる
void CStressCPUDlg::OnBnClickedEnding()
{
	// 全スレッド終了設定
	// スレッドチェックボックス休止・無効
	this->SetThrdCheckCtrl(
		MT::e_Status::st_sleep,
		FALSE,
		FALSE
	);
	// 現在実行中スレッドのカウントをゼロにする
	this->hmultiThrd->ZeroCounter();
	// スレッド停止
	this->SetInitExec(false);
	// 終了メッセージ
	auto CEnd = new CDialogEnding();
	CEnd->DoModal();
	// 本体操作ダイアログを閉じる
	CDialogEx::OnCancel();
}
// 最大スレッド数選択コンボボックスのイベント処理
void CStressCPUDlg::OnSelchangeComThread()
{
	// 選択された値を取得
	auto selvar = this->m_COM_Thread.GetCurSel() + 1;
	//
	//this->WaitActiveCtrl(&this->m_COM_Thread);
	// 最大対象スレッド数設定
	this->SetMaxThrdCount(selvar);
}
// バージョン情報表示
void CStressCPUDlg::OnClickedBuVersion()
{
	auto Cver = new CVersionDlg();
	Cver->SetWarnFlgOFF();
	Cver->DoModal();
}
// ストレス処理時間平均値設定
void CStressCPUDlg::SetFlpsCalTimerEven(double var)
{
	// スレッド計算速度情報収集の排他オブジェクト
	std::lock_guard<std::mutex> lock(SCPU::mtx_flps);
	this->FlpsCalTimerEven = var;
}
// ストレス処理時間平均値取得
double CStressCPUDlg::GetFlpsCalTimerEven(void)
{
	// スレッド計算速度情報収集の排他オブジェクト
	std::lock_guard<std::mutex> lock(SCPU::mtx_flps);
	return this->FlpsCalTimerEven;
}
/// 各スレッドの計算処理時間の平均値を収集（時間[nano]）
/// 0.5秒毎に収集し、平均値を取得する
void CStressCPUDlg::FlpsGetCollectCalculationVolume(void)
{
	this->hFlpsPerThread = new std::thread(
		SCPU::FlopsMultiThreadCollectCalculationVolume,
		this
	);
	// 基準優先度を優先度クラスより2だけ低く設定
	const int thread_pri = THREAD_PRIORITY_LOWEST;
	SetThreadPriority(
		this->hFlpsPerThread->native_handle(),
		thread_pri
	);
}
/// FLOPS表示スレッド起動
/// 全スレッドが終了するまで無限にループ
void CStressCPUDlg::FlpsDispValue(void)
{
	// FLOPS表示スレッド
	static std::thread* hThread = nullptr;
	hThread = new std::thread(
		SCPU::FlopsDispValueThread,
		this
	);
	// 基準優先度を優先度クラスより2だけ低く設定
	const int thread_pri = THREAD_PRIORITY_LOWEST;
	SetThreadPriority(
		hThread->native_handle(),
		thread_pri
	);
}
// FLOPS表示
void CStressCPUDlg::SetSTFlops(
	CString stvar,
	CStatic& cst
)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_Flops);
	cst.SetWindowTextA(stvar);
}
// 倍精度演算消費時間サンプリング実行フラグ設定
void CStressCPUDlg::SetFlpsSamplingFlg(bool var)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_flpssmpflg);
	this->m_FlpsSamplingFlg = var;
}
// 倍精度演算消費時間サンプリング実行フラグ取得
bool CStressCPUDlg::GetFlpsSamplingFlg(void) const
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_flpssmpflg);
	return this->m_FlpsSamplingFlg;
}

// コントロールの色を変更する
//スレッドの状態表示色を変える
HBRUSH CStressCPUDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: ここで DC の属性を変更してください。
	BYTE BG_rcolor = 0xff;
	BYTE BG_gcolor = 0xff;
	BYTE BG_bcolor = 0xff;

	BYTE FG_rcolor = 0;
	BYTE FG_gcolor = 0;
	BYTE FG_bcolor = 0;
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC: // スタティックテキストコントロール
		switch (pWnd->GetDlgCtrlID()) // 該当ID特定
		{
		case IDC_ST_THRD_STAT: // スレッド準備状態
			//スレッドは停止中
			if (this->GetInitExec() == false)
			{
				// 停止中
				// HotPink
				FG_rcolor = 0xff;
				FG_gcolor = 0x69;
				FG_bcolor = 0xb4;
			}
			else
			{
				// 起動可能
				// Blue
				FG_bcolor = 0xff;
			}
			// テキストカラー
			pDC->SetTextColor(RGB(FG_rcolor, FG_gcolor, FG_bcolor));
			// 背景色
			pDC->SetBkColor(RGB(BG_rcolor, BG_gcolor, BG_bcolor));
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	// TODO: 既定値を使用したくない場合は別のブラシを返します。
	return hbr;
}
// プログレスバー更新フラグ設定
void CStressCPUDlg::SetPrgBarUpdateFlg(BOOL bval)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_prgbar);
	this->chk_PrgBar.SetCheck(bval);
}
// プログレスバー更新フラグ取得
BOOL CStressCPUDlg::GetPrgBarUpdateFlg(void)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_prgbar);
	return this->chk_PrgBar.GetCheck();
}
// スレッド毎の乱数表示フラグ設定
void CStressCPUDlg::SetThreadRndDispFlg(BOOL bval)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_rnddisp);
	this->chk_RndVal.SetCheck(bval);
}
// スレッド毎の乱数表示フラグ取得
BOOL CStressCPUDlg::GetThreadRndDispFlg(void)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_rnddisp);
	return this->chk_RndVal.GetCheck();
}
// プログレスバー更新の実行・停止
void CStressCPUDlg::OnClickedChkPrgbar()
{
	// 更新実行
	if (this->chk_PrgBar.GetCheck() == TRUE)
	{
		this->SetPrgBarUpdateFlg(TRUE);
	}
	// 更新停止
	else
	{
		this->SetPrgBarUpdateFlg(FALSE);
	}
	// 一定時間クリックベントを無効にする
	this->WaitActiveCtrl(&this->chk_PrgBar);
}
// スレッド毎の乱数表示の実行・停止
void CStressCPUDlg::OnClickedChkRndval()
{
	// 表示実行
	if (this->chk_RndVal.GetCheck() == TRUE)
	{
		this->SetThreadRndDispFlg(TRUE);
	}
	// 表示停止
	else
	{
		this->SetThreadRndDispFlg(FALSE);
	}
	// 一定時間クリックベントを無効にする
	this->WaitActiveCtrl(&this->chk_RndVal);
}
/// <summary>
/// ピクチャーコントロールへの描画処理
/// </summary>
/// <param name="cpu_vender">CPUベンダー情報</param>
void CStressCPUDlg::setPictureControl(CPUID& cpuid)
{
	CDC* cdc;
	int iWidth, iHeight;
	//IDC_PICTURE2にファイルを描画
	cdc = this->m_CPU_IMG.GetDC();
	iWidth = m_cpu_rect.right - m_cpu_rect.left;
	iHeight = m_cpu_rect.bottom - m_cpu_rect.top;
	//読み込む画像ファイルパス選択
	CString cs;
	fs::path CPU_IMG;
	if (cpuid.IsAMD() == true)
	{
		CPU_IMG = this->m_amd_cpu_file;
	}
	else if (cpuid.IsIntel() == true)
	{
		CPU_IMG = this->m_intel_cpu_file;
	}
	else
	{
		CPU_IMG = this->m_intel_cpu_file;
	}
	cs = (this->m_CurrentPath / "res" / CPU_IMG).c_str();
	CImage img;
	HRESULT ret = img.Load(cs);
	if (img.IsNull()) {
		CString msg;
		msg.Format(
			"画像の読み込み失敗：%sが見つかりません。\n"
			"~StressCPU2/resフォルダを本体プログラムと同じフォルダにコピーして下さい。",
			cs.GetString()
		);
		::MessageBox(
			this->GetSafeHwnd(),
			msg.GetString(),
			"CPU画像読み込み処理",
			MB_ICONERROR
		);
		return;
	}
	//StretchBltで描画領域に合わせてリサイズ
	CDC bmpDC;
	CBitmap* cbmp;
	cbmp = CBitmap::FromHandle(img);
	bmpDC.CreateCompatibleDC(cdc);
	CBitmap* oldbmp = bmpDC.SelectObject(cbmp);
	//伸縮すると画像が汚くなるので伸縮モードを指定
	//詳細はMSDN参照
	cdc->SetStretchBltMode(STRETCH_HALFTONE);
	//ブラシのずれを防止するためSetBrushOrgExを呼び出す
	cdc->SetBrushOrg(0, 0);
	//画像を伸縮してピクチャーボックスに表示
	//詳細はMSDN参照
	cdc->StretchBlt(0, 0, iWidth, iHeight, &bmpDC, 0, 0, img.GetWidth(), img.GetHeight(), SRCCOPY);
	bmpDC.SelectObject(oldbmp);
	//後片付け
	cbmp->DeleteObject();
	bmpDC.DeleteDC();
	ReleaseDC(cdc);
}
// リセットボタン
// 全てのスレッドを停止、MAXスレッドを再選択可能
void CStressCPUDlg::OnClickedBuReset()
{
	// 全てのスレッドを休止・無効
	this->SetThrdCheckCtrl(
		MT::e_Status::st_sleep,
		FALSE,
		FALSE
	);
	// 現在実行中スレッドのカウントをゼロにする
	this->hmultiThrd->ZeroCounter();
	// 最大スレッド数選択無効化
	this->m_COM_Thread.EnableWindow(TRUE);
	// 一括待機を無効
	this->m_BU_AllSleep.EnableWindow(FALSE);
	// 一括復帰を無効
	this->m_BU_AllWake.EnableWindow(FALSE);
	// 待機／復帰反転を無効
	this->m_BU_XorThrd.EnableWindow(FALSE);
	// スレッド準備ボタンを有効
	this->m_BU_ALL_Exec.EnableWindow(TRUE);
	// スレッド乱数表示初期化
	this->SetRndInitial();
	/////////////////////////////////////////////////////////////////
	// スレッド終了状態にする
	// 表示色をピンクにする
	this->SetInitExec(false);
	// リセットボタンを非活性
	this->m_Btn_Reset.EnableWindow(FALSE);
}
/// <summary>
/// スレッド稼働フラグ設定
/// </summary>
/// <param name="flg">スレッド稼働フラグ
/// true：稼働中
/// fase：停止中
/// </param>
void CStressCPUDlg::SetInitExec(bool flg)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_scpu2);
	this->m_InitExec = flg;
}
/// <summary>
/// スレッド稼働フラグ取得
/// </summary>
/// <param name="">なし</param>
/// <returns>スレッド稼働フラグ
/// true：稼働中
/// fase：停止中
/// </returns>
bool CStressCPUDlg::GetInitExec(void)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_scpu2);
	return this->m_InitExec;
}
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
void CStressCPUDlg::SetThrdCheckCtrl(
	MT::e_Status thrdStat,
	const BOOL chk,
	const BOOL enbl
)
{
	const int max_count = this->GetMaxThrdCount();
	for (int ii = 0; ii < max_count; ii++)
	{
		// スレッドの状態を設定
		this->hmultiThrd->CtrlThread(
			ii,
			thrdStat
		);
		// 各待機チェックをON/OFF＆有効/無効
		this->SetCH_Idles(ii, chk, enbl);
	}
}
/// <summary>
/// スレッド乱数表示の初期化
/// </summary>
/// <param name="">なし</param>
void CStressCPUDlg::SetRndInitial(void)
{
	for (const auto& var : this->m_StVector)
	{
		var->SetWindowTextA(this->cpu_vender.c_str());
	}
}
/// <summary>
/// コントロールを一定時間非活性
/// </summary>
/// <param name="lpbtnctrl">対象のコントロールポインタ</param>
/// <param name="wait_time_ms">非活性時間(ms)：デフォルト500ms</param>
void CStressCPUDlg::WaitActiveCtrl(
	CButton* lpbtnctrl,
	UINT32 wait_time_ms
)
{
	// 一定時間クリックイベントを無効にする
	static std::thread* thrHnd;
	thrHnd = new std::thread(
		SCPU::EventWait,
		lpbtnctrl,
		wait_time_ms
	);
}
// FLOPS値設定
void CStressCPUDlg::SetFlopsVar(double var)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_FlopsVar);
	this->m_Flops = var;
}
// FLOPS値取得
double CStressCPUDlg::GetFlopVar(void)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_FlopsVar);
	return this->m_Flops;
}
// FLOPS値MAX設定
void CStressCPUDlg::SetFlopsMax(double var)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_FlopsMax);
	this->m_FlopsMax = var;
}
// FLOPS値MAX取得
double CStressCPUDlg::GetFlopsMax(void)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_FlopsMax);
	return this->m_FlopsMax;
}
// FLOPS値MIN設定
void CStressCPUDlg::SetFlopsMin(double var)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_FlopsMin);
	this->m_FlopsMin = var;
}
// FLOPS値MIN取得
double CStressCPUDlg::GetFlopsMin(void)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_FlopsMin);
	return this->m_FlopsMin;
}

// FLOPS値のMAX値を入力するイベント処理
void CStressCPUDlg::OnChangeEdtFlopsmax()
{
	// TODO: これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CDialogEx::OnInitDialog() 関数をオーバーライドし、
	// CRichEditCtrl().SetEventMask() を関数し呼び出します。
	// OR 状態の ENM_CHANGE フラグをマスクに入れて呼び出す必要があります。

	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
	CString var;
	this->m_EDT_FlopsMAX.GetWindowTextA(var);
	auto val = std::atoi(var.GetString());
	if (val < 100 || 10000 < val)
	{
		// チェックボックスを無効にします。
		this->m_Chk_FlopsMAX.EnableWindow(FALSE);
		return;
	}
	else
	{
		// チェックボックスを有効にします。
		this->m_Chk_FlopsMAX.EnableWindow(TRUE);
		// メンバー変数にセット
		this->SetFlopsMAXLimited(val);
	}
}

// FLOPS値のMAX値の入力欄の制御
void CStressCPUDlg::OnClickedChkFlopsmax()
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_ChkFlopsMaxLimited);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (this->m_Chk_FlopsMAX.GetCheck() == BST_CHECKED)
	{
		// チェックボックスの名称を有効に変更
		this->m_Chk_FlopsMAX.SetWindowTextA("有効");
		// FLOPS値MAX入力欄を無効
		this->m_EDT_FlopsMAX.SetReadOnly(TRUE);
	}
	else
	{
		// チェックボックスの名称を無効に変更
		this->m_Chk_FlopsMAX.SetWindowTextA("無効");
		// FLOPS値MAX入力欄を有効
		this->m_EDT_FlopsMAX.SetReadOnly(FALSE);
	}
}
/// <summary>
/// FLOPS値MAXを設定
/// </summary>
/// <param name="var">MAX値</param>
void CStressCPUDlg::SetFlopsMAXLimited(int var)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_FlopsMaxLimited);
	this->m_FlopsMAXLimited = var;
}
/// <summary>
/// FLOPS値MAXを取得
/// </summary>
/// <return>MAX値</return>
int CStressCPUDlg::GetFlopsMAXLimited(void)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_FlopsMaxLimited);
	return this->m_FlopsMAXLimited;
}
/// <summary>
/// FLOPS値MAXチェックON/OFF取得
/// </summary>
/// <returns>チェックON/OFF</returns>
BOOL CStressCPUDlg::GetChkFlopsMAXLimited(void)
{
	// 参照する前にロックを取得する
	std::lock_guard<std::mutex> lock(SCPU::mtx_ChkFlopsMaxLimited);
	return this->m_Chk_FlopsMAX.GetCheck();
}

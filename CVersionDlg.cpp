// CVersionDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "StressCPU.h"
#include "CVersionDlg.h"
#include "afxdialogex.h"


// CVersionDlg ダイアログ

IMPLEMENT_DYNAMIC(CVersionDlg, CDialogEx)

CVersionDlg::CVersionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VERSION, pParent),
	// 警告フラグ
	m_warn_flg(true),
	// 初回起動時のメッセージ
	m_Initmsg(
		"警告：\n"
		"このソフトウェアは他の起動中のソフトウェアに何ならかの影響を"
		"与える可能性があります―（異常終了、応答なし、異常動作、OSの異常終了など）"
		"場合によっては貴方の大切なデータを破壊してしまうかも知れません。"
		"ご使用中のソフトウェアは極力停止させた状態でご使用する事をお薦めします。"
		"もし万が一データの破損など貴方の利益が損失した場合など一切の責任は貴方自身で"
		"負う事を前提にご使用下さい。\n"
		"こちらは一切の責任を負いません。あらかじめご了承下さい。\n"
		"それでもご使用される場合は［OK］ボタンを押して下さい。\n"
		"ご使用しない場合は［キャンセル］ボタンを押して下さい。"
	),
	// 通常のバージョン情報
	m_NormalMsg(
		"説明：\n"
		"このソフトウェアはCPUのコアやスレッドに100%の使用率となるように"
		"負荷をかけます。その状態の時のCPU温度変化や冷却ファンの回転数、動作周波数の変化を"
		"確認するためのツールです。\n"
		"また参考値として動作中のCPUの性能を「FLOPS値」でリアルタイム、最大値、最小値を"
		"表示します。\n"
		"本画面上で表示しているFLOPS値は開発マシン（AMD RYZEN 5 1600AF）で理論値に近い値に"
		"なるように調整しておりますので他のCPU（IntelやARMや他のAMDのCPU）で表示されるFLOPS値は"
		"必ずしも理論値に近い値となる保証はありません。"
	)
{
}

CVersionDlg::~CVersionDlg()
{
}

void CVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, VerCancelBTN);
	DDX_Control(pDX, IDC_STATIC_VerInfo, ST_VerInfo);
}


BEGIN_MESSAGE_MAP(CVersionDlg, CDialogEx)
END_MESSAGE_MAP()

// 初回起動時のメッセージ設定
void CVersionDlg::SetInitMsg()
{
	// 初期バージョンメッセージ
	this->ST_VerInfo.SetWindowTextA(this->m_Initmsg.GetString());
}
// 通常のバージョン情報表示
void CVersionDlg::SetNormalMsg()
{
	// 初期バージョンメッセージ
	this->ST_VerInfo.SetWindowTextA(this->m_NormalMsg.GetString());
}
// 警告フラグOFF
void CVersionDlg::SetWarnFlgOFF()
{
	this->m_warn_flg = false;
}

// CVersionDlg メッセージ ハンドラー
// キャンセルボタン表示メソッド
// swflg=TRUE:表示、FALSE:非表示
void CVersionDlg::CancelBtn(BOOL swflg)
{
	int swval;
	if (swflg == TRUE)
	{
		swval = SW_SHOW;
	}
	else
	{
		swval = SW_HIDE;
	}
	this->VerCancelBTN.ShowWindow(swval);
}


BOOL CVersionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (this->m_warn_flg == true)
	{
		// 初回警告メッセージ設定
		this->SetInitMsg();
		// キャンセルボタン表示
		this->CancelBtn(TRUE);
		// タイトル
		this->SetWindowTextA("警告");
	}
	else
	{
		// 通常のバージョン情報表示
		this->SetNormalMsg();
		// キャンセルボタン非表示
		this->CancelBtn(FALSE);
		// タイトル
		this->SetWindowTextA("バージョン情報");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

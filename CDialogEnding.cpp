// CDialogEnding.cpp : 実装ファイル
//

#include "pch.h"
#include "StressCPU.h"
#include "CDialogEnding.h"
#include "afxdialogex.h"


// CDialogEnding ダイアログ

IMPLEMENT_DYNAMIC(CDialogEnding, CDialogEx)

CDialogEnding::CDialogEnding(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ENDING, pParent)
{

}

CDialogEnding::~CDialogEnding()
{
}

void CDialogEnding::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogEnding, CDialogEx)
END_MESSAGE_MAP()


// CDialogEnding メッセージ ハンドラー

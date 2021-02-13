#pragma once


// CDialogEnding ダイアログ

class CDialogEnding : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogEnding)

public:
	CDialogEnding(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CDialogEnding();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ENDING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
};

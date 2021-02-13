#pragma once


// CVersionDlg ダイアログ

class CVersionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVersionDlg)

public:
	CVersionDlg(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CVersionDlg();
	// キャンセルボタン表示／非表示メソッド
	void CancelBtn(BOOL swflg);
	// 初回起動時のメッセージ設定
	void SetInitMsg();
	// 通常のバージョン情報表示
	void SetNormalMsg();
	// 警告フラグOFF
	void SetWarnFlgOFF();
// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VERSION };
#endif
private:
	// 警告フラグ
	bool m_warn_flg;
	// 初回起動時のメッセージ
	const CString m_Initmsg;
	// 通常のバージョン情報
	const CString m_NormalMsg;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	// バージョン情報のキャンセルボタン
	CButton VerCancelBTN;
	// バージョン情報の説明文用表示コントロール
	CStatic ST_VerInfo;
	virtual BOOL OnInitDialog();
};

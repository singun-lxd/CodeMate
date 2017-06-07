#pragma once


// CChangeTypeDlg 对话框

class CChangeTypeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeTypeDlg)

public:
	CChangeTypeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChangeTypeDlg();

// 对话框数据
	enum { IDD = IDD_CHANGELANG_DIALOG };

protected:
	virtual BOOL OnInitDialog();	// 对话框初始化
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);	// 列表视图改变选择
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);			// 列表视图项目被双击
	CString m_strAppPath;		// 运行目录
	CString m_strWorkPath;	// 工作目录
	CImageList m_wndImageList;	// 定义图像列表控件
	CListCtrl m_ListCode;	// 定义列表视图控件
	int m_CodeType;			// 代码类型
	UINT m_uiLanguage;			// 界面语言
};

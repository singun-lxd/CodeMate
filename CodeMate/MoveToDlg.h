#pragma once

// CMoveTo 对话框

class CMoveToDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMoveToDlg)

public:
	CMoveToDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMoveToDlg();

// 对话框数据
	enum { IDD = IDD_MOVETO_DIALOG };

protected:
	virtual BOOL OnInitDialog();	// 对话框初始化
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HTREEITEM root;			// 根目录指针
	CString m_strAppPath;		// 运行目录
	CString m_strWorkPath;	// 工作目录
	CString FullPath;		// 全路径
	CString m_Path;			// 虚拟路径
	CImageList m_wndImageList;	// 图像列表控件
	CTreeCtrl m_wndTreeView;	// 树形控件
	UINT m_uiLanguage;			// 界面语言
	afx_msg void OnBnClickedOk();	// 确认操作
	// 以下函数注释请参看CodeMateDlg.h
	afx_msg void OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	void GetClassDir(HTREEITEM hParent);
	void GetMainClass(HTREEITEM hParent, CString strText);
	void AddSubItem(HTREEITEM hParent);
	CString ItemToPath(HTREEITEM hCurrent);
};

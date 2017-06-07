#pragma once

// CAddItem 对话框

class CAddItemDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddItemDlg)

public:
	CAddItemDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddItemDlg();

// 对话框数据
	enum { IDD = IDD_ADDITEM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void AddBitmap(CBitmap &bm, UINT uiBmpId);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChangeClass();	// 选择分类
	afx_msg void OnBnClickedSetDefault();	// 设置默认类型
	afx_msg void OnBnClickedOk();			// 确认操作
	CString m_strAppPath;	// 运行目录
	CString m_strWorkPath;	// 工作目录
	CString m_strFullPath;	// 全路径
	CString m_strTitle;		// 标题
	CString m_strClass;		// 分类，虚拟路径
	bool m_bSaveAsDefault;		// 设为默认
	UINT m_uiSourceType;		// 创建方式（来源）
	UINT m_uiLangType;			// 语言类型
	bool m_bReConfirm;			// 分类是否更改，是否需要重新定位
	CImageList m_wndImageList;	// 图像列表控件
	CComboBoxEx	m_cboLanguage;	// 扩展组合框控件
	UINT m_uiLanguage;			// 界面语言
};

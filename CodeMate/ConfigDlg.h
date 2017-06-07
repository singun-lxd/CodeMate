#pragma once
#include "afxcolorbutton.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "CodeEdit.h"

// CConfigDlg 对话框

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	~CConfigDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

public:
	UINT m_uiLanguage;			// 界面语言
	COLORINFO m_coColor;
	CMFCColorButton m_ColorLineNumBG;		// 颜色选择按钮，下同
	CMFCColorButton m_ColorLineNumFG;
	CMFCColorButton m_ColorKeyword;
	CMFCColorButton m_ColorConstant;
	CMFCColorButton m_ColorComment;
	CMFCColorButton m_ColorText;
	CMFCColorButton m_ColorBackground;
	CMFCColorButton m_ColorForeground;
	CMFCEditBrowseCtrl m_Folder_Browser;	// 浏览文件夹控件
	CVSListBox m_VSListBox;	// 关键词添加列表控件
	CListCtrl m_ListCode;	// 语言列表
	CImageList m_wndImageList;	// 定义图像列表控件
	CStringArray m_arrKeyWord;	// 关键词列表数组
	CStringArray m_arrConstant;	// 常量列表数组
	int m_CodeType;			// 代码类型
	LOGFONT font;		// 保存字体信息
	CEdit m_FontShow;	// 显示字体信息的编辑框
	CTabCtrl m_Tabs;	// 选项卡控件
	CComboBox m_Combo_Encoding;	// 选择编码下拉框
	CButton m_Check_LineNum;	// 是否显示行号复选框
	CEdit m_Edit_Tabs;			// Tab 长度
	CSpinButtonCtrl m_Spin;		// 文字调整
	CButton m_Button_Font;		// 选择字体
	UINT m_uiDefEncode;		// 默认编码
	CString m_strAppPath;	// 运行目录
	CString m_strWorkPath;	// 工作目录
	CString bold;		// 保存“粗体”的多语言字符串
	CString italic;		// 保存“斜体”的多语言字符串
	CString name;		// 字体标题
	int m_iSize;		// 字号
	int m_iTabLen;			// Tab 长度
	BOOL m_bShowLineNum;	// 是否显示行号
	BOOL isKeyWord;		// 当前设置的是关键词或常量
	bool isMoved;		// 文件夹是否已经移动
	bool MoveFolder(CString strFromPath, CString strToPath);		// 移动文件夹
	void AddWords(LPTSTR lpszKeywords, int iLen);					// 添加关键词
	void SaveWords();												// 保存关键词
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);	// 选项卡切换
	afx_msg void OnBnClickedRadio1();	// 单选钮单击
	afx_msg void OnBnClickedRadio2();	// 单选钮单击
	afx_msg void OnEnChangeEdit2();		// 编辑框内容改变
	afx_msg void OnBnClickedOk();		// 按下“确定”
	afx_msg void OnBnClickedButton1();	// 选择字体
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};

#pragma once

#ifdef _WIN64
#define VER64 TRUE
#else
#define VER64 FALSE
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	virtual BOOL OnInitDialog();	// 初始化对话框
	HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	CString GetAppVersion();		// 获得软件版本信息

	DECLARE_MESSAGE_MAP()
public:
	CString m_strWorkPath;	// 工作文件夹
	CString m_strAppPath;	// 运行文件夹
	CStatic m_stcVersion;	// 显示版本号
	UINT m_uiLanguage;		// 界面语言
};
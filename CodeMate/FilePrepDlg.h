#pragma once


// CFilePrepDlg 对话框

class CFilePrepDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFilePrepDlg)

public:
	CFilePrepDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFilePrepDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PREP };

protected:
	virtual BOOL OnInitDialog();	// 对话框初始化
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL GetDirTime(CString strDirName, SYSTEMTIME &Cretime, SYSTEMTIME &Modtime, SYSTEMTIME &Acctime);	// 获得文件和文件夹时间属性
	bool m_bIsFolder;		// 是否为文件夹
	int m_iFileType;		// 文件类型
	CString m_strAppPath;	// 运行目录
	CString m_strWorkPath;	// 工作目录
	CString m_strFullPath;	// 全路径
	CString m_strTitle;		// 显示文件标题
	CString m_strType;		// 显示文件类型
	CString m_strPath;		// 显示文件虚拟路径
	CString m_strCreTime;	// 显示文件创建时间
	CString m_strModTime;	// 显示文件修改时间
	CString m_strAccTime;	// 显示文件访问时间
	UINT m_uiLanguage;		// 界面语言
};

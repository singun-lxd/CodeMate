#pragma once
#include "afxcmn.h"


// CGoToDlg 对话框

class CGoToDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGoToDlg)

public:
	CGoToDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGoToDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_GOTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();				// 对话框初始化

	DECLARE_MESSAGE_MAP()

public:
	int LineNum;		// 行数
	int m_LineIndex;	// 将跳转到第几行
	int m_ColumnIndex;	// 将跳转到第几列
	int *pMaxChar;		// 每行最大字符数
	bool SetRange;		// 是否设置范围
	UINT m_uiLanguage;		// 界面语言
	CString m_strAppPath;		// 运行目录
	CString m_strWorkPath;	// 工作目录
	CSpinButtonCtrl m_Spin;				// 旋转控件
	CSpinButtonCtrl m_Spin2;			// 旋转控件2
	afx_msg void OnEnChangeEditLine();		// 编辑框1文本变化
	afx_msg void OnEnChangeEditColumn();	// 编辑框2文本变化
};

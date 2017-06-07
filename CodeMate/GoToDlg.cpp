// GoToDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CodeMate.h"
#include "GoToDlg.h"


// CGoToDlg 对话框

IMPLEMENT_DYNAMIC(CGoToDlg, CDialogEx)

CGoToDlg::CGoToDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGoToDlg::IDD, pParent)
	, m_LineIndex(0)
	, m_ColumnIndex(0)
{

}

CGoToDlg::~CGoToDlg()
{
	delete []pMaxChar;
}

void CGoToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN1, m_Spin);
	DDX_Control(pDX, IDC_SPIN2, m_Spin2);
	DDX_Text(pDX, IDC_EDIT_LINE, m_LineIndex);
	DDX_Text(pDX, IDC_EDIT_COLUMN, m_ColumnIndex);
}


BEGIN_MESSAGE_MAP(CGoToDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_LINE, &CGoToDlg::OnEnChangeEditLine)
	ON_EN_CHANGE(IDC_EDIT_COLUMN, &CGoToDlg::OnEnChangeEditColumn)
END_MESSAGE_MAP()


// CGoToDlg 消息处理程序
BOOL CGoToDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetRange = true;
	CString strLoad;
	if (m_uiLanguage != 0)
	{
		SetCurrentDirectory(m_strAppPath);
		LPWSTR strFileName = NULL;
		if (m_uiLanguage == 1)
			strFileName = LANG_TCHINESE_FILE;
		else if (m_uiLanguage == 2)
			strFileName = LANG_ENGLISH_FILE;
		LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
		// 标题栏
		GetPrivateProfileString(LANG_GOTO_DLG, _T("DIALOG_TITLE"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		SetWindowText(strLoad);
		// 按钮
		CButton *pButton = NULL;
		GetPrivateProfileString(LANG_STRING, _T("DIALOG_OK"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDOK);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_STRING, _T("DIALOG_CANCEL"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDCANCEL);
		pButton -> SetWindowText(strLoad);
		// 静态文本
		CStatic *pStatic = NULL;
		GetPrivateProfileString(LANG_GOTO_DLG, _T("STATIC_LINE_NUMBER"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_LINE_NO);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_GOTO_DLG, _T("STATIC_COLUMN_NUMBER"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLUMN_NO);
		pStatic -> SetWindowText(strLoad);
		SetCurrentDirectory(m_strWorkPath);
	}
	m_Spin.SetRange(1, LineNum);
	m_Spin2.SetRange(0, pMaxChar[m_LineIndex-1]);

	return TRUE;
}

void CGoToDlg::OnEnChangeEditLine()
{
	if (!SetRange) return;
	CString num;
	CEdit *p = (CEdit *)GetDlgItem(IDC_EDIT_LINE);
	p->GetWindowText(num);
	int value = _ttoi(num);
	CEdit *q = (CEdit *)GetDlgItem(IDC_EDIT_COLUMN);
	p->GetWindowText(num);
	int value2 = _ttoi(num);
	if (value > LineNum)
	{
		num.Format(_T("%d"), LineNum);
		p->SetWindowText(num);
		if (value2 > pMaxChar[LineNum-1])
		{
			num.Format(_T("%d"), pMaxChar[LineNum-1]);
			q->SetWindowText(num);
		}
		m_Spin2.SetRange(0, pMaxChar[LineNum-1]);
	}
	else if (value == 0)
	{
		p->SetWindowText(_T("1"));
		if (value2 > pMaxChar[0])
		{
			num.Format(_T("%d"), pMaxChar[0]);
			q->SetWindowText(num);
		}
		m_Spin2.SetRange(0, pMaxChar[0]);
	}
	else
	{
		if (value2 > pMaxChar[value-1])
		{
			num.Format(_T("%d"), pMaxChar[value-1]);
			q->SetWindowText(num);
		}
		m_Spin2.SetRange(0, pMaxChar[value-1]);
	}
}


void CGoToDlg::OnEnChangeEditColumn()
{
	CString num;
	CEdit *p = (CEdit *)GetDlgItem(IDC_EDIT_LINE);
	p->GetWindowText(num);
	int value = _ttoi(num);
	p = (CEdit *)GetDlgItem(IDC_EDIT_COLUMN);
	p->GetWindowText(num);
	int value2 = _ttoi(num);
	if (value2 > pMaxChar[value-1])
	{
		num.Format(_T("%d"), pMaxChar[value-1]);
		p->SetWindowText(num);
	}
}

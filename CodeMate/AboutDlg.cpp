// AboutDlg.cpp : 实现文件
// 

#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.h"

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VERSION, m_stcVersion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString strLoad, strVersion;

	CStatic *p = NULL;
	CFont font;
	font.CreateFont(0, 0, 0, 0,	FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("宋体"));
	p = (CStatic *) GetDlgItem(IDC_STATIC_MAIN);
	p->SetFont(&font);
	

	if (m_uiLanguage == 1)
	{
		p = (CStatic *) GetDlgItem(IDC_STATIC_DES);
		strLoad.LoadString(IDS_ABOUT_DES_T);
		p->SetWindowText(strLoad);
		p = (CStatic *) GetDlgItem(IDC_STATIC_CR);
		strLoad.LoadString(IDS_ABOUT_CR_T);
		p->SetWindowText(strLoad);
		p = (CStatic *) GetDlgItem(IDC_STATIC_OPEN);
		strLoad.LoadString(IDS_ABOUT_OPENSOURCE_T);
		p->SetWindowText(strLoad);
		strLoad.LoadString(IDS_ABOUT_TITLE_T);
		SetWindowText(strLoad);
	}
	else if (m_uiLanguage == 2)
	{
		p = (CStatic *) GetDlgItem(IDC_STATIC_DES);
		strLoad.LoadString(IDS_ABOUT_DES_E);
		p->SetWindowText(strLoad);
		p = (CStatic *) GetDlgItem(IDC_STATIC_CR);
		strLoad.LoadString(IDS_ABOUT_CR_E);
		p->SetWindowText(strLoad);
		p = (CStatic *) GetDlgItem(IDC_STATIC_OPEN);
		strLoad.LoadString(IDS_ABOUT_OPENSOURCE_E);
		p->SetWindowText(strLoad);
		strLoad.LoadString(IDS_ABOUT_TITLE_E);
		SetWindowText(strLoad);
	}

	if (m_uiLanguage == 2)
		strLoad.LoadString(IDS_ABOUT_VERSION_E);
	else
		strLoad.LoadString(IDS_ABOUT_VERSION);
	CString Version = GetAppVersion();
	strVersion.Format(strLoad, Version);
	if (VER64)
		strVersion += _T(" x64");
	else
		strVersion += _T(" x86");
	m_stcVersion.SetWindowText(strVersion);

	CButton *pButton = (CButton *) GetDlgItem(IDOK);
	if (m_uiLanguage != 0)
	{
		SetCurrentDirectory(m_strAppPath);
		LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
		if (m_uiLanguage == 1)
			GetPrivateProfileString(LANG_STRING, _T("DIALOG_OK"), _T(""), lpszLang, LANG_STRING_LEN, LANG_TCHINESE_FILE);
		else if (m_uiLanguage == 2)
			GetPrivateProfileString(LANG_STRING, _T("DIALOG_OK"), _T(""), lpszLang, LANG_STRING_LEN, LANG_ENGLISH_FILE);
		strLoad = lpszLang;
		pButton -> SetWindowText(strLoad);
		delete []lpszLang;
		SetCurrentDirectory(m_strWorkPath);
	}

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

HBRUSH CAboutDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	int nID = pWnd->GetDlgCtrlID();
	pDC->SetBkMode(OPAQUE);
	if(nCtlColor == CTLCOLOR_STATIC && nID != IDC_STATIC_IMG) //设定控件的属性,对所有的标签
	{
		if (nID == IDC_STATIC_MAIN)
			pDC->SetTextColor(RGB(0,0,255));
		if (nID == IDC_STATIC_DES)
			pDC->SetTextColor(RGB(255,0,0));
		return HBRUSH(GetStockObject(HOLLOW_BRUSH));
	}

	return hbr;
}

CString CAboutDlg::GetAppVersion()
{
	// 获得可执行文件路径
	TCHAR szAppPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szAppPath, MAX_PATH);

	// 做好准备工作
	VS_FIXEDFILEINFO *pVInfo;
	UINT iSize;
	char *lpszBuf;
	int iVerMain, iVerSub, iVerUpd, iVerFix;
	CString sReturn = _T("");
	iSize = GetFileVersionInfoSize(szAppPath, 0);
	lpszBuf = new char[iSize];

	// 获取程序版本信息
	if (GetFileVersionInfo(szAppPath, 0, iSize, lpszBuf) != 0)
	{
		if (VerQueryValue(lpszBuf, _T("\\"), (LPVOID*)&pVInfo, &iSize))
		{
			iVerMain = pVInfo->dwFileVersionMS >> 16;
			iVerSub = pVInfo->dwFileVersionMS & 0x00ff;
			iVerUpd = pVInfo->dwFileVersionLS >> 16;
			iVerFix = pVInfo->dwFileVersionLS & 0x00ff;
			// 格式化结果
			sReturn.Format(_T("%d.%d.%d.%d"),
				iVerMain, iVerSub, iVerUpd, iVerFix);
		}
	}
	delete []lpszBuf;
	return sReturn;
}
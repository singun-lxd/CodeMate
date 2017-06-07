// AddItem.cpp : 实现文件
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "AddItemDlg.h"
#include "MoveToDlg.h"
#include "IsValidFileName.h"

// CAddItem 对话框

IMPLEMENT_DYNAMIC(CAddItemDlg, CDialogEx)
CAddItemDlg::CAddItemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddItemDlg::IDD, pParent)
	, m_strTitle(_T(""))
	, m_strClass(_T(""))
	, m_bReConfirm(false)
{
}

CAddItemDlg::~CAddItemDlg()
{
}

void CAddItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
	DDX_Text(pDX, IDC_EDIT_CLASS, m_strClass);
	DDX_Control(pDX, IDC_COMBO_LANG, m_cboLanguage);
}


BEGIN_MESSAGE_MAP(CAddItemDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHANGE_CLASS, OnBnClickedChangeClass)
	ON_BN_CLICKED(IDC_SET_DEFAULT, &CAddItemDlg::OnBnClickedSetDefault)
END_MESSAGE_MAP()

// CAddItem 消息处理程序

BOOL CAddItemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 加载语言
	CString strLoad;
	if (m_uiLanguage != 0)
	{
		SetCurrentDirectory(m_strAppPath);
		LPWSTR lpszFileName = NULL;
		if (m_uiLanguage == 1)
			lpszFileName = LANG_TCHINESE_FILE;
		else if (m_uiLanguage == 2)
			lpszFileName = LANG_ENGLISH_FILE;
		LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
		// 标题栏
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("DIALOG_TITLE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		SetWindowText(strLoad);
		// 按钮
		CButton *pButton = NULL;
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("BUTTON_CHANGE_CLASS"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_CHANGE_CLASS);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("BUTTON_SET_AS_DEFAULT"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_SET_DEFAULT);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("RADIO_EMPTY_FILE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_RADIO1);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("RADIO_CLIPBOARD"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_RADIO2);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("RADIO_CODE_FILE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_RADIO3);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_STRING, _T("DIALOG_OK"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDOK);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_STRING, _T("DIALOG_CANCEL"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDCANCEL);
		pButton -> SetWindowText(strLoad);
		// 静态文本
		CStatic *pStatic = NULL;
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("STATIC_TITLE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_TITLE);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("STATIC_CLASS"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_CLASS);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("STATIC_LANGUAGE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_LANG);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("STATIC_SOURCE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_SOURCE);
		pStatic -> SetWindowText(strLoad);
		delete []lpszLang;
		SetCurrentDirectory(m_strWorkPath);
	}

	// 以下为图像列表初始化过程
	m_wndImageList.Create(16, 16, ILC_COLOR24|ILC_MASK, 4, 4);						// 创建图像列表
	CBitmap bm;
	AddBitmap(bm, IDB_BITMAP1);
	AddBitmap(bm, IDB_BITMAP2);
	for (UINT nId = IDB_BITMAP5; nId <= IDB_BITMAP18; nId++)
		AddBitmap(bm, nId);
	bm.Detach();

	// 为组合框添加图像列表
	m_cboLanguage.SetImageList(&m_wndImageList);
	// 为组合框添加项目
	for(int i=0; i<8; i++)
	{
		COMBOBOXEXITEM     cbi ={0};
		CString            str;
		int                nItem;

		cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |
			CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
		cbi.iItem = i;
		switch(i)
		{
		case 0:
			{
				cbi.iImage = 1;
				cbi.iSelectedImage = 0;
				str=_T("C++");
			}
			break;
		case 1:
			{
				cbi.iImage = 3;
				cbi.iSelectedImage = 2;
				str=_T("ANSI C");
			}
			break;
		case 2:
			{
				cbi.iImage = 15;
				cbi.iSelectedImage = 14;
				str=_T("C#.NET");
			}
			break;
		case 3:
			{
				cbi.iImage = 5;
				cbi.iSelectedImage = 4;
				str=_T("HTML");
			}
			break;
		case 4:
			{
				cbi.iImage = 7;
				cbi.iSelectedImage = 6;
				str=_T("Java Script");
			}
			break;
		case 5:
			{
				cbi.iImage = 13;
				cbi.iSelectedImage = 12;
				str=_T("VB Script");
			}
			break;
		case 6:
			{
				cbi.iImage = 9;
				cbi.iSelectedImage = 8;
				str=_T("SQL");
			}
			break;
		case 7:
			{
				cbi.iImage = 11;
				cbi.iSelectedImage = 10;
				LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
				if (m_uiLanguage == 0)
					str.LoadString(IDS_NORMAL_TEXT);
				else if (m_uiLanguage == 1)
				{
					SetCurrentDirectory(m_strAppPath);
					GetPrivateProfileString(LANG_STRING, _T("NORMAL_TEXT"),
						_T(""), lpszLang, LANG_STRING_LEN, LANG_TCHINESE_FILE);
					str = lpszLang;
					SetCurrentDirectory(m_strWorkPath);
				}
				else if (m_uiLanguage == 2)
				{
					SetCurrentDirectory(m_strAppPath);
					GetPrivateProfileString(LANG_STRING, _T("NORMAL_TEXT"),
						_T(""), lpszLang, LANG_STRING_LEN, LANG_ENGLISH_FILE);
					str = lpszLang;
					SetCurrentDirectory(m_strWorkPath);
				}
				delete []lpszLang;
			}
			break;
		}
		cbi.pszText = (LPTSTR)(LPCTSTR)str;
		cbi.cchTextMax = str.GetLength();
		cbi.iOverlay = 2;
		cbi.iIndent = 0;
		nItem = m_cboLanguage.InsertItem(&cbi);
	}
	// 设置组合框默认选中项目
	m_cboLanguage.SetCurSel(m_uiLangType);
	m_bSaveAsDefault = false;
	// 选中单选钮
	CButton * p1=(CButton *) GetDlgItem(IDC_RADIO1);
	p1->SetCheck(1);
	p1=NULL;
	delete p1;
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CAddItemDlg::AddBitmap(CBitmap &bm, UINT uiBmpId)
{
	bm.LoadBitmap(uiBmpId);									// 载入Bitmap资源
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);				// 添加到图像列表
	bm.DeleteObject();
}

void CAddItemDlg::OnBnClickedChangeClass()
{
	CMoveToDlg dlgMoveTo;
	// 通过移动到分类对话框更改分类信息
	dlgMoveTo.FullPath = m_strFullPath;
	dlgMoveTo.m_Path = m_strClass;
	dlgMoveTo.m_uiLanguage = m_uiLanguage;
	dlgMoveTo.m_strAppPath = m_strAppPath;
	dlgMoveTo.m_strWorkPath = m_strWorkPath;
	if (dlgMoveTo.DoModal()==IDOK)
	{
		if (m_strFullPath == dlgMoveTo.FullPath)
			m_bReConfirm = false;
		else
			m_bReConfirm = true;
		m_strFullPath = dlgMoveTo.FullPath;
		m_strClass = dlgMoveTo.m_Path;
		// 从变量更新到控件
		UpdateData(FALSE);
	}
}

void CAddItemDlg::OnBnClickedSetDefault()
{
	// 设置更改默认类型的标志
	m_bSaveAsDefault = true;
}

void CAddItemDlg::OnBnClickedOk()
{
	// 更新控件信息到变量
	UpdateData(TRUE);
	CString strTitle,strContent;
	if (m_strTitle.IsEmpty())	// 标题输入为空
	{
		if (m_uiLanguage == 0)
		{
			strTitle.LoadString(IDS_MSGBOX_WARN);
			strContent.LoadString(IDS_MSGBOX_NOTEMPTY);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPWSTR lpszFileName = NULL;
			if (m_uiLanguage == 1)
				lpszFileName = LANG_TCHINESE_FILE;
			else if (m_uiLanguage == 2)
				lpszFileName = LANG_ENGLISH_FILE;
			LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_TITLE_EMPTY"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_WARNING"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
			strTitle = lpszLang;
			delete []lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		MessageBox(strContent, strTitle, MB_ICONWARNING);
		CEdit * p1=(CEdit *) GetDlgItem(IDC_EDIT_TITLE);
		p1->SetFocus();
		p1=NULL;
		delete p1;
		return;
	}
	else	// 有输入标题
	{
		int iResult;
		iResult = IsValidFileName(m_strTitle);
		if (iResult != 0)	// 标题不符合文件命名规则
		{
			if (m_uiLanguage == 0)
			{
				strContent.LoadString(IDS_MSGBOX_EXITWARN);
				strTitle.LoadString(IDS_MSGBOX_QUES);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPWSTR lpszFileName = NULL;
				if (m_uiLanguage == 1)
					lpszFileName = LANG_TCHINESE_FILE;
				else if (m_uiLanguage == 2)
					lpszFileName = LANG_ENGLISH_FILE;
				LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_INVILID_NAME"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
				strContent = lpszLang;
				strContent.Replace(_T("\\n"), _T("\n"));
				GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_WARNING"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
				strTitle = lpszLang;
				delete []lpszLang;
				SetCurrentDirectory(m_strWorkPath);
			}
			MessageBox(strContent, strTitle, MB_ICONWARNING);
			CEdit * p1=(CEdit *) GetDlgItem(IDC_EDIT_TITLE);
			p1->SetFocus();
			p1=NULL;
			delete p1;
			return;
		}
	}
	// 获得全路径
	CString filepath = m_strFullPath + _T("\\") + m_strTitle;
	// 判断要创建的文件类型
	m_uiLangType = m_cboLanguage.GetCurSel();
	switch(m_uiLangType)
	{
	case 0:
			filepath += _T(".c-p");
		break;
	case 1:
			filepath += _T(".c-a");
		break;
	case 2:
			filepath += _T(".h-t");
		break;
	case 3:
			filepath += _T(".j-s");
		break;
	case 4:
			filepath += _T(".s-q");
		break;
	case 5:
			filepath += _T(".txt");
		break;
	}
	CFileFind filefind;
	if(filefind.FindFile(filepath))	// 文件已经存在
	{
		if (m_uiLanguage == 0)
		{
			strTitle.LoadString(IDS_MSGBOX_WARN);
			strContent.LoadString(IDS_MSGBOX_FILEEXIST);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPWSTR lpszFileName = NULL;
			if (m_uiLanguage == 1)
				lpszFileName = LANG_TCHINESE_FILE;
			else if (m_uiLanguage == 2)
				lpszFileName = LANG_ENGLISH_FILE;
			LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_FILE_EXIST"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_WARNING"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
			strTitle = lpszLang;
			delete []lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		MessageBox(strContent, strTitle, MB_ICONWARNING);
		CEdit * p1=(CEdit *) GetDlgItem(IDC_EDIT_TITLE);
		p1->SetFocus();
		p1=NULL;
		delete p1;
		return;
	}
	// 判断创建的方式
	CButton * p1=(CButton *) GetDlgItem(IDC_RADIO1);
	m_uiSourceType = p1->GetCheck();
	if (m_uiSourceType == 0)
	{
		p1=(CButton *) GetDlgItem(IDC_RADIO2);
		m_uiSourceType = p1->GetCheck();
		if (m_uiSourceType == 0)
			m_uiSourceType = 2;
		else
			m_uiSourceType = 1;
	}
	else
		m_uiSourceType = 0;
	p1=NULL;
	delete p1;
	OnOK();
}
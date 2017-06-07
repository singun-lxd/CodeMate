// MoveTo.cpp : 实现文件
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "MoveToDlg.h"


// CMoveTo 对话框

IMPLEMENT_DYNAMIC(CMoveToDlg, CDialogEx)
CMoveToDlg::CMoveToDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMoveToDlg::IDD, pParent)
	, m_Path(_T(""))
{
}

CMoveToDlg::~CMoveToDlg()
{
}

void CMoveToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_wndTreeView);
	DDX_Text(pDX, IDC_EDIT1, m_Path);
}


BEGIN_MESSAGE_MAP(CMoveToDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE1, OnTvnItemexpandedTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
END_MESSAGE_MAP()

// CMoveTo 消息处理程序

BOOL CMoveToDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString strLoad;
	if (m_uiLanguage != 0)
	{
		SetCurrentDirectory(m_strAppPath);
		LPWSTR strFileName = NULL;
		if (m_uiLanguage == 1)
			strFileName = LANG_TCHINESE_FILE;
		else if (m_uiLanguage == 2)
			strFileName = LANG_ENGLISH_FILE;
		LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
		// 标题栏
		GetPrivateProfileString(LANG_MOVETO_DLG, _T("DIALOG_TITLE"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
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
		CStatic *pStatic = (CStatic *) GetDlgItem(IDC_STATIC_INPUT);
		GetPrivateProfileString(LANG_MOVETO_DLG, _T("STATIC_INPUT"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic -> SetWindowText(strLoad);
		SetCurrentDirectory(m_strWorkPath);
	}

	// 图像列表初始化
	m_wndImageList.Create(16, 16, ILC_COLOR24|ILC_MASK, 4, 4);						// 创建图像列表
	CBitmap bm;
	bm.LoadBitmap(IDB_BITMAP3);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP4);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	m_wndTreeView.SetImageList(&m_wndImageList,TVSIL_NORMAL); // 为树形控件添加图像列表
	if (m_uiLanguage == 0)
		strLoad.LoadString(IDS_CLASS_NORMAL);
	else
	{
		SetCurrentDirectory(m_strAppPath);
		LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
		if (m_uiLanguage == 1)
			GetPrivateProfileString(LANG_STRING, _T("TREE_ALL_CLASSES"), _T(""), lpszLang, LANG_STRING_LEN, LANG_TCHINESE_FILE);
		else if (m_uiLanguage == 2)
			GetPrivateProfileString(LANG_STRING, _T("TREE_ALL_CLASSES"), _T(""), lpszLang, LANG_STRING_LEN, LANG_ENGLISH_FILE);
		strLoad = lpszLang;
		delete []lpszLang;
		SetCurrentDirectory(m_strWorkPath);
	}
	root=m_wndTreeView.InsertItem(strLoad,1,1,TVI_ROOT);// 添加根结点
	GetMainClass(root, _T(".\\Code\\*.*"));
	GetClassDir(root);
	m_wndTreeView.Expand(root,TVE_EXPAND);// 只展开根节点
	m_wndTreeView.SelectItem(root);	// 选中根节点
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CMoveToDlg::OnBnClickedOk()
{
	// 更新控件内容到变量
	UpdateData(TRUE);
	CString strConv,strCompare;
	strConv = m_wndTreeView.GetItemText(root);
	int iStart = strConv.GetLength();
	strCompare = strConv + _T("\\");
	// 以下对路径合法性做简单判断，具体判断交给 PathToItem 函数
	if (m_Path != strConv && m_Path.Left(iStart + 1) != strCompare)
	{
		CString strContent,strTitle;
		if (m_uiLanguage == 0)
		{
			strContent.LoadString(IDS_MSGBOX_PATHERR);
			strTitle.LoadString(IDS_MSGBOX_WARN);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPWSTR strFileName = NULL;
			if (m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_INVALID_PATH"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_WARNING"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
			strTitle = lpszLang;
			delete []lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		MessageBox(strContent, strTitle, MB_ICONWARNING);
		return;
	}
	FullPath = _T(".\\Code") + m_Path.Mid(iStart);
	OnOK();
}

// 以下所有函数注释请参看 CodeMateDlg.cpp

void CMoveToDlg::GetMainClass(HTREEITEM hParent, CString strText)
{
	CFileFind file;
	BOOL bContinue = file.FindFile(strText);
	while(bContinue)
	{
		bContinue = file.FindNextFile();
		CString filename = file.GetFileName();
		if (!filename.IsEmpty() && !file.IsDots())
		{
			if(file.IsDirectory())
				m_wndTreeView.InsertItem(filename,1,1,hParent);
		}
	}
}

void CMoveToDlg::GetClassDir(HTREEITEM hParent)
{
	HTREEITEM hChild = m_wndTreeView.GetChildItem(hParent);
	while(hChild)
	{
		CString strText = _T(".\\Code\\") + m_wndTreeView.GetItemText(hChild);
		strText += _T("\\*.*");
		CFileFind file;
		BOOL bContinue = file.FindFile(strText);
		while(bContinue)
		{
			bContinue = file.FindNextFile();
			CString filename = file.GetFileName();
			if (!filename.IsEmpty() && !file.IsDots())
			{
				if(file.IsDirectory() )
					m_wndTreeView.InsertItem(filename,1,1,hChild);
			}
		}
		hChild = m_wndTreeView.GetNextItem(hChild,TVGN_NEXT);
	}	
}

void CMoveToDlg::AddSubItem(HTREEITEM hParent)
{
	CString strPath = ItemToPath(hParent);
	strPath += _T("\\*.*");
	CFileFind file;
	BOOL bContinue = file.FindFile(strPath);
	while(bContinue)
	{
		bContinue = file.FindNextFile();
		CString filename = file.GetFileName();
		if (!filename.IsEmpty() && !file.IsDots())
		{
			if(file.IsDirectory() )
				m_wndTreeView.InsertItem(filename,1,1,hParent);
		}
	}
}

CString CMoveToDlg::ItemToPath(HTREEITEM hCurrent)
{
	CString strTemp;
	CString strReturn = m_wndTreeView.GetItemText(hCurrent);
	hCurrent = m_wndTreeView.GetParentItem(hCurrent);
	while(hCurrent != root)
	{
		strTemp = m_wndTreeView.GetItemText(hCurrent);
		strTemp += _T("\\");
		strReturn = strTemp  + strReturn;
		hCurrent = m_wndTreeView.GetParentItem(hCurrent);
	}
	strReturn = _T(".\\Code\\") + strReturn;
	return strReturn;
}

void CMoveToDlg::OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM item = pNMTreeView->itemNew;
	if (pNMTreeView-> action == 1)
	{
		m_wndTreeView.SetItemImage(item.hItem, 1, 1);
	}
	else if (pNMTreeView-> action == 2)
	{
		m_wndTreeView.SetItemImage(item.hItem, 0, 0);
		if(item.hItem == root)
			return;
		HTREEITEM hChild = m_wndTreeView.GetChildItem(item.hItem);
		while(hChild)
		{
			if (m_wndTreeView.GetChildItem(hChild)==NULL) AddSubItem(hChild);
			hChild = m_wndTreeView.GetNextItem(hChild,TVGN_NEXT);
		}
	}
	*pResult = 0;
}

void CMoveToDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM item = pNMTreeView->itemNew;
	CString strConv;
	strConv = m_wndTreeView.GetItemText(root);
	if(item.hItem == root)
	{
		CString strLoad;
		if (m_uiLanguage == 0)
			strLoad.LoadString(IDS_CLASS_NORMAL);
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
			if (m_uiLanguage == 1)
				GetPrivateProfileString(LANG_STRING, _T("TREE_ALL_CLASSES"), _T(""), lpszLang, LANG_STRING_LEN, LANG_TCHINESE_FILE);
			else if (m_uiLanguage == 2)
				GetPrivateProfileString(LANG_STRING, _T("TREE_ALL_CLASSES"), _T(""), lpszLang, LANG_STRING_LEN, LANG_ENGLISH_FILE);
			strLoad = lpszLang;
			delete []lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		m_Path = strLoad;
		UpdateData(FALSE);
	}
	else
	{
		CString path = ItemToPath(item.hItem);
		path = path.Mid(7);
		path = strConv +_T("\\") + path;
		m_Path = path;
		UpdateData(FALSE);
	}
	*pResult = 0;
}

// ChangeTypeDlg.cpp : 实现文件
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "ChangeTypeDlg.h"


// CChangeTypeDlg 对话框

IMPLEMENT_DYNAMIC(CChangeTypeDlg, CDialogEx)
CChangeTypeDlg::CChangeTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChangeTypeDlg::IDD, pParent)
{
}

CChangeTypeDlg::~CChangeTypeDlg()
{
}

void CChangeTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCode);
}


BEGIN_MESSAGE_MAP(CChangeTypeDlg, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CChangeTypeDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// CChangeTypeDlg 消息处理程序

BOOL CChangeTypeDlg::OnInitDialog()
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
		LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
		// 标题栏
		GetPrivateProfileString(LANG_CHTYPE_DLG, _T("DIALOG_TITLE"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		SetWindowText(strLoad);
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
		GetPrivateProfileString(LANG_CHTYPE_DLG, _T("STATIC_TEXT"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		CStatic *pStatic = (CStatic *) GetDlgItem(IDC_STATIC_TEXT);
		pStatic -> SetWindowText(strLoad);
		delete []lpszLang;
		SetCurrentDirectory(m_strWorkPath);
	}

	// 以下为初始化图像列表过程
	m_wndImageList.Create(16, 16, ILC_COLOR24|ILC_MASK, 4, 4);						// 创建图像列表
	CBitmap bm;
	bm.LoadBitmap(IDB_BITMAP1);									// 载入Bitmap资源
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);							// 添加到图像列表
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP2);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP5);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP6);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP17);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP18);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP7);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP8);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP9);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP10);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP15);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP16);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP11);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP12);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP13);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.LoadBitmap(IDB_BITMAP14);
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);
	bm.DeleteObject();
	bm.Detach();

	m_ListCode.SetImageList(&m_wndImageList,LVSIL_SMALL); // 为列表控件添加图像列表
	// 列表视图控件初始化
	m_ListCode.SetExtendedStyle(LVS_EX_FULLROWSELECT); 
	m_ListCode.ModifyStyle(0, LVS_SINGLESEL);
	m_ListCode.InsertColumn(0,_T("代码类型"),LVCFMT_IMAGE|LVCFMT_LEFT,222,0);// 设置列 
	m_ListCode.InsertItem(0,_T("C++"),1);
	m_ListCode.InsertItem(1,_T("ANSI C"),3);
	m_ListCode.InsertItem(2,_T("C#.NET"),5);
	m_ListCode.InsertItem(3,_T("HTML"),7);
	m_ListCode.InsertItem(4,_T("Java Script"),9);
	m_ListCode.InsertItem(5,_T("VB Script"),11);
	m_ListCode.InsertItem(6,_T("SQL"),13);
	LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
	if (m_uiLanguage == 0)
		strLoad.LoadString(IDS_NORMAL_TEXT);
	else if (m_uiLanguage == 1)
	{
		SetCurrentDirectory(m_strAppPath);
		GetPrivateProfileString(LANG_STRING, _T("NORMAL_TEXT"), _T(""), lpszLang, LANG_STRING_LEN, LANG_TCHINESE_FILE);
		strLoad = lpszLang;
		SetCurrentDirectory(m_strWorkPath);
	}
	else if (m_uiLanguage == 2)
	{
		SetCurrentDirectory(m_strAppPath);
		GetPrivateProfileString(LANG_STRING, _T("NORMAL_TEXT"), _T(""), lpszLang, LANG_STRING_LEN, LANG_ENGLISH_FILE);
		strLoad = lpszLang;
		SetCurrentDirectory(m_strWorkPath);
	}
	delete []lpszLang;
	m_ListCode.InsertItem(7,strLoad,15);
	// 获取当前文档代码类型
	switch(m_CodeType)
	{
	case 0:
		m_ListCode.SetItemState(7,LVIS_SELECTED,LVIF_IMAGE);
		break;
	case 1:
		m_ListCode.SetItemState(0,LVIS_SELECTED,LVIF_IMAGE);
		break;
	case 2:
		m_ListCode.SetItemState(1,LVIS_SELECTED,LVIF_IMAGE);
		break;
	case 3:
		m_ListCode.SetItemState(2,LVIS_SELECTED,LVIF_IMAGE);
		break;
	case 4:
		m_ListCode.SetItemState(3,LVIS_SELECTED,LVIF_IMAGE);
		break;
	case 5:
		m_ListCode.SetItemState(4,LVIS_SELECTED,LVIF_IMAGE);
		break;
	case 6:
		m_ListCode.SetItemState(5,LVIS_SELECTED,LVIF_IMAGE);
		break;
	case 7:
		m_ListCode.SetItemState(6,LVIS_SELECTED,LVIF_IMAGE);
		break;
	}
	return TRUE;
}

void CChangeTypeDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uChanged == LVIF_STATE)	// 改变选择
	{
		if (pNMLV->uNewState)	// 列表项处于新状态
		{
			// 令选中项目的图标为绿色
			m_ListCode.SetItem(m_CodeType, 0, LVIF_IMAGE, NULL,2*(m_CodeType)+1, 0, 0, 0);
			m_ListCode.SetItem(pNMLV->iItem, 0, LVIF_IMAGE, NULL,2*(pNMLV->iItem), 0, 0, 0);
			m_CodeType = pNMLV->iItem;
		}
	}
	*pResult = 0;
}

void CChangeTypeDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	OnOK();
	*pResult = 0;
}

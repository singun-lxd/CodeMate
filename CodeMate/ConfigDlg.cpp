// ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CodeMate.h"
#include "ConfigDlg.h"

#define ID_FOLDER_BROWSE 1068
#define ID_LIST_KEYWORDS ID_FOLDER_BROWSE + 1
#define ID_COLOR_BUTTON1 ID_LIST_KEYWORDS + 1
#define ID_COLOR_BUTTON2 ID_COLOR_BUTTON1 + 1
#define ID_COLOR_BUTTON3 ID_COLOR_BUTTON2 + 1
#define ID_COLOR_BUTTON4 ID_COLOR_BUTTON3 + 1
#define ID_COLOR_BUTTON5 ID_COLOR_BUTTON4 + 1
#define ID_COLOR_BUTTON6 ID_COLOR_BUTTON5 + 1
#define ID_COLOR_BUTTON7 ID_COLOR_BUTTON6 + 1
#define ID_COLOR_BUTTON8 ID_COLOR_BUTTON7 + 1

// CConfigDlg 对话框

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

	CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigDlg::IDD, pParent)
	, isKeyWord(TRUE)
{

}

CConfigDlg::~CConfigDlg()
{
	m_arrKeyWord.RemoveAll();
	m_arrKeyWord.FreeExtra();
	m_arrConstant.RemoveAll();
	m_arrConstant.FreeExtra();

	CMFCVisualManager::DestroyInstance(TRUE);
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_FontShow);
	DDX_Control(pDX, IDC_TAB1, m_Tabs);
	DDX_Control(pDX, IDC_CHECK1, m_Check_LineNum);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_Tabs);
	DDX_Control(pDX, IDC_SPIN1, m_Spin);
	DDX_Control(pDX, IDC_BUTTON1, m_Button_Font);
	DDX_Control(pDX, IDC_LIST1, m_ListCode);
	DDX_Control(pDX, IDC_COMBO1, m_Combo_Encoding);
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CConfigDlg::OnBnClickedButton1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CConfigDlg::OnTcnSelchangeTab1)
	ON_EN_CHANGE(IDC_EDIT2, &CConfigDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_RADIO1, &CConfigDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CConfigDlg::OnBnClickedRadio2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CConfigDlg::OnLvnItemchangedList1)
END_MESSAGE_MAP()

// CConfigDlg 消息处理程序
BOOL CConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 设置主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
	// 加载控件并设置Tab键顺序，这么写是为了兼容VS2008SP1
	// 第一页
	CWnd * p1 = (GetDlgItem(IDC_TAB1));
	p1->SetWindowPos(&wndTop,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_POSITION));
	p1->SetWindowPos(GetDlgItem(IDC_TAB1),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_Folder_Browser.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOHSCROLL,CRect(30,70,308,93),this,ID_FOLDER_BROWSE);
	m_Folder_Browser.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );
	m_Folder_Browser.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_COLOR6));
	p1->SetWindowPos(GetDlgItem(ID_FOLDER_BROWSE),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_ColorForeground.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_TABSTOP,CRect(180,118,295,143),this,ID_COLOR_BUTTON1);
	m_ColorForeground.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_COLOR5));
	p1->SetWindowPos(GetDlgItem(ID_COLOR_BUTTON1),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_ColorBackground.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_TABSTOP,CRect(180,148,295,173),this,ID_COLOR_BUTTON2);
	m_ColorBackground.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_ADD));
	p1->SetWindowPos(GetDlgItem(ID_COLOR_BUTTON2),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_RADIO1));
	p1->SetWindowPos(GetDlgItem(IDC_STATIC_ADD),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_RADIO2));
	p1->SetWindowPos(GetDlgItem(IDC_RADIO1),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_LAN));
	p1->SetWindowPos(GetDlgItem(IDC_RADIO2),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_ListCode.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_VSListBox.Create(_T(""), WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_DISABLED,CRect(185,210,295,320),this,ID_LIST_KEYWORDS);
	m_VSListBox.SetWindowPos(GetDlgItem(IDC_LIST1),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	// 第二页
	p1 = (GetDlgItem(IDC_STATIC_ENCODING));
	p1->SetWindowPos(GetDlgItem(ID_LIST_KEYWORDS),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_Combo_Encoding.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_TABS));
	p1->SetWindowPos(GetDlgItem(IDC_COMBO1),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_Edit_Tabs.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_Spin.SetWindowPos(GetDlgItem(IDC_EDIT2),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_Check_LineNum.SetWindowPos(GetDlgItem(IDC_SPIN1),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_EDITOR));
	p1->SetWindowPos(GetDlgItem(IDC_CHECK1),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_COLOR7));
	p1->SetWindowPos(GetDlgItem(IDC_STATIC_EDITOR),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_ColorLineNumFG.Create(_T(""),WS_CHILD|WS_TABSTOP,CRect(180,125,295,150),this,ID_COLOR_BUTTON7);
	m_ColorLineNumFG.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_COLOR8));
	p1->SetWindowPos(GetDlgItem(ID_COLOR_BUTTON7),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_ColorLineNumBG.Create(_T(""),WS_CHILD|WS_TABSTOP,CRect(180,153,295,178),this,ID_COLOR_BUTTON8);
	m_ColorLineNumBG.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_COLOR1));
	p1->SetWindowPos(GetDlgItem(ID_COLOR_BUTTON8),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_ColorKeyword.Create(_T(""),WS_CHILD|WS_TABSTOP,CRect(180,181,295,206),this,ID_COLOR_BUTTON3);
	m_ColorKeyword.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_COLOR2));
	p1->SetWindowPos(GetDlgItem(ID_COLOR_BUTTON3),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_ColorConstant.Create(_T(""),WS_CHILD|WS_TABSTOP,CRect(180,209,295,234),this,ID_COLOR_BUTTON4);
	m_ColorConstant.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_COLOR3));
	p1->SetWindowPos(GetDlgItem(ID_COLOR_BUTTON4),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_ColorComment.Create(_T(""),WS_CHILD|WS_TABSTOP,CRect(180,237,295,262),this,ID_COLOR_BUTTON5);
	m_ColorComment.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	p1 = (GetDlgItem(IDC_STATIC_COLOR4));
	p1->SetWindowPos(GetDlgItem(ID_COLOR_BUTTON5),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	m_ColorText.Create(_T(""),WS_CHILD|WS_TABSTOP,CRect(180,265,295,290),this,ID_COLOR_BUTTON6);
	m_ColorText.SetWindowPos(p1,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	// 加载语言
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
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("DIALOG_TITLE"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		SetWindowText(strLoad);
		// 按钮
		CButton *pButton = NULL;
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("RADIO_KEYWORDS"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_RADIO1);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("RADIO_CONSTANTS"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_RADIO2);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_STRING, _T("DIALOG_OK"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDOK);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_STRING, _T("DIALOG_CANCEL"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDCANCEL);
		pButton -> SetWindowText(strLoad);
		// 颜色按钮
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("BUTTON_DEFAULT_COLOR"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		m_ColorLineNumBG.EnableAutomaticButton (strLoad, RGB(128,128,128));
		m_ColorLineNumFG.EnableAutomaticButton (strLoad, RGB(255,255,255));
		m_ColorKeyword.EnableAutomaticButton (strLoad, RGB(0,0,255));
		m_ColorConstant.EnableAutomaticButton (strLoad, RGB(128,128,128));
		m_ColorComment.EnableAutomaticButton (strLoad, RGB(0,128,0));
		m_ColorText.EnableAutomaticButton (strLoad, RGB(128,0,0));
		m_ColorBackground.EnableAutomaticButton(strLoad, RGB(255,255,255));
		m_ColorForeground.EnableAutomaticButton(strLoad, RGB(0,0,0));
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("BUTTON_MORE_COLOR"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		m_ColorLineNumBG.EnableOtherButton (strLoad);
		m_ColorLineNumFG.EnableOtherButton (strLoad);
		m_ColorKeyword.EnableOtherButton (strLoad);
		m_ColorConstant.EnableOtherButton (strLoad);
		m_ColorComment.EnableOtherButton (strLoad);
		m_ColorText.EnableOtherButton (strLoad);
		m_ColorBackground.EnableOtherButton(strLoad);
		m_ColorForeground.EnableOtherButton(strLoad);
		// 字符串
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STRING_BOLD"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		bold.Format(_T("%s"), lpszLang);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STRING_ITALIC"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		italic.Format(_T("%s"), lpszLang);
		// Tab 页
		CStatic *pStatic = NULL;
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_GENERAL"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		m_Tabs.InsertItem(0,strLoad);	// 添加标签页
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_EDITOR"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		m_Tabs.InsertItem(1,strLoad);	// 添加标签页
		// 复选框
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("CHECK_SHOW_LINE_NUMBERS"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		m_Check_LineNum.SetWindowText(strLoad);
		// 静态文本
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_LOCATION"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_POSITION);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_INTERFACE_COLORS"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_GENERALS);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_COLOR_FORE"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR6);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_COLOR_BACK"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR5);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_ADD_CONTENTS"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_ADD);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_LANGUAGE"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_LAN);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_DEFAULT_ENCODING"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_ENCODING);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_TAB_LENGTH"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_TABS);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_COLORS"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_EDITOR);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_COLOR_KEYWORD"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR1);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_COLOR_CONSTANT"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR2);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_COLOR_COMMENT"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR3);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_COLOR_STRING"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR4);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_COLOR_LINENUM_F"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR7);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_COLOR_LINENUM_B"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR8);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_CONFIG_DLG, _T("STATIC_FONT"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_FONT);
		pStatic -> SetWindowText(strLoad);
		delete []lpszLang;
		SetCurrentDirectory(m_strWorkPath);
	}
	else
	{
		strLoad.LoadString(IDS_TAB_APPEARANCE);
		m_Tabs.InsertItem(0,strLoad);	// 添加标签页
		strLoad.LoadString(IDS_TAB_EDITOR);
		m_Tabs.InsertItem(1,strLoad);	// 添加标签页
		strLoad.LoadString(IDS_DEFAULT_COLOR);
		m_ColorLineNumBG.EnableAutomaticButton (strLoad, RGB(128,128,128));
		m_ColorLineNumFG.EnableAutomaticButton (strLoad, RGB(255,255,255));
		m_ColorKeyword.EnableAutomaticButton (strLoad, RGB(0,0,255));
		m_ColorConstant.EnableAutomaticButton (strLoad, RGB(128,128,128));
		m_ColorComment.EnableAutomaticButton (strLoad, RGB(0,128,0));
		m_ColorText.EnableAutomaticButton (strLoad, RGB(128,0,0));
		m_ColorBackground.EnableAutomaticButton(strLoad, RGB(255,255,255));
		m_ColorForeground.EnableAutomaticButton(strLoad, RGB(0,0,0));
		strLoad.LoadString(IDS_MORE_COLOR);
		m_ColorLineNumBG.EnableOtherButton (strLoad);
		m_ColorLineNumFG.EnableOtherButton (strLoad);
		m_ColorKeyword.EnableOtherButton (strLoad);
		m_ColorConstant.EnableOtherButton (strLoad);
		m_ColorComment.EnableOtherButton (strLoad);
		m_ColorText.EnableOtherButton (strLoad);
		m_ColorBackground.EnableOtherButton(strLoad);
		m_ColorForeground.EnableOtherButton(strLoad);
	}
	// 控件值初始化
	m_Folder_Browser.EnableFolderBrowseButton();
	m_Folder_Browser.SetFont(m_Edit_Tabs.GetFont());
	m_Folder_Browser.SetWindowText(m_strWorkPath);
	// 文本居中
	CRect rc = CRect(0,0,0,0);
	m_Folder_Browser.GetClientRect(&rc);
	CDC* pDC = m_Folder_Browser.GetDC();
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	int nMargin = (rc.Height() - nFontHeight) / 2;
	rc.DeflateRect(0,nMargin);
	m_Folder_Browser.SetRectNP(&rc);
	// 继续初始化
	m_VSListBox.SetStandardButtons();
	CButton * pRadio = (CButton *)GetDlgItem(IDC_RADIO1);
	pRadio->SetCheck(1);
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
	bm.Detach();
	m_ListCode.SetImageList(&m_wndImageList,LVSIL_SMALL); // 为列表控件添加图像列表
	// 列表视图控件初始化
	m_ListCode.SetExtendedStyle(LVS_EX_FULLROWSELECT); 
	m_ListCode.ModifyStyle(0, LVS_SINGLESEL);
	m_ListCode.InsertColumn(0,_T("代码类型"),LVCFMT_IMAGE|LVCFMT_LEFT,110,0);// 设置列 
	m_ListCode.InsertItem(0,_T("C++"),1);
	m_ListCode.InsertItem(1,_T("ANSI C"),3);
	m_ListCode.InsertItem(2,_T("C#.NET"),5);
	m_ListCode.InsertItem(3,_T("HTML"),7);
	m_ListCode.InsertItem(4,_T("Java Script"),9);
	m_ListCode.InsertItem(5,_T("VB Script"),11);
	m_ListCode.InsertItem(6,_T("SQL"),13);
	m_CodeType = -1;
	// 添加编码列表
	m_Combo_Encoding.InsertString(0, _T("ANSI"));
	m_Combo_Encoding.InsertString(1, _T("Unicode"));
	m_Combo_Encoding.InsertString(2, _T("Unicode big endian"));
	m_Combo_Encoding.InsertString(3, _T("UTF-8"));
	m_Combo_Encoding.SetCurSel(m_uiDefEncode);
	// 初始化其他控件
	m_Spin.SetRange(1,8);
	name.Format(_T("%d"), m_iTabLen);
	m_Edit_Tabs.SetWindowText(name);
	m_Check_LineNum.SetCheck(m_bShowLineNum);
	m_ColorLineNumBG.SetColor(m_coColor.crLineNumBGColor);
	m_ColorLineNumFG.SetColor(m_coColor.crLineNumFGColor);
	m_ColorKeyword.SetColor(m_coColor.crKeywordColor);
	m_ColorConstant.SetColor(m_coColor.crConstantColor);
	m_ColorComment.SetColor(m_coColor.crCommentColor);
	m_ColorText.SetColor(m_coColor.crTextColor);
	m_ColorBackground.SetColor(m_coColor.crBackgroundColor);
	m_ColorForeground.SetColor(m_coColor.crForegroundColor);
	m_ColorLineNumBG.SetColumnsNumber(6);
	m_ColorLineNumFG.SetColumnsNumber(6);
	m_ColorKeyword.SetColumnsNumber(6);
	m_ColorConstant.SetColumnsNumber(6);
	m_ColorComment.SetColumnsNumber(6);
	m_ColorText.SetColumnsNumber(6);
	m_ColorBackground.SetColumnsNumber(6);
	m_ColorForeground.SetColumnsNumber(6);
	name.Format(_T("%s, %d"), font.lfFaceName, m_iSize);
	CString s;
	s.Format(_T(", %d"), font.lfCharSet);
	name += s;
	if (font.lfWeight == FW_BOLD)
	{
		if (m_uiLanguage == 0)
			bold.LoadString(IDS_FONT_BOLD);
		s.Format(_T(", %s"), bold);
		name += s;
	}
	if (font.lfItalic == (BYTE)TRUE)
	{
		if (m_uiLanguage == 0)
			italic.LoadString(IDS_FONT_ITALIC);
		s.Format(_T(", %s"), italic);
		name += s;
	}
	m_FontShow.SetWindowText(name);

	return TRUE;
}

HBRUSH CConfigDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
	int nID = pWnd->GetDlgCtrlID();
	if(nID == IDC_CHECK1 || nID == IDC_RADIO1 || nID == IDC_RADIO2 || nID == IDC_EDIT1
		|| nID == IDC_STATIC_GENERALS || nID == IDC_STATIC_EDITOR || nID == IDC_STATIC_ADD)
	{
		HBRUSH hbru = ::CreateSolidBrush (RGB(255,255,255));
		return hbru; 
	}

	if(nCtlColor == CTLCOLOR_STATIC) // 设定控件的属性,对所有的标签
	{
		pDC->SetBkMode(TRANSPARENT);
		return HBRUSH(GetStockObject(HOLLOW_BRUSH));
	}
	else 
	{ 
		return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor); 
	} 
}

void CConfigDlg::OnBnClickedButton1()
{
	CFontDialog dlgFont(&font, CF_SCREENFONTS | CF_FORCEFONTEXIST | CF_TTONLY | CF_FIXEDPITCHONLY);

	if (dlgFont.DoModal() == IDOK)
	{
		// 保存各种设置信息到变量
		dlgFont.GetCurrentFont(&font);
		name = dlgFont.GetFaceName();
		m_iSize = dlgFont.GetSize()/10;
		CString s;
		s.Format(_T(", %d"), m_iSize);
		name += s;
		s.Format(_T(", %d"), font.lfCharSet);
		name += s;
		if (dlgFont.IsBold())
		{
			if (m_uiLanguage == 0)
				bold.LoadString(IDS_FONT_BOLD);
			s.Format(_T(", %s"), bold);
			name += s;
		}
		if (dlgFont.IsItalic())
		{
			if (m_uiLanguage == 0)
				italic.LoadString(IDS_FONT_ITALIC);
			s.Format(_T(", %s"), italic);
			name += s;
		}
		m_FontShow.SetWindowText(name);
	}
}

void CConfigDlg::OnBnClickedOk()
{
	// 获取设置信息
	CString NewPath;
	m_Folder_Browser.GetWindowText(NewPath);
	if (!PathFileExists(NewPath))
	{
		CString strContent, strTitle;
		if (m_uiLanguage == 0)
		{
			strContent.LoadString(IDS_MSGBOX_PATH_INVILID);
			strTitle.LoadString(IDS_MSGBOX_WARN);
		}
		else
		{
			LPWSTR strFileName = NULL;
			LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
			if (m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			SetCurrentDirectory(m_strAppPath);
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_INVILID_LOCATION"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_WARNING"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
			strTitle = lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		MessageBox(strContent, strTitle, MB_ICONWARNING);
		return;
	}
	if (NewPath != m_strWorkPath)
	{
		// 设置尚未移动标志
		isMoved = false;
		// 添加斜杠
		if (m_strWorkPath.Right(1) != _T("\\")) m_strWorkPath += _T("\\");
		CString strFromPath = m_strWorkPath + _T("Code");
		if (NewPath.Right(1) != _T("\\")) NewPath += _T("\\");
		CString strToPath = NewPath + _T("Code");
		// 创建变量
		CString strContent,strTitle;
		LPWSTR strFileName = NULL;
		LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
		if (m_uiLanguage == 0)
		{
			strContent.LoadString(IDS_MSGBOX_PATHCHANGE);
			strTitle.LoadString(IDS_MSGBOX_QUES);
		}
		else
		{
			if (m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			SetCurrentDirectory(m_strAppPath);
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_MOVE_ALL"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_QUESTION"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
			strTitle = lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		// 是否移动
		if (MessageBox(strContent, strTitle, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			if (!MoveFolder(strFromPath, strToPath))	// 如果移动失败
			{
				CString strContent,strTitle;
				if (m_uiLanguage == 0)
				{
					strTitle.LoadString(IDS_MSGBOX_ERROR);
					strContent.LoadString(IDS_MSGBOX_MOVETOERR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_MOVETO_ERROR"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_ERROR"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
					strTitle = lpszLang;
					SetCurrentDirectory(m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				m_Folder_Browser.SetFocus();
				return;
			}
			else	// 移动分类成功
			{
				isMoved = true;
				m_strWorkPath = NewPath;
			}
		}
		else
		{
			CString strLoad;
			// 判断路径是否存在   
			if  ( !PathIsDirectory(strToPath))
			{
				if ( CreateDirectory(strToPath, NULL))
				{
					if (m_uiLanguage == 0)
						strLoad.LoadString(IDS_ITEM_SHOW);
					else
					{
						SetCurrentDirectory(m_strAppPath);
						GetPrivateProfileString(LANG_MESSAGEBOX, _T("TREE_NORMAL_CLASS"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
						strLoad = lpszLang;
						SetCurrentDirectory(m_strWorkPath);
					}
					strToPath = strToPath + _T("\\") + strLoad;
					CreateDirectory(strToPath, NULL);
				}
				else
					AfxMessageBox(_T("致命错误：未能创建分类。"), MB_ICONERROR);
			}
			m_strWorkPath = NewPath;
		}
		delete []lpszLang;
	}
	// 保存关键词
	if (m_CodeType != -1) SaveWords();
	// 保存各个设置变量内容
	m_uiDefEncode = m_Combo_Encoding.GetCurSel();
	CString strNum;
	m_Edit_Tabs.GetWindowText(strNum);
	m_iTabLen = _ttoi(strNum);
	m_bShowLineNum = m_Check_LineNum.GetCheck();
	m_coColor.crKeywordColor = m_ColorKeyword.GetColor();
	m_coColor.crConstantColor = m_ColorConstant.GetColor();
	m_coColor.crCommentColor = m_ColorComment.GetColor();
	m_coColor.crTextColor = m_ColorText.GetColor();
	m_coColor.crBackgroundColor = m_ColorBackground.GetColor();
	m_coColor.crForegroundColor = m_ColorForeground.GetColor();
	m_coColor.crLineNumBGColor = m_ColorLineNumBG.GetColor();
	m_coColor.crLineNumFGColor = m_ColorLineNumFG.GetColor();
	if (m_coColor.crKeywordColor == -1 )
		m_coColor.crKeywordColor = m_ColorKeyword.GetAutomaticColor();
	if (m_coColor.crConstantColor == -1 )
		m_coColor.crConstantColor = m_ColorConstant.GetAutomaticColor();
	if (m_coColor.crCommentColor  == -1 )
		m_coColor.crCommentColor  = m_ColorComment.GetAutomaticColor();
	if (m_coColor.crTextColor == -1 )
		m_coColor.crTextColor = m_ColorText.GetAutomaticColor();
	if (m_coColor.crBackgroundColor  == -1 )
		m_coColor.crBackgroundColor  = m_ColorBackground.GetAutomaticColor();
	if (m_coColor.crForegroundColor  == -1 )
		m_coColor.crForegroundColor  = m_ColorForeground.GetAutomaticColor();
	if (m_coColor.crLineNumBGColor  == -1 )
		m_coColor.crLineNumBGColor  = m_ColorLineNumBG.GetAutomaticColor();
	if (m_coColor.crLineNumFGColor  == -1 )
		m_coColor.crLineNumFGColor  = m_ColorLineNumFG.GetAutomaticColor();

	CDialogEx::OnOK();
}

bool CConfigDlg::MoveFolder(CString strFromPath,CString strToPath)
{
	// 字符串类型转换
	WCHAR *strChar1,*strChar2;
	strChar1=strFromPath.GetBuffer(strFromPath.GetLength()+1);
	strChar1[_tcslen(strChar1)+1]=0;
	strChar2=strToPath.GetBuffer(strToPath.GetLength()+1);
	strChar2[_tcslen(strChar2)+1]=0;
	// 定义结构并分配空间
	SHFILEOPSTRUCTW FileOp;
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCTW));
	// 设置参数：移动
	FileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION ;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = strFromPath;
	FileOp.pTo = strToPath;
	FileOp.wFunc = FO_MOVE;
	// 调用API函数并返回结果
	return SHFileOperation(&FileOp) == 0;
}

void CConfigDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int i = m_Tabs.GetCurSel();//所选标签号
	CStatic *pStatic = NULL;
	CButton *pButton = NULL;
	switch(i)
	{
	case 0:
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_POSITION);
		pStatic->ShowWindow(SW_SHOW);
		m_Folder_Browser.ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_GENERALS);
		pStatic->ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR6);
		pStatic->ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR5);
		pStatic->ShowWindow(SW_SHOW);
		m_ColorForeground.ShowWindow(SW_SHOW);
		m_ColorBackground.ShowWindow(SW_SHOW);
		pStatic = (CStatic *)GetDlgItem(IDC_STATIC_ADD);
		pStatic->ShowWindow(SW_SHOW);
		pButton = (CButton *)GetDlgItem(IDC_RADIO1);
		pButton->ShowWindow(SW_SHOW);
		pButton = (CButton *)GetDlgItem(IDC_RADIO2);
		pButton->ShowWindow(SW_SHOW);
		pStatic = (CStatic *)GetDlgItem(IDC_STATIC_LAN);
		pStatic->ShowWindow(SW_SHOW);
		m_ListCode.ShowWindow(SW_SHOW);
		m_VSListBox.ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_ENCODING);
		pStatic->ShowWindow(SW_HIDE);
		m_Combo_Encoding.ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_TABS);
		pStatic->ShowWindow(SW_HIDE);
		m_Edit_Tabs.ShowWindow(SW_HIDE);
		m_Spin.ShowWindow(SW_HIDE);
		m_Check_LineNum.ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_EDITOR);
		pStatic->ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR1);
		pStatic->ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR2);
		pStatic->ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR3);
		pStatic->ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR4);
		pStatic->ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR7);
		pStatic->ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR8);
		pStatic->ShowWindow(SW_HIDE);
		m_ColorLineNumBG.ShowWindow(SW_HIDE);
		m_ColorLineNumFG.ShowWindow(SW_HIDE);
		m_ColorKeyword.ShowWindow(SW_HIDE);
		m_ColorConstant.ShowWindow(SW_HIDE);
		m_ColorComment.ShowWindow(SW_HIDE);
		m_ColorText.ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_FONT);
		pStatic->ShowWindow(SW_HIDE);
		m_FontShow.ShowWindow(SW_HIDE);
		m_Button_Font.ShowWindow(SW_HIDE);
		m_Folder_Browser.SetFocus();
		break;
	case 1:
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_POSITION);
		pStatic->ShowWindow(SW_HIDE);
		m_Folder_Browser.ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_GENERALS);
		pStatic->ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR6);
		pStatic->ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR5);
		pStatic->ShowWindow(SW_HIDE);
		m_ColorForeground.ShowWindow(SW_HIDE);
		m_ColorBackground.ShowWindow(SW_HIDE);
		pStatic = (CStatic *)GetDlgItem(IDC_STATIC_ADD);
		pStatic->ShowWindow(SW_HIDE);
		pButton = (CButton *)GetDlgItem(IDC_RADIO1);
		pButton->ShowWindow(SW_HIDE);
		pButton = (CButton *)GetDlgItem(IDC_RADIO2);
		pButton->ShowWindow(SW_HIDE);
		pStatic = (CStatic *)GetDlgItem(IDC_STATIC_LAN);
		pStatic->ShowWindow(SW_HIDE);
		m_ListCode.ShowWindow(SW_HIDE);
		m_VSListBox.ShowWindow(SW_HIDE);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_ENCODING);
		pStatic->ShowWindow(SW_SHOW);
		m_Combo_Encoding.ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_TABS);
		pStatic->ShowWindow(SW_SHOW);
		m_Edit_Tabs.ShowWindow(SW_SHOW);
		m_Spin.ShowWindow(SW_SHOW);
		m_Check_LineNum.ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_EDITOR);
		pStatic->ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR1);
		pStatic->ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR2);
		pStatic->ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR3);
		pStatic->ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR4);
		pStatic->ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR7);
		pStatic->ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_COLOR8);
		pStatic->ShowWindow(SW_SHOW);
		m_ColorLineNumBG.ShowWindow(SW_SHOW);
		m_ColorLineNumFG.ShowWindow(SW_SHOW);
		m_ColorKeyword.ShowWindow(SW_SHOW);
		m_ColorConstant.ShowWindow(SW_SHOW);
		m_ColorComment.ShowWindow(SW_SHOW);
		m_ColorText.ShowWindow(SW_SHOW);
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_FONT);
		pStatic->ShowWindow(SW_SHOW);
		m_FontShow.ShowWindow(SW_SHOW);
		m_Button_Font.ShowWindow(SW_SHOW);
		m_Combo_Encoding.SetFocus();
		break;
	}
	*pResult = 0;
}

void CConfigDlg::OnEnChangeEdit2()
{
	CString num;
	CEdit *p = (CEdit *)GetDlgItem(IDC_EDIT2);
	p->GetWindowText(num);
	int value = _ttoi(num);
	if (value > 8)
	{
		num.Format(_T("%d"), 8);
		p->SetWindowText(num);
	}
	else if (value == 0)
	{
		p->SetWindowText(_T("1"));
	}

}

void CConfigDlg::OnBnClickedRadio1()
{
	if (isKeyWord == FALSE)
	{
		if (m_CodeType != -1)
		{
			// 保存配置
			SaveWords();
			// 清空列表
			for (int i = m_VSListBox.GetCount() - 1; i >= 0; i--)
				m_VSListBox.RemoveItem(i);
			// 读取配置
			AddWords((LPTSTR)(LPCTSTR)m_arrKeyWord[m_CodeType], m_arrKeyWord[m_CodeType].GetLength());
		}
		isKeyWord = TRUE;
	}
}

void CConfigDlg::OnBnClickedRadio2()
{
	if (isKeyWord == TRUE)
	{
		if (m_CodeType != -1)
		{
			// 保存配置
			SaveWords();
			// 清空列表
			for (int i = m_VSListBox.GetCount() - 1; i >= 0; i--)
				m_VSListBox.RemoveItem(i);
			// 读取配置
			AddWords((LPTSTR)(LPCTSTR)m_arrConstant[m_CodeType], m_arrKeyWord[m_CodeType].GetLength());
		}
		isKeyWord = FALSE;
	}
}

void CConfigDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uChanged == LVIF_STATE)	// 改变选择
	{
		if (pNMLV->uNewState)	// 列表项处于新状态
		{
			if (m_CodeType != -1)
			{
				// 保存配置
				SaveWords();
				// 令选中项目的图标为绿色
				m_ListCode.SetItem(m_CodeType, 0, LVIF_IMAGE, NULL,2*(m_CodeType)+1, 0, 0, 0);
				for (int i = m_VSListBox.GetCount() - 1; i >= 0; i--)
					m_VSListBox.RemoveItem(i);
			}
			m_ListCode.SetItem(pNMLV->iItem, 0, LVIF_IMAGE, NULL,2*(pNMLV->iItem), 0, 0, 0);
			m_CodeType = pNMLV->iItem;
			if (isKeyWord)
			{
				// 读取配置
				AddWords((LPTSTR)(LPCTSTR)m_arrKeyWord[m_CodeType], m_arrKeyWord[m_CodeType].GetLength());
			}
			else
			{
				// 读取配置
				AddWords((LPTSTR)(LPCTSTR)m_arrConstant[m_CodeType], m_arrKeyWord[m_CodeType].GetLength());
			}
		}
	}
	if (m_CodeType == -1) m_VSListBox.EnableWindow();
	*pResult = 0;
}

void CConfigDlg::AddWords(LPTSTR lpszKeywords, int iLen)
{
	// 提取关键词
	int iOffset = 0;
	int iOldOffset = iOffset;
	int iCount;
	LPTSTR lpszNextKeyword = NULL;
	LPTSTR KeyWordsNow = new _TCHAR[1024];
	while ((lpszNextKeyword=_tcsstr(lpszKeywords+iOffset, _T(";"))) != NULL)
	{
		// 提取关键词
		iCount = (int)(lpszNextKeyword - (lpszKeywords+iOffset));
		iOffset = (int)(lpszNextKeyword - lpszKeywords);
		_tcsncpy_s(KeyWordsNow, 1024, lpszKeywords+iOldOffset, iCount);
		KeyWordsNow[iCount] = '\0';
		m_VSListBox.AddItem((CString)KeyWordsNow);
		iOffset++;
		iOldOffset = iOffset;
	}
	delete KeyWordsNow;
}

void CConfigDlg::SaveWords()
{
	if (isKeyWord)
	{
		// 清空关键词
		m_arrKeyWord[m_CodeType].Empty();
		// 添加到列表
		for (int i = 0; i < m_VSListBox.GetCount(); i++)
			m_arrKeyWord[m_CodeType] += (m_VSListBox.GetItemText(i) + _T(";"));
	}
	else
	{
		// 清空关键词
		m_arrConstant[m_CodeType].Empty();
		// 添加到列表
		for (int i = 0; i < m_VSListBox.GetCount(); i++)
			m_arrConstant[m_CodeType] += (m_VSListBox.GetItemText(i) + _T(";"));
	}
}
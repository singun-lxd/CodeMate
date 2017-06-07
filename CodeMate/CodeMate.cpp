
// CodeMate.cpp : 定义应用程序的类行为。
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "CodeMateDlg.h"

#pragma comment(lib, "SkinPPWTL.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define defWinUniqueMutex	_T("{CodeMate_LiuXindian_201110211500}")
#define defWinClass			_T("CodeMateClass")

// CCodeMateApp

BEGIN_MESSAGE_MAP(CCodeMateApp, CWinApp)
END_MESSAGE_MAP()


// CCodeMateApp 构造

CCodeMateApp::CCodeMateApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CCodeMateApp 对象

CCodeMateApp theApp;


// CCodeMateApp 初始化

BOOL CCodeMateApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	// 则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	WNDCLASS wc;
	// 获取窗口类信息。MFC 默认的所有对话框的窗口类名为 #32770 
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);
	// 改变窗口类名 
	wc.lpszClassName = defWinClass;
	// 注册新窗口类，使程序能使用它 
	AfxRegisterClass(&wc);

	CWinApp::InitInstance();

	bool bSkinLoad = false;
	LPTSTR lpszWords = new _TCHAR[LANG_STRING_LEN];
	GetPrivateProfileString(NAME_MAIN_FRAME, _T("Skin"), _T("AquaOS.ssk"),
		lpszWords, LANG_STRING_LEN, CONFIG_FILE_NAME);

	CString strSkin;
	strSkin.Format(_T("%s"), lpszWords);
	delete []lpszWords;
	if (strSkin.IsEmpty())
	{
		bSkinLoad = (skinppLoadSkin("skins\\AquaOS.ssk") != FALSE);
		if (!bSkinLoad)
			AfxMessageBox(_T("加载皮肤文件 AquaOS.ssk 失败！"));
	}
	else
	{
		if (strSkin != _T("<系统主题>"))
		{
			CString PathName = _T("Skins\\") + strSkin;
			char *pText = NULL;
#ifdef _UNICODE
			int n = PathName.GetLength();
			int len = WideCharToMultiByte(CP_ACP,0,PathName,n,NULL,0,NULL,NULL);
			pText = new char[len+1];
			WideCharToMultiByte(CP_ACP,0,PathName,n,pText,len,NULL,NULL);
			pText[len] = '\0';
#else
			int len = PathName.GetLength();
			pText = new char[len+1];
			strcpy_s(pText, len+1, PathName.GetBuffer(0));
			pText[len] = '\0';
#endif
			bSkinLoad = (skinppLoadSkin(pText) != FALSE);
			if (!bSkinLoad)
				AfxMessageBox(_T("加载皮肤文件 ") + strSkin + _T(" 失败！"));

			delete []pText;
		}
	}

	AfxEnableControlContainer();

	// 使用 Mutex 让程序只能运行一个实例
	HANDLE hMutex;
	hMutex = CreateMutex(NULL, true, defWinUniqueMutex);
	if(hMutex)
	{
		HWND hWnd = NULL;
		if(ERROR_ALREADY_EXISTS == GetLastError())
		{
			hWnd = ::FindWindow(defWinClass, NULL);
			if (hWnd != NULL)
			{
				if (::IsIconic(hWnd))
					::ShowWindow(hWnd, SW_RESTORE);

				hWnd = ::GetLastActivePopup(hWnd);
				::SwitchToThisWindow(hWnd, TRUE);
			}
			return FALSE;
		}
	}
	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	CCodeMateDlg dlgMain;
	dlgMain.m_bSkinLoad = bSkinLoad;
	m_pMainWnd = &dlgMain;
	INT_PTR nResponse = dlgMain.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		// “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		// “取消”来关闭对话框的代码
	}


	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}


	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

int CCodeMateApp::ExitInstance()
{
	skinppExitSkin();
	return 0;
}
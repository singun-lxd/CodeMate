#pragma once


// CMyToolBar

class CMyToolBar : public CToolBar
{
	DECLARE_DYNAMIC(CMyToolBar)

public:
	CMyToolBar();
	virtual ~CMyToolBar();

protected:
	DECLARE_MESSAGE_MAP()

private:
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);	// 处理用于处理界面更新的 WM_IDLEUPDATECMDUI 消息 
	BOOL   SetTrueColorToolBar(UINT uToolBarType, UINT uToolBar, int nBtnWidth);		// 设置真彩图标

public: 
	BOOL   LoadTrueColorToolBar(int nBtnWidth, UINT uToolBar, UINT uToolBarHot, UINT uToolBarDisabled = 0);		// 加载真彩图标
};



#pragma once


// CSplitter

class CSplitter : public CBitmapButton
{
	DECLARE_DYNAMIC(CSplitter)

public:
	CSplitter();
	virtual ~CSplitter();
	BOOL AutoLoad(UINT nID, CWnd* pParent);     // 重写载入函数
	void DrawItem(LPDRAWITEMSTRUCT lpDIS);		// 重写绘制函数

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);	// 鼠标左键按下
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);	// 鼠标左键弹起
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	// 鼠标移动

	DECLARE_MESSAGE_MAP()
};



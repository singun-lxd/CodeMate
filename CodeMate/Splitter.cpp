// Splitter.cpp : 实现文件
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "Splitter.h"
#include "CodeMateDlg.h"

// CSplitter

IMPLEMENT_DYNAMIC(CSplitter, CBitmapButton)
CSplitter::CSplitter()
{
}

CSplitter::~CSplitter()
{
}


BEGIN_MESSAGE_MAP(CSplitter, CBitmapButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CSplitter 消息处理程序
BOOL CSplitter::AutoLoad(UINT nID, CWnd* pParent)
{
	// 第一次和对话框控件关联
	if (!SubclassDlgItem(nID, pParent))
		return FALSE;

	CString buttonName;
	GetWindowText(buttonName);
	ASSERT(!buttonName.IsEmpty());      // 必须提供标题

	LoadBitmaps(buttonName + _T("U"), buttonName + _T("D"),
		buttonName + _T("F"), buttonName + _T("X"));

	// 需要至少主位图
	if (m_bitmap.m_hObject == NULL)
		return FALSE;

	return TRUE;
}

void CSplitter::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	// 至少要有一幅位图才能调用此函数
	ASSERT(m_bitmap.m_hObject != NULL);     // 必须的

	// 主位图在上，选中位图在下
	CBitmap* pBitmap = &m_bitmap;
	UINT state = lpDIS->itemState;
	if ((state & ODS_SELECTED) && m_bitmapSel.m_hObject != NULL)
		pBitmap = &m_bitmapSel;
	else if ((state & ODS_FOCUS) && m_bitmapFocus.m_hObject != NULL)
		pBitmap = &m_bitmapFocus;   // 第三个图像表示有焦点状态
	else if ((state & ODS_DISABLED) && m_bitmapDisabled.m_hObject != NULL)
		pBitmap = &m_bitmapDisabled;   // 最后的图像表示无效状态

	// 绘制整个按钮
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap* pOld = memDC.SelectObject(pBitmap);
	if (pOld == NULL)
		return;
	// 拉伸图片
	CRect rect;
	rect.CopyRect(&lpDIS->rcItem);
	BITMAP bits;
	pBitmap->GetObject(sizeof(BITMAP),&bits);
	pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),
		&memDC,0,0,bits.bmWidth, bits.bmHeight, SRCCOPY);

	memDC.SelectObject(pOld);
}

void CSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// 加载并显示切分鼠标指针
	HCURSOR hCursor = (HCURSOR)AfxGetApp()->LoadCursor(IDC_CURSOR_SPLITTER);
	::SetCursor(hCursor);
	// 执行主框架处理函数
	((CCodeMateDlg*)GetParent())->OnSplitterDown(this->GetDlgCtrlID(), point);
	CButton::OnLButtonDown(nFlags, point);
}

void CSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// 执行主框架处理函数
	((CCodeMateDlg*)GetParent())->OnSplitterUp(this->GetDlgCtrlID(), point);
	CButton::OnLButtonUp(nFlags, point);
}

void CSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	// 加载并显示切分鼠标指针
	HCURSOR hCursor = (HCURSOR)AfxGetApp()->LoadCursor(IDC_CURSOR_SPLITTER);
	::SetCursor(hCursor);
	// 执行主框架处理函数
	((CCodeMateDlg*)GetParent())->OnSplitterMove(this->GetDlgCtrlID(), point);
	CButton::OnMouseMove(nFlags, point);
}
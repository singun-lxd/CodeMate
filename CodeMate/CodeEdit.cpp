// CodeEdit.cpp :  实现文件
// 基于 CUltraEditWnd 改进
// 请查看 http://www.codeproject.com/KB/edit/CUltraPadWnd.aspx
// 开源协议 CPOL http://www.codeproject.com/info/cpol10.aspx
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "CodeEdit.h"
#include "CodeMateDlg.h"

// CCodeEdit
IMPLEMENT_DYNAMIC(CCodeEdit, CWnd)

	CCodeEdit::CCodeEdit()
{
	// 初始化成员
	m_iSize = 2;
	m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
	m_lpszText[0] = '\r';
	m_lpszText[1] = '\n';
	m_Font.CreateFont(-15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("Courier New"));
	m_iLineStartSelection = m_iLineEndSelection = -1;
	m_iCharStartSelection = m_iCharEndSelection = -1;
	m_iNumberLines = 1;
	m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
	m_UndoInfo.nLen = 0;
	m_UndoInfo.iChar = 0;
	m_UndoInfo.iLine = 0;
	m_UndoInfo.iEndChar = 0;
	m_UndoInfo.iEndLine = 0;
	m_UndoInfo.iLastLineLen = 0;
	m_lpLineInfo = (LPLINEINFO)malloc(m_iNumberLines*sizeof(LINEINFO));
	m_lpLineInfo[m_iNumberLines-1].dwOffset = 0;
	m_lpLineInfo[m_iNumberLines-1].nLen = 0;
	m_crKeywordColor = RGB(0,0,0);
	m_lpKeywordInfo = NULL;
	m_iNumberKeywords = 0;
	m_bKeywordsCaseSensitive = NULL;
	m_crConstantColor = RGB(0,0,0);
	m_lpConstantInfo = NULL;
	m_iNumberConstants = 0;
	m_bConstantsCaseSensitive = NULL;
	m_bUper = NULL;
	m_crCommentColor = RGB(0,0,0);
	m_crTextColor = RGB(0,0,0);
	m_crCodeColor = RGB(0,0,0);
	m_FileEncoding = FET_UNICODE;
	m_SyntaxColoring = SCT_NONE;
	m_iCurrentLine = 0;
	m_iCurrentChar = 0;
	m_iTABs = 4;
	m_bComment = FALSE;
	m_bText = FALSE;
	m_bSelecting = FALSE;
	m_bCopying = FALSE;
	m_crBgColor = RGB(255,255,255);
	m_crLineNumFgColor = RGB(255,255,255);
	m_crLineNumBgColor = RGB(128,128,128);
	m_lpfnLineDrawProc = NULL;
	m_changed = FALSE;
	m_bCanUndo = FALSE;
	m_LineNumbers = TRUE;
}

CCodeEdit::~CCodeEdit()
{
	// 销毁光标
	DestroyCaret();

	// 清除数据
	if (m_lpszText != NULL)
		free(m_lpszText);
	if (m_lpLineInfo != NULL)
	{
		free(m_lpLineInfo);
		m_lpLineInfo = NULL;
	}
	if (m_UndoInfo.lpszUndoData != NULL)
		free(m_UndoInfo.lpszUndoData);
	for (int i=0; i<m_iNumberKeywords; i++)
		delete m_lpKeywordInfo[i].lpszTagName;
	free(m_lpKeywordInfo);
	m_lpKeywordInfo = NULL;
	for (int i=0; i<m_iNumberConstants; i++)
		delete m_lpConstantInfo[i].lpszTagName;
	free(m_lpConstantInfo);
	m_lpConstantInfo = NULL;

	// 清除成员
	if (m_MemDC.m_hDC != NULL)
	{
		m_MemDC.SelectObject(m_pOldMemBitmap);
		m_MemDC.SelectObject(m_pOldFont);
		m_MemDC.DeleteDC();
		m_MemBitmap.DeleteObject();
		m_Font.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CCodeEdit, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEWHEEL()
	ON_LBN_DBLCLK(IDC_LIST_AUTOCOMPLETE, &CCodeEdit::OnLbnDblclkListAutoComplete)
END_MESSAGE_MAP()



// CCodeEdit 消息处理程序
BOOL CCodeEdit::Create(DWORD dwExStyle, DWORD dwStyle, RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::CreateEx(dwExStyle, NULL, NULL, dwStyle, rect, pParentWnd, nID, NULL);
}

void CCodeEdit::DrawContent()
{
	// 文档不为空
	if (m_lpszText != NULL)
	{
		// 获得客户区的大小和位置
		RECT rcClient;
		GetClientRect(&rcClient);

		// 获得滚动条信息
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// 获得可见行的范围
		POINT ptTop = {0, rcClient.top+iVerticalOffset};
		POINT ptBottom = {0, (rcClient.bottom-rcClient.top)+iVerticalOffset};
		int iLineStart = LineFromPosition(ptTop);
		int iLineEnd = LineFromPosition(ptBottom);

		// 处理不可见的行
		m_bComment = FALSE;
		m_bText = FALSE;
		int iTmpOffset = 0;
		int iTmpOldOffset = iTmpOffset;
		int iTmpCurrentLine = 0;
		LPTSTR lpszTmpNextLine = NULL;
		while ((lpszTmpNextLine=GetLine(iTmpOffset)) != NULL)
		{
			// 仅对不可见的行进行处理
			if (iTmpCurrentLine < iLineStart)
			{
				// 处理行
				int iLen = iTmpOffset - iTmpOldOffset;
				ProcessLine(lpszTmpNextLine, m_lpLineInfo[iTmpCurrentLine].nLen);
			}
			iTmpOldOffset = iTmpOffset;
			iTmpCurrentLine++;

			// 退出
			if (iTmpCurrentLine >= iLineStart)
				break;
		}

		// 绘制行
		RECT rcLine = {-iHorizontalOffset, 0, rcClient.right, m_szTextSize.cy};
		int iOffset = m_lpLineInfo[iLineStart].dwOffset;
		int iOldOffset = iOffset;
		int iCurrentLine = iLineStart;
		LPTSTR lpszNextLine = NULL;
		while ((lpszNextLine=GetLine(iOffset)) != NULL)
		{
			// 绘制行号
			if (m_LineNumbers)
			{
				COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crLineNumFgColor);
				COLORREF cOldBkColor = m_MemDC.SetBkColor(m_crLineNumBgColor);
				CString LineNumber;
				LineNumber.Format(_T("%4d "), iCurrentLine+1);
				m_MemDC.TextOut(rcLine.left, rcLine.top, LineNumber, 5);
				m_MemDC.SetBkColor(cOldBkColor);
				m_MemDC.SetTextColor(cOldTextColor);
			}

			// 仅绘制可见的行
			if ((iCurrentLine >= iLineStart) && (iCurrentLine <= iLineEnd))
			{
				// 获得可见字符的范围
				POINT ptLeft = {iHorizontalOffset, 0};
				POINT ptRight = {rcLine.right+iHorizontalOffset, 0};
				int iCharStart = CharFromPosition(iCurrentLine, ptLeft);
				int iCharEnd = CharFromPosition(iCurrentLine, ptRight);

				// 计算文本长度
				int iLen = iOffset - iOldOffset;

				// 检查无效的选区
				if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
				{
					// 格式化选区
					int iLineStartSelection = min(m_iLineStartSelection, m_iLineEndSelection);
					int iLineEndSelection = max(m_iLineStartSelection, m_iLineEndSelection);
					int iCharStartSelection = m_iCharStartSelection;
					int iCharEndSelection = m_iCharEndSelection;

					// 检查选中的文本
					if ((iCurrentLine >= iLineStartSelection) && (iCurrentLine <= iLineEndSelection))
					{
						// 计算选中的字符
						int iSelectionStart;
						if (iCurrentLine == iLineStartSelection)
						{
							if (m_iLineStartSelection < m_iLineEndSelection)
								iSelectionStart = max(iCharStartSelection, iCharStart);
							else if (m_iLineStartSelection > m_iLineEndSelection)
								iSelectionStart = max(iCharEndSelection, iCharStart);
							else
								iSelectionStart = max(min(m_iCharStartSelection, m_iCharEndSelection), iCharStart);
						}
						else if (iCurrentLine == iLineEndSelection)
						{
							if (m_iLineStartSelection < m_iLineEndSelection)
								iSelectionStart = iCharStart;
							else if (m_iLineStartSelection > m_iLineEndSelection)
								iSelectionStart = iCharStart;
							else
								iSelectionStart = max(min(m_iCharStartSelection, m_iCharEndSelection), iCharStart);
						}
						else
							iSelectionStart = iCharStart;
						int iSelectionEnd;
						if (iCurrentLine == iLineStartSelection)
						{
							if (m_iLineStartSelection < m_iLineEndSelection)
								iSelectionEnd = iCharEnd;
							else if (m_iLineStartSelection > m_iLineEndSelection)
								iSelectionEnd = iCharEnd;
							else
								iSelectionEnd = min(max(m_iCharStartSelection, m_iCharEndSelection), iCharEnd);
						}
						else if (iCurrentLine == iLineEndSelection)
						{
							if (m_iLineStartSelection < m_iLineEndSelection)
								iSelectionEnd = min(iCharEndSelection, iCharEnd);
							else if (m_iLineStartSelection > m_iLineEndSelection)
								iSelectionEnd = min(iCharStartSelection, iCharEnd);
							else
								iSelectionEnd = min(max(m_iCharStartSelection, m_iCharEndSelection), iCharEnd);
						}
						else
							iSelectionEnd = iCharEnd;
						int iSelected = iSelectionEnd - iSelectionStart;
						if (iSelected > 0)
						{
							// 绘制选中的文本
							if (m_lpfnLineDrawProc == NULL)
								DrawLine(lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, iSelectionStart, iSelectionEnd);
							else
								m_lpfnLineDrawProc(m_MemDC.m_hDC, lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, iSelectionStart, iSelectionEnd);
						}
						else
						{
							// 绘制普通的文本
							if (m_lpfnLineDrawProc == NULL)
								DrawLine(lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
							else
								m_lpfnLineDrawProc(m_MemDC.m_hDC, lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
						}
					}
					else
					{
						// 绘制普通的文本
						if (m_lpfnLineDrawProc == NULL)
							DrawLine(lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
						else
							m_lpfnLineDrawProc(m_MemDC.m_hDC, lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
					}
				}
				else
				{
					// 绘制普通的文本
					if (m_lpfnLineDrawProc == NULL)
						DrawLine(lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
					else
						m_lpfnLineDrawProc(m_MemDC.m_hDC, lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
				}

				// 更新文本区域
				OffsetRect(&rcLine, 0, m_szTextSize.cy);
			}
			iOldOffset = iOffset;
			iCurrentLine++;

			// 退出
			if (iCurrentLine > iLineEnd)
				break;
		}

		// 绘制空白行号区域
		if (m_LineNumbers)
		{
			while (rcLine.top < rcClient.bottom)
			{
				COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crLineNumFgColor);
				COLORREF cOldBkColor = m_MemDC.SetBkColor(m_crLineNumBgColor);
				m_MemDC.TextOut(rcLine.left, rcLine.top, _T("     "), 5);
				m_MemDC.SetBkColor(cOldBkColor);
				m_MemDC.SetTextColor(cOldTextColor);
				rcLine.top += m_szTextSize.cy;
			}
		}
	}
}

void CCodeEdit::DrawLine(LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd)
{
	// 绘制单行文本
	int iStartTAB=-1, iEndTAB, iTABs;
	int iStart=0, i=0;
	int iEnd=-1, iCount;
	POINT pt = {rect.left+LINE_NUMBER_TO_EDITOR, rect.top};
	if (m_LineNumbers) pt.x += 5*m_szTextSize.cx;
	BOOL bComment = m_bComment;
	BOOL bText = m_bText;
	while (i <= iLen)
	{
		// 检查特殊字符
		if ((IsSpecial(lpszText[i])) || (i == iLen))
		{
			// 获得单词位置
			iEnd = i - 1;
			iCount = iEnd - iStart + 1;

			// 检查注释
			BOOL bTurnCommentOff = FALSE;
			if ((!bComment) && (!m_bComment))
			{
				switch (m_SyntaxColoring)
				{
				case SCT_C_ANSI:
				case SCT_C_PLUS_PLUS:
				case SCT_C_SHARP:
				case SCT_JSCRIPT:
					{
						if (_tcsncmp(lpszText+iStart, _T("//"), 2) == 0)
							bComment = TRUE;
						else if (_tcsncmp(lpszText+iStart, _T("/*"), 2) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;

				case SCT_VBSCRIPT:
					{
						if (_tcsncmp(lpszText+iStart, _T("'"), 1) == 0)
							bComment = TRUE;
						LPTSTR lpszKeyword = (LPTSTR)malloc(5*sizeof(_TCHAR));;
						_tcsncpy_s(lpszKeyword, 5, lpszText+iStart, 4);
						lpszKeyword[4] = '\0';
						_tcsupr_s(lpszKeyword, 5);
						if (_tcsncmp(lpszKeyword, _T("REM "), 4) == 0)
							bComment = TRUE;
						free(lpszKeyword);
					}
					break;

				case SCT_HTML:
					{
						if (_tcsncmp(lpszText+iStart, _T("<!--"), 4) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;

				case SCT_SQL:
					{
						if (_tcsncmp(lpszText+iStart, _T("--"), 2) == 0)
							bComment = TRUE;
						else if (_tcsncmp(lpszText+iStart, _T("/*"), 2) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;
				}
			}
			else if (m_bComment)
			{
				switch (m_SyntaxColoring)
				{
				case SCT_C_ANSI:
				case SCT_C_PLUS_PLUS:
				case SCT_C_SHARP:
				case SCT_JSCRIPT:
				case SCT_SQL:
					{
						if (_tcsncmp(lpszText+iStart, _T("*/"), 2) == 0)
						{
							m_bComment = FALSE;
							bTurnCommentOff = TRUE;
						}
					}
					break;

				case SCT_HTML:
					{
						if (_tcsncmp(lpszText+iStart, _T("-->"), 3) == 0)
						{
							m_bComment = FALSE;
							bTurnCommentOff = TRUE;
						}
					}
					break;
				}
			}

			// 检查注释
			if (!bComment)
			{
				// 绘制普通单词
				m_MemDC.SetTextColor(m_crCodeColor);
				m_MemDC.TextOut(pt.x, pt.y, lpszText+iStart, iCount);

				// 检查关键词
				int iKeywordIndex = -1;
				int iKeywordOffset = -1;
				if (IsKeyword(lpszText+iStart, iCount, iKeywordIndex, iKeywordOffset))
				{
					// 绘制关键词
					COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crKeywordColor);
					m_MemDC.TextOut(pt.x+iKeywordOffset*m_szTextSize.cx, pt.y, lpszText+iStart+iKeywordOffset, m_lpKeywordInfo[iKeywordIndex].nLen-1);
					m_MemDC.SetTextColor(cOldTextColor);
				}

				// 检查常量
				int iConstantIndex = -1;
				int iConstantOffset = -1;
				if (IsConstant(lpszText+iStart, iCount, iConstantIndex, iConstantOffset))
				{
					// 绘制常量
					COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crConstantColor);
					m_MemDC.TextOut(pt.x+iConstantOffset*m_szTextSize.cx, pt.y, m_lpConstantInfo[iConstantIndex].lpszTagName, m_lpConstantInfo[iConstantIndex].nLen-1);
					m_MemDC.SetTextColor(cOldTextColor);
				}

				// 检查字符串
				int iTextStart = -1;
				int iTextEnd = -1;
				if (IsText(lpszText+iStart, iCount, iTextStart, iTextEnd) || (bText))
				{
					// 更新文本偏移量
					if (!bText)
					{
						if (iTextEnd == -1)
						{
							iTextEnd = iCount - 1;
							bText = TRUE;
						}
					}
					else
					{
						if (iTextEnd == -1)
						{
							iTextEnd = iTextStart;
							if (iTextEnd == -1)
								iTextEnd = iCount - 1;
							else
								bText = FALSE;
							iTextStart = 0;
						}
					}

					// 绘制字符串
					COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crTextColor);
					m_MemDC.TextOut(pt.x+iTextStart*m_szTextSize.cx, pt.y, lpszText+iStart+iTextStart, iTextEnd-iTextStart+1);
					m_MemDC.SetTextColor(cOldTextColor);
					m_bText = bText;
				}
			}
			else
			{
				// 绘制注释
				COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crCommentColor);
				m_MemDC.TextOut(pt.x, pt.y, lpszText+iStart, iCount);
				m_MemDC.SetTextColor(cOldTextColor);
				if (bTurnCommentOff == TRUE)
					bComment = FALSE;
			}

			// 检查无效选区
			int iWordStartSelection = max(iStart, iSelectionStart);
			int iWordEndSelection = min(iEnd+1, iSelectionEnd);
			int iNumberSelected = iWordEndSelection - iWordStartSelection;

			if (iNumberSelected > 0)
			{
				// 计算选区开始绘制位置
				int iSelectionOffset = 0;
				for (int ch = iStart; ch < iWordStartSelection; ch++)
				{
					if (IsCJKChar(lpszText[ch]))
						iSelectionOffset += m_szCJKTextSize.cx;
					else
						iSelectionOffset += m_szTextSize.cx;
				}

				// 绘制选区
				COLORREF cOldTextColor = m_MemDC.SetTextColor(RGB(255,255,255));
				COLORREF cOldBkColor = m_MemDC.SetBkColor(RGB(51,153,255));
				m_MemDC.TextOut(pt.x+iSelectionOffset, pt.y, lpszText+iWordStartSelection, iNumberSelected);
				m_MemDC.SetBkColor(cOldBkColor);
				m_MemDC.SetTextColor(cOldTextColor);
			}

			// 更新偏移量
			int iSpaceOffset = 0;
			for (int ch = iStart; ch < iEnd + 1; ch++)
			{
				if (IsCJKChar(lpszText[ch]))
					iSpaceOffset += m_szCJKTextSize.cx;
				else
					iSpaceOffset += m_szTextSize.cx;
			}
			pt.x += iSpaceOffset;

			// 检查空格字符
			if (lpszText[i] == ' ')
			{
				// 检查无效选区
				if ((i >= iSelectionStart) && (i < iSelectionEnd))
				{
					// 绘制选区
					COLORREF cOldTextColor = m_MemDC.SetTextColor(RGB(255,255,255));
					COLORREF cOldBkColor = m_MemDC.SetBkColor(RGB(51,153,255));
					m_MemDC.TextOut(pt.x, pt.y, _T(" "), 1);
					m_MemDC.SetBkColor(cOldBkColor);
					m_MemDC.SetTextColor(cOldTextColor);
				}

				// 更新偏移量
				pt.x += m_szTextSize.cx;
				iStart = i + 1;
			}
			// 检查 TAB 字符
			else if (lpszText[i] == '\t')
			{
				// 更新 TAB 字符
				if (iStartTAB == -1)
				{
					iTABs = m_iTABs;
					iStartTAB = i;
				}
				else
				{
					iEndTAB = i;
					iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
					if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
						iTABs++;
					if ((iEndTAB - iStartTAB) == 1)
						iTABs = m_iTABs;
					iStartTAB = iEndTAB;
				}

				// 检查无效选区
				if ((i >= iSelectionStart) && (i < iSelectionEnd))
				{

					// 绘制选区
					COLORREF cOldTextColor = m_MemDC.SetTextColor(RGB(255,255,255));
					COLORREF cOldBkColor = m_MemDC.SetBkColor(RGB(51,153,255));
					m_MemDC.TextOut(pt.x, pt.y, _T("        "), iTABs);
					m_MemDC.SetBkColor(cOldBkColor);
					m_MemDC.SetTextColor(cOldTextColor);
				}

				// 更新偏移量
				pt.x += (iTABs * m_szTextSize.cx);
				iStart = i + 1;
			}
			else
				break;
		}
		i++;
	}
}

void CCodeEdit::ProcessLine(LPTSTR lpszText, int iLen)
{
	// 单行文本检查
	int iStartTAB=-1, iEndTAB, iTABs;
	int iStart=0, i=0;
	int iEnd=-1, iCount;
	BOOL bComment = m_bComment;
	BOOL bText = m_bText;
	while (i <= iLen)
	{
		// 检查特殊字符
		if ((IsSpecial(lpszText[i])) || (i == iLen))
		{
			// 获得单词的结尾位置
			iEnd = i - 1;
			iCount = iEnd - iStart + 1;

			// 检查是否为注释
			BOOL bTurnCommentOff = FALSE;
			if ((!bComment) && (!m_bComment))
			{
				switch (m_SyntaxColoring)
				{
				case SCT_C_ANSI:
				case SCT_C_PLUS_PLUS:
				case SCT_C_SHARP:
				case SCT_JSCRIPT:
					{
						if (_tcsncmp(lpszText+iStart, _T("//"), 2) == 0)
							bComment = TRUE;
						else if (_tcsncmp(lpszText+iStart, _T("/*"), 2) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;

				case SCT_VBSCRIPT:
					{
						if (_tcsncmp(lpszText+iStart, _T("'"), 1) == 0)
							bComment = TRUE;
						LPTSTR lpszKeyword = (LPTSTR)malloc(5*sizeof(_TCHAR));;
						_tcsncpy_s(lpszKeyword, 5, lpszText+iStart, 4);
						lpszKeyword[4] = '\0';
						_tcsupr_s(lpszKeyword, 5);
						if (_tcsncmp(lpszKeyword, _T("REM "), 4) == 0)
							bComment = TRUE;
						free(lpszKeyword);
					}
					break;

				case SCT_HTML:
					{
						if (_tcsncmp(lpszText+iStart, _T("<!--"), 4) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;

				case SCT_SQL:
					{
						if (_tcsncmp(lpszText+iStart, _T("--"), 2) == 0)
							bComment = TRUE;
						else if (_tcsncmp(lpszText+iStart, _T("/*"), 2) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;
				}
			}
			else if (m_bComment)
			{
				switch (m_SyntaxColoring)
				{
				case SCT_C_ANSI:
				case SCT_C_PLUS_PLUS:
				case SCT_C_SHARP:
				case SCT_JSCRIPT:
				case SCT_SQL:
					{
						if (_tcsncmp(lpszText+iStart, _T("*/"), 2) == 0)
						{
							m_bComment = FALSE;
							bTurnCommentOff = TRUE;
						}
					}
					break;

				case SCT_HTML:
					{
						if (_tcsncmp(lpszText+iStart, _T("-->"), 3) == 0)
						{
							m_bComment = FALSE;
							bTurnCommentOff = TRUE;
						}
					}
					break;
				}
			}

			// 如果不是注释
			if (!bComment)
			{
				// 检查是否是普通文本
				int iTextStart = -1;
				int iTextEnd = -1;
				if (IsText(lpszText+iStart, iCount, iTextStart, iTextEnd) || (bText))
				{
					// 更新文本偏移量
					if (!bText)
					{
						if (iTextEnd == -1)
						{
							iTextEnd = iCount - 1;
							bText = TRUE;
						}
					}
					else
					{
						if (iTextEnd == -1)
						{
							iTextEnd = iTextStart;
							if (iTextEnd == -1)
								iTextEnd = iCount - 1;
							else
								bText = FALSE;
							iTextStart = 0;
						}
					}
					m_bText = bText;
				}
			}
			else
			{
				// 关闭注释
				if (bTurnCommentOff == TRUE)
					bComment = FALSE;
			}

			// 检查空格字符
			if (lpszText[i] == ' ')
			{
				// 更新偏移量
				iStart = i + 1;
			}
			// 检查 TAB 字符
			else if (lpszText[i] == '\t')
			{
				// 更新 TAB 字符
				if (iStartTAB == -1)
				{
					iTABs = m_iTABs;
					iStartTAB = i;
				}
				else
				{
					iEndTAB = i;
					iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
					if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
						iTABs++;
					if ((iEndTAB - iStartTAB) == 1)
						iTABs = m_iTABs;
					iStartTAB = iEndTAB;
				}

				// 更新偏移量
				iStart = i + 1;
			}
			else
				break;
		}
		i++;
	}
}

void CCodeEdit::UpdateControl()
{
	// 清除重绘的标志
	SetRedraw(FALSE);

	// 清除行信息
	if (m_lpLineInfo != NULL)
	{
		free(m_lpLineInfo);
		m_lpLineInfo = NULL;
	}

	// 获得客户区大小和位置
	RECT rcClient;
	GetClientRect(&rcClient);

	// 获得字符的尺寸
	m_szTextSize = m_MemDC.GetTextExtent(_T("A"), 1);
	m_szCJKTextSize = m_MemDC.GetTextExtent(_T("灰"), 1);

	// 计算内容尺寸
	int iWidth = 0;
	int iHeight = 0;
	int iOffset = 0;
	int iOldOffset = iOffset;
	m_iNumberLines = 0;
	LPTSTR lpszNextLine = NULL;
	while ((lpszNextLine=GetLine(iOffset)) != NULL)
	{
		// 更新行信息
		m_iNumberLines++;
		if (m_lpLineInfo == NULL)
			m_lpLineInfo = (LPLINEINFO)malloc(m_iNumberLines*sizeof(LINEINFO));
		else
			m_lpLineInfo = (LPLINEINFO)realloc(m_lpLineInfo, m_iNumberLines*sizeof(LINEINFO));
		m_lpLineInfo[m_iNumberLines-1].dwOffset = iOldOffset;

		// 计算行长度
		int iLen = iOffset - iOldOffset;
		iOldOffset = iOffset;
		if ((lpszNextLine[iLen-2] == '\r') && (lpszNextLine[iLen-1] == '\n'))
			m_lpLineInfo[m_iNumberLines-1].nLen = iLen - 2;
		else if (lpszNextLine[iLen-1] == '\n')
			m_lpLineInfo[m_iNumberLines-1].nLen = iLen - 1;
		else
			m_lpLineInfo[m_iNumberLines-1].nLen = iLen;

		// 计算单行尺寸
		SIZE szLine = GetLineSize(lpszNextLine, m_lpLineInfo[m_iNumberLines-1].nLen);
		if (iWidth < szLine.cx)
			iWidth = szLine.cx;
		iHeight += szLine.cy;
	}

	// 获得滚动条尺寸
	int iHorizontalOffset = GetSystemMetrics(SM_CYHSCROLL);
	if (iHeight <= (rcClient.bottom - rcClient.top))
		iHorizontalOffset = 0;
	int iVerticalOffset = GetSystemMetrics(SM_CXVSCROLL);
	if (iWidth <= (rcClient.right - rcClient.left))
		iVerticalOffset = 0;

	// 检查内容宽度
	if (iWidth > (rcClient.right - rcClient.left))
	{
		// 显示水平滚动条
		SCROLLINFO si;
		memset(&si, 0, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMax = iWidth;
		si.nPage = ((rcClient.right - rcClient.left - iVerticalOffset) / m_szTextSize.cx + 1) * m_szTextSize.cx;
		si.nPos = GetScrollPos(SB_HORZ);
		SetScrollInfo(SB_HORZ, &si, TRUE);
		ShowScrollBar(SB_HORZ, TRUE);
	}
	else
	{
		// 更新水平滚动条
		SCROLLINFO si;
		memset(&si, 0, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMax = iWidth;
		si.nPage = ((rcClient.right - rcClient.left - iVerticalOffset) / m_szTextSize.cx + 1) * m_szTextSize.cx;
		SetScrollInfo(SB_HORZ, &si, TRUE);
		SetScrollPos(SB_HORZ, 0, TRUE);
		ShowScrollBar(SB_HORZ, FALSE);
	}

	// 检查内容高度
	if (iHeight > (rcClient.bottom - rcClient.top))
	{
		// 显示垂直滚动条
		SCROLLINFO si;
		memset(&si, 0, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMax = iHeight;
		si.nPage = ((rcClient.bottom - rcClient.top - iHorizontalOffset) / m_szTextSize.cy) * m_szTextSize.cy;
		si.nPos = GetScrollPos(SB_VERT);
		SetScrollInfo(SB_VERT, &si, TRUE);
		ShowScrollBar(SB_VERT, TRUE);
	}
	else
	{
		// 更新垂直滚动条
		SCROLLINFO si;
		memset(&si, 0, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMax = iHeight;
		si.nPage = ((rcClient.bottom - rcClient.top - iHorizontalOffset) / m_szTextSize.cy) * m_szTextSize.cy;
		SetScrollInfo(SB_VERT, &si, TRUE);
		SetScrollPos(SB_VERT, 0, TRUE);
		ShowScrollBar(SB_VERT, FALSE);
	}

	// 设置重绘的标志
	SetRedraw(TRUE);
}

void CCodeEdit::DestroyList()
{
	if (m_wndListBox.m_hWnd != NULL)
	{
		// 销毁列表框
		m_wndListBox.DestroyWindow();
	}
}

void CCodeEdit::DeleteText()
{
	// 文档不为空
	if (m_lpszText != NULL)
	{
		// 选区有效
		if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
		{
			// 获得选择范围
			int iStartChar, iEndChar;
			int iStartLine, iEndLine;
			if (m_iLineStartSelection < m_iLineEndSelection)
			{
				iStartChar = m_iCharStartSelection;
				iEndChar = m_iCharEndSelection;
				iStartLine = m_iLineStartSelection;
				iEndLine = m_iLineEndSelection;
			}
			else if (m_iLineStartSelection > m_iLineEndSelection)
			{
				iStartChar = m_iCharEndSelection;
				iEndChar = m_iCharStartSelection;
				iStartLine = m_iLineEndSelection;
				iEndLine = m_iLineStartSelection;
			}
			else
			{
				iStartChar = min(m_iCharStartSelection, m_iCharEndSelection);
				iEndChar = max(m_iCharStartSelection, m_iCharEndSelection);
				iStartLine = m_iLineStartSelection;
				iEndLine = m_iLineEndSelection;
			}
			DelText(iStartLine, iStartChar, iEndLine, iEndChar);

			m_iCurrentChar = iStartChar;
			m_iCurrentLine = iStartLine;
		}
	}
}

void CCodeEdit::DelText(int iLine, int iChar, int iEndLine, int iEndChar)
{
	int iStartOffset = GetCharOffset(iLine, iChar);
	if (m_lpszText[iStartOffset] == '\r')
		iStartOffset += 2;
	int iEndOffset = GetCharOffset(iEndLine, iEndChar);
	int iLen = iEndOffset - iStartOffset;

	if (m_lpszText != NULL)
	{
		// 保存撤消信息
		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(iLen*sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, iLen*sizeof(_TCHAR));
		m_UndoInfo.iLastLineLen = m_lpLineInfo[max(m_UndoInfo.iLine - 1, 0)].nLen;
		m_UndoInfo.nLen = iLen;
		m_UndoInfo.iLine = iLine;
		m_UndoInfo.iChar = iChar;
		m_UndoInfo.iEndLine = iEndLine;
		m_UndoInfo.iEndChar = iEndChar;
		memcpy(m_UndoInfo.lpszUndoData, m_lpszText+iStartOffset, iLen*sizeof(_TCHAR));
		m_UndoInfo.nOperation = UOT_DELTEXT;
		m_bCanUndo = TRUE;

		// 删除文本
		int iOldSize = m_iSize;
		LPTSTR lpszTemp = new _TCHAR[iOldSize];
		memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
		m_iSize -= iLen;
		free(m_lpszText);
		m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
		if (m_lpszText != NULL)
		{
			memcpy(m_lpszText, lpszTemp, iStartOffset*sizeof(_TCHAR));
			memcpy(m_lpszText+iStartOffset, lpszTemp+iStartOffset+iLen, (iOldSize-iStartOffset-iLen)*sizeof(_TCHAR));
		}
		delete lpszTemp;

		// 文档已改变
		m_changed = TRUE;

		// 清除选区
		m_iLineStartSelection = -1;
		m_iLineEndSelection = -1;
		m_iCharStartSelection = -1;
		m_iCharEndSelection = -1;
	}
}

void CCodeEdit::AddText(LPTSTR lpszText, int iLine, int iChar, int iLen)
{
	// 文本不为空
	if ((m_lpszText != NULL) && (lpszText != NULL))
	{
		int iStartOffset = GetCharOffset(iLine, iChar);

		// 保存撤消信息
		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(iLen*sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, iLen*sizeof(_TCHAR));
		m_UndoInfo.nLen = iLen;
		m_UndoInfo.iLine = iLine;
		m_UndoInfo.iChar = iChar;
		memcpy(m_UndoInfo.lpszUndoData, lpszText, iLen*sizeof(_TCHAR));
		m_UndoInfo.nOperation = UOT_ADDTEXT;
		m_bCanUndo = TRUE;

		int iOldSize = m_iSize;
		LPTSTR lpszTemp = new _TCHAR[iOldSize];
		memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
		m_iSize += iLen;
		free(m_lpszText);
		m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
		memcpy(m_lpszText, lpszTemp, iStartOffset*sizeof(_TCHAR));
		memcpy(m_lpszText+iStartOffset, lpszText, iLen*sizeof(_TCHAR));
		memcpy(m_lpszText+iStartOffset+iLen, lpszTemp+iStartOffset, (iOldSize-iStartOffset)*sizeof(_TCHAR));
		delete lpszTemp;

		m_changed = TRUE;
	}
}

void CCodeEdit::DelChar(int iLine, int iChar)
{
	int nCharOffset = GetCharOffset(iLine, iChar);
	if ((m_lpszText != NULL) && (nCharOffset < m_iSize-2))
	{
		// 保存撤消信息
		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
		m_UndoInfo.iLastLineLen = m_lpLineInfo[iLine].nLen;
		m_UndoInfo.nLen = 1;
		m_UndoInfo.iLine = iLine;
		m_UndoInfo.iChar = iChar;
		m_UndoInfo.lpszUndoData[0] = m_lpszText[nCharOffset];
		m_UndoInfo.nOperation = UOT_DELETE;
		m_bCanUndo = TRUE;

		// 删除光标后字符
		int iOldSize = m_iSize;
		LPTSTR lpszTemp = new _TCHAR[iOldSize];
		memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
		if (m_lpszText[nCharOffset+1] == '\n')
		{
			m_iSize -= 2;
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			if (m_lpszText != NULL)
			{
				memcpy(m_lpszText, lpszTemp, nCharOffset*sizeof(_TCHAR));
				if (nCharOffset <= m_iSize)
					memcpy(m_lpszText+nCharOffset, lpszTemp+nCharOffset+2, (iOldSize-nCharOffset-2)*sizeof(_TCHAR));
			}
			m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
		}
		else
		{
			m_iSize--;
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			if (m_lpszText != NULL)
			{
				memcpy(m_lpszText, lpszTemp, nCharOffset*sizeof(_TCHAR));
				if (nCharOffset <= m_iSize-1)
					memcpy(m_lpszText+nCharOffset, lpszTemp+nCharOffset+1, (iOldSize-nCharOffset-1)*sizeof(_TCHAR));
			}
		}
		delete lpszTemp;

		m_changed = TRUE;
	}
}

int CCodeEdit::BackChar(int iLine, int iChar)
{
	int Myresult = 0;

	// 获得偏移量
	int iCurrentOffset = GetCharOffset(iLine, iChar);

	if ((m_lpszText != NULL) && (iCurrentOffset > 0))
	{
		// 保存撤消信息
		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
		m_UndoInfo.iLastLineLen = m_lpLineInfo[iLine-1].nLen;
		m_UndoInfo.nLen = 1;
		m_UndoInfo.iLine = iLine;
		m_UndoInfo.iChar = iChar;
		m_UndoInfo.lpszUndoData[0] = m_lpszText[iCurrentOffset-1];
		m_UndoInfo.nOperation = UOT_BACK;
		m_bCanUndo = TRUE;

		// 删除光标前字符
		int iOldSize = m_iSize;
		LPTSTR lpszTemp = new _TCHAR[iOldSize];
		memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
		if (m_lpszText[iCurrentOffset-1] == '\n')
		{
			m_iSize -= 2;
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			if (m_lpszText != NULL)
			{
				memcpy(m_lpszText, lpszTemp, (iCurrentOffset-2)*sizeof(_TCHAR));
				if (iCurrentOffset <= m_iSize)
					memcpy(m_lpszText+iCurrentOffset-2, lpszTemp+iCurrentOffset, (iOldSize-iCurrentOffset)*sizeof(_TCHAR));
			}
			Myresult = 1;
		}
		else
		{
			m_iSize--;
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			if (m_lpszText != NULL)
			{
				memcpy(m_lpszText, lpszTemp, (iCurrentOffset-1)*sizeof(_TCHAR));
				if (iCurrentOffset <= m_iSize)
					memcpy(m_lpszText+iCurrentOffset-1, lpszTemp+iCurrentOffset, (iOldSize-iCurrentOffset)*sizeof(_TCHAR));
			}
			Myresult = 2;
		}
		delete lpszTemp;
		m_changed = TRUE;
	}
	return Myresult;
}

void CCodeEdit::AddChar(int iLine, int iChar, UINT nChar)
{
	int iCurrentOffset = GetCharOffset(iLine, iChar);

	// 添加一个字符
	int iOldSize = m_iSize;
	LPTSTR lpszTemp = new _TCHAR[iOldSize];
	memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
	m_iSize++;
	free(m_lpszText);
	m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
	memcpy(m_lpszText, lpszTemp, iCurrentOffset*sizeof(_TCHAR));
	m_lpszText[iCurrentOffset] = nChar;
	memcpy(m_lpszText+iCurrentOffset+1, lpszTemp+iCurrentOffset, (iOldSize-iCurrentOffset)*sizeof(_TCHAR));
	delete lpszTemp;
	m_changed = TRUE;

	// 保存撤消信息
	m_UndoInfo.nOperation = UOT_CHAR;
	if (m_UndoInfo.lpszUndoData == NULL)
		m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
	else
		m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
	m_UndoInfo.iLastLineLen = m_lpLineInfo[m_UndoInfo.iLine - 1].nLen;
	m_UndoInfo.nLen = 1;
	m_UndoInfo.iLine = iLine;
	m_UndoInfo.iChar = iChar;
	m_UndoInfo.lpszUndoData[0] = nChar;
	m_bCanUndo = TRUE;
}

void CCodeEdit::AddLine(int iLine, int iChar)
{
	// 偏移量
	int iCurrentOffset = GetCharOffset(iLine, iChar);

	int iOldSize = m_iSize;
	LPTSTR lpszTemp = new _TCHAR[iOldSize];
	memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
	m_iSize += 2;
	free(m_lpszText);
	m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
	memcpy(m_lpszText, lpszTemp, iCurrentOffset*sizeof(_TCHAR));
	m_lpszText[iCurrentOffset] = '\r';
	m_lpszText[iCurrentOffset+1] = '\n';
	memcpy(m_lpszText+iCurrentOffset+2, lpszTemp+iCurrentOffset, (iOldSize-iCurrentOffset)*sizeof(_TCHAR));
	delete lpszTemp;
	m_changed = TRUE;

	// 保存撤消信息
	m_UndoInfo.nOperation = UOT_ADDLINE;
	if (m_UndoInfo.lpszUndoData == NULL)
		m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
	else
		m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
	m_UndoInfo.iLastLineLen = m_lpLineInfo[iLine].nLen;
	m_UndoInfo.nLen = 1;
	m_UndoInfo.iLine = iLine;
	m_UndoInfo.iChar = iChar;
	m_UndoInfo.lpszUndoData[0] = '\n';
	m_bCanUndo = TRUE;
}

SIZE CCodeEdit::GetLineSize(LPTSTR lpszText, int iLen)
{
	SIZE szResult = {0, m_szTextSize.cy};

	// 获得行宽
	BOOL bDone = FALSE;
	int iStartTAB=-1;
	int iEndTAB, iTABs;
	for (int i=0; i<iLen; i++)
	{
		// 检查 TAB 字符 
		if (lpszText[i] == '\t')
		{
			if (iStartTAB == -1)
			{
				iTABs = m_iTABs;
				iStartTAB = i;
			}
			else
			{
				iEndTAB = i;
				iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
				if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
					iTABs++;
				if ((iEndTAB - iStartTAB) == 1)
					iTABs = m_iTABs;
				iStartTAB = iEndTAB;
			}
			szResult.cx += (iTABs * m_szTextSize.cx);
		}
		else
		{
			if (IsCJKChar(lpszText[i]))
				szResult.cx += m_szCJKTextSize.cx;
			else
				szResult.cx += m_szTextSize.cx;
		}
	}

	szResult.cx += 3;
	if (m_LineNumbers) szResult.cx += m_szTextSize.cx * 5;

	return szResult;
}

void CCodeEdit::ClearText()
{
	// 删除所有文本
	if (m_iSize != 2)
		DelText(0, 0, m_iNumberLines - 1, m_lpLineInfo[m_iNumberLines - 1].nLen);
	m_iCurrentChar = 0;
	m_iCurrentLine = 0;

	// 更新控件
	UpdateControl();

	// 更新光标
	UpdateCaret();
}

void CCodeEdit::Undo()
{
	// 判断操作类型
	switch(m_UndoInfo.nOperation)
	{
	case UOT_CHAR:	// 输入一个字符
		{
			m_iCurrentLine = m_UndoInfo.iLine;
			m_iCurrentChar = m_UndoInfo.iChar;
			BackChar(m_UndoInfo.iLine, m_UndoInfo.iChar + 1);
		}
		break;
	case UOT_BACK:	// 删除前面一个字符
		{
			if (m_UndoInfo.lpszUndoData[0] == '\n')
			{
				AddLine(m_UndoInfo.iLine - 1, m_UndoInfo.iLastLineLen);
				m_iCurrentLine = m_UndoInfo.iLine + 1;
				m_iCurrentChar = 0;
			}
			else
			{
				AddChar(m_UndoInfo.iLine, m_UndoInfo.iChar - 1, m_UndoInfo.lpszUndoData[0]);
				m_iCurrentLine = m_UndoInfo.iLine;
				m_iCurrentChar = m_UndoInfo.iChar + 1;
			}
		}
		break;
	case UOT_DELETE:	// 删除后面一个字符
		{
			if (m_UndoInfo.lpszUndoData[0] == '\r')
			{
				m_iCurrentLine = m_UndoInfo.iLine;
				m_iCurrentChar = m_UndoInfo.iChar;
				AddLine(m_UndoInfo.iLine, m_UndoInfo.iChar);
			}
			else
			{
				m_iCurrentLine = m_UndoInfo.iLine;
				m_iCurrentChar = m_UndoInfo.iChar;
				AddChar(m_UndoInfo.iLine, m_UndoInfo.iChar, m_UndoInfo.lpszUndoData[0]);
			}
		}
		break;
	case UOT_ADDLINE:	// 添加一行
		{
			m_iCurrentLine = m_UndoInfo.iLine;
			m_iCurrentChar = m_UndoInfo.iChar;
			BackChar(m_UndoInfo.iLine + 1, 0);
		}
		break;
	case UOT_DELTEXT:	// 删除文本
		{
			// 恢复选区
			m_iLineStartSelection = m_UndoInfo.iLine;
			m_iLineEndSelection = m_UndoInfo.iEndLine;
			m_iCharStartSelection = m_UndoInfo.iChar;
			m_iCharEndSelection = m_UndoInfo.iEndChar;

			// 添加文本
			AddText(m_UndoInfo.lpszUndoData, m_UndoInfo.iLine, m_UndoInfo.iChar, m_UndoInfo.nLen);

			// 设置光标位置
			m_iCurrentLine = m_UndoInfo.iEndLine;
			m_iCurrentChar = m_UndoInfo.iEndChar;
		}
		break;
	case UOT_ADDTEXT:	// 添加文本
		{
			// 设置光标位置
			m_iCurrentLine = m_UndoInfo.iLine;
			m_iCurrentChar = m_UndoInfo.iChar;

			// 删除文本
			DelText(m_UndoInfo.iLine, m_UndoInfo.iChar, m_UndoInfo.iEndLine, m_UndoInfo.iEndChar);
		}
		break;
	}

	// 更新控件
	UpdateControl();

	// 更新光标
	UpdateCaret();
}

BOOL CCodeEdit::Find(LPTSTR lpszText, int iLen, BOOL bMatchCase, BOOL bMatchWhole, BOOL bDirectionDown)
{
	// 复制查找字符串
	LPTSTR lpszFind = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
	memcpy(lpszFind, lpszText, iLen*sizeof(_TCHAR));
	lpszFind[iLen] = '\0';

	if (bDirectionDown)
	{
		// 定义开始查找位置
		int iLine, iChar;
		if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
		{
			iLine = m_iLineEndSelection;
			iChar = m_iCharEndSelection;
		}
		else
		{
			iLine = m_iCurrentLine;
			iChar = m_iCurrentChar;
		}
		int iStartOffset = GetCharOffset(iLine, iChar);

		// 开始查找
		for (int i = iStartOffset; i <= m_iSize - iLen - 2; i++)
		{
			// 找换行
			if (m_lpszText[i] == '\r')
			{
				i++;
				iLine++;
				iChar = 0;
				continue;
			}

			// 为空行准备
			if (m_lpszText[i] == '\n')
			{
				iLine++;
				iChar = 0;
				continue;
			}

			// 复制要比较的字符串
			LPTSTR lpszTemp = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
			memcpy(lpszTemp, m_lpszText+i, iLen*sizeof(_TCHAR));
			lpszTemp[iLen] = '\0';

			// 如果不区分大小写
			if (!bMatchCase)
			{
				// 将字符串转为大写
				_tcsupr_s(lpszFind, iLen+1);
				_tcsupr_s(lpszTemp, iLen+1);
			}

			// 开始匹配
			BOOL bMatch = TRUE;
			for (int j = 0; j < iLen; j++)
			{
				if (!(lpszTemp[j] == lpszFind[j]))
				{
					bMatch = FALSE;
					break;
				}
			}

			free(lpszTemp);

			// 如果全字匹配
			if (bMatchWhole && bMatch)
			{
				if (i >= 1)
				{
					if(IsLetter(m_lpszText[i - 1]))
						bMatch = FALSE;
				}
				if (i <= m_iSize - iLen - 3)
				{
					if(IsLetter(m_lpszText[i + iLen]))
						bMatch = FALSE;
				}
			}

			// 如果匹配成功
			if (bMatch == TRUE)
			{
				// 设置选区和光标
				m_iLineStartSelection = iLine;
				m_iCharStartSelection = iChar;
				m_iLineEndSelection = iLine;
				m_iCharEndSelection = iChar + iLen;
				m_iCurrentLine = m_iLineEndSelection;
				m_iCurrentChar = m_iCharEndSelection;

				// 更新光标
				UpdateCaret();

				// 确保光标在视图范围
				EnsureVisible();

				free(lpszFind);

				return TRUE;
			}
			iChar++;
		}
	}
	else
	{
		// 定义开始查找位置
		int iLine, iChar;
		if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
		{
			iLine = m_iLineStartSelection;
			iChar = m_iCharStartSelection;
		}
		else
		{
			iLine = m_iCurrentLine;
			iChar = m_iCurrentChar;
		}
		int iStartOffset = GetCharOffset(iLine, iChar);

		// 开始查找
		for (int i = iStartOffset - 1; i >= iLen - 1; i--)
		{
			// 找换行
			if (m_lpszText[i] == '\n')
			{
				i--;
				iLine--;
				iChar = m_lpLineInfo[iLine].nLen;
				continue;
			}

			// 为空行准备
			if (m_lpszText[i] == '\r')
			{
				iLine--;
				iChar = m_lpLineInfo[iLine].nLen;
				continue;
			}

			// 复制要比较的字符串
			LPTSTR lpszTemp = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
			memcpy(lpszTemp, m_lpszText+i-iLen+1, iLen*sizeof(_TCHAR));
			lpszTemp[iLen] = '\0';

			// 如果不区分大小写
			if (!bMatchCase)
			{
				// 将字符串转为大写
				_tcsupr_s(lpszFind, iLen+1);
				_tcsupr_s(lpszTemp, iLen+1);
			}

			// 开始匹配
			BOOL bMatch = TRUE;
			for (int j = 0; j < iLen; j++)
			{
				if (!(lpszTemp[j] == lpszFind[j]))
				{
					bMatch = FALSE;
					break;
				}
			}

			free(lpszTemp);

			// 如果全字匹配
			if (bMatchWhole && bMatch)
			{
				if (i <= m_iSize - 3)
				{
					if(IsLetter(m_lpszText[i + 1]))
						bMatch = FALSE;
				}
				if (i >= iLen)
				{
					if(IsLetter(m_lpszText[i - iLen]))
						bMatch = FALSE;
				}
			}

			// 如果匹配成功
			if (bMatch == TRUE)
			{
				// 设置选区和光标
				m_iLineStartSelection = iLine;
				m_iCharStartSelection = iChar - iLen;
				m_iLineEndSelection = iLine;
				m_iCharEndSelection = iChar;
				m_iCurrentLine = m_iLineEndSelection;
				m_iCurrentChar = m_iCharEndSelection;

				// 更新光标
				UpdateCaret();

				// 确保光标在视图范围
				EnsureVisible();

				free(lpszFind);

				return TRUE;
			}
			iChar--;
		}
	}
	free(lpszFind);

	return FALSE;
}

BOOL CCodeEdit::Replace(LPTSTR lpszText, int iLen, LPTSTR lpszText2, int iLen2, BOOL bMatchCase, BOOL bMatchWhole)
{
	// 复制查找字符串
	LPTSTR lpszFind = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
	memcpy(lpszFind, lpszText, iLen*sizeof(_TCHAR));
	lpszFind[iLen] = '\0';

	// 复制替换字符串
	LPTSTR lpszReplace = LPTSTR(malloc((iLen2+1)*sizeof(_TCHAR)));
	memcpy(lpszReplace, lpszText2, iLen2*sizeof(_TCHAR));
	lpszReplace[iLen2] = '\0';

	// 复制选区字符串
	LPTSTR lpszTemp = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
	if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
	{
		int iLine, iChar;

		iLine = m_iLineStartSelection;
		iChar = m_iCharStartSelection;

		int iSelection = GetCharOffset(iLine, iChar);
		memcpy(lpszTemp, m_lpszText+iSelection, iLen*sizeof(_TCHAR));
		lpszTemp[iLen] = '\0';

		// 如果不区分大小写
		if (!bMatchCase)
		{
			// 将字符串转为大写
			_tcsupr_s(lpszFind, iLen+1);
			_tcsupr_s(lpszTemp, iLen+1);
		}

		// 开始匹配
		BOOL bMatch = TRUE;
		for (int j = 0; j < iLen; j++)
		{
			if (!(lpszTemp[j] == lpszFind[j]))
			{
				bMatch = FALSE;
				break;
			}
		}

		// 如果匹配，则替换字符串
		if (bMatch)
		{
			// 保存撤消信息
			if (iLen2 == 0)
			{
				if (m_UndoInfo.lpszUndoData == NULL)
					m_UndoInfo.lpszUndoData = (LPTSTR)malloc(iLen*sizeof(_TCHAR));
				else
					m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, iLen*sizeof(_TCHAR));
			}
			else
			{
				if (m_UndoInfo.lpszUndoData == NULL)
					m_UndoInfo.lpszUndoData = (LPTSTR)malloc(iLen2*sizeof(_TCHAR));
				else
					m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, iLen2*sizeof(_TCHAR));
			}
			m_UndoInfo.nLen = (iLen2 == 0) ? iLen : iLen2;
			m_UndoInfo.iLine = iLine;
			m_UndoInfo.iChar = iChar;
			m_UndoInfo.iEndLine = iLine;
			m_UndoInfo.iEndChar = (iLen2 == 0) ? (iChar + iLen) : (iChar + iLen2);
			if (iLen2 == 0)
			{
				memcpy(m_UndoInfo.lpszUndoData, lpszText, iLen*sizeof(_TCHAR));
				m_UndoInfo.nOperation = UOT_DELTEXT;
			}
			else
			{
				memcpy(m_UndoInfo.lpszUndoData, lpszText2, iLen2*sizeof(_TCHAR));
				m_UndoInfo.nOperation = UOT_ADDTEXT;
			}
			m_bCanUndo = TRUE;

			int iOldSize = m_iSize;
			LPTSTR lpszTmp = new _TCHAR[iOldSize];
			memcpy(lpszTmp, m_lpszText, iOldSize*sizeof(_TCHAR));
			m_iSize += (iLen2 - iLen);
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			memcpy(m_lpszText, lpszTmp, iSelection*sizeof(_TCHAR));
			memcpy(m_lpszText+iSelection, lpszReplace, iLen2*sizeof(_TCHAR));
			memcpy(m_lpszText+iSelection+iLen2, lpszTmp+iSelection+iLen, (iOldSize-iSelection)*sizeof(_TCHAR));
			delete lpszTmp;

			m_changed = TRUE;

			// 清除选区
			m_iLineStartSelection = -1;
			m_iLineEndSelection = -1;
			m_iCharStartSelection = -1;
			m_iCharEndSelection = -1;

			// 设置光标
			m_iCurrentChar = iChar + iLen2;
			m_iCurrentLine = iLine;

			// 更新控件
			UpdateControl();

			// 更新显示
			Invalidate();

			// 更新光标
			UpdateCaret();

			free(lpszTemp);
			free(lpszFind);
			free(lpszReplace);

			// 查找下一个
			if (Find(lpszText, iLen, bMatchCase, bMatchWhole, TRUE))
				return TRUE;
			else
				return FALSE;
		}
	}
	free(lpszTemp);
	free(lpszFind);
	free(lpszReplace);

	// 查找下一个
	if (Find(lpszText, iLen, bMatchCase, bMatchWhole, TRUE)) return TRUE;

	return FALSE;
}

void CCodeEdit::ReplaceAll(LPTSTR lpszText, int iLen, LPTSTR lpszText2, int iLen2, BOOL bMatchCase, BOOL bMatchWhole)
{
	// 复制查找字符串
	LPTSTR lpszFind = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
	memcpy(lpszFind, lpszText, iLen*sizeof(_TCHAR));
	lpszFind[iLen] = '\0';

	// 复制替换字符串
	LPTSTR lpszReplace = LPTSTR(malloc((iLen2+1)*sizeof(_TCHAR)));
	memcpy(lpszReplace, lpszText2, iLen2*sizeof(_TCHAR));
	lpszReplace[iLen2] = '\0';

	// 定义开始查找位置
	int iLine = 0, iChar = 0;

	// 开始查找
	for (int i = 0; i <= m_iSize - iLen - 2; i++)
	{
		// 找换行
		if (m_lpszText[i] == '\r')
		{
			i++;
			iLine++;
			iChar = 0;
			continue;
		}

		// 为空行准备
		if (m_lpszText[i] == '\n')
		{
			iLine++;
			iChar = 0;
			continue;
		}

		// 复制要比较的字符串
		LPTSTR lpszTemp = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
		memcpy(lpszTemp, m_lpszText+i, iLen*sizeof(_TCHAR));
		lpszTemp[iLen] = '\0';

		// 如果不区分大小写
		if (!bMatchCase)
		{
			// 将字符串转为大写
			_tcsupr_s(lpszFind, iLen+1);
			_tcsupr_s(lpszTemp, iLen+1);
		}

		// 开始匹配
		BOOL bMatch = TRUE;
		for (int j = 0; j < iLen; j++)
		{
			if (!(lpszTemp[j] == lpszFind[j]))
			{
				bMatch = FALSE;
				break;
			}
		}

		free(lpszTemp);

		// 如果全字匹配
		if (bMatchWhole && bMatch)
		{
			if (i >= 1)
			{
				if(IsLetter(m_lpszText[i - 1]))
					bMatch = FALSE;
			}
			if (i <= m_iSize - iLen - 3)
			{
				if(IsLetter(m_lpszText[i + iLen]))
					bMatch = FALSE;
			}
		}

		// 如果匹配成功
		if (bMatch == TRUE)
		{
			int iOldSize = m_iSize;
			LPTSTR lpszTmp = new _TCHAR[iOldSize];
			memcpy(lpszTmp, m_lpszText, iOldSize*sizeof(_TCHAR));
			m_iSize += (iLen2 - iLen);
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			memcpy(m_lpszText, lpszTmp, i*sizeof(_TCHAR));
			memcpy(m_lpszText+i, lpszReplace, iLen2*sizeof(_TCHAR));
			memcpy(m_lpszText+i+iLen2, lpszTmp+i+iLen, (iOldSize-i)*sizeof(_TCHAR));
			delete lpszTmp;

			m_changed = TRUE;

			iChar += (iLen2 - iLen);
		}
		iChar++;
	}
	free(lpszFind);
	free(lpszReplace);

	// 清除选区
	m_iLineStartSelection = -1;
	m_iLineEndSelection = -1;
	m_iCharStartSelection = -1;
	m_iCharEndSelection = -1;

	// 光标位置
	if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
		m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;

	// 更新控件
	UpdateControl();
	
	// 更新光标
	UpdateCaret();

	// 确保光标在视图范围
	EnsureVisible();
}

BOOL CCodeEdit::GetUndoStatus()
{
	return m_bCanUndo;
}

BOOL CCodeEdit::Load(LPTSTR lpszFilePath)
{
	BOOL bResult = FALSE;

	// 打开磁盘文件
	CFile file;
	if (file.Open(lpszFilePath, CFile::modeRead, NULL))
	{
		// 清除旧数据
		if (m_lpszText != NULL)
			free(m_lpszText);

		// 清除选区
		m_iLineStartSelection = -1;
		m_iLineEndSelection = -1;
		m_iCharStartSelection = -1;
		m_iCharEndSelection = -1;

		// 从文件读取数据
		DWORD dwSize = (DWORD)file.GetLength();
		LPBYTE lpData = (LPBYTE)malloc(dwSize*sizeof(BYTE));
		file.Read(lpData, dwSize);
		file.Close();
		bResult = TRUE;

		// 检查文件类型
		WORD wFlag = 0;
		memcpy(&wFlag, lpData, 2);
		DWORD dwFlag = 0;
		memcpy(&dwFlag, lpData, 3);
		int iOffset;
		if (wFlag == 0xFEFF)
		{
			// 复制 UNICODE 数据
			iOffset = 1;
			if (dwSize == sizeof(_TCHAR))
			{
				m_iSize = 0;
			}
			else
			{
				m_iSize = (dwSize-sizeof(_TCHAR)) / sizeof(_TCHAR);
				m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
				memcpy(m_lpszText, (LPTSTR)lpData+iOffset, m_iSize*sizeof(_TCHAR));
			}
		}
		else if (wFlag == 0xFFFE)
		{
			// 复制 UNICODE BIG ENDIAN 数据
			iOffset = 1;
			if (dwSize == sizeof(_TCHAR))
			{
				m_iSize = 0;
			}
			else
			{
				BYTE *pText = (BYTE*)lpData+iOffset;
				// 交换每两个字节的位置
				for(DWORD i = 0; i < dwSize - 3; i += 2)
				{
					BYTE tmp = pText[iOffset+i];
					pText[iOffset+i] = pText[iOffset+i+1];
					pText[iOffset+i+1] = tmp;
				}
				m_iSize = (dwSize-sizeof(_TCHAR)) / sizeof(_TCHAR);
				m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
				memcpy(m_lpszText, (LPTSTR)lpData+iOffset, m_iSize*sizeof(_TCHAR));
				pText = NULL;
				delete pText;
			}
		}
		else if (dwFlag == 0xBFBBEF)
		{
			// 从 UTF-8 转换到 UNICODE 数据
			iOffset = 3;
			if (dwSize == 3)
			{
				m_iSize = 0;
			}
			else
			{
				m_iSize = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpData+iOffset, dwSize-3, NULL, 0);
				m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
				MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpData+iOffset, dwSize-3, m_lpszText, m_iSize);
			}
		}
		else
		{
			// 从 ANSI 转换到 UNICODE 数据
			iOffset = 0;
			if (dwSize == 0)
			{
				m_iSize = 0;
			}
			else
			{
				m_iSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpData+iOffset, dwSize, NULL, 0);
				m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpData+iOffset, dwSize, m_lpszText, m_iSize);
			}
		}
		free(lpData);

		// 恢复滚动条位置
		SetScrollPos(SB_HORZ, 0, TRUE);
		SetScrollPos(SB_VERT, 0, TRUE);

		// 取消列表框显示状态
		m_bCopying = FALSE;

		// 更新文本
		m_iSize += 2;
		if (m_iSize == 2)
			m_lpszText = (LPTSTR)malloc(2*sizeof(_TCHAR));
		else
			m_lpszText = (LPTSTR)realloc(m_lpszText, m_iSize*sizeof(_TCHAR));
		m_lpszText[m_iSize-2] = '\r';
		m_lpszText[m_iSize-1] = '\n';

		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
		m_UndoInfo.nLen = 0;
		m_UndoInfo.iChar = 0;
		m_UndoInfo.iLine = 0;
		m_UndoInfo.iEndChar = 0;
		m_UndoInfo.iEndLine = 0;
		m_UndoInfo.iLastLineLen = 0;

		m_changed = FALSE;
		m_bCanUndo = FALSE;

		// 销毁列表框
		DestroyList();

		// 更新控件
		UpdateControl();

		// 更新屏幕
		Invalidate(FALSE);
		UpdateWindow();
	}

	return bResult;
}

BOOL CCodeEdit::Save(LPTSTR lpszFilePath)
{
	BOOL bResult = FALSE;

	switch(m_FileEncoding)
	{
	case FET_ANSI:
		{
			// 计算 ANSI 字符串长度
			int ansiLen = WideCharToMultiByte(CP_ACP, NULL, m_lpszText, m_iSize - 2, NULL, 0, NULL, NULL);
			// 分配空间要给'\0'留个空间
			char* szAnsi = new char[ansiLen];
			// 转换
			WideCharToMultiByte(CP_ACP, NULL, m_lpszText, m_iSize - 2, szAnsi, ansiLen, NULL, NULL);
			// 写文本文件，ANSI文件没有BOM
			CFile cFile;
			if (cFile.Open(lpszFilePath, CFile::modeWrite | CFile::modeCreate))
			{
				// 文件开头
				cFile.SeekToBegin();
				// 写入内容
				cFile.Write(szAnsi, ansiLen * sizeof(char));
				cFile.Flush();
				cFile.Close();
				m_changed = FALSE;
				bResult = TRUE;
			}
			delete[] szAnsi;
			szAnsi = NULL;
		}
		break;
	case FET_UNICODE:
		{
			CFile cFile;
			if (cFile.Open(lpszFilePath, CFile::modeWrite | CFile::modeCreate))
			{
				// 文件开头
				cFile.SeekToBegin();
				// 头 2 个字节 0xfeff，低位 0xff 写在前
				cFile.Write("\xff\xfe", 2);
				// 写入内容
				cFile.Write(m_lpszText, (m_iSize - 2) * sizeof(wchar_t));
				cFile.Flush();
				cFile.Close();
				m_changed = FALSE;
				bResult = TRUE;
			}
		}
		break;
	case FET_UNICODE_BE:
		{
			DWORD dwSize = (DWORD)((m_iSize - 2) * sizeof(wchar_t));
			LPBYTE lpData = (LPBYTE)malloc(dwSize*sizeof(BYTE));
			memcpy(lpData, (LPBYTE)m_lpszText, dwSize*sizeof(BYTE));
			// 交换每两个字节的位置
			for(DWORD i = 0; i < dwSize; i += 2)
			{
				BYTE tmp = lpData[i];
				lpData[i] = lpData[i+1];
				lpData[i+1] = tmp;
			}
			CFile cFile;
			if (cFile.Open(lpszFilePath, CFile::modeWrite | CFile::modeCreate))
			{
				// 文件开头
				cFile.SeekToBegin();
				// 头 2 个字节 0xfffe，低位 0xfe 写在前
				cFile.Write("\xfe\xff", 2);
				// 写入内容
				cFile.Write(lpData, dwSize);
				cFile.Flush();
				cFile.Close();
				m_changed = FALSE;
				bResult = TRUE;
			}
		}
		break;
	case FET_UTF_8:
		{
			int u8Len = WideCharToMultiByte(CP_UTF8, NULL, m_lpszText, m_iSize - 2, NULL, 0, NULL, NULL);
			// UTF8 虽然是 Unicode 的压缩形式，但也是多字节字符串，所以可以以 char 的形式保存
			char* szU8 = new char[u8Len];
			// 转换
			WideCharToMultiByte(CP_UTF8, NULL, m_lpszText, m_iSize - 2, szU8, u8Len, NULL, NULL);
			// 写文本文件，UTF8 的 BOM 是 0xbfbbef
			CFile cFile;
			if (cFile.Open(lpszFilePath, CFile::modeWrite | CFile::modeCreate))
			{
				// 文件开头
				cFile.SeekToBegin();
				// 写 BOM，同样低位写在前
				cFile.Write("\xef\xbb\xbf", 3);
				// 写入内容
				cFile.Write(szU8, u8Len * sizeof(char));
				cFile.Flush();
				cFile.Close();
				m_changed = FALSE;
				bResult = TRUE;
			}
			delete[] szU8;
			szU8 =NULL;
		}
		break;
	}
	return bResult;
}

BOOL CCodeEdit::GetChangedStatus()
{
	return m_changed;
}

void CCodeEdit::SetChangedStatus(BOOL status)
{
	m_changed = status;
}

void CCodeEdit::DeleteText(int iStartLine, int iStartChar, int iEndLine, int iEndChar)
{
	// 文本不为空
	if (m_lpszText != NULL)
	{
		// 格式化选区
		m_iLineStartSelection = max(0, iStartLine);
		m_iCharStartSelection = max(0, iStartChar);
		m_iLineEndSelection = min(m_iNumberLines-1, iEndLine);
		if (m_iLineEndSelection == -1)
			m_iLineEndSelection = m_iNumberLines - 1;
		m_iCharEndSelection = min(m_lpLineInfo[m_iLineEndSelection].nLen, iEndChar);
		if (m_iCharEndSelection == -1)
			m_iCharEndSelection = m_lpLineInfo[m_iLineEndSelection].nLen;

		// 删除选中的区域
		OnChar(VK_BACK, 0, 0);
		m_changed = TRUE;
		m_bCanUndo = TRUE;
	}
}

LPTSTR CCodeEdit::GetTextBuffer()
{
	return m_lpszText;
}

int CCodeEdit::GetBufferLen()
{
	return m_iSize;
}

void CCodeEdit::GetTextFromLine(int iLineIndex, LPTSTR lpszText)
{
	// 检查无效数据
	int iLine = max(0, min(m_iNumberLines-1, iLineIndex));
	if (lpszText != NULL)
	{
		// 获得行文本
		int iOffset = m_lpLineInfo[iLine].dwOffset;
		int iOldOffset = iOffset;
		LPTSTR lpszNextLine = NULL;
		if ((lpszNextLine=GetLine(iOffset)) != NULL)
		{
			// 计算行长度
			int iLen = iOffset - iOldOffset;
			if ((lpszNextLine[iLen-2] == '\r') && (lpszNextLine[iLen-1] == '\n'))
				iLen = iLen - 2;
			else if (lpszNextLine[iLen-1] == '\n')
				iLen = iLen - 1;

			// 复制行
			_tcsncpy_s(lpszText, iLen+1, lpszNextLine, iLen);
			lpszText[iLen] = '\0';
		}
	}
}

int CCodeEdit::LineFromPosition(POINT pt)
{
	int iResult = -1;

	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 获得行号
		iResult = min(pt.y/m_szTextSize.cy, m_iNumberLines-1);
	}

	return iResult;
}

int CCodeEdit::CharFromPosition(int iLineIndex, POINT pt)
{
	int iResult = -1;

	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 获得字符序号
		int iOffset = m_lpLineInfo[iLineIndex].dwOffset;
		int iOldOffset = iOffset;
		int iCurrentLine = iLineIndex;
		LPTSTR lpszNextLine = NULL;
		while ((lpszNextLine=GetLine(iOffset)) != NULL)
		{
			// 计算行长度
			int iLen = iOffset - iOldOffset;
			iOldOffset = iOffset;

			// 检查无效行
			if (iCurrentLine == iLineIndex)
			{
				// 获得字符序号
				BOOL bDone = FALSE;
				int iStartTAB=-1;
				int iEndTAB, iTABs;
				int iCharOffset = 0;
				for (int i=0; i<m_lpLineInfo[iLineIndex].nLen; i++)
				{
					// 检查 TAB 字符
					if (lpszNextLine[i] == '\t')
					{
						if (iStartTAB == -1)
						{
							iTABs = m_iTABs;
							iStartTAB = i;
						}
						else
						{
							iEndTAB = i;
							iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
							if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
								iTABs++;
							if ((iEndTAB - iStartTAB) == 1)
								iTABs = m_iTABs;
							iStartTAB = iEndTAB;
						}
						iCharOffset += (iTABs * m_szTextSize.cx);
					}
					else
					{
						if(IsCJKChar(lpszNextLine[i]))		// 字符是CJK汉字
						{
							iCharOffset += m_szCJKTextSize.cx;
						}
						else
							iCharOffset += m_szTextSize.cx;
					}
					int PointOffset;
					if (m_LineNumbers)
						PointOffset = (int(pt.x) - 4 * m_szTextSize.cx - LINE_NUMBER_TO_EDITOR);
					else
						PointOffset = int(pt.x) + m_szTextSize.cx - LINE_NUMBER_TO_EDITOR;
					if (iCharOffset >= PointOffset)
					{
						bDone = TRUE;
						iResult = i;
						break;
					}
				}
				if (!bDone)
					iResult = m_lpLineInfo[iLineIndex].nLen;
			}
			iCurrentLine++;
			break;
		}
	}

	return iResult;
}

POINT CCodeEdit::PositionFromChar(int iLineIndex, int iCharIndex)
{
	POINT ptResult = {0, 0};

	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 获得字符位置
		int iOffset = m_lpLineInfo[iLineIndex].dwOffset;
		int iOldOffset = iOffset;
		int iCurrentLine = iLineIndex;
		LPTSTR lpszNextLine = NULL;
		while ((lpszNextLine=GetLine(iOffset)) != NULL)
		{
			// 计算行长度
			int iLen = iOffset - iOldOffset;
			iOldOffset = iOffset;

			// 检查无效行
			if (iCurrentLine == iLineIndex)
			{
				// 获得字符序号
				BOOL bDone = FALSE;
				int iStartTAB=-1;
				int iEndTAB, iTABs;
				int iCharOffset = 0;
				for (int i=0; i<=m_lpLineInfo[iLineIndex].nLen; i++)
				{
					// 检查找到的字符
					if (i == iCharIndex)
					{
						bDone = TRUE;
						ptResult.y = iLineIndex * m_szTextSize.cy;
						ptResult.x = iCharOffset;
						break;
					}
					else
					{
						// 检查 TAB 字符
						if (lpszNextLine[i] == '\t')
						{
							if (iStartTAB == -1)
							{
								iTABs = m_iTABs;
								iStartTAB = i;
							}
							else
							{
								iEndTAB = i;
								iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
								if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
									iTABs++;
								if ((iEndTAB - iStartTAB) == 1)
									iTABs = m_iTABs;
								iStartTAB = iEndTAB;
							}
							iCharOffset += (iTABs * m_szTextSize.cx);
						}
						else
						{
							if(IsCJKChar(lpszNextLine[i]))		// 字符是CJK汉字
								iCharOffset += m_szCJKTextSize.cx;
							else
								iCharOffset += m_szTextSize.cx;
						}
					}
				}
				if (!bDone)
				{
					ptResult.y = iLineIndex * m_szTextSize.cy;
					ptResult.x = iCharOffset - m_szTextSize.cx;
				}
			}
			iCurrentLine++;
			break;
		}
	}

	ptResult.x += LINE_NUMBER_TO_EDITOR;
	if (m_LineNumbers) ptResult.x += 5 * m_szTextSize.cx;

	return ptResult;
}

int CCodeEdit::GetCharOffset(int iLineIndex, int iCharIndex)
{
	int iResult = 0;

	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 获得字符偏移量
		iResult = m_lpLineInfo[iLineIndex].dwOffset + iCharIndex;
	}

	return iResult;
}

int CCodeEdit::GetCharIndex(int iLine, int iColumn)
{
	int iCNum=0, i=0;

	// 遍历判断字符长度
	for (i = 0; i < m_lpLineInfo[iLine].nLen; i++)
	{
		if (IsCJKChar(m_lpszText[m_lpLineInfo[iLine].dwOffset + i]))
			iCNum += 2;
		else if (m_lpszText[m_lpLineInfo[iLine].dwOffset + i] == '\t')
			iCNum += m_iTABs;
		else
			iCNum++;
		for (int j = 0; j < m_iTABs; j++)
		{
			if (iCNum == iColumn + j) return i+1;
		}
	}
	return i+1;
}

int CCodeEdit::GetColumnIndex(int iLine, int iChar)
{
	int iCNum=0, i=0;

	// 遍历计算字符宽度
	for (i = 0; i < iChar; i++)
	{
		if (IsCJKChar(m_lpszText[m_lpLineInfo[iLine].dwOffset + i]))
			iCNum += 2;
		else if (m_lpszText[m_lpLineInfo[iLine].dwOffset + i] == '\t')
			iCNum += m_iTABs;
		else
			iCNum++;
	}

	return iCNum;
}

void CCodeEdit::UpdateCaret()
{
	// 确保光标可见
	EnsureVisible(m_iCurrentLine, m_iCurrentChar);

	// 获得滚动条信息
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// 设置字符位置
	POINT pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
	pt.x -= iHorizontalOffset;
	pt.y -= iVerticalOffset;
	SetCaretPos(pt);
}

void CCodeEdit::EnsureVisible(int iLineIndex, int iCharIndex)
{
	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 检查无效文本尺寸
		if (m_iSize == 2)
		{
			// 更新屏幕
			Invalidate(FALSE);
			UpdateWindow();
			return;
		}

		// 清除正在绘制标志
		SetRedraw(FALSE);

		// 获得客户区大小和位置
		RECT rcClient;
		GetClientRect(&rcClient);

		// 获得滚动条信息
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// 获得可见行范围
		POINT ptTop = {0, rcClient.top+iVerticalOffset};
		POINT ptBottom = {0, (rcClient.bottom-rcClient.top)+iVerticalOffset};
		int iLineStart = LineFromPosition(ptTop);
		int iLineEnd = LineFromPosition(ptBottom);
		RECT rcLine = {-iHorizontalOffset, 0, rcClient.right, m_szTextSize.cy};
		POINT ptLeft = {iHorizontalOffset, 0};
		POINT ptRight = {rcLine.right+iHorizontalOffset, 0};
		int iCharStart = CharFromPosition(iLineIndex, ptLeft);
		int iCharEnd = CharFromPosition(iLineIndex, ptRight);

		// 获得当前光标位置
		POINT ptCurrent = PositionFromChar(iLineIndex, iCharIndex);

		// 确保光标可见
		if (iLineIndex >= iLineEnd)
		{
			// 向下滚动
			int iNumberScrolls = iLineIndex - iLineEnd;
			for (int i=0; i<=iNumberScrolls; i++)
				OnVScroll(SB_LINEDOWN, 0, NULL);
		}
		else if (iLineIndex < iLineStart)
		{
			// 向上滚动
			int iNumberScrolls = iLineStart - iLineIndex;
			for (int i=0; i<iNumberScrolls; i++)
				OnVScroll(SB_LINEUP, 0, NULL);
		}
		if ((iCharIndex > iCharEnd) && (iLineIndex <= m_iNumberLines-1))
		{
			// 向右滚动
			POINT ptEnd = PositionFromChar(iLineIndex, iCharEnd);
			int iNumberScrolls = (ptCurrent.x - ptEnd.x) / m_szTextSize.cx;
			for (int i=0; i<iNumberScrolls; i++)
				OnHScroll(SB_LINERIGHT, 0, NULL);
		}
		else if (iCharIndex <= iCharStart)
		{
			// 向左滚动
			int iNumberScrolls = (iHorizontalOffset - ptCurrent.x) / m_szTextSize.cx;
			for (int i=0; i<=iNumberScrolls; i++)
				OnHScroll(SB_LINELEFT, 0, NULL);
		}

		// 设置正在绘制标志
		SetRedraw(TRUE);
	}

	// 更新屏幕
	Invalidate(FALSE);
	UpdateWindow();
}

LPTSTR CCodeEdit::GetLine(int& iOffset)
{
	LPTSTR lpszResult = NULL;

	// 检查无效偏移量
	if (iOffset < m_iSize)
	{
		// 获取下一行
		lpszResult = m_lpszText + iOffset;
		LPTSTR lpszNextLine = _tcsstr(lpszResult, _T("\r\n"));
		if (lpszNextLine != NULL)
		{
			// 更新偏移量
			iOffset = (int)(lpszNextLine - m_lpszText + 2);
		}
		else
		{
			// 更新偏移量
			iOffset = m_iSize;
		}
	}

	return lpszResult;
}

void CCodeEdit::SelectWord(POINT pt, BOOL FirstLetter)
{
	// 获得滚动条信息
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// 获得字符
	int iSelectedLine = LineFromPosition(pt);
	int iSelectedChar = CharFromPosition(iSelectedLine, pt);

	// 检查单词双击
	int iOffset = m_lpLineInfo[iSelectedLine].dwOffset;
	int iOldOffset = iOffset;
	int iCurrentLine = iSelectedLine;
	LPTSTR lpszNextLine = NULL;
	while ((lpszNextLine=GetLine(iOffset)) != NULL)
	{
		// 计算行长度
		int iLen = iOffset - iOldOffset;
		iOldOffset = iOffset;

		// 检查无效行
		if (iCurrentLine == iSelectedLine)
		{
			// 检查选中的单词
			if (!IsSpecial(lpszNextLine[iSelectedChar]))
			{
				// 选择单词
				int iStart = iSelectedChar;
				while ((IsLetter(lpszNextLine[iStart])) && (iStart > 0))
					iStart--;
				if ((iStart >= 0) && !IsLetter(lpszNextLine[iStart]))
					iStart++;
				int iEnd = iSelectedChar;
				while ((IsLetter(lpszNextLine[iEnd])) && (iEnd < iLen))
					iEnd++;

				// 设置选区
				m_iLineStartSelection = iSelectedLine;
				m_iLineEndSelection = iSelectedLine;
				m_iCharStartSelection = iStart;
				m_iCharEndSelection = iEnd;

				// 设置光标位置
				m_iCurrentLine = iSelectedLine;
				if (FirstLetter)
					m_iCurrentChar = iStart;					
				else
					m_iCurrentChar = iEnd;
				POINT pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
				pt.x -= iHorizontalOffset;
				pt.y -= iVerticalOffset;
				SetCaretPos(pt);
			}
			else
			{
				// 清除选区
				m_iLineStartSelection = -1;
				m_iLineEndSelection = -1;
				m_iCharStartSelection = -1;
				m_iCharEndSelection = -1;
			}
			break;
		}
		iCurrentLine++;
	}
}

BOOL CCodeEdit::IsSpecial(_TCHAR tch)
{
	BOOL bResult = FALSE;

	// 检查特殊字符 —— 空格, TAB, 换行, 回车
	if ((tch == ' ') || (tch == '\t') || (tch == '\r') || (tch == '\n'))
	{
		// 退出
		bResult = TRUE;
	}

	return bResult;
}

BOOL CCodeEdit::IsLetter(_TCHAR tch)
{
	BOOL bResult = FALSE;

	// 检查字母和数字 —— 'A' 到 'Z', 'a' 到 'z', '0' 到 '9'
	if (((tch >= 'A') && (tch <= 'Z')) || ((tch >= 'a') && (tch <= 'z')) || ((tch >= '0') && (tch <= '9')) || (tch == '_') || (tch == '#'))
	{
		// 退出
		bResult = TRUE;
	}

	return bResult;
}

BOOL CCodeEdit::IsCJKChar(_TCHAR tch)
{
	BOOL bResult = FALSE;

	// 检查是否在Unicode中文CJK字符范围之内
	if ((tch >= 0x4E00 && tch <= 0x9FA5) || (tch >= 0x3400 && tch <= 0x4DB5) 
		|| (tch >= 0x20000 && tch <= 0x2A6D6) || (tch >= 0x9FA6 && tch <= 0x9FBB)
		|| (tch >= 0x2F800 && tch <= 0x2FA1D) || (tch >= 0xF900 && tch <= 0xFA2D)
		|| (tch >= 0xFA30 && tch <= 0xFA6A) || (tch >= 0xFA70 && tch <= 0xFAD9)
		|| (tch >= 0xFF00 && tch <= 0xFFEF)	|| (tch >= 0x2E80 && tch <= 0x2EFF)
		|| (tch >= 0x2010 && tch <= 0x33D5))
	{
		// 退出
		bResult = TRUE;
	}

	return bResult;
}

BOOL CCodeEdit::IsKeyword(LPTSTR lpszText, int iLen, int& iKeywordIndex, int& iKeywordOffset)
{
	BOOL bResult = FALSE;

	// 检查无效长度
	if (iLen == 0)
		return FALSE;

	// 格式化文本
	LPTSTR lpszKeywordText = new _TCHAR[iLen+1];
	_tcsncpy_s(lpszKeywordText, iLen+1, lpszText, iLen);
	lpszKeywordText[iLen] = '\0';
	if (!m_bKeywordsCaseSensitive)
	{
		if (m_bUper)
			_tcsupr_s(lpszKeywordText, iLen+1);
		else
			_tcslwr_s(lpszKeywordText, iLen+1);
	}

	// 检查关键词
	for (int i=0; i<m_iNumberKeywords; i++)
	{
		// 检查关键词
		LPTSTR lpszSearch = _tcsstr(lpszKeywordText, m_lpKeywordInfo[i].lpszTagName);
		if (lpszSearch != NULL)
		{
			// 检查无效关键词
			int iFirst = (int)(lpszSearch - lpszKeywordText - 1);
			int iLast = iFirst + m_lpKeywordInfo[i].nLen;
			bResult = TRUE;
			if (iFirst >= 0)
			{
				if (m_SyntaxColoring != SCT_HTML)
				{
					if (((lpszKeywordText[iFirst] >= 'A') && (lpszKeywordText[iFirst] <= 'Z')) || 
						((lpszKeywordText[iFirst] >= 'a') && (lpszKeywordText[iFirst] <= 'z')))
						bResult = FALSE;
				}

			}
			if (bResult)
			{
				if (((lpszKeywordText[iLast] >= 'A') && (lpszKeywordText[iLast] <= 'Z')) || 
					((lpszKeywordText[iLast] >= 'a') && (lpszKeywordText[iLast] <= 'z')))
					bResult = FALSE;
				else
				{
					// 退出
					iKeywordIndex = i;
					iKeywordOffset = iFirst + 1;
					break;
				}
			}
		}
	}
	delete lpszKeywordText;

	return bResult;
}

BOOL CCodeEdit::IsConstant(LPTSTR lpszText, int iLen, int& iConstantIndex, int& iConstantOffset)
{
	BOOL bResult = FALSE;

	// 格式化文本
	LPTSTR lpszConstantText = new _TCHAR[iLen+1];
	_tcsncpy_s(lpszConstantText, iLen+1, lpszText, iLen);
	lpszConstantText[iLen] = '\0';
	if (!m_bConstantsCaseSensitive)
	{
		if (m_bUper)
			_tcsupr_s(lpszConstantText, iLen+1);
		else
			_tcslwr_s(lpszConstantText, iLen+1);
	}

	// 检查常量
	for (int i=0; i<m_iNumberConstants; i++)
	{
		// 检查常量
		LPTSTR lpszSearch = _tcsstr(lpszConstantText, m_lpConstantInfo[i].lpszTagName);
		if (lpszSearch != NULL)
		{
			// 检查无效常量
			int iFirst = (int)(lpszSearch - lpszConstantText - 1);
			int iLast = iFirst + m_lpConstantInfo[i].nLen;
			bResult = TRUE;
			if (iFirst >= 0)
			{
				if (((lpszConstantText[iFirst] >= 'A') && (lpszConstantText[iFirst] <= 'Z')) || 
					((lpszConstantText[iFirst] >= 'a') && (lpszConstantText[iFirst] <= 'z')))
					bResult = FALSE;

			}
			if (bResult)
			{
				if (((lpszConstantText[iLast] >= 'A') && (lpszConstantText[iLast] <= 'Z')) || 
					((lpszConstantText[iLast] >= 'a') && (lpszConstantText[iLast] <= 'z')))
					bResult = FALSE;
				else
				{
					// 退出
					iConstantIndex = i;
					iConstantOffset = iFirst + 1;
					break;
				}
			}
		}
	}
	delete lpszConstantText;

	return bResult;
}

BOOL CCodeEdit::IsText(LPTSTR lpszText, int iLen, int& iTextStart, int& iTextEnd)
{
	BOOL bResult = FALSE;

	// 检查字符串
	iTextStart = -1;
	iTextEnd = -1;
	for (int i=0; i<iLen; i++)
	{
		// 检查字符串开始序号
		if ((iTextStart == -1) && ((lpszText[i] == '\"') || (lpszText[i] == '\'')))
		{
			iTextStart = i;
			bResult = TRUE;
		}
		else
		{
			// 检查字符串结束序号
			if ((iTextStart != -1) && ((lpszText[i] == '\"') || (lpszText[i] == '\'')))
				iTextEnd = i;
		}
	}

	return bResult;
}

void CCodeEdit::ShowAutoComplete()
{
	if (m_wndListBox.m_hWnd != NULL)
	{
		// 销毁列表框
		m_wndListBox.DestroyWindow();

		return;
	}

	// 获得滚动条信息
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// 创建列表框
	POINT pt = PositionFromChar(m_iCurrentLine, (m_iCurrentChar)==0 ? m_iCurrentChar : m_iCurrentChar-1);
	int iLine = m_iCurrentLine;
	int iChar = m_iCurrentChar;
	SelectWord(pt, TRUE);
	pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
	pt.x -= iHorizontalOffset;
	pt.y -= iVerticalOffset;
	RECT rect = {pt.x, pt.y+m_szTextSize.cy, pt.x+LIST_HEIGHT, pt.y+m_szTextSize.cy+LIST_WIDTH};
	m_wndListBox.Create(WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_HSCROLL | LBS_STANDARD | LBS_WANTKEYBOARDINPUT, rect, this, IDC_LIST_AUTOCOMPLETE);
	m_wndListBox.SetFont(&m_Font, TRUE);

	// 检查关键词或常量
	int iEndOffset = GetCharOffset(iLine, iChar);
	if (!IsLetter(m_lpszText[iEndOffset]))
		iEndOffset--;
	int iStartOffset = iEndOffset;
	while ((iStartOffset > 0) && (IsLetter(m_lpszText[iStartOffset])))
		iStartOffset--;
	if (iStartOffset > 0)
		iStartOffset++;
	int iLen = iEndOffset - iStartOffset + 1;
	if (iLen > 0)
	{
		LPTSTR lpszSearch = new _TCHAR[iLen+1];
		_tcsncpy_s(lpszSearch, iLen+1, m_lpszText+iStartOffset, iLen);
		lpszSearch[iLen] = '\0';
		if (!m_bKeywordsCaseSensitive)
		{
			if (m_bUper)
				_tcsupr_s(lpszSearch, iLen+1);
			else
				_tcslwr_s(lpszSearch, iLen+1);
		}
		for (int i=0; i<m_iNumberKeywords; i++)
		{
			// 检查无效关键词
			if (_tcsncmp(m_lpKeywordInfo[i].lpszTagName, lpszSearch, iLen) == 0)
			{
				// 添加关键词
				m_wndListBox.AddString(m_lpKeywordInfo[i].lpszTagName);
			}
		}
		if (m_bConstantsCaseSensitive)
		{
			delete lpszSearch;
			lpszSearch = new _TCHAR[iLen+1];
			_tcsncpy_s(lpszSearch, iLen+1, m_lpszText+iStartOffset, iLen);
			lpszSearch[iLen] = '\0';
		}
		for (int j=0; j<m_iNumberConstants; j++)
		{
			// 检查无效常量
			if (_tcsncmp(m_lpConstantInfo[j].lpszTagName, lpszSearch, iLen) == 0)
			{
				// 添加常量
				m_wndListBox.AddString(m_lpConstantInfo[j].lpszTagName);
			}
		}
		delete lpszSearch;
	}

	// 显示列表框
	if (m_wndListBox.GetCount() > 0)
	{
		m_wndListBox.SetFocus();
		m_wndListBox.SetCurSel(0);
		// 编辑框设置焦点以接收键盘操作
		((CCodeMateDlg*)GetParent())->m_bFocused = true;
	}
	else
	{
		m_wndListBox.DestroyWindow();
	}
}

void CCodeEdit::OnLbnDblclkListAutoComplete()
{
	// 获得选中关键词
	CString strKeyword;
	m_wndListBox.GetText(m_wndListBox.GetCurSel(), strKeyword);

	// 销毁列表框
	m_wndListBox.DestroyWindow();

	// 离开复制模式
	m_bCopying = FALSE;

	// 删除选中文本
	DeleteText();

	// 计算文本长度
	int iLen = strKeyword.GetLength();

	// 添加文本
	AddText((LPTSTR)((LPCTSTR)strKeyword), m_iCurrentLine, m_iCurrentChar, iLen);

	// 保存撤消信息
	m_UndoInfo.iEndLine = m_iCurrentLine;
	m_UndoInfo.iEndChar = m_iCurrentChar + iLen;

	// 更新控件
	UpdateControl();

	// 更新光标位置
	m_iCurrentChar += strKeyword.GetLength();
	UpdateCaret();
}

void CCodeEdit::SetKeywordColor(COLORREF cKeywordColor)
{
	m_crKeywordColor = cKeywordColor;
}

COLORREF CCodeEdit::GetKeywordColor()
{
	return m_crKeywordColor;
}

void CCodeEdit::SetKeywords(LPTSTR lpszKeywords, int iLen, BOOL bCaseSensitive, BOOL bUper)
{
	// 清除关键词
	for (int i=0; i<m_iNumberKeywords; i++)
		delete m_lpKeywordInfo[i].lpszTagName;
	free(m_lpKeywordInfo);
	m_lpKeywordInfo = NULL;

	// 设置大小写敏感标志
	m_bKeywordsCaseSensitive = bCaseSensitive;
	m_bUper = bUper;

	// 清空数量
	m_iNumberKeywords = 0;

	// 添加关键词
	AddKeywords(lpszKeywords, iLen);
}

void CCodeEdit::AddKeywords(LPTSTR lpszKeywords, int iLen)
{
	// 提取关键词
	int iOffset = 0;
	int iOldOffset = iOffset;
	int iCount;
	LPTSTR lpszNextKeyword = NULL;
	while ((lpszNextKeyword=_tcsstr(lpszKeywords+iOffset, _T(";"))) != NULL)
	{
		// 提取关键词
		iCount = (int)(lpszNextKeyword - (lpszKeywords+iOffset));
		iOffset = (int)(lpszNextKeyword - lpszKeywords);
		m_iNumberKeywords++;
		if (m_lpKeywordInfo == NULL)
			m_lpKeywordInfo = (LPTAGINFO)malloc(m_iNumberKeywords*sizeof(TAGINFO));
		else
			m_lpKeywordInfo = (LPTAGINFO)realloc(m_lpKeywordInfo, m_iNumberKeywords*sizeof(TAGINFO));
		m_lpKeywordInfo[m_iNumberKeywords-1].nLen = iCount + 1;
		m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName = new _TCHAR[m_lpKeywordInfo[m_iNumberKeywords-1].nLen];
		_tcsncpy_s(m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName, m_lpKeywordInfo[m_iNumberKeywords-1].nLen, lpszKeywords+iOldOffset, m_lpKeywordInfo[m_iNumberKeywords-1].nLen-1);
		m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName[m_lpKeywordInfo[m_iNumberKeywords-1].nLen-1] = '\0';
		if (!m_bKeywordsCaseSensitive)
		{
			if (m_bUper)
				_tcsupr_s(m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName, m_lpKeywordInfo[m_iNumberKeywords-1].nLen);
			else
				_tcslwr_s(m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName, m_lpKeywordInfo[m_iNumberKeywords-1].nLen);
		}
		iOffset++;
		iOldOffset = iOffset;
	}
}

void CCodeEdit::SetConstantColor(COLORREF crConstantColor)
{
	m_crConstantColor = crConstantColor;
}

COLORREF CCodeEdit::GetConstantColor()
{
	return m_crConstantColor;
}

void CCodeEdit::SetConstants(LPTSTR lpszConstants, int iLen, BOOL bCaseSensitive, BOOL bUper)
{
	// 清除常量
	for (int i=0; i<m_iNumberConstants; i++)
		delete m_lpConstantInfo[i].lpszTagName;
	free(m_lpConstantInfo);
	m_lpConstantInfo = NULL;

	// 设置大小写敏感标志
	m_bConstantsCaseSensitive = bCaseSensitive;
	m_bUper = bUper;

	// 清空数量
	m_iNumberConstants = 0;

	// 添加常量
	AddConstants(lpszConstants, iLen);
}

void CCodeEdit::AddConstants(LPTSTR lpszConstants, int iLen)
{
	// 提取常量
	int iOffset = 0;
	int iOldOffset = iOffset;
	int iCount;
	LPTSTR lpszNextConstant = NULL;
	while ((lpszNextConstant=_tcsstr(lpszConstants+iOffset, _T(";"))) != NULL)
	{
		// 提取常量
		iCount = (int)(lpszNextConstant - (lpszConstants+iOffset));
		iOffset = (int)(lpszNextConstant - lpszConstants);
		m_iNumberConstants++;
		if (m_lpConstantInfo == NULL)
			m_lpConstantInfo = (LPTAGINFO)malloc(m_iNumberConstants*sizeof(TAGINFO));
		else
			m_lpConstantInfo = (LPTAGINFO)realloc(m_lpConstantInfo, m_iNumberConstants*sizeof(TAGINFO));
		m_lpConstantInfo[m_iNumberConstants-1].nLen = iCount + 1;
		m_lpConstantInfo[m_iNumberConstants-1].lpszTagName = new _TCHAR[m_lpConstantInfo[m_iNumberConstants-1].nLen];
		_tcsncpy_s(m_lpConstantInfo[m_iNumberConstants-1].lpszTagName, m_lpConstantInfo[m_iNumberConstants-1].nLen, lpszConstants+iOldOffset, m_lpConstantInfo[m_iNumberConstants-1].nLen-1);
		m_lpConstantInfo[m_iNumberConstants-1].lpszTagName[m_lpConstantInfo[m_iNumberConstants-1].nLen-1] = '\0';
		if (!m_bConstantsCaseSensitive)
		{
			if (m_bUper)
				_tcsupr_s(m_lpConstantInfo[m_iNumberConstants-1].lpszTagName, m_lpConstantInfo[m_iNumberConstants-1].nLen);
			else
				_tcslwr_s(m_lpConstantInfo[m_iNumberConstants-1].lpszTagName, m_lpConstantInfo[m_iNumberConstants-1].nLen);
		}
		iOffset++;
		iOldOffset = iOffset;
	}
}

void CCodeEdit::UpdateSyntaxColoring()
{
	// 更新屏幕
	Invalidate(FALSE);
	UpdateWindow();
}

void CCodeEdit::SetCommentColor(COLORREF crCommentColor)
{
	m_crCommentColor = crCommentColor;
}

COLORREF CCodeEdit::GetCommentColor()
{
	return m_crCommentColor;
}

void CCodeEdit::SetTextColor(COLORREF crTextColor)
{
	m_crTextColor = crTextColor;
}

COLORREF CCodeEdit::GetTextColor()
{
	return m_crTextColor;
}

void CCodeEdit::SetNormalCodeColor(COLORREF crCodeColor)
{
	m_crCodeColor = crCodeColor;
}

COLORREF CCodeEdit::GetNormalCodeColor()
{
	return m_crCodeColor;
}

void CCodeEdit::SetColorStruct(COLORINFO coColor)
{
	m_crKeywordColor   = coColor.crKeywordColor;
	m_crConstantColor  = coColor.crConstantColor;
	m_crCommentColor   = coColor.crCommentColor;
	m_crTextColor      = coColor.crTextColor;
	m_crCodeColor	   = coColor.crForegroundColor;
	m_crBgColor		   = coColor.crBackgroundColor;
	m_crLineNumBgColor = coColor.crLineNumBGColor;
	m_crLineNumFgColor = coColor.crLineNumFGColor;
}

void CCodeEdit::GetColorStruct(COLORINFO &coColor)
{
	coColor.crKeywordColor    = m_crKeywordColor;
	coColor.crConstantColor	= m_crConstantColor;
	coColor.crCommentColor	= m_crCommentColor;
	coColor.crTextColor		= m_crTextColor;
	coColor.crForegroundColor = m_crCodeColor;
	coColor.crBackgroundColor = m_crBgColor;
	coColor.crLineNumBGColor	= m_crLineNumBgColor;
	coColor.crLineNumFGColor	= m_crLineNumFgColor;
}

void CCodeEdit::SetEditorFont(LPTSTR lpszFacename, LONG iHeight, BOOL bBold, BOOL bItalic, BYTE uiCharSet)
{
	m_Font.DeleteObject();
	m_Font.CreateFont(
		iHeight, // 高度
		0, 0, 0,
		(bBold ? FW_BOLD :FW_NORMAL),
		bItalic, // 倾斜
		FALSE, // 下划线
		FALSE, // 删除线
		uiCharSet, // 字符集
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_MODERN,
		lpszFacename); // 字体名称
}

void CCodeEdit::SetDefaultFileEncoding(FILE_ENCODING_TYPE fileEncoding)
{
	m_FileEncoding = fileEncoding;
}

void CCodeEdit::SetSyntaxColoring(SYNTAX_COLORING_TYPE syntaxColoring)
{
	// 设置语法着色信息
	m_SyntaxColoring = syntaxColoring;

	// 更新语法着色
	switch (m_SyntaxColoring)
	{
	case SCT_NONE:
		{
			SetKeywords(_T(""), 0, TRUE, FALSE);
			SetConstants(_T(""), 0, TRUE, FALSE);
			break;
		}

	case SCT_C_ANSI:
		{
			// 设置 ANSI C 语法着色
			CString strKeywords = _T("auto;__asm;__based;break;case;__cdecl;char;const;continue;__declspec;default;");
			strKeywords += _T("dllexport;dllimport;do;double;__except;else;enum;extern;__fastcall;__finally;float;for;");
			strKeywords += _T("goto;__inline;__int8;__int16;__int32;__int64;if;int;__leave;long;naked;register;return;");
			strKeywords += _T("__stdcall;short;signed;sizeof;static;struct;switch;__try;thread;typedef;union;unsigned;");
			strKeywords += _T("void;volatile;while;once;pack;#define;#elif;#else;#endif;#error;");
			strKeywords += _T("#if;#ifdef;#ifndef;#import;#include;#line;#pragma;#undef;#using;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), TRUE, FALSE);
			SetConstants(_T(""), 0, TRUE, FALSE);
		}
		break;

	case SCT_C_PLUS_PLUS:
		{
			// 设置 C++ 语法着色
			CString strKeywords = _T("__abstract;__alignof;__asm;__assume;__based;__box;__cdecl;__declspec;__delegate;");
			strKeywords += _T("__event;__except;__fastcall;__finally;__forceinline;__gc;__hook;__identifier;__if_exists;");
			strKeywords += _T("__if_not_exists;__inline;__int8;__int16;__int32;__int64;__interface;__leave;__m64;__m128;");
			strKeywords += _T("__m128d;__m128i;__multiple_inheritance;__nogc;__noop;__pin;__property;__raise;__sealed;");
			strKeywords += _T("__single_inheritance;__stdcall;__super;__try_cast;__try;__except;__unhook;__uuidof;");
			strKeywords += _T("__value;__virtual_inheritance;__w64;bool;break;case;catch;char;class;const;const_cast;continue;");
			strKeywords += _T("default;delete;deprecated;dllexport;dllimport;do;double;dynamic_cast;else;enum;explicit;extern;");
			strKeywords += _T("false;float;for;friend;goto;if;inline;int;long;mutable;naked;namespace;new;noinline;noreturn;");
			strKeywords += _T("nothrow;novtable;operator;private;property;protected;public;register;reinterpret_cast;return;");
			strKeywords += _T("selectany;short;signed;sizeof;static;static_cast;struct;switch;template;this;thread;throw;true;");
			strKeywords += _T("try;typedef;typeid;typename;union;unsigned;using;uuid;virtual;void;volatile;__wchar_t;wchar_t;while;");
			strKeywords += _T("auto;#define;#elif;#else;#endif;#error;#if;#ifdef;#ifndef;#import;#include;");
			strKeywords += _T("#line;#pragma;#undef;#using;auto_rename;auto_search;embedded_idl;exclude;once;pack;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), TRUE, FALSE);
			CString strConstants = _T("TRUE;FALSE;NULL;__LINE__;__FILE__;__DATE__;__TIME__;_cplusplus;__STDC__;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), TRUE, FALSE);
		}
		break;

	case SCT_C_SHARP:
		{
			// 设置 C# 语法着色
			CString strKeywords = _T("abstract;as;base;bool;break;byte;case;catch;char;checked;class;const;continue;decimal;default;");
			strKeywords += _T("delegate;do;double;else;enum;explicit;extern;False;finally;fixed;float;for;foreach;goto;if;implicit;");
			strKeywords += _T("in;int;interface;internal;is;lock;long;new;null;object;operator;out;override;params;private;protected;");
			strKeywords += _T("public;readonly;ref;return;sbyte;sealed;short;sizeof;stackalloc;static;string;volatile;while;true;false;");
			strKeywords += _T("struct;switch;this;throw;True;try;typeof;uint;ulong;unchecked;unsafe;ushort;using;virtual;void;");
			strKeywords += _T("add;dynamic;from;get;global;group;into;join;let;orderby;partial;remove;select;set;value;var;where;yield;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), TRUE, FALSE);
			CString strConstants = _T("TRUE;FALSE;NULL;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), TRUE, FALSE);
		}
		break;

	case SCT_HTML:
		{
			// 设置 HTML 语法着色
			CString strKeywords = _T("<a>;</a>;<a;<acronym>;</acronym>;<acronym;<address>;</address>;<address;<applet>;");
			strKeywords += _T("</applet>;<applet;<area>;</area>;<area;<b>;</b>;<b;<base>;</base>;<base;<baseFont>;</baseFont>;");
			strKeywords += _T("<baseFont;<bdo>;</bdo>;<bdo;<bgSound>;</bgSound>;<bgSound;<big>;</big>;<big;<blockQuote>;");
			strKeywords += _T("</blockQuote>;<blockQuote;<body>;</body>;<body;<br>;</br>;<br;<button>;</button>;<button;");
			strKeywords += _T("<caption>;</caption>;<caption;<center>;</center>;<center;<cite>;</cite>;<cite;<code>;</code>;<code;");
			strKeywords += _T("<col>;</col>;<col;<colGroup>;</colGroup>;<colGroup;<comment>;</comment>;<comment;<custom>;</custom>;<custom;");
			strKeywords += _T("<dd>;</dd>;<dd;<del>;</del>;<del;<dfn>;</dfn>;<dfn;<dir>;</dir>;<dir;<div>;</div>;<div;<dl>;</dl>;<dl;");
			strKeywords += _T("<dt>;</dt>;<dt;<em>;</em>;<em;<embed>;</embed>;<embed;<fieldSet>;</fieldSet>;<fieldSet;<font>;</font>;<font;");
			strKeywords += _T("<form>;</form>;<form;<frame>;</frame>;<frame;<frameSet>;</frameSet>;<frameSet;<head>;</head>;<head;");
			strKeywords += _T("<hn>;</hn>;<hn;<hr>;</hr>;<hr;<html>;</html>;<html;<i>;</i>;<i;<iframe>;</iframe>;<iframe;<img>;</img>;<img;");
			strKeywords += _T("<input>;</input>;<input;<ins>;</ins>;<ins;<isIndex>;</isIndex>;<isIndex;<kbd>;</kbd>;<kbd;<label>;</label>;<label;");
			strKeywords += _T("<legend>;</legend>;<legend;<li>;</li>;<li;<link>;</link>;<link;<listing>;</listing>;<listing;<map>;</map>;<map;");
			strKeywords += _T("<marquee>;</marquee>;<marquee;<menu>;</menu>;<menu;<meta>;</meta>;<meta;<noBR>;</noBR>;<noBR;");
			strKeywords += _T("<noFrames>;</noFrames>;<noFrames;<noScript>;</noScript>;<noScript;<object>;</object>;<object;<ol>;</ol>;<ol;");
			strKeywords += _T("<optGroup>;</optGroup>;<optGroup;<option>;</option>;<option;<p>;</p>;<p;<param>;</param>;<param;");
			strKeywords += _T("<plainText>;</plainText>;<plainText;<pre>;</pre>;<pre;<q>;</q>;<q;<rt>;</rt>;<rt;<ruby>;</ruby>;<ruby;");
			strKeywords += _T("<s>;</s>;<s;<samp>;</samp>;<samp;<script>;</script>;<script;<select>;</select>;<select;<small>;</small>;<small;");
			strKeywords += _T("<span>;</span>;<span;<strike>;</strike>;<strike;<strong>;</strong>;<strong;<style>;</style>;<style;");
			strKeywords += _T("<sub>;</sub>;<sub;<sup>;</sup>;<sup;<table>;</table>;<table;<tBody>;</tBody>;<tBody;<td>;</td>;<td;");
			strKeywords += _T("<textArea>;</textArea>;<textArea;<tFoot>;</tFoot>;<tFoot;<th>;</th>;<th;<tHead>;</tHead>;<tHead;");
			strKeywords += _T("<title>;</title>;<title;<tr>;</tr>;<tr;<tt>;</tt>;<tt;<u>;</u>;<u;<ul>;</ul>;<ul;<var>;</var>;<var;");
			strKeywords += _T("<wbr>;</wbr>;<wbr;<xml>;</xml>;<xml;<xmp>;</xmp>;<xmp;<h1>;</h1>;<h1;<h2>;</h2>;<h2;<h3>;</h3>;<h3;");
			strKeywords += _T("<h4>;</h4>;<h4;<h5>;</h5>;<h5;<h6></h6>;<h6;<;/>;>;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), FALSE, TRUE);
			SetConstants(_T(""), 0, TRUE, FALSE);
		}
		break;

	case SCT_JSCRIPT:
		{
			// 设置 Java Script 语法着色
			CString strKeywords = _T("break;catch;@cc_on;continue;debugger;do;else;@elif;@else;@end;for;function;in;@if;if;");
			strKeywords += _T("new;return;switch;case;default;@set;this;throw;try;var;while;with;delete;finally;instanceof;typeof;");
			strKeywords += _T("abstract;boolean;byte;char;class;const;double;enum;export;extends;final;float;goto;implements;import;int;");
			strKeywords += _T("interface;long;native;package;private;protected;public;short;static;super;synchronized;throws;transient;volatile;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), FALSE, FALSE);
			CString strConstants = _T("true;false;null;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), FALSE, FALSE);
		}
		break;

	case SCT_VBSCRIPT:
		{
			// 设置 VB Script 语法着色
			CString strKeywords = _T("static;dim;public;private;const;option;explicit;do;else;for;function;sub;step;end;if;then;");
			strKeywords += _T("new;select;as;to;on;error;resume;while;with;goto;case;call;msgbox;next;loop;wend;each;");
			strKeywords += _T("variant;byte;boolean;integer;long;single;double;currency;date;object;string;is;not;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), FALSE, FALSE);
			CString strConstants = _T("true;false;null;empty;nothing;pi;scode;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), FALSE, FALSE);
		}
		break;


	case SCT_SQL:
		{
			// 设置 SQL 语法着色
			CString strKeywords = _T("@@IDENTITY;ENCRYPTION;ORDER;ADD;END;ALL;ERRLVL;OVER;ALTER;ESCAPE;PERCENT;");
			strKeywords += _T("AND;EXCEPT;PLAN;ANY;EXEC;PRECISION;AS;EXECUTE;PRIMARY;EXISTS;PRINT;AUTHORIZATION;");
			strKeywords += _T("EXIT;PROC;EXPRESSION;PROCEDURE;BACKUP;FETCH;PUBLIC;BEGIN;FILE;RAISERROR;BETWEEN;");
			strKeywords += _T("FILLFACTOR;READ;ASC;BREAK;FOR;READTEXT;BROWSE;FOREIGN;RECONFIGURE;BULK;FREETEXT;");
			strKeywords += _T("REFERENCES;BY;FREETEXTTABLE;REPLICATION;CASCADE;FROM;RESTORE;CASE;RESTRICT;CHECK;");
			strKeywords += _T("FUNCTION;RETURN;CHECKPOINT;GOTO;REVOKE;CLOSE;GRANT;CLUSTERED;GROUP;ROLLBACK;");
			strKeywords += _T("COALESCE;HAVING;ROWCOUNT;COLLATE;HOLDLOCK;ROWGUIDCOL;COLUMN;IDENTITY;RULE;COMMIT;IDENTITY_INSERT;");
			strKeywords += _T("SAVE;COMPUTE;IDENTITYCOL;SCHEMA;CONSTRAINT;IF;SELECT;CONTAINS;IN;SESSION_USER;CONTAINSTABLE;");
			strKeywords += _T("INDEX;SET;CONTINUE;SETUSER;INSERT;SHUTDOWN;INTERSECT;SOME;CREATE;INTO;");
			strKeywords += _T("STATISTICS;IS;CURRENT;JOIN;SYSTEM_USER;CURRENT_DATE;KEY;TABLE;CURRENT_TIME;KILL;");
			strKeywords += _T("TEXTSIZE;CURRENT_TIMESTAMP;THEN;CURRENT_USER;LIKE;TO;CURSOR;LINENO;TOP;DATABASE;LOAD;");
			strKeywords += _T("TRAN;DATABASEPASSWORD;TRANSACTION;DATEADD;TRIGGER;DATEDIFF;NATIONAL;TRUNCATE;DATENAME;");
			strKeywords += _T("NOCHECK;TSEQUAL;DATEPART;NONCLUSTERED;UNION;DBCC;NOT;UNIQUE;DEALLOCATE;UPDATE;DECLARE;NULLIF;");
			strKeywords += _T("UPDATETEXT;DEFAULT;OF;USE;DELETE;OFF;USER;DENY;OFFSETS;VALUES;DESC;ON;VARYING;DISK;OPEN;VIEW;");
			strKeywords += _T("DISTINCT;OPENDATASOURCE;WAITFOR;DISTRIBUTED;OPENQUERY;WHEN;DOUBLE;OPENROWSET;WHERE;DROP;OPENXML;");
			strKeywords += _T("WHILE;DUMP;OPTION;WITH;ELSE;OR;WRITETEXT;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), FALSE, TRUE);
			CString strConstants = _T("INNER;OUTER;CROSS;FULL;LEFT;RIGHT;TRUE;FALSE;NULL;COLUMNS;INDEXES;KEY_COLUMN_USAGE;DECIMAL;");
			strConstants += _T("PROVIDER_TYPES;TABLES;TABLE_CONSTRAINTS;BIGINT;INTEGER;SMALLINT;TINYINT;BIT;NUMERIC;MONEY;CHAR;VARCHAR;");
			strConstants += _T("FLOAT;REAL;DATETIME;NCHAR;NVARCHAR;NTEXT;BINARY;VARBINARY;IMAGE;UNIQUEIDENTIFIER;IDENTITY;ROWGUIDCOL;");
			strConstants += _T("AVG;MAX;MIN;SUM;COUNT;DATEADD;DATEDIFF;DATENAME;DATEPART;GETDATE;ABS;ACOS;ASIN;ATAN;ATN2;CEILING;COS;");
			strConstants += _T("COT;DEGREES;EXP;FLOOR;LOG;LOG10;PI;POWER;RADIANS;RAND;ROUND;SIGN;SIN;SQRT;TAN;NCHAR;CHARINDEX;LEN;");
			strConstants += _T("LOWER;LTRIM;PATINDEX;REPLACE;REPLICATE;RTRIM;SPACE;STR;STUFF;SUBSTRING;UNICODE;UPPER;CONVERT;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), FALSE, TRUE);
		}
		break;
	}
}

SYNTAX_COLORING_TYPE CCodeEdit::GetSyntaxColoring()
{
	return m_SyntaxColoring;
}

void CCodeEdit::SetCurrentLine(int index)
{
	m_iCurrentLine = max(0, min(m_iNumberLines-1, index));

	// 取消选区
	m_iLineStartSelection = -1;
	m_iLineEndSelection = -1;
	m_iCharStartSelection = -1;
	m_iCharEndSelection = -1;
}

int CCodeEdit::GetCurrentLine()
{
	return m_iCurrentLine;
}

void CCodeEdit::SetCurrentChar(int index)
{
	m_iCurrentChar = max(0, min(m_lpLineInfo[m_iCurrentLine].nLen, index));

	// 取消选区
	m_iLineStartSelection = -1;
	m_iLineEndSelection = -1;
	m_iCharStartSelection = -1;
	m_iCharEndSelection = -1;
}

int CCodeEdit::GetCurrentChar()
{
	return m_iCurrentChar;
}

int CCodeEdit::GetCurrentColumn()
{
	return GetColumnIndex(m_iCurrentLine, m_iCurrentChar);
}

void CCodeEdit::SetTABs(int iTABs)
{
	m_iTABs = max(1, min(8, iTABs));
}

int CCodeEdit::GetTABs()
{
	return m_iTABs;
}

void CCodeEdit::EnsureVisible()
{
	UpdateCaret();
}

void CCodeEdit::SetSelection(int iStartLine, int iStartChar, int iEndLine, int iEndChar)
{
	// 设置选区
	m_iLineStartSelection = max(0, iStartLine);
	m_iCharStartSelection = max(0, iStartChar);
	m_iLineEndSelection = min(m_iNumberLines-1, iEndLine);
	if (m_iLineEndSelection == -1)
		m_iLineEndSelection = m_iNumberLines - 1;
	m_iCharEndSelection = min(m_lpLineInfo[m_iLineEndSelection].nLen, iEndChar);
	if (m_iCharEndSelection == -1)
		m_iCharEndSelection = m_lpLineInfo[m_iLineEndSelection].nLen;
}

void CCodeEdit::GetSelection(int& iStartLine, int& iStartChar, int& iEndLine, int& iEndChar)
{
	// 获取选区
	iStartLine = m_iLineStartSelection;
	iStartChar = m_iCharStartSelection;
	iEndLine = m_iLineEndSelection;
	iEndChar = m_iCharEndSelection;
}

int CCodeEdit::GetNumberLines()
{
	return m_iNumberLines;
}

int CCodeEdit::GetLineLen(int LineIndex)
{
	if (LineIndex >= m_iNumberLines || LineIndex < 0)
		return -1;
	else
		return m_lpLineInfo[LineIndex].nLen;
}

void CCodeEdit::SetBackgroundColor(COLORREF cBgColor)
{
	m_crBgColor = cBgColor;
}

COLORREF CCodeEdit::GetBackgroundColor()
{
	return m_crBgColor;
}

void CCodeEdit::SetLineNumberBackgroundColor(COLORREF cBgColor)
{
	m_crLineNumBgColor = cBgColor;
}

COLORREF CCodeEdit::GetLineNumberBackgroundColor()
{
	return m_crLineNumBgColor;
}

void CCodeEdit::SetLineNumberForegroundColor(COLORREF cBgColor)
{
	m_crLineNumFgColor = cBgColor;
}

COLORREF CCodeEdit::GetLineNumberForegroundColor()
{
	return m_crLineNumFgColor;
}

void CCodeEdit::SetShowLineNumberStatus(BOOL isShowed)
{
	m_LineNumbers = isShowed;
}

void CCodeEdit::SetLineDrawProc(LPLINEDRAWPROC lpfnLineDrawProc)
{
	m_lpfnLineDrawProc = lpfnLineDrawProc;
}

void CCodeEdit::DefaultLineDrawProc(LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd)
{
	// 用默认方式绘制行
	DrawLine(lpszText, iLen, rect, iSelectionStart, iSelectionEnd);
}

BOOL CCodeEdit::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CCodeEdit::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// 获得客户端大小和位置
	RECT rcClient;
	GetClientRect(&rcClient);

	// 清除成员
	if (m_MemDC.m_hDC != NULL)
	{
		m_MemDC.SelectObject(m_pOldMemBitmap);
		m_MemDC.SelectObject(m_pOldFont);
		m_MemDC.DeleteDC();
		m_MemBitmap.DeleteObject();
	}

	// 创建内存 DC 和位图
	CDC* pDC = GetDC();
	m_MemDC.CreateCompatibleDC(pDC);
	m_MemBitmap.CreateCompatibleBitmap(pDC, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
	m_pOldMemBitmap = m_MemDC.SelectObject(&m_MemBitmap);
	m_pOldFont = m_MemDC.SelectObject(&m_Font);
	ReleaseDC(pDC);

	// 创建光标
	DestroyCaret();
	CSize sz = m_MemDC.GetTextExtent(_T("A"), 1);
	::CreateCaret(m_hWnd, NULL, 1, sz.cy);
	CPoint pt = GetCaretPos();
	SetCaretPos(pt);
	if (((CCodeMateDlg*)GetParent())->m_bFocused == true)
	{
		ShowCaret();
	}

	// 更新控件
	UpdateControl();

	// 更新屏幕
	Invalidate(FALSE);
	UpdateWindow();
}

void CCodeEdit::OnPaint()
{
	CPaintDC dc(this);

	// 清除背景
	RECT rcClient;
	GetClientRect(&rcClient);
	m_MemDC.FillSolidRect(&rcClient, m_crBgColor);

	// 绘制内容
	DrawContent();

	// 绘制到屏幕 DC
	dc.BitBlt(0, 0, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, &m_MemDC, 0, 0, SRCCOPY);
}

void CCodeEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// 销毁列表框
	DestroyList();

	// 获得滚动条信息
	SCROLLINFO si;
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &si);

	// 更新水平滚动
	switch (nSBCode)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			// 更新滚动位置
			si.nPos = (si.nTrackPos / m_szTextSize.cy) * m_szTextSize.cy;
		}
		break;

	case SB_LINEDOWN:
		{
			// 更新滚动位置
			si.nPos += m_szTextSize.cy;
			if (si.nPos > (int)(si.nMax-si.nPage))
				si.nPos -= m_szTextSize.cy;
		}
		break;

	case SB_PAGEDOWN:
		{
			// 获得客户区的大小和位置
			RECT rcClient;
			GetClientRect(&rcClient);

			// 获得页面高度
			int iHeight = rcClient.bottom - rcClient.top;

			// 更新光标位置和滚动位置
			m_iCurrentLine += int(iHeight / m_szTextSize.cy);
			if (m_iCurrentLine > m_iNumberLines - 1)
				m_iCurrentLine = m_iNumberLines - 1;
			if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
				m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
			si.nPos += iHeight;
			if (si.nPos > (int)(si.nMax-si.nPage))
				si.nPos = (int)(si.nMax-si.nPage);
		}
		break;

	case SB_LINEUP:
		{
			// 更新滚动位置
			si.nPos -= m_szTextSize.cy;
			if (si.nPos < si.nMin)
				si.nPos = si.nMin;
		}
		break;

	case SB_PAGEUP:
		{
			// 获得客户区的大小和位置
			RECT rcClient;
			GetClientRect(&rcClient);

			// 获得页面高度
			int iHeight = rcClient.bottom - rcClient.top;

			// 更新光标位置和滚动位置
			m_iCurrentLine -= int(iHeight / m_szTextSize.cy);
			if (m_iCurrentLine < 0)
				m_iCurrentLine = 0;
			if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
				m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
			si.nPos -= iHeight;
			if (si.nPos < si.nMin)
				si.nPos = si.nMin;
		}
		break;
	}

	// 设置滚动条信息
	SetScrollInfo(SB_VERT, &si, TRUE);

	// 获得滚动条信息
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// 获得字符位置
	POINT pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
	pt.x -= iHorizontalOffset;
	pt.y -= iVerticalOffset;
	SetCaretPos(pt);

	// 更新屏幕
	Invalidate(FALSE);
	UpdateWindow();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CCodeEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// 销毁列表框
	DestroyList();

	// 获取滚动条信息
	SCROLLINFO si;
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &si);

	// 更新垂直滚动
	switch (nSBCode)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			// 更新滚动位置
			si.nPos = (si.nTrackPos / m_szTextSize.cx) * m_szTextSize.cx;
		}
		break;

	case SB_LINERIGHT:
	case SB_PAGERIGHT:
		{
			// 更新滚动位置
			si.nPos += m_szTextSize.cx;
			if (si.nPos > (int)(si.nMax-si.nPage))
				si.nPos -= m_szTextSize.cx;
		}
		break;

	case SB_LINELEFT:
	case SB_PAGELEFT:
		{
			// 更新滚动位置
			si.nPos -= m_szTextSize.cx;
			if (si.nPos < si.nMin)
				si.nPos = si.nMin;
		}
		break;
	}

	// 设置滚动条信息
	SetScrollInfo(SB_HORZ, &si, TRUE);

	// 获得滚动条信息
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// 获得字符位置
	POINT pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
	pt.x -= iHorizontalOffset;
	pt.y -= iVerticalOffset;
	SetCaretPos(pt);

	// 更新屏幕
	Invalidate(FALSE);
	UpdateWindow();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CCodeEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 检查列表框是否可见
	if (m_wndListBox.m_hWnd != NULL)
	{
		// 发送消息到列表框
		if (zDelta<0)
			m_wndListBox.SendMessage(WM_VSCROLL,SB_LINEDOWN);
		else
			m_wndListBox.SendMessage(WM_VSCROLL,SB_LINEUP);
		return FALSE;
	}

	// 设置鼠标滚轮信息
	if (zDelta < 0)
	{
		// 向下滚动
		OnVScroll(SB_LINEDOWN, 0, 0);
	}
	else
	{
		// 向上滚动
		OnVScroll(SB_LINEUP, 0, 0);
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CCodeEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 销毁列表框
		DestroyList();

		// 捕捉鼠标
		SetCapture();

		// 获得滚动信息
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// 获得鼠标位置
		POINT pt = {point.x+iHorizontalOffset, point.y+iVerticalOffset};

		// 检查是否显示行号
		if (m_LineNumbers)
		{
			// 如果鼠标位于行号显示范围
			if (int(point.x) < 5 * m_szTextSize.cx - iHorizontalOffset)
			{
				// 设置选区范围
				m_iLineStartSelection = LineFromPosition(pt);
				m_iLineEndSelection = (m_iLineStartSelection ==  m_iNumberLines - 1) ? m_iNumberLines - 1 : m_iLineStartSelection + 1;
				m_iCharStartSelection = 0;
				m_iCharEndSelection = (m_iLineStartSelection ==  m_iNumberLines - 1) ? m_lpLineInfo[m_iNumberLines - 1].nLen : 0;

				// 设置光标位置
				m_iCurrentLine = m_iLineEndSelection;
				m_iCurrentChar = m_iCharEndSelection;

				POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
				ptCaret.x -= iHorizontalOffset;
				ptCaret.y -= iVerticalOffset;
				SetCaretPos(ptCaret);

				// 编辑器获得焦点
				SetFocus();

				// 更新屏幕
				Invalidate(FALSE);
				UpdateWindow();

				// 退出
				return;
			}
		}

		// 检查是否正在选择
		if (!m_bSelecting)
		{
			// 设置选区范围
			m_iLineStartSelection = LineFromPosition(pt);
			m_iLineEndSelection = m_iLineStartSelection;
			m_iCharStartSelection = CharFromPosition(m_iLineStartSelection, pt);
			m_iCharEndSelection = m_iCharStartSelection;

			// 设置光标位置
			m_iCurrentLine = m_iLineStartSelection;
			m_iCurrentChar = m_iCharStartSelection;
			POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
			ptCaret.x -= iHorizontalOffset;
			ptCaret.y -= iVerticalOffset;
			SetCaretPos(ptCaret);
		}
		else
		{
			// 设置选区结束位置
			m_iLineEndSelection = LineFromPosition(pt);
			m_iCharEndSelection = CharFromPosition(m_iLineEndSelection, pt);

			// 设置光标位置
			m_iCurrentLine = m_iLineEndSelection;
			m_iCurrentChar = m_iCharEndSelection;
			POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
			ptCaret.x -= iHorizontalOffset;
			ptCaret.y -= iVerticalOffset;
			SetCaretPos(ptCaret);
		}

		// 编辑器获得焦点
		SetFocus();

		// 更新屏幕
		Invalidate(FALSE);
		UpdateWindow();
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CCodeEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	// 检查是否显示行号
	if (m_LineNumbers)
	{
		// 如果鼠标位于行号显示范围
		int iHorizontalOffset = GetScrollPos(SB_HORZ);
		if (int(point.x) < 5 * m_szTextSize.cx - iHorizontalOffset) return;
	}

	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 销毁列表框
		DestroyList();

		// 捕捉鼠标
		SetCapture();

		// 获得滚动条信息
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// 获得鼠标位置
		POINT pt = {point.x+iHorizontalOffset, point.y+iVerticalOffset};

		if (m_iLineStartSelection==m_iLineEndSelection && m_iCharStartSelection==m_iCharEndSelection)
		{
			// 设置选取开始位置
			m_iLineStartSelection = LineFromPosition(pt);
			m_iLineEndSelection = m_iLineStartSelection;
			m_iCharStartSelection = CharFromPosition(m_iLineStartSelection, pt);
			m_iCharEndSelection = m_iCharStartSelection;

			// 设置光标位置
			m_iCurrentLine = m_iLineStartSelection;
			m_iCurrentChar = m_iCharStartSelection;
			POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
			ptCaret.x -= iHorizontalOffset;
			ptCaret.y -= iVerticalOffset;
			SetCaretPos(ptCaret);
		}

		// 编辑器获得焦点
		SetFocus();

		// 更新屏幕
		Invalidate(FALSE);
		UpdateWindow();
	}

	// 加载右键菜单
	CMenu menu,*pSubMenu;
	menu.LoadMenu(IDR_MENU_RBUTTON);
	pSubMenu = menu.GetSubMenu(1);

	// 设置菜单图标
	if (((CCodeMateDlg*)GetParent())->m_bShowMenuImage == true)
	{
		pSubMenu ->SetMenuItemBitmaps (ID_EDIT_UNDO, MF_BYCOMMAND, &(((CCodeMateDlg*)GetParent())->m_bmpMenu[3]), &(((CCodeMateDlg*)GetParent())->m_bmpMenu[3]));
		pSubMenu ->SetMenuItemBitmaps (ID_EDIT_CUT, MF_BYCOMMAND, &(((CCodeMateDlg*)GetParent())->m_bmpMenu[4]), &(((CCodeMateDlg*)GetParent())->m_bmpMenu[4]));
		pSubMenu ->SetMenuItemBitmaps (ID_EDIT_COPY, MF_BYCOMMAND, &(((CCodeMateDlg*)GetParent())->m_bmpMenu[5]), &(((CCodeMateDlg*)GetParent())->m_bmpMenu[5]));
		pSubMenu ->SetMenuItemBitmaps (ID_EDIT_PASTE, MF_BYCOMMAND, &(((CCodeMateDlg*)GetParent())->m_bmpMenu[6]), &(((CCodeMateDlg*)GetParent())->m_bmpMenu[6]));
	}

	// 设置菜单语言
	if (((CCodeMateDlg*)GetParent())->m_sMyConfig.m_uiLanguage != 0)
	{
		pSubMenu->ModifyMenu(ID_EDIT_UNDO, MF_BYCOMMAND, ID_EDIT_UNDO, ((CCodeMateDlg*)GetParent())->m_arrLanString[21]);
		pSubMenu->ModifyMenu(ID_EDIT_CUT, MF_BYCOMMAND, ID_EDIT_CUT, ((CCodeMateDlg*)GetParent())->m_arrLanString[22]);
		pSubMenu->ModifyMenu(ID_EDIT_COPY, MF_BYCOMMAND, ID_EDIT_COPY, ((CCodeMateDlg*)GetParent())->m_arrLanString[23]);
		pSubMenu->ModifyMenu(ID_EDIT_PASTE, MF_BYCOMMAND, ID_EDIT_PASTE, ((CCodeMateDlg*)GetParent())->m_arrLanString[24]);
		pSubMenu->ModifyMenu(ID_EDIT_CLEAR, MF_BYCOMMAND, ID_EDIT_CLEAR, ((CCodeMateDlg*)GetParent())->m_arrLanString[25]);
		pSubMenu->ModifyMenu(ID_EDIT_SELALL, MF_BYCOMMAND, ID_EDIT_SELALL, ((CCodeMateDlg*)GetParent())->m_arrLanString[26]);
		pSubMenu->ModifyMenu(ID_A_LINE, MF_BYCOMMAND, ID_A_LINE, ((CCodeMateDlg*)GetParent())->m_arrLanString[72]);
		pSubMenu->ModifyMenu(ID_DEL_A_LINE, MF_BYCOMMAND, ID_DEL_A_LINE, ((CCodeMateDlg*)GetParent())->m_arrLanString[74]);
		pSubMenu->ModifyMenu(ID_EDIT_SHOWLIST, MF_BYCOMMAND, ID_EDIT_SHOWLIST, ((CCodeMateDlg*)GetParent())->m_arrLanString[27]);
	}

	// 显示菜单
	CPoint oPoint;
	GetCursorPos(&oPoint);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,oPoint.x,oPoint.y,((CCodeMateDlg*)GetParent()));
	pSubMenu=NULL;
	delete pSubMenu;

	CWnd::OnRButtonDown(nFlags, point);
}

void CCodeEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 获得滚动条信息
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// 检查是否显示行号
		if (m_LineNumbers)
		{
			// 如果鼠标位于行号显示范围
			if (int(point.x) < 5 * m_szTextSize.cx - iHorizontalOffset)
			{
				HCURSOR hCursor = (HCURSOR)AfxGetApp()->LoadCursor(IDC_POINTER);
				SetCursor(hCursor);
			}
		}

		// 检查鼠标拖动
		if ((nFlags & MK_LBUTTON) && (m_iLineStartSelection != -1) && (m_iCharStartSelection != -1))
		{
			// 获得鼠标位置
			POINT pt = {max(0, point.x+iHorizontalOffset), max(0, point.y+iVerticalOffset)};

			// 设置选区结束位置
			m_iLineEndSelection = LineFromPosition(pt);
			m_iCharEndSelection = CharFromPosition(m_iLineEndSelection, pt);

			// 设置光标位置
			m_iCurrentLine = m_iLineEndSelection;
			m_iCurrentChar = m_iCharEndSelection;

			POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
			ptCaret.x -= iHorizontalOffset;
			ptCaret.y -= iVerticalOffset;
			SetCaretPos(ptCaret);

			// 确保光标显示
			EnsureVisible(m_iCurrentLine, m_iCurrentChar);

			// 更新屏幕
			Invalidate(FALSE);
			UpdateWindow();
		}
	}

	CString sText;
	if (((CCodeMateDlg*)GetParent())->m_sMyConfig.m_uiLanguage == 0)
		sText.LoadString(AFX_IDS_IDLEMESSAGE);
	else
		sText = ((CCodeMateDlg*)GetParent())->m_arrLanString[53];
	((CCodeMateDlg*)GetParent())->m_wndStatusBar.SetPaneText(0, sText);

	CWnd::OnMouseMove(nFlags, point);
}

void CCodeEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 释放鼠标
		ReleaseCapture();

		// 检查无效选区
		if ((m_iLineStartSelection == m_iLineEndSelection) && (m_iCharStartSelection == m_iCharEndSelection))
		{
			// 清除选区
			m_iLineStartSelection = -1;
			m_iLineEndSelection = -1;
			m_iCharStartSelection = -1;
			m_iCharEndSelection = -1;
		}

		// 更新屏幕
		Invalidate(FALSE);
		UpdateWindow();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CCodeEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 获得滚动条信息
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// 获得鼠标位置
		POINT pt = {point.x+iHorizontalOffset, point.y+iVerticalOffset};

		// 选择单词
		SelectWord(pt, FALSE);

		// 更新屏幕
		Invalidate(FALSE);
		UpdateWindow();
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CCodeEdit::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	DestroyCaret();
	CSize sz = m_MemDC.GetTextExtent(_T("A"), 1);
	::CreateCaret(m_hWnd, NULL, 1, sz.cy);

	// 创建光标
	CPoint pt = GetCaretPos();
	SetCaretPos(pt);

	// 更新光标
	UpdateCaret();

	// 显示光标
	ShowCaret();
	((CCodeMateDlg*)GetParent())->m_bFocused=true;
}

void CCodeEdit::OnKillFocus(CWnd* pOldWnd)
{
	CWnd::OnKillFocus(pOldWnd);
	HideCaret();
	((CCodeMateDlg*)GetParent())->m_bFocused=false;
}

void CCodeEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 处理键盘输入
		BOOL bUpdate = FALSE;
		switch (nChar)
		{
		case VK_LEFT:	// 左方向键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);

					// 更新模式
					bUpdate = TRUE;

					return;
				}
				else
				{
					// 将光标向左移动
					if (m_iCurrentChar > 0)
						m_iCurrentChar--;
					else if (m_iCurrentLine > 0)
					{
						m_iCurrentLine--;
						m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
					}

					// 检查是否正在选择
					if (m_bSelecting)
					{
						// 设置选区结束位置
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_RIGHT:	// 右方向键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);

					// 更新模式
					bUpdate = TRUE;

					return;
				}
				else
				{
					// 将光标向右移动
					if (m_iCurrentChar < m_lpLineInfo[m_iCurrentLine].nLen)
						m_iCurrentChar++;
					else if (m_iCurrentLine < m_iNumberLines-1)
					{
						m_iCurrentLine++;
						m_iCurrentChar = 0;
					}

					// 检查是否正在选择
					if (m_bSelecting)
					{
						// 设置选区结束位置
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_UP:	// 上方向键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);

					// 更新模式
					bUpdate = TRUE;

					return;
				}
				else
				{
					// 将光标向上移动
					if (m_iCurrentLine > 0)
					{
						int cnum = GetColumnIndex(m_iCurrentLine, m_iCurrentChar);
						m_iCurrentChar = GetCharIndex(m_iCurrentLine - 1, cnum);
						m_iCurrentLine--;
						if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
							m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
					}
					else
						m_iCurrentChar = 0;

					// 检查是否正在选择
					if (m_bSelecting)
					{
						// 设置选区结束位置
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_DOWN:	// 下方向键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);

					// 更新模式
					bUpdate = TRUE;

					return;
				}
				else
				{
					// 将光标向下移动
					if (m_iCurrentLine < m_iNumberLines-1)
					{
						int cnum = GetColumnIndex(m_iCurrentLine, m_iCurrentChar);
						m_iCurrentChar = GetCharIndex(m_iCurrentLine + 1, cnum);
						m_iCurrentLine++;
						if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
							m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
					}
					else
						m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;

					// 检查是否正在选择
					if (m_bSelecting)
					{
						// 设置选区结束位置
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_HOME:	// Home 键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					if (m_bCopying == TRUE)
					{
						// 将光标移动到第一行
						m_iCurrentLine = 0;
					}

					// 将光标移动到最左边
					m_iCurrentChar = 0;

					// 检查无效选区
					if (m_bSelecting)
					{
						// 设置选区结束位置
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_END:	// End 键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					if (m_bCopying == TRUE)
					{
						// 将光标移动到最后一行
						m_iCurrentLine = m_iNumberLines - 1;
					}

					// 将光标移动到最右边
					m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;

					// 检查无效选区
					if (m_bSelecting)
					{
						// 设置选区结束位置
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_SHIFT:	// Shift 键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// 检查无效选区
					if (!m_bSelecting)
					{
						// 选择模式
						m_bSelecting = TRUE;

						// 设置选区开始与结束位置
						m_iLineStartSelection = m_iCurrentLine;
						m_iCharStartSelection = m_iCurrentChar;
						m_iLineEndSelection = m_iLineStartSelection;
						m_iCharEndSelection = m_iCharStartSelection;
					}
				}
			}
			break;

		case VK_CONTROL:	// Ctrl 键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// 正在选择模式
					m_bCopying = TRUE;
				}
			}
			break;

		case VK_DELETE:	// Delete 键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// 如果按住 Shift
					if (m_bSelecting)
					{
						m_bSelecting = FALSE;
						return;
					}

					// 检查无效选区
					if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
					{
						// 删除选中文本
						DeleteText();
					}
					else
					{
						// 删除下一个字符
						DelChar(m_iCurrentLine, m_iCurrentChar);
					}

					// 更新控件
					UpdateControl();

					// 更新模式
					bUpdate = TRUE;
				}
			}
			break;

		case VK_BACK:	// Back Space 键
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// 更新模式
					bUpdate = TRUE;
				}
			}
			break;

		case VK_PRIOR:	// Page Up 键
			{
				OnVScroll(SB_PAGEUP, 0, 0);
			}
			break;

		case VK_NEXT:	// Page Down 键
			{
				OnVScroll(SB_PAGEDOWN, 0, 0);
			}
			break;

		case VK_LWIN:
		case VK_RWIN:
		case VK_APPS:
		case VK_NUMLOCK:
		case VK_SCROLL:
		case VK_INSERT:
		case VK_PAUSE:
		case VK_CAPITAL:
		case VK_ESCAPE:
		case VK_F1:
			break;

		default:
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// 检查正在复制标志
					if (!m_bCopying)
					{
						// 检查无效选区
						if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
						{
							// 删除选中文本
							DeleteText();
						}
					}
				}
			}
			break;
		}

		// 检查无效选区
		if ((!m_bSelecting) && (!bUpdate) && (!m_bCopying))
		{
			// 清除选区
			m_iLineStartSelection = -1;
			m_iLineEndSelection = -1;
			m_iCharStartSelection = -1;
			m_iCharEndSelection = -1;
		}

		// 更新光标
		UpdateCaret();
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCodeEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 检查无效数据
	if (m_lpszText != NULL)
	{
		// 处理键盘输入
		switch (nChar)
		{
		case VK_SHIFT:
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
				}
				else
				{
					// 选择模式
					m_bSelecting = FALSE;
				}
			}
			break;

		case VK_CONTROL:
			{
				// 检查列表框是否可见
				if (m_wndListBox.m_hWnd != NULL)
				{
					// 发送消息到列表框
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
				}
				else
				{
					// 正在选择模式
					m_bCopying = FALSE;
				}
			}
			break;
		}
	}

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCodeEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 检查键盘输入
	switch (nChar)
	{
	case VK_RETURN:		// 回车 && Ctrl+M
		{
			// 检查列表框是否可见
			if (m_wndListBox.m_hWnd != NULL)
			{
				OnLbnDblclkListAutoComplete();
				return;
			}
			else
			{
				// 添加新行
				AddLine(m_iCurrentLine, m_iCurrentChar);

				// 光标位置
				m_iCurrentChar = 0;
				m_iCurrentLine++;
			}
		}
		break;

	case VK_BACK:		// 退格
		{
			// 销毁列表框
			DestroyList();

			// 检查无效选区
			if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
			{
				// 删除选中文本
				DeleteText();
			}
			else
			{
				// 删除前面字符
				int back = BackChar(m_iCurrentLine, m_iCurrentChar);
				if (back == 1)
				{
					m_iCurrentLine = max(0, m_iCurrentLine-1);
					m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
				}
				else if (back == 2)
				{
					m_iCurrentChar = max(0, m_iCurrentChar-1);
				}
			}
		}
		break;

	case VK_ESCAPE:		// ESC
		{
			// 清除标志
			m_bSelecting = FALSE;
			m_bCopying = FALSE;

			// 销毁列表框
			DestroyList();
		}
		break;

	case 0x1A:		// Ctrl+Z
		{
			// 如果可以撤消，调用撤消函数
			if (m_bCanUndo == TRUE)
			{
				// 销毁列表框
				DestroyList();

				// 撤消操作
				Undo();
			}
		}
		break;

	case 0x03:		// Ctrl+C
		{
			// 检查无效选区
			if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
			{
				// 获得选区范围
				int iStartChar, iEndChar;
				int iStartLine, iEndLine;
				if (m_iLineStartSelection < m_iLineEndSelection)
				{
					iStartChar = m_iCharStartSelection;
					iEndChar = m_iCharEndSelection;
					iStartLine = m_iLineStartSelection;
					iEndLine = m_iLineEndSelection;
				}
				else if (m_iLineStartSelection > m_iLineEndSelection)
				{
					iStartChar = m_iCharEndSelection;
					iEndChar = m_iCharStartSelection;
					iStartLine = m_iLineEndSelection;
					iEndLine = m_iLineStartSelection;
				}
				else
				{
					iStartChar = min(m_iCharStartSelection, m_iCharEndSelection);
					iEndChar = max(m_iCharStartSelection, m_iCharEndSelection);
					iStartLine = m_iLineStartSelection;
					iEndLine = m_iLineEndSelection;
				}
				int iStartOffset = GetCharOffset(iStartLine, iStartChar);
				if (m_lpszText[iStartOffset] == '\r')
					iStartOffset += 2;
				int iEndOffset = GetCharOffset(iEndLine, iEndChar);
				int iCount = iEndOffset - iStartOffset;

				// 复制选中文本到剪贴板
				if (::OpenClipboard(m_hWnd))
				{
					::EmptyClipboard();
					HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, (iCount+1)*sizeof(_TCHAR));
					LPTSTR lpszText = (LPTSTR)::GlobalLock(hMem);
					memcpy(lpszText, m_lpszText+iStartOffset, iCount*sizeof(_TCHAR));
					lpszText[iCount] = '\0';
					::SetClipboardData(CF_UNICODETEXT, hMem);
					::GlobalUnlock(hMem);
					::CloseClipboard();
				}
			}
		}
		break;

	case 0x16:		// Ctrl+V
		{
			// 销毁列表框
			DestroyList();

			// 检查无效选区
			if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
			{
				// 删除选中文本
				DeleteText();
			}

			// 从剪贴板粘贴文本
			if (::OpenClipboard(m_hWnd))
			{
				int iLine = m_iCurrentLine;
				int iChar = 0;
				HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT);
				LPTSTR lpszText = (LPTSTR)::GlobalLock(hMem);
				if (lpszText != NULL)
				{
					// 添加 UNICODE 文本
					int iLen = (int)_tcslen(lpszText);
					AddText(lpszText, m_iCurrentLine, m_iCurrentChar, iLen);
					for (int i = 0; i < iLen; i++)
					{
						if (lpszText[i] == '\n')
						{
							iLine++;
							iChar = 0;
						}
						else
						{
							if (lpszText[i] != '\r')
								iChar++;
						}
					}
				}
				else
				{
					// 添加 ANSI 文本
					hMem = ::GetClipboardData(CF_TEXT);
					LPSTR lpszText = (LPSTR)::GlobalLock(hMem);
					if (lpszText != NULL)
					{
						int iLen = (int)strlen(lpszText);
						int iSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpszText, iLen, NULL, 0);
						LPTSTR lpszNewText = (LPTSTR)malloc(iSize*sizeof(_TCHAR));
						MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpszText, iLen, lpszNewText, iSize);
						AddText(lpszNewText, m_iCurrentLine, m_iCurrentChar, iLen);
						for (int i = 0; i < iLen; i++)
						{
							if (lpszText[i] == '\n')
							{
								iLine++;
								iChar = 0;
							}
							else
							{
								if (lpszText[i] != '\r')
									iChar++;
							}
						}
						free(lpszNewText);
					}
				}
				::GlobalUnlock(hMem);
				::CloseClipboard();

				// 设置光标位置
				if (m_iCurrentLine == iLine)
					m_iCurrentChar += iChar;
				else
					m_iCurrentChar = iChar;
				m_iCurrentLine = iLine;

				// 保存光标信息
				m_UndoInfo.iEndLine = m_iCurrentLine;
				m_UndoInfo.iEndChar = m_iCurrentChar;
			}
		}
		break;

	case 0x18:		// Ctrl+X
		{
			// 销毁列表框
			DestroyList();

			if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
			{
				// 获得选区范围
				int iStartChar, iEndChar;
				int iStartLine, iEndLine;
				if (m_iLineStartSelection < m_iLineEndSelection)
				{
					iStartChar = m_iCharStartSelection;
					iEndChar = m_iCharEndSelection;
					iStartLine = m_iLineStartSelection;
					iEndLine = m_iLineEndSelection;
				}
				else if (m_iLineStartSelection > m_iLineEndSelection)
				{
					iStartChar = m_iCharEndSelection;
					iEndChar = m_iCharStartSelection;
					iStartLine = m_iLineEndSelection;
					iEndLine = m_iLineStartSelection;
				}
				else
				{
					iStartChar = min(m_iCharStartSelection, m_iCharEndSelection);
					iEndChar = max(m_iCharStartSelection, m_iCharEndSelection);
					iStartLine = m_iLineStartSelection;
					iEndLine = m_iLineEndSelection;
				}
				int iStartOffset = GetCharOffset(iStartLine, iStartChar);
				if (m_lpszText[iStartOffset] == '\r')
					iStartOffset += 2;
				int iEndOffset = GetCharOffset(iEndLine, iEndChar);
				int iCount = iEndOffset - iStartOffset;

				// 复制选区内容到剪贴板
				if (::OpenClipboard(m_hWnd))
				{
					::EmptyClipboard();
					HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, (iCount+1)*sizeof(_TCHAR));
					LPTSTR lpszText = (LPTSTR)::GlobalLock(hMem);
					memcpy(lpszText, m_lpszText+iStartOffset, iCount*sizeof(_TCHAR));
					lpszText[iCount] = '\0';
					::SetClipboardData(CF_UNICODETEXT, hMem);
					::GlobalUnlock(hMem);
					::CloseClipboard();
				}

				// 删除选区文本
				DeleteText();
			}
		}
		break;

	case 0x01:		// Ctrl+A
		{
			// 销毁列表框
			DestroyList();

			// 选择全部文本
			m_iCharStartSelection = 0;
			m_iLineStartSelection = 0;
			m_iCharEndSelection = m_lpLineInfo[m_iNumberLines-1].nLen;
			m_iLineEndSelection = m_iNumberLines - 1;

			// 设置光标位置
			m_iCurrentLine = m_iLineEndSelection;
			m_iCurrentChar = m_iCharEndSelection;
		}
		break;

	case 0x0a:	// Ctrl+J
		{
			// 清除标志
			m_bSelecting = FALSE;

			// 显示自动完成列表框
			ShowAutoComplete();

			// 退出
			return;
		}
		break;
	case 0x0c:		//Ctrl+L 删除一行
		{
			if (m_lpszText != NULL)
			{
				// 清除选区
				m_iLineStartSelection = -1;
				m_iLineEndSelection = -1;
				m_iCharStartSelection = -1;
				m_iCharEndSelection = -1;

				// 销毁列表框
				DestroyList();

				// 判断要删除的范围
				if (m_iCurrentLine == m_iNumberLines - 1)	// 如果是最后一行
				{
					// 非第一行需要删除换行
					if (m_lpLineInfo[m_iCurrentLine].nLen == 0)	// 是空行
					{
						BackChar(m_iCurrentLine, 0);
					}
					else
					{
						// 删除文本
						if (m_iNumberLines != 1)
							DelText(m_iCurrentLine - 1, m_lpLineInfo[m_iCurrentLine - 1].nLen, m_iCurrentLine, m_lpLineInfo[m_iCurrentLine].nLen);
						else
							DelText(m_iCurrentLine, 0, m_iCurrentLine, m_lpLineInfo[m_iCurrentLine].nLen);
					}
					m_iCurrentLine = max(0, m_iCurrentLine-1);
				}
				else
				{
					if (m_lpLineInfo[m_iCurrentLine].nLen == 0)	// 是空行
					{
						BackChar(m_iCurrentLine, 0);
					}
					else
					{
						// 删除文本
						DelText(m_iCurrentLine, 0, m_iCurrentLine + 1, 0);
					}
				}
				m_iCurrentChar = 0;
			}
		}
		break;

	default:
		{
			// 检查正在复制标志
			if (!m_bCopying)
			{
				// 添加新字符
				AddChar(m_iCurrentLine, m_iCurrentChar, nChar);
				m_iCurrentChar++;
			}
			else
			{
				// 清除标志
				m_bSelecting = FALSE;
				m_bCopying = FALSE;

				// 销毁列表框
				DestroyList();
			}
		}
	}

	// 更新控件
	UpdateControl();

	// 更新光标
	UpdateCaret();

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CCodeEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// 更新鼠标指针
	if ((nHitTest == HTCLIENT) && (m_wndListBox.m_hWnd == NULL))
	{
		SetCursor(LoadCursor(NULL, IDC_IBEAM));
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

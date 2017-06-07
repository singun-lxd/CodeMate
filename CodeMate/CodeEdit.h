
// CodeEdit.h : 头文件
// 基于 CUltraEditWnd 改进
// 请查看 http://www.codeproject.com/KB/edit/CUltraPadWnd.aspx
// 开源协议 CPOL http://www.codeproject.com/info/cpol10.aspx
// 
#pragma once


typedef enum _SYNTAX_COLORING_TYPE	// 语言类型
{
	SCT_ERROR = -1,		// 没有打开文档
	SCT_NONE = 0,		// 普通文本
	SCT_C_PLUS_PLUS,	// C++ 语言
	SCT_C_ANSI,			// ANSI 标准 C 语言
	SCT_C_SHARP,		// C# 语言
	SCT_HTML,			// HTML 语言
	SCT_JSCRIPT,		// Java Script 脚本
	SCT_VBSCRIPT,		// VB Script 脚本
	SCT_SQL				// SQL 语言
} SYNTAX_COLORING_TYPE;

typedef enum _UNDO_OPERATION_TYPE	// 撤消操作类型
{
	UOT_NONE = 0,		// 无效操作
	UOT_BACK,			// 退格
	UOT_CHAR,			// 输入一个字符
	UOT_ADDLINE,		// 添加一行
	UOT_DELETE,			// 删除字符
	UOT_ADDTEXT,		// 添加字符串
	UOT_DELTEXT,		// 删除字符串
} UNDO_OPERATION_TYPE;

typedef enum _FILE_ENCODING_TYPE	// 编码类型
{
	FET_ANSI = 0,		// ANSI 编码
	FET_UNICODE,		// Unicode 编码
	FET_UNICODE_BE,		// Unicode big endian 编码
	FET_UTF_8			// UTF-8 编码
} FILE_ENCODING_TYPE;

typedef struct _LINEINFO	// 编辑器行信息
{
	DWORD dwOffset;		// 偏移量
	int nLen;			// 长度
} LINEINFO, *LPLINEINFO;

typedef struct _TAGINFO		// 关键词
{
	LPTSTR lpszTagName;		// 名称
	int nLen;				// 长度
} TAGINFO, *LPTAGINFO;

typedef struct _UNDOINFO	// 撤消结构信息
{
	LPTSTR lpszUndoData;				// 撤消数据
	int iLastLineLen;					// 行原始长度
	int nLen;							// 文本长度
	int iLine;							// 原光标所在行号
	int iChar;							// 原光标所在列号
	int iEndLine;						// 原选区末尾所在行号
	int iEndChar;						// 原选区末尾所在列号
	UNDO_OPERATION_TYPE nOperation;		// 上一次操作
} UNDOINFO;

typedef struct _COLORINFO	// 保存各种颜色信息
{
	COLORREF crLineNumBGColor;
	COLORREF crLineNumFGColor;
	COLORREF crKeywordColor;
	COLORREF crConstantColor;
	COLORREF crCommentColor;
	COLORREF crTextColor;
	COLORREF crForegroundColor;
	COLORREF crBackgroundColor;
} COLORINFO;

// 行绘画过程
typedef void (*LPLINEDRAWPROC)(HDC hDC, LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd);

#define IDC_LIST_AUTOCOMPLETE 0x10001	// 定义自动完成列表框的控件ID
#define LIST_HEIGHT	150		// 列表框高度
#define LIST_WIDTH 150		// 列表框宽度
#define LINE_NUMBER_TO_EDITOR 5			// 定义行号和编辑器的距离

// CCodeEdit
class CCodeEdit : public CWnd
{
	DECLARE_DYNAMIC(CCodeEdit)

// 构造
public:
	CCodeEdit();			// 构造函数
	virtual ~CCodeEdit();	// 析构函数
	BOOL Create(DWORD dwExStyle, DWORD dwStyle, RECT& rect, CWnd* pParentWnd, UINT nID);	// 创建控件

// 实现
public:
	// 公共方法
	BOOL Load(LPTSTR lpszFilePath);		// 加载文件
	BOOL Save(LPTSTR lpszFilePath);		// 保存文件
	BOOL GetChangedStatus();			// 获得文档是否改变的标志
	void SetChangedStatus(BOOL status);	// 设置文档是否改变的标志
	BOOL GetUndoStatus();				// 获得编辑器是否可以撤销的标志
	void Undo();						// 撤消操作
	BOOL Find(LPTSTR lpszText, int iLen, BOOL bMatchCase, BOOL bMatchWhole, BOOL bDirectionDown);				// 查找文本
	BOOL Replace(LPTSTR lpszText, int iLen, LPTSTR lpszText2, int iLen2, BOOL bMatchCase, BOOL bMatchWhole);	// 替换文本
	void ReplaceAll(LPTSTR lpszText, int iLen, LPTSTR lpszText2, int iLen2, BOOL bMatchCase, BOOL bMatchWhole);	// 全部替换
	void AddText(LPTSTR lpszText, int iLine, int iChar, int iLen);					// 添加文本到特定位置
	void DeleteText(int iStartLine, int iStartChar, int iEndLine, int iEndChar);	// 删除文本
	void ClearText();					// 清空编辑器
	void DestroyList();					// 销毁列表框
	LPTSTR GetTextBuffer();				// 获得文本缓冲区数据
	int GetBufferLen();					// 获得文本缓冲区长度
	void GetTextFromLine(int iLineIndex, LPTSTR lpszText);					// 获得第 iLineIndex 行的文本
	void SetEditorFont(LPTSTR lpszFacename, LONG iHeight, BOOL bBold, BOOL bItalic, BYTE uiCharSet);	// 设置字体
	void SetKeywordColor(COLORREF crKeywordColor);							// 设置关键词的颜色
	COLORREF GetKeywordColor();												// 获得关键词颜色
	void AddKeywords(LPTSTR lpszKeywords, int iLen);						// 添加关键词
	void SetConstantColor(COLORREF crConstantColor);						// 设置常量颜色
	COLORREF GetConstantColor();											// 获得常量颜色
	void AddConstants(LPTSTR lpszConstants, int iLen);						// 添加常量
	void SetCommentColor(COLORREF crCommentColor);							// 设置注释颜色
	COLORREF GetCommentColor();												// 获得注释颜色
	void SetTextColor(COLORREF crTextColor);								// 设置字符串颜色
	COLORREF GetTextColor();												// 获得字符串颜色
	void SetNormalCodeColor(COLORREF crTextColor);							// 设置普通代码颜色
	COLORREF GetNormalCodeColor();											// 获得普通代码颜色
	void SetColorStruct(COLORINFO coColor);									// 设置所有颜色信息
	void GetColorStruct(COLORINFO &coColor);								// 获取所有颜色信息
	void SetDefaultFileEncoding(FILE_ENCODING_TYPE fileEncoding);			// 设置默认保存编码
	void SetSyntaxColoring(SYNTAX_COLORING_TYPE syntaxColoring);			// 设置语法着色语言
	SYNTAX_COLORING_TYPE GetSyntaxColoring();								// 获得语法着色语言
	void UpdateSyntaxColoring();											// 更新语法着色显示
	void SetCurrentLine(int index);											// 设置当前行号（跳转）
	int GetCurrentLine();													// 获得当前行号
	void SetCurrentChar(int index);											// 设置当前字符序号
	int GetCurrentChar();													// 获得当前字符序号
	int GetCurrentColumn();													// 获得当前列号
	void SetTABs(int iTABs);												// 设置 Tab 长度
	int GetTABs();															// 获得 Tab 长度
	void EnsureVisible();																// 确保编辑器光标可见
	void SetSelection(int iStartLine, int iStartChar, int iEndLine, int iEndChar);		// 设置选区
	void GetSelection(int& iStartLine, int& iStartChar, int& iEndLine, int& iEndChar);	// 获得选区
	int GetNumberLines();							// 获得行数
	int GetLineLen(int LineIndex);					// 获得相应行最大字符数
	void SetBackgroundColor(COLORREF cBgColor);		// 设置背景色
	COLORREF GetBackgroundColor();					// 获得背景色
	void SetLineNumberBackgroundColor(COLORREF cBgColor);		// 设置行号背景色
	COLORREF GetLineNumberBackgroundColor();					// 获得行号背景色
	void SetLineNumberForegroundColor(COLORREF cBgColor);		// 设置行号背景色
	COLORREF GetLineNumberForegroundColor();					// 获得行号背景色
	void SetShowLineNumberStatus(BOOL isShowed);				// 设置是否显示行号
	void ShowAutoComplete();								// 显示自动完成列表
	void SetLineDrawProc(LPLINEDRAWPROC lpfnLineDrawProc);														// 设置自绘行显示函数
	void DefaultLineDrawProc(LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd);		// 默认绘制行显示函数

private:
	// 私有方法
	void UpdateControl();		// 更新控件
	void DeleteText();			// 删除选中文本
	void DelText(int iLine, int iChar, int iEndLine, int iEndChar);					// 删除特定位置文本
	void DelChar(int iLine, int iChar);					// 删除后面一个字符
	int BackChar(int iLine, int iChar);					// 删除前面一个字符
	void AddChar(int iLine, int iChar, UINT nChar);		// 添加一个字符
	void AddLine(int iLine, int iChar);					// 添加一行
	SIZE GetLineSize(LPTSTR lpszText, int iLen);	// 获得行尺寸
	void DrawContent();																				// 绘制内容
	void DrawLine(LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd);	// 绘制行
	void ProcessLine(LPTSTR lpszText, int iLen);	// 对单行文本进行语法着色检查
	int LineFromPosition(POINT pt);							// 从坐标位置获得行号
	int CharFromPosition(int iLineIndex, POINT pt);			// 从坐标位置获得字符序号
	POINT PositionFromChar(int iLineIndex, int iCharIndex);	// 从字符序号获得坐标位置
	int GetCharOffset(int iLineIndex, int iCharIndex);		// 获得字符偏移量
	int GetCharIndex(int iLine, int iColumn);				// 由列号获得字符序号
	int GetColumnIndex(int iLine, int iChar);				// 由字符序号获得列号
	void UpdateCaret();										// 更新光标显示
	void EnsureVisible(int iLineIndex, int iCharIndex);		// 确保显示光标
	LPTSTR GetLine(int& iOffset);							// 获得行内容
	void SelectWord(POINT pt, BOOL FirstLetter);			// 选择单词
	BOOL IsSpecial(_TCHAR tch);								// 是否为特殊字符
	BOOL IsLetter(_TCHAR tch);								// 是否是字母
	BOOL IsCJKChar(_TCHAR tch);								// 是否是汉字
	BOOL IsKeyword(LPTSTR lpszText, int iLen, int& iKeywordIndex, int& iKeywordOffset);		// 是否是关键词
	BOOL IsConstant(LPTSTR lpszText, int iLen, int& iConstantIndex, int& iConstantOffset);	// 是否是常量
	BOOL IsText(LPTSTR lpszText, int iLen, int& iTextStart, int& iTextEnd);					// 是否是字符串
	void SetKeywords(LPTSTR lpszKeywords, int iLen, BOOL bCaseSensitive, BOOL bUper);	// 设置关键词
	void SetConstants(LPTSTR lpszConstants, int iLen, BOOL bCaseSensitive, BOOL bUper);	// 设置常量

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);			// 擦除编辑器背景
	afx_msg void OnPaint();							// 编辑器绘制
	afx_msg void OnSize(UINT nType, int cx, int cy);// 编辑器调整大小
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	// 编辑器视图垂直滚动
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	// 编辑器视图水平滚动
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);			// 鼠标滚轮滚动
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);						// 鼠标左键按下
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);						// 鼠标移过~灰！
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);						// 鼠标右键按下
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);						// 鼠标左键弹起
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);					// 鼠标左键双击
	afx_msg void OnSetFocus(CWnd* pOldWnd);										// 编辑器获得焦点
	afx_msg void OnKillFocus(CWnd* pOldWnd);									// 编辑器失去焦点
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);				// 有按键按下
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);				// 有按键弹起
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);					// 有字符被输入
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);			// 设置鼠标指针
	afx_msg void OnLbnDblclkListAutoComplete();									// 自动完成列表控件双击事件

private:
	CDC m_MemDC;				// 绘制编辑器的图形对象
	CBitmap m_MemBitmap;		// 保存内存位图对象
	CBitmap* m_pOldMemBitmap;	// 保存旧的内存位图对象
	CFont m_Font;				// 字体
	CFont* m_pOldFont;			// 旧字体
	LPTSTR m_lpszText;			// 保存编辑器所有文本信息
	int m_iSize;				// 文本长度
	CSize m_szTextSize;			// 字符尺寸
	CSize m_szCJKTextSize;		// CJK字符尺寸
	int m_iLineStartSelection;	// 选区开始的行号
	int m_iLineEndSelection;	// 选区结束的行号
	int m_iCharStartSelection;	// 选区开始的字符序号
	int m_iCharEndSelection;	// 选区结束的字符序号
	int m_iNumberLines;			// 总行数
	UNDOINFO m_UndoInfo;		// 撤消信息
	LPLINEINFO m_lpLineInfo;	// 行信息
	COLORREF m_crKeywordColor;	// 关键词颜色
	LPTAGINFO m_lpKeywordInfo;	// 关键词信息
	int m_iNumberKeywords;		// 关键词数量
	BOOL m_bKeywordsCaseSensitive;	// 关键词是否大小写敏感
	BOOL m_bUper;					// 转换为大写还是小写
	COLORREF m_crConstantColor;		// 常量颜色
	LPTAGINFO m_lpConstantInfo;		// 常量信息
	int m_iNumberConstants;			// 常量数量
	BOOL m_bConstantsCaseSensitive;	// 常量是否大小写敏感
	COLORREF m_crCommentColor;		// 注释颜色
	COLORREF m_crTextColor;			// 字符串颜色
	FILE_ENCODING_TYPE m_FileEncoding;		// 默认保存文件编码
	SYNTAX_COLORING_TYPE m_SyntaxColoring;	// 当前语法着色语言
	COLORREF m_crCodeColor;		// 普通代码颜色
	int m_iCurrentLine;			// 当前行
	int m_iCurrentChar;			// 当前字符序号
	int m_iTABs;				// Tab 长度
	BOOL m_LineNumbers;			// 是否显示行号
	BOOL m_bComment;			// 是否注释
	BOOL m_bText;				// 是否文本
	BOOL m_bSelecting;			// 是否正在选择,Shift
	BOOL m_bCopying;			// 是否正在复制,Ctrl
	COLORREF m_crBgColor;		// 背景颜色
	BOOL m_changed;				// 文档是否已经更改
	COLORREF m_crLineNumBgColor;	// 行号背景颜色
	COLORREF m_crLineNumFgColor;	// 行号前景颜色
	BOOL m_bCanUndo;				// 编辑器是否可以撤消
	CListBox m_wndListBox;			// 自动完成列表框
	LPLINEDRAWPROC m_lpfnLineDrawProc;		// 行自绘函数
};


// CodeMateDlg.h : 头文件
// 

#include "CodeEdit.h"
#include "MyToolBar.h"
#include "Splitter.h"

#pragma once

const int WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);	// 将FINDMSGSTRING注册为WM_FINDREPLACE消息
#define IDC_EDITOR 0x10000	// 编辑器ID
#define IMG_COUNT 10		// 工具栏按钮数量
#define LIMITED_LEFT 200	// 限制区域距左边界长度
#define LIMITED_RIGHT 200	// 限制区域距右边界长度
#define MINDLG_HEIGHT 200	// 对话框最小高度
#define MINDLG_WIDTH 500	// 对话框最小宽度

// 保存配置信息的类定义
class CSetting
{
public:
	// 成员变量
	UINT m_uiLanguage;		// 界面语言
	CString m_strSkin;		// 界面皮肤
	BOOL m_bMaximize;		// 是否最大化
	CRect m_recWinPos;		// 窗口位置
	long m_lSplPos;			// 切分条位置
	BOOL m_bShowToolBar;	// 显示工具栏
	BOOL m_bShowStatusBar;	// 显示状态栏
	CString m_strWorkPath;	// 工作目录
	BOOL m_bShowLineNum;	// 显示行号
	UINT m_iTabLen;			// Tab 长度
	CString m_strFontName;	// 字体名称
	int m_iHeight;			// 字体高度
	int m_iSize;			// 字号
	BOOL m_bBold;			// 粗体
	BOOL m_bItalic;			// 斜体
	int m_iCharSet;			// 字符集
	UINT m_uiDefType;		// 默认文件类型
	UINT m_uiDefEncode;		// 默认文件编码
	COLORINFO m_coColor;	// 用户自定义颜色
	BOOL m_bMatchCase;		// 区分大小写
	BOOL m_bMatchWholeWord;	// 全字匹配
	BOOL m_bDirectionDown;	// 向下查找
	CString strFindString;	// 查找字符串
	CStringArray m_arrKeyWord;	// 关键词列表数组
	CStringArray m_arrConstant;	// 常量列表数组

public:
	// 成员函数
	void SaveSettings(LPCTSTR lpszFileName);	// 保存配置信息
	void LoadSettings(LPCTSTR lpszFileName);	// 加载配置信息
};

// CCodeMateDlg 对话框
class CCodeMateDlg : public CDialogEx
{
	// 构造
public:
	CCodeMateDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CCodeMateDlg();	// 析构函数

	// 对话框数据
	enum { IDD = IDD_CODEMATE_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON m_hIcon;		// 图标
	CMenu* m_pMainMenu;	// 菜单
	HTREEITEM m_hRoot;	// 分类根节点
	HTREEITEM m_hSelected;	// 已打开的文件节点
	int m_iWidth,m_iHeight;	// 显示编辑器时需要恢复正确的宽度和高度
	CString m_strAppTitle;	// 应用程序标题
	CString m_strAppPath;	// 应用程序目录
	CString m_strFileName;	// 当前的文件名称
	CString m_strTitleShow;	// 标题栏要显示的文件路径
	SYNTAX_COLORING_TYPE m_enFileType;		// 当前打开的文件类型，如果为-1则没有打开文件
	bool m_bTextSelected;	// 是否在编辑器中选中文本
	bool m_bItemSelected;	// 是否在树形控件中选择了实际的分类或项目
	bool m_bIsFolder;		// 当前选中项是否为分类（文件夹）
	bool m_bEscPressed;		// 重命名时是否按下ESC键取消操作
	unsigned short m_usOperaType;		// 标志树形控件编辑文本的操作是重命名还是新建分类，或者其他
	POINT m_ptLineStart,m_ptLineEnd;	// 切分条移动时绘图线条的的起点和终点
	long m_point_x;		// 保存切分条移动时原来的位置
	bool m_bMoving;		// 标志切分条是否正在移动
	CFindReplaceDialog *m_pFindDlg;	// 查找替换对话框指针

	HACCEL hAcc;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();	// 初始化对话框
	virtual BOOL ContinueModal();	// 用于发送状态消息给工具栏
	virtual BOOL PreTranslateMessage(MSG* pMsg);	// 窗体翻译消息之前的操作
	afx_msg HCURSOR OnQueryDragIcon();				// 当用户拖动最小化窗口时系统调用此函数取得光标显示。
	afx_msg void OnMove(int x, int y);					// 窗体位置改变调用此函数，用以保存窗口位置信息
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);	// 窗体大小正在变化调用此函数，用以限制窗口最小的大小
	afx_msg void OnSize(UINT uiType, int iX, int iY);	// 窗体大小变化调用此函数，用以保存窗口大小信息
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);		// 显示工具栏提示信息
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);	// 鼠标移动到菜单上，在状态栏显示提示信息
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex, BOOL bSysMenu);		// 初始化菜单状态
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);		// 窗体设置鼠标指针时调用此函数，用于显示状态栏提示
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);		// 按下窗体标题按钮或系统菜单调用此函数
	afx_msg void OnPaint();		// 窗体绘画
	afx_msg void OnSkinList(UINT nIdMenuCmd);
	LRESULT OnThemeChanged();	// 系统主题改变

	DECLARE_MESSAGE_MAP()

public:
	bool m_bFocused;			// 编辑器是否获得焦点
	bool m_bShowMenuImage;		// 是否显示菜单图标
	bool m_bShowSearch;			// 是否显	示搜索框
	bool m_bSkinLoad;			// 是否加载皮肤
	CSetting m_sMyConfig;		// 实例化配置
	CImageList m_wndImageList;		// 定义图像列表控件
	CBitmap m_bmpMenu[IMG_COUNT];	// 保存菜单图片
	CCodeEdit m_wndCodeEdit;	// 定义编辑器控件
	CStatusBar m_wndStatusBar;	// 定义状态栏控件
	CMyToolBar m_wndToolBar;	// 定义工具栏控件
	CTreeCtrl m_wndTreeView;	// 定义树形控件
	CButton m_btnSearch;		// 定义搜索按钮
	CEdit m_edtKeyWord;			// 定义搜索关键词
	CStatic m_stcContent;		// 显示提示的静态控件
	CButton m_chkCase;			// 是否区分大小写的复选框
	CComboBox m_cboType;		// 定义类型下拉框
	CButton m_btnHide;			// 定义隐藏按钮
	CSplitter m_wndSplitter;	// 定义切分条控件
	CStringArray m_arrLanString;		// 存放语言字符串的数组
	void inline ShowKeyBoardState();	// （内联函数）判断键盘NUMLOCK、CAPSLOCK、SCROLLLOCK灯状态，并显示在状态栏
	void inline AddBitmap(CBitmap &bm, UINT uiBmpId);	// （内联函数）加载图片资源到内存
	BOOL LoadSkinMenu();								// 加载动态皮肤菜单
	BOOL SetMainLanguage(UINT uiLanguage, LPCTSTR lpszFileName);				// 设置主界面语言
	void ShowInterfaceLanguage(LPCTSTR lpszFileName);							// 将指定语言字符串显示到主界面
	void GetLanguageString(UINT nItemID, LPTSTR strText, bool bTooltip);		// 获取经常需要用到的特定语言的字符串
	void SetMenuImage(bool bFirst, bool bDelImg);								// 设置菜单图标
	void GetListImage(CImageList &ImageList, CBitmap &Bitmap, int nImageIdx);	// 从图像列表中获得图片信息
	void OnSplitterDown(UINT nID, CPoint point);	// 按下切分条
	void OnSplitterUp(UINT nID, CPoint point);		// 放开切分条
	void OnSplitterMove(UINT nID, CPoint point);	// 移动切分条
	void SetPaneText(UINT nID = 0);					// 设置状态栏文字
	void GetMainClass(HTREEITEM hParent, LPCTSTR strText);	// 第一层分类信息并添加到树形控件
	void GetClassDir(HTREEITEM hParent);					// 获取第二层分类的信息并添加到树形控件
	void AddSubItem(HTREEITEM hParent);						// 为树形控件添加下一层内容
	CString ItemToPath(HTREEITEM hCurrent);					// 将树形控件项目转换为全路径
	HTREEITEM PathToItem(CString strFullPath);					// 将全路径转换为树形控件对应项目
	bool DeleteDirectory(CString strDirName);					// 删除文件夹
	bool ReNameFolder(CString strFromPath,CString strToPath);	// 重命名文件或文件夹
	bool MoveFolder(CString strFromPath,CString strToPath);		// 移动文件或文件夹
	void SearchTreeItem(HTREEITEM hItem, CString strText, int iIconType, bool bMatchCase);	// 搜索分类和项目
	afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);	// 查找和替换对话框响应函数
	afx_msg void OnMainAbout();		// 显示“关于”对话框
	afx_msg void OnFileExit();		// 退出程序
	afx_msg void OnFileAdditem();	// 添加项目
	afx_msg void OnFileAddclass();	// 添加分类
	afx_msg void OnFileReload();	// 重新载入项目和分类
	afx_msg void OnClearClass();	// 清空所有分类和项目
	afx_msg void OnFileSave();		// 保存当前文件
	afx_msg void OnFileSaveAs();	// 导出当前文件
	afx_msg void OnFileClose();		// 关闭当前文件
	afx_msg void OnRenameItem();	// 重命名选中的项目或分类
	afx_msg void OnDeleteItem();	// 删除选中的项目或分类
	afx_msg void OnFileMoveto();	// 移动选中的项目或分类
	afx_msg void OnFilePrep();		// 查看选中的项目或分类的属性
	afx_msg void OnFileSearch();	// 显示搜索框
	afx_msg void OnEditUndo();		// 撤消编辑器操作
	afx_msg void OnEditCut();		// 剪切选中文本
	afx_msg void OnEditCopy();		// 复制选中文本
	afx_msg void OnEditPaste();		// 粘贴剪贴板文本
	afx_msg void OnEditClear();		// 删除选中文本
	afx_msg void OnEditSelall();	// 选中全部文本
	afx_msg void OnEditClearAll();	// 清空编辑器
	afx_msg void OnEditFind();		// 显示查找对话框
	afx_msg void OnEditReplace();	// 显示替换对话框
	afx_msg void OnEditFindNext();	// 查找下一个匹配字符串
	afx_msg void OnEditGoto();		// 文本定位，转到特定行
	afx_msg void OnALine();			// 添加一行
	afx_msg void OnDelALine();		// 删除一行
	afx_msg void OnEditShowlist();	// 显示自动完成列表
	afx_msg void OnEditChangeType();// 更改当前文件的类型
	afx_msg void OnSetOption();		// 打开设置对话框
	afx_msg void OnViewToolbar();	// 显示或隐藏工具栏
	afx_msg void OnViewStatusBar();	// 显示或隐藏状态栏
	afx_msg void OnLanChns();		// 设置界面语言为简体中文
	afx_msg void OnLanChnt();		// 设置界面语言为繁体中文
	afx_msg void OnLanEng();		// 设置界面语言为英文
	afx_msg void OnHelp();			// 显示帮助文件
	// 以下为菜单和工具栏命令状态更新函数
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileClose(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilePrep(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileMoveTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileDeleteItem(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRenameItem(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditSelall(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditClearAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditFind(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditFindNext(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditReplace(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditGoto(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditShowlist(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditChangeType(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewToolbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewStatusBar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLanChns(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLanChnt(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLanEng(CCmdUI *pCmdUI);
	// 以上为菜单和工具栏命令状态更新函数
	afx_msg void OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult);			// 树形控件单击鼠标右键
	afx_msg void OnNMSetfocusTree1(NMHDR *pNMHDR, LRESULT *pResult);		// 树形控件获得焦点
	afx_msg void OnTvnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult);	// 树形控件正在改变选择
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);		// 树形控件改变选择
	afx_msg void OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);	// 树形控件开始编辑文本
	afx_msg void OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);	// 树形控件结束编辑文本
	afx_msg void OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult);	// 树形控件展开分类
	afx_msg void OnBnClickedButtonHide();		// 隐藏搜索框
	afx_msg void OnBnClickedButtonSearch();		// 搜索
};
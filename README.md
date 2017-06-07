# CodeMate 代码片段管理器

项目概述请参阅：[ReadMe.md](https://github.com/singun-lxd/CodeMate/blob/master/CodeMate/ReadMe.md)

版本历史请参阅：[History.md](https://github.com/singun-lxd/CodeMate/blob/master/HISTORY.md)

## 介绍
经常编程的人应该会发现，自己长期以来写下来的代码很多很多，其中不乏一些相对复杂一点的项目。这些代码之中，很多是作者当时思考了很久得出的智慧结晶。而很久之后，突然在某个项目里需要用到以前的代码，在一堆的代码里可能一时间难以找到需要的那一段。
我自己遇到这种情况不少次了，于是萌生了写一个代码管理器项目的想法。有了这么一个管理器，平时可以将自己辛苦写出来的代码片段或算法记录下来。这不是一个大项目，于是我很快就动手了。

Code Mate 意思是代码伴侣，当然也可以认为是配偶~此项目用 MFC 开发，目前用的开发环境是 VS2010，为 Unicode 项目，此项目仅用于学习目的。
由于各种因素的影响，这个项目拖了很久没有开发了。。。2011 年 8 月 2 日之后会找时间继续更新。
下面简单介绍 Code Mate 项目。

### 已经实现的：
- 基于MFC对话框应用程序模版
- 主控件为树形控件和自定义的支持语法高亮的代码编辑器控件
- 支持树形控件和编辑器的切分操作
- 界面带有菜单栏、工具栏以及状态栏
- 树形控件、工具栏和菜单图标全部采用 24 位真彩色位图
- 文件类型支持 ANSI、UTF-8、Unicode、Unicode big endian 编码文本
- 分类和项目操作支持添加、移动、更名、删除、刷新、清空等
- 分类与项目的搜索
- 编辑器支持撤销、剪切、复制、粘贴、删除、全选等功能
- 支持文本的查找和替换（包括区分大小写、全字匹配以及向上或向下查找）
- 主窗口支持各种快捷键操作
- 自动保存ini配置文件
- 多语言支持（目前可以支持简体中文、繁体中文和英文的动态切换）
- 选项设置对话框
- 帮助文件
### 正在准备实现的：
- 支持在文件中查找
- 支持分类与项目的排序
- 支持更多选项的设置
- 更多……
 
目前正在修复一些 Bug。最新版本为 2011 年 8 月 9 日发布，版本号为 1.0.21.62。

## 部分截图

### 主界面：
![image](https://raw.githubusercontent.com/singun-lxd/CodeMate/master/ScreenShots/MainFrame.jpg)

### 多语言切换：
![image](https://raw.githubusercontent.com/singun-lxd/CodeMate/master/ScreenShots/LanguageMenu.png)

### 综合选项：
![image](https://raw.githubusercontent.com/singun-lxd/CodeMate/master/ScreenShots/SettingMainTab.png)

### 编辑器选项：
![image](https://raw.githubusercontent.com/singun-lxd/CodeMate/master/ScreenShots/SettingEditorTab.png)

### 转到特定行和字符：
![image](https://raw.githubusercontent.com/singun-lxd/CodeMate/master/ScreenShots/GoToDialog.png)

### 更改代码类型：
![image](https://raw.githubusercontent.com/singun-lxd/CodeMate/master/ScreenShots/ModifyCodeTypeDialog.png)

### 添加项目：
![image](https://raw.githubusercontent.com/singun-lxd/CodeMate/master/ScreenShots/AddProjectDialog.png)

### 移动到分类：
![image](https://raw.githubusercontent.com/singun-lxd/CodeMate/master/ScreenShots/MoveToClassDialog.png)

### 帮助文档：
![image](https://raw.githubusercontent.com/singun-lxd/CodeMate/master/ScreenShots/HelpDocument.png)

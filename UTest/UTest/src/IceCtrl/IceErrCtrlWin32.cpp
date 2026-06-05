/**
@file		IceErrCtrlWin32.cpp WINDOWS下的调试信息输出函数
@author		
@date 		2007/8/1   17:17
@version	v1.0 Copyright (c) 2007 - All Rights Reserved
@warning 	Windows 专用，勿用于vxworks,CE等系统
此系统另有IceErrCtrlVxworks.c与IceErrCtrlCE.c可用。
使用USE_CONSOLE_WINDOW宏选择是用console窗还是用自己开发的控件函数显示

*********************************************************************/



//#define USE_CONSOLE_WINDOW
#include "pch.h"

#include <conio.h>
#include <Windows.h>

//#include "ComTestDlg.h"


extern "C" void IceExternLogWin32(int logID, const char *pStr)
{
// 缺省选择Windows Console窗口显示信息
#ifdef _WIN32
	AllocConsole();
	_cprintf(pStr);
	_cprintf("\n");
#else
	//可以使用自己开发的控件函数显示
	//g_CComTestDlg->LogInfo(logID, pStr);
#endif
}
#pragma once
#include"windows.h"

class TrayIconService
{
public:
	/// <summary>
	/// 创建托盘图标
	/// </summary>
	/// <param name="hwnd">窗口句柄</param>
	static void createTrayIcon(HWND hwnd, HICON hicon);

	/// <summary>
	/// 创建托盘图标菜单
	/// </summary>
	static LRESULT createTrayMenu(HWND hwnd);
};


#include "TrayIconService.h"
#include "../common/Constants.h"
#include "../window/MainWindow.h"
#include "../utils/MyUtils.h"
#include "../utils/LanguageManager.h"
#include "ConfigService.h"


void TrayIconService::createTrayIcon(HWND hwnd, HICON hicon)
{
	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	nid.uCallbackMessage = WM_TRAYICON;

	nid.hIcon = hicon;

	wcscpy_s(nid.szTip, L"OneWinDesk");

	Shell_NotifyIconW(NIM_ADD, &nid);
}

LRESULT TrayIconService::createTrayMenu(HWND hwnd)
{
	POINT pt;
	GetCursorPos(&pt);

	HMENU hMenu = CreatePopupMenu();

	// 如果当前为 启用单窗口服务, 显示暂停
	if (MainWindow::isServiceEnable()) {
		AppendMenuW(
			hMenu,
			MF_STRING,
			ID_TRAY_DISABLE,
			LanguageManager::res.trayMenu_pause.c_str());
	}
	// 如果当前为暂停状态, 显示开启
	else {
		AppendMenuW(
			hMenu,
			MF_STRING,
			ID_TRAY_ENABLE,
			LanguageManager::res.trayMenu_enable.c_str());
	}

	AppendMenuW(
		hMenu,
		MF_SEPARATOR,
		0,
		nullptr);

	AppendMenuW(
		hMenu,
		MF_STRING | (MyUtils::isAutoStartEnabled()
			? MF_CHECKED
			: 0),
		ID_TRAY_AUTOSTART,
		LanguageManager::res.trayMenu_startOnStartup.c_str());

	AppendMenuW(
		hMenu,
		MF_SEPARATOR,
		0,
		nullptr);

	AppendMenuW(
		hMenu,
		MF_STRING,
		ID_TRAY_ADD_WHITELIST,
		LanguageManager::res.trayMenu_addToWhitelist.c_str());

	AppendMenuW(
		hMenu,
		MF_STRING,
		ID_TRAY_WHITELIST_LISTVIEW,
		LanguageManager::res.WHITELIST_WINDOW_NAME.c_str());

	AppendMenuW(
		hMenu,
		MF_SEPARATOR,
		0,
		nullptr);

	AppendMenuW(
		hMenu,
		MF_STRING | (ConfigService::isFilterSameProcessWindow() ? MF_CHECKED : 0),
		ID_TRAY_FILTER_SAME_PROCESS,
		LanguageManager::res.trayMenu_filterSameProcess.c_str());

	AppendMenuW(
		hMenu,
		MF_SEPARATOR,
		0,
		nullptr);

	AppendMenuW(
		hMenu,
		MF_STRING,
		ID_TRAY_EXIT,
		LanguageManager::res.trayMenu_exit.c_str());

	SetForegroundWindow(hwnd);

	// 显示右键菜单前暂停服务
	MainWindow::stopService();

	TrackPopupMenu(
		hMenu,
		TPM_RIGHTBUTTON,
		pt.x,
		pt.y,
		0,
		hwnd,
		nullptr);

	// 恢复服务
	MainWindow::restartService();

	// 发送一条空消息到窗口, 完成菜单收尾工作
	PostMessageW(hwnd, WM_NULL, 0, 0);

	DestroyMenu(hMenu);
	return 0;
}

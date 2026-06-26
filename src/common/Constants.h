#pragma once
#include <Windows.h>
#include <string>

// 软件名称
const wchar_t APP_NAME[] = L"OneWinDesk";

// 自定义窗口消息
// 托盘图标
constexpr UINT WM_TRAYICON = WM_APP + 1;
// 前台窗口变化
constexpr UINT WM_FOREGROUND_CHANGED = WM_USER + 1;

// 托盘菜单ID
enum TrayMenuId : UINT
{
	ID_TRAY_ENABLE = 1001,
	ID_TRAY_DISABLE = 1002,
	ID_TRAY_AUTOSTART = 1003,
	ID_TRAY_EXIT = 1004,
	ID_TRAY_ADD_WHITELIST = 1005,
	ID_TRAY_WHITELIST_LISTVIEW = 1006,
	ID_TRAY_FILTER_SAME_PROCESS = 1007,
};

enum WhiteListId : UINT
{
	IDC_LISTVIEW = 1010,
	IDC_ADD = 1011,
	IDC_DELETE = 1012,
	IDC_CLEAR = 1013,
};

// 白名单
const std::string string_whitelist = "whitelist";
// 排除相同进程的窗口
const std::string string_filter_same_process_window = "filterSameProcessWindow";

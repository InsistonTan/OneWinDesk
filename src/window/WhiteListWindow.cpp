#include "WhiteListWindow.h"
#include "../service/ConfigService.h"
#include "../common/Constants.h"
#include "../utils/MyUtils.h"
#include "../../resource.h"
#include "../utils/LanguageManager.h"
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")

HWND g_listView, g_addBtn, g_deleteBtn, g_clearBtn;
HFONT g_btnFont;

void WhiteListWindow::show(HINSTANCE hInstance, HICON hIcon)
{
	if (hwnd)
	{
		refreshList();
		ShowWindow(hwnd, SW_SHOW);
		SetForegroundWindow(hwnd);
		return;
	}

	const wchar_t* CLASS_NAME = L"WhiteListWindowClass";

	WNDCLASS wc{};
	wc.lpfnWndProc = wndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hIcon = hIcon;
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

	RegisterClassW(&wc);

	hwnd = CreateWindowExW(
		0,
		CLASS_NAME,
		LanguageManager::res.WHITELIST_WINDOW_NAME.c_str(),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	// 设置图标
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// 获取当前窗口所在显示器的dpi值
	DPI = GetDpiForWindow(hwnd);

	// 根据 DPI 重新设置窗口大小
	MyUtils::setWindowSizeAndPosCenter(hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);

	// 显示窗口
	ShowWindow(hwnd, SW_SHOW);
}

void WhiteListWindow::close()
{
	if (hwnd)
	{
		DestroyWindow(hwnd);
		hwnd = nullptr;
	}
}

LRESULT CALLBACK WhiteListWindow::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// 窗口创建
	case WM_CREATE: {
		// 创建 ListView + 按钮
		initListView(hwnd);
		return 0;
	}
	// 窗口reszie
	case WM_SIZE:
	{
		resizeComponents(hwnd);
		return 0;
	}
	// 窗口关闭
	case WM_CLOSE: 
	{
		ShowWindow(hwnd, SW_HIDE);
		return 0;
	}
	case WM_ERASEBKGND:
	{
		return 1; // 不让系统擦背景（避免闪烁）
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rc;
		GetClientRect(hwnd, &rc);

		HBRUSH brush = CreateSolidBrush(RGB(240, 240, 240)); // 浅灰色背景
		FillRect(hdc, &rc, brush);
		DeleteObject(brush);

		EndPaint(hwnd, &ps);
		return 0;
	}
	// 控件消息
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case IDC_ADD: {
			addItem();
			return 0;
		}
		case IDC_DELETE: {
			// 删除选中
			deleteSelected();
			return 0;
		}
		case IDC_CLEAR: {
			// 清空
			clearAll();
			return 0;
		}
		}
		break;
	}
	case WM_DESTROY: {
		hwnd = nullptr;
		return 0;
	}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WhiteListWindow::refreshList()
{
	ListView_DeleteAllItems(g_listView);

	int index = 0;
	for (const auto& item : ConfigService::getWhitelist())
	{
		LVITEMW lv{};
		lv.mask = LVIF_TEXT;
		lv.iItem = index;
		auto text = MyUtils::stringToWstring(item);
		lv.pszText = (LPWSTR)text.c_str();

		// ListView_InsertItem只会插入第一列(主列) 的信息, 如果需要设置其它列, 使用 ListView_SetItem()
		ListView_InsertItem(g_listView, &lv);
		index++;
	}
}

void WhiteListWindow::deleteSelected()
{
	int sel = ListView_GetNextItem(g_listView, -1, LVNI_SELECTED);
	if (sel == -1)
		return;

	wchar_t buffer[260]{};
	ListView_GetItemText(g_listView, sel, 0, buffer, 260);

	std::wstring processName = buffer;

	ConfigService::removeWhitelist(MyUtils::wstringToString(processName));

	refreshList();
}

void WhiteListWindow::clearAll()
{
	ConfigService::clearWhitelist();
	refreshList();
}

void WhiteListWindow::addItem()
{
	//ConfigService::addWhitelist(L"示例.exe");
	//refreshList();
}

void WhiteListWindow::initListView(HWND hwnd)
{
	INITCOMMONCONTROLSEX icex{};
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	// ListView
	g_listView = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		WC_LISTVIEWW,
		L"",
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
		0, 0, // xy位置
		0, 0,// 大小
		hwnd,
		(HMENU)IDC_LISTVIEW, // 控件id
		GetModuleHandleW(nullptr),
		nullptr
	);

	// ListView的标题行
	LVCOLUMNW col{};
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.cx = 100;// 列宽
	col.pszText = (LPWSTR)LanguageManager::res.whitelist_ListView_Col0_Title.c_str();
	ListView_InsertColumn(g_listView, 0, &col);

	// 按钮
	//g_addBtn = CreateWindowW(L"BUTTON", L"添加",
	//	WS_CHILD | WS_VISIBLE,
	//	0, 0,
	//	0, 0,
	//	hwnd, (HMENU)IDC_ADD, nullptr, nullptr); 

	g_deleteBtn = CreateWindowW(L"BUTTON", LanguageManager::res.whitelist_delete.c_str(),
		WS_CHILD | WS_VISIBLE,
		0, 0,
		0, 0,
		hwnd, (HMENU)IDC_DELETE, nullptr, nullptr);

	g_clearBtn = CreateWindowW(L"BUTTON", LanguageManager::res.whitelist_clear.c_str(),
		WS_CHILD | WS_VISIBLE,
		0, 0,
		0, 0,
		hwnd, (HMENU)IDC_CLEAR, nullptr, nullptr);

	// 初始化按钮的字体
	g_btnFont = CreateFontW(
		MyUtils::scaleDpi(16, DPI), 0, 0, 0,
		FW_SEMIBOLD,
		FALSE, FALSE, FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY,
		DEFAULT_PITCH,
		L"Segoe UI"
	);

	refreshList();
}

void WhiteListWindow::resizeComponents(HWND hwnd) {
	int margin = MyUtils::scaleDpi(10, DPI);

	int listViewWidth = MyUtils::scaleDpi(WINDOW_WIDTH, DPI) - margin * 3;
	int listViewHeight = MyUtils::scaleDpi(WINDOW_HEIGHT - 80, DPI) - margin * 2;

	// 重绘listView
	MoveWindow(g_listView, margin, margin, listViewWidth, listViewHeight, TRUE);

	// 改列宽
	ListView_SetColumnWidth(g_listView, 0, listViewWidth - 10);

	// 按钮位置
	int buttonX = margin;
	int buttonY = listViewHeight + margin * 2;
	// 按钮大小
	int buttonWidth = MyUtils::scaleDpi(80, DPI);
	int buttonHeight = MyUtils::scaleDpi(30, DPI);

	//MoveWindow(g_addBtn, buttonX, buttonY, buttonWidth, buttonHeight, TRUE);

	// 下一个按钮的位置
	//buttonX += buttonWidth + margin;
	MoveWindow(g_deleteBtn, buttonX, buttonY, buttonWidth, buttonHeight, TRUE);

	// 下一个按钮的位置
	buttonX += buttonWidth + margin;
	MoveWindow(g_clearBtn, buttonX, buttonY, buttonWidth, buttonHeight, TRUE);

	// 设置按钮字体大小
	SendMessage(g_addBtn, WM_SETFONT, (WPARAM)g_btnFont, TRUE);
	SendMessage(g_deleteBtn, WM_SETFONT, (WPARAM)g_btnFont, TRUE);
	SendMessage(g_clearBtn, WM_SETFONT, (WPARAM)g_btnFont, TRUE);
}
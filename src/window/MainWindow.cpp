#include "MainWindow.h"
#include "../common/Constants.h"
#include "../utils/MyUtils.h"
#include "../service/TrayIconService.h"
#include "../service/ConfigService.h"
#include "WhiteListWindow.h"
#include "../../resource.h"

#include <string>
#include <format>


MainWindow::MainWindow(HINSTANCE hInstance, HICON hIcon)
	: m_hInstance(hInstance), hIcon(hIcon)
{}

void MainWindow::run()
{
	{
		// 加锁
		std::lock_guard<std::mutex> lock(g_runMutex);

		// 主窗口已存在实例
		if (g_mainWindow != nullptr) {
			return;
		}
		g_mainWindow = this;
	}

	// 加载用户配置
	ConfigService::load();
	
	WNDCLASS wc = {};
	// 设置窗口的消息回调函数
	wc.lpfnWndProc = windowProc;
	// 设置这个窗口类属于哪个 EXE 实例
	wc.hInstance = m_hInstance;
	// 设置窗口类的名称
	wc.lpszClassName = APP_NAME;
	wc.hIcon = hIcon;
	// 向 Windows 注册一个窗口类（Window Class）
	RegisterClassW(&wc);

	// 创建窗口, 但不显示窗口, 只保留托盘图标
	m_hwnd = CreateWindowExW(
		0, // dwExStyle 扩展窗口样式
		APP_NAME, // lpClassName 窗口类名称
		APP_NAME, // lpWindowName 窗口标题
		0, // dwStyle 窗口样式
		0, 0, // 窗口的 x y坐标
		0, 0, // 窗口的 宽度 高度
		nullptr, // hWndParent 父窗口
		nullptr, // hMenu 菜单句柄
		m_hInstance, // 程序实例
		nullptr // lpParam 自定义参数, 可以把当前窗口对象设为自定义参数, 后续可在静态的消息回调函数中取出并保存到窗口句柄
	);

	// 设置图标
	SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// 创建托盘图标
	TrayIconService::createTrayIcon(m_hwnd, hIcon);

	// 监听前台窗口切换的事件
	g_winEventHook = SetWinEventHook(
		EVENT_SYSTEM_FOREGROUND,
		EVENT_SYSTEM_FOREGROUND,
		nullptr,
		winEventProc,
		0,
		0,
		WINEVENT_OUTOFCONTEXT);

	MyUtils::showTrayMessage(m_hwnd, L"OneWinDesk 正在运行");

	// 开始消息循环
	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

LRESULT CALLBACK MainWindow::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//switch (msg)
	//{
	//    // 窗口即将创建时发送的第一个重要消息之一, 这是第一个能获取到 创建窗口时设置的自定义参数 的消息
	//    case WM_NCCREATE: 
	//    {
	//        // 取出创建窗口时自定义参数设置的 当前窗口对象的指针
	//        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
	//        MainWindow* pThis = static_cast<MainWindow*>(cs->lpCreateParams);

	//        // 把当前 MainWindow 对象指针存到窗口对象 (HWND) 里面
	//        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

	//        return TRUE;
	//    }
	//    default:
	//    {
	//        // 取出窗口对象指针
	//        MainWindow* pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	//        // 转发给真正的成员函数
	//        if (pThis)
	//        {
	//            return pThis->handleMessage(hwnd, msg, wParam, lParam);
	//        }
	//    }
	//}

	// 处理消息
	if (g_mainWindow)
	{
		return g_mainWindow->handleMessage(hwnd, msg, wParam, lParam);
	}

	// 系统默认的消息处理
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT MainWindow::handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	// 托盘图标发来的消息
	case WM_TRAYICON:
	{
		// 鼠标右键抬起
		if (lParam == WM_RBUTTONUP)
		{
			// 创建托盘菜单
			return TrayIconService::createTrayMenu(m_hwnd);
		}
		break;
	}

	// 前台窗口切换 消息
	case WM_FOREGROUND_CHANGED:
	{
		HWND foreground = (HWND)wParam;
		handleForegroundChange(foreground);
		return 0;
	}

	// 菜单、按钮等控件发来的消息
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		// 启用
		case ID_TRAY_ENABLE: {
			g_enableOneWinDeskService = true;
			return 0;
		}

	    // 暂停
		case ID_TRAY_DISABLE: {
			g_enableOneWinDeskService = false;
			return 0;
		}

		// 开机自启动
		case ID_TRAY_AUTOSTART: {
			bool enabled = MyUtils::isAutoStartEnabled();
			MyUtils::setAutoStart(!enabled);
			return 0;
		}

		// 添加当前窗口进程到白名单
		case ID_TRAY_ADD_WHITELIST:
		{
			addCurrentWindowToWhitelist();
			return 0;
		}
		// 白名单管理
		case ID_TRAY_WHITELIST_LISTVIEW:
		{
			// 显示白名单列表窗口
			WhiteListWindow::show(m_hInstance, hIcon);
			return 0;
		}
			
		// 退出软件
		case ID_TRAY_EXIT: {
			DestroyWindow(hwnd);
			return 0;
		}
		}
		break;
	}

	// 窗口销毁的消息
	case WM_DESTROY:
	{
		// 停止消息循环, 退出程序
		PostQuitMessage(0);
		return 0;
	}
	}

	// 系统默认的消息处理
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void CALLBACK MainWindow::winEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	// 暂停了桌面单窗口服务
	if (g_enableOneWinDeskService == false)
		return;

	// 只处理前台窗口的事件
	if (event != EVENT_SYSTEM_FOREGROUND)
		return;

	// 跳过自触发的事件
	if (g_selfTrigger)
		return;

	if (g_mainWindow != nullptr) {
		// 发送 前台窗口事件 的消息
		PostMessageW(g_mainWindow->m_hwnd, WM_FOREGROUND_CHANGED, (WPARAM)hwnd, 0);
	}
}

void MainWindow::handleForegroundChange(HWND foreground)
{
	// 上一次的前台窗口
	static HWND last_foreground = nullptr;
	// 上一次时间
	static DWORD lastTick = 0;

	// 前台窗口没发生改变
	if (foreground == last_foreground)
		return;

	// 过滤工具窗口
	LONG_PTR exStyle = GetWindowLongPtrW(foreground, GWL_EXSTYLE);
	if (exStyle & WS_EX_TOOLWINDOW)
		return;

	// 过滤有 Owner 的弹出窗口
	if (GetWindow(foreground, GW_OWNER) != nullptr) {
		//OutputDebugStringW(L"跳过有 Owner 的弹出窗口\n\n");
		return;
	}

	// 过滤子窗口
	if (GetParent(foreground) != NULL) {
		//OutputDebugStringW(L"跳过子窗口\n\n");
		return;
	}

	// 时间防抖, 防止短时间内大量触发
	DWORD now = GetTickCount64();
	if (now - lastTick < 30)
		return;

	
	last_foreground = foreground;
	lastTick = now;

	// 如果前台窗口不是本程序, 保存前台窗口(用于添加当前窗口到白名单)
	if (foreground != m_hwnd) {
		g_currentForcegroundWindow = foreground;
	}

	// 最小化其它窗口
	g_selfTrigger = true;
	minimizeOtherWindows(foreground);
	g_selfTrigger = false;;
}

void MainWindow::minimizeOtherWindows(HWND hwndForeground)
{
	EnumWindows(enumWindowsProc, (LPARAM)hwndForeground);
}

BOOL CALLBACK MainWindow::enumWindowsProc(HWND hwnd, LPARAM lParam_foreground)
{
	HWND foreground = (HWND)lParam_foreground;

	//// 前台窗口的标题
	//wchar_t foregroundTitle[256] = {};
	//GetWindowTextW(foreground, foregroundTitle, 256);
	//// 当前处理的窗口的标题
	//wchar_t currentWinTitle[256] = {};
	//GetWindowTextW(hwnd, currentWinTitle, 256);

	//std::wstring msg = L"前台窗口: ";
	//msg += foregroundTitle;
	//msg += L", 当前处理窗口: ";
	//msg += currentWinTitle;
	//msg += L"\n";

	//OutputDebugStringW(msg.c_str());


	// 跳过当前前台窗口
	if (hwnd == foreground) {
		//OutputDebugStringW(L"跳过当前前台窗口\n\n");
		return TRUE;
	}

	// 跳过桌面
	if (hwnd == GetShellWindow()) {
		//OutputDebugStringW(L"跳过GetShellWindow()\n\n");
		return TRUE;
	}

	// 跳过工具窗口(悬浮窗)
	LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
	if (exStyle & WS_EX_TOOLWINDOW) {
		//OutputDebugStringW(L"跳过工具窗口(悬浮窗)\n\n");
		return TRUE;
	}

	// 跳过不可见窗口
	if (!IsWindowVisible(hwnd)) {
		//OutputDebugStringW(L"跳过不可见窗口\n\n");
		return TRUE;
	}

	// 跳过已最小化
	if (IsIconic(hwnd)) {
		//OutputDebugStringW(L"跳过已最小化窗口\n\n");
		return TRUE;
	}

	//DWORD currentPid;
	//DWORD foregroundPid;
	//GetWindowThreadProcessId(hwnd, &currentPid);
	//GetWindowThreadProcessId(foreground, &foregroundPid);
	//// 跳过与当前前台窗口 同一进程的窗口
	//if (currentPid == foregroundPid) {
	//	//OutputDebugStringW(L"跳过与当前前台窗口 同一进程的窗口\n\n");
	//	return TRUE;
	//}

	// 跳过白名单
	std::wstring processName = MyUtils::getProcessFileName(hwnd);
	if (ConfigService::isProcessNameInWhitelist(processName))
	{
		//OutputDebugStringW(L"跳过白名单进程\n\n");
		return TRUE;
	}

	// 最小化窗口
	ShowWindow(hwnd, SW_MINIMIZE);

	return TRUE;
}

void MainWindow::addCurrentWindowToWhitelist()
{
	if (g_currentForcegroundWindow == nullptr)
		return;

	auto processName = MyUtils::getProcessFileName(g_currentForcegroundWindow);
	ConfigService::addWhitelist(processName);

	// 提示信息
	auto msg = 
		LanguageManager::res.trayNotify_msg_afterAddWhitelist_start 
		+ processName 
		+ LanguageManager::res.trayNotify_msg_afterAddWhitelist_end;
	MyUtils::showTrayMessage(MainWindow::g_mainWindow->m_hwnd, msg);
}


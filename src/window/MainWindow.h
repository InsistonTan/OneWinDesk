#pragma once
#include "windows.h"
#include <mutex>

class MainWindow
{
public:
    // 主窗口对象
    inline static MainWindow* g_mainWindow = nullptr;
    // 监听前台窗口切换的事件
    inline static HWINEVENTHOOK g_winEventHook = nullptr;
    // 自触发锁
    inline static bool g_selfTrigger = false;

    /// <summary>
    /// exe程序实例
    /// </summary>
    HINSTANCE m_hInstance{};
    /// <summary>
    /// 软件图标
    /// </summary>
    HICON hIcon{};
    /// <summary>
    /// 窗口句柄
    /// </summary>
    HWND m_hwnd{};

    /// <summary>
    /// 构造函数
    /// </summary>
    explicit MainWindow(HINSTANCE hInstance, HICON hIcon);

	/// <summary>
	/// 初始化主窗口, 并开始循环接收消息
	/// </summary>
	void run();

    /// <summary>
    /// 获取服务状态
    /// </summary>
    /// <returns>如果服务为开启状态, 返回true. 否则返回 false</returns>
    static bool isServiceEnable() {
        return g_enableOneWinDeskService;
    }
    /// <summary>
    /// 暂停服务
    /// </summary>
    static void stopService() {
        g_enableOneWinDeskService = false;
    }
    /// <summary>
    /// 重启服务
    /// </summary>
    static void restartService() {
        g_enableOneWinDeskService = true;
    }

private:
    /// <summary>
    /// 是否开启桌面单窗口服务
    /// </summary>
    inline static bool g_enableOneWinDeskService = true;
    /// <summary>
    /// run()时使用的锁, 当前主窗口只允许创建一个
    /// </summary>
    std::mutex g_runMutex;
    /// <summary>
    /// 当前前台窗口
    /// </summary>
    inline static HWND g_currentForcegroundWindow = nullptr;

    /// <summary>
    /// 当前窗口消息回调函数, 静态
    /// </summary>
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// 消息处理器
    /// </summary>
    LRESULT handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// win系统事件的回调函数
    /// </summary>
    static void winEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);

    /// <summary>
    /// 前台窗口事件的处理
    /// </summary>
    void handleForegroundChange(HWND hwndForeground);

    /// <summary>
    /// 最小化其它窗口
    /// </summary>
    void minimizeOtherWindows(HWND hwndForeground);

    /// <summary>
    /// 枚举所有窗口的回调函数
    /// </summary>
    static BOOL enumWindowsProc(HWND hwnd, LPARAM lParam_foreground);

    /// <summary>
    /// 添加当前窗口进程到白名单
    /// </summary>
    void addCurrentWindowToWhitelist();
};


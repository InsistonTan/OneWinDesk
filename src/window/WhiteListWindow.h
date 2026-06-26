#pragma once
#include "windows.h"

/// <summary>
/// 白名单列表展示窗口
/// </summary>
class WhiteListWindow
{
public:
    // 当前窗口句柄
    inline static HWND hwnd = nullptr;
    // 系统dpi
    inline static int DPI;

    // 显示白名单窗口
    static void show(HINSTANCE hInstance, HICON hIcon);
    static void close();

private:
    // 窗口大小
    inline static int WINDOW_WIDTH = 400;
    inline static int WINDOW_HEIGHT = 300;

    // 窗口消息回调函数
    static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
    // 刷新列表
    static void refreshList();
    // 删除选中项
    static void deleteSelected();
    // 清空列表
    static void clearAll();
    // 增加
    static void addItem();
    // 初始化 listview
    static void initListView(HWND hwnd);
    // 刷新组件大小
    static void resizeComponents(HWND hwnd);
};
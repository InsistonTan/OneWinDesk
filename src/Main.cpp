#include "Main.h"
#include <windows.h>
#include "window/MainWindow.h"
#include "../resource.h"

/// <summary>
/// 程序入口
/// </summary>
int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    PWSTR,
    int)
{
    // 开启DPI感知
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // 软件图标
    HICON hIcon = (HICON)LoadImage(
        hInstance,
        MAKEINTRESOURCE(IDI_ICON1),
        IMAGE_ICON,
        0, 0,
        LR_DEFAULTSIZE | LR_SHARED
    );

    MainWindow app(hInstance, hIcon);
    app.run();

    return 0;
}



#pragma once
#include "windows.h"
#include "../common/Constants.h"
#include <string>
#include <algorithm>
#include <cctype>
#include "LanguageManager.h"

class MyUtils
{
public:

    /// <summary>
    /// 获取进程名
    /// </summary>
    /// <param name="hwnd">窗口句柄</param>
    /// <returns>进程名, 例如: "Taskmgr.exe"</returns>
    static std::wstring getProcessFileName(HWND hwnd)
    {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);

        if (pid == 0)
            return L"";

        HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            pid);

        if (!hProcess)
            return L"";

        wchar_t path[MAX_PATH] = {};
        DWORD size = MAX_PATH;

        QueryFullProcessImageNameW(
            hProcess,
            0,
            path,
            &size);

        CloseHandle(hProcess);

        wchar_t* fileName = wcsrchr(path, L'\\');

        return fileName ? fileName + 1 : path;
    }

    /// <summary>
    /// 设置当前程序 开机自启动
    /// </summary>
    /// <param name="enable">true 开启, false 关闭</param>
    /// <returns>操作成功, 返回 true, 否则返回 false</returns>
    static bool setAutoStart(bool enable)
    {
        HKEY hKey;

        LONG result = RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
            0,
            KEY_SET_VALUE,
            &hKey);

        if (result != ERROR_SUCCESS)
            return false;

        if (enable) {
            // 当前程序 exe 路径
            wchar_t tempPath[MAX_PATH];
            GetModuleFileNameW(nullptr, tempPath, MAX_PATH);
            // 路径为空, 终止操作
            DWORD len = GetModuleFileNameW(nullptr, tempPath, MAX_PATH);
            if (len == 0)
            {
                RegCloseKey(hKey);
                return false;
            }

            // 路径使用双引号包住
            std::wstring exePath = L"\"";
            exePath += tempPath;
            exePath += L"\"";

            result = RegSetValueExW(
                hKey,
                APP_NAME,
                0,
                REG_SZ,
                reinterpret_cast<const BYTE*>(exePath.c_str()),
                static_cast<DWORD>((exePath.size() + 1) * sizeof(wchar_t)));
        }
        else {
            result = RegDeleteValueW(hKey, APP_NAME);
            // 如果删除时, 注册表项没找到, 也返回成功
            if (result == ERROR_FILE_NOT_FOUND)
            {
                result = ERROR_SUCCESS;
            }
        }
        
        RegCloseKey(hKey);

        return result == ERROR_SUCCESS;
    }

    /// <summary>
    /// 当前是否已开启了 开机自启动
    /// </summary>
    /// <returns></returns>
    static bool isAutoStartEnabled()
    {
        HKEY hKey;

        LONG result = RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
            0,
            KEY_QUERY_VALUE,
            &hKey);

        if (result != ERROR_SUCCESS)
            return false;

        result = RegQueryValueExW(
            hKey,
            APP_NAME,
            nullptr,
            nullptr,
            nullptr,
            nullptr);

        RegCloseKey(hKey);

        return result == ERROR_SUCCESS;
    }

    /// <summary>
    /// wstring 转 string
    /// </summary>
    static std::string wstringToString(const std::wstring& wstr)
    {
        if (wstr.empty())
            return {};

        int size = WideCharToMultiByte(
            CP_UTF8,
            0,
            wstr.c_str(),
            -1,
            nullptr,
            0,
            nullptr,
            nullptr);

        std::string result(size - 1, 0);

        WideCharToMultiByte(
            CP_UTF8,
            0,
            wstr.c_str(),
            -1,
            result.data(),
            size,
            nullptr,
            nullptr);

        return result;
    }

    /// <summary>
    /// string 转 wstring
    /// </summary>
    static std::wstring stringToWstring(const std::string& str)
    {
        if (str.empty())
            return {};

        int size = MultiByteToWideChar(
            CP_UTF8,
            0,
            str.c_str(),
            -1,
            nullptr,
            0);

        std::wstring result(size - 1, 0);

        MultiByteToWideChar(
            CP_UTF8,
            0,
            str.c_str(),
            -1,
            result.data(),
            size);

        return result;
    }

    /// <summary>
    /// 去掉string左右两端的空格
    /// </summary>
    static std::string trim(const std::string& str)
    {
        auto start = std::find_if_not(
            str.begin(),
            str.end(),
            [](unsigned char ch)
        {
            return std::isspace(ch);
        });

        auto end = std::find_if_not(
            str.rbegin(),
            str.rend(),
            [](unsigned char ch)
        {
            return std::isspace(ch);
        }).base();

        if (start >= end)
            return "";

        return std::string(start, end);
    }

    /// <summary>
    /// dpi缩放
    /// </summary>
    /// <param name="base_size">初始尺寸</param>
    /// <param name="dpi">dpi</param>
    /// <returns>dpi缩放后的尺寸</returns>
    static int scaleDpi(int base_size, int dpi) {
        return base_size * dpi / 96;
    }

    /// <summary>
    /// 根据dpi设置窗口大小, 然后居中
    /// 参数flag: 
    /// </summary>
    static void setWindowSizeAndPosCenter(HWND hwnd, int width, int height, UINT flag = SWP_NOZORDER) {
        // hwnd窗口所在显示器的dpi
        int dpi = GetDpiForWindow(hwnd);

        int cal_width = MulDiv(width, dpi, 96);
        int cal_height = MulDiv(height, dpi, 96);

        RECT rc;
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rc, 0);

        int x = rc.left + (rc.right - rc.left - cal_width) / 2;
        int y = rc.top + (rc.bottom - rc.top - cal_height) / 2;

        SetWindowPos(hwnd, nullptr, x, y, cal_width, cal_height, flag);
    }

    /// <summary>
    /// 显示托盘气泡消息
    /// </summary>
    /// <param name="hwnd">窗口句柄</param>
    /// <param name="msg">托盘气泡消息内容</param>
    static void showTrayMessage(HWND hwnd,std::wstring msg) {
        if (msg.length() <= 0)
            return;

        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uTimeout = 3000;
        nid.uFlags = NIF_INFO;

        wcscpy_s(nid.szInfo, msg.c_str());
        wcscpy_s(nid.szInfoTitle, LanguageManager::res.trayNotify_type_info.c_str());
        nid.dwInfoFlags = NIIF_INFO; // 图标类型

        Shell_NotifyIconW(NIM_MODIFY, &nid);
    }
};


#pragma once
#include "../common/Constants.h"
#include "windows.h"

/// <summary>
/// 多语言管理器
/// </summary>
class LanguageManager {
private:
	/// <summary>
	/// 语言资源类
	/// </summary>
	class LanguageResouces {
	public:
		std::wstring trayMenu_pause;// 暂停
		std::wstring trayMenu_enable;// 启动
		std::wstring trayMenu_startOnStartup;// 开机自启动
		std::wstring trayMenu_addToWhitelist;// 将前台窗口进程添加到白名单
		std::wstring trayMenu_filterSameProcess;// 排除相同进程的窗口
		std::wstring trayMenu_exit;// 退出

		std::wstring WHITELIST_WINDOW_NAME; // 白名单管理 窗口名称
		std::wstring whitelist_ListView_Col0_Title;// 白名单管理-列表 第一列的标题
		std::wstring whitelist_delete;// L"删除"
		std::wstring whitelist_clear;// L"清空"

		std::wstring trayNotify_type_info;// 托盘气泡提醒类型 - 提示 
		std::wstring trayNotify_msg_afterRun;// 程序运行后显示的托盘气泡消息内容
		std::wstring trayNotify_msg_afterAddWhitelist_start;// 添加前台窗口进程到白名单成功后  显示消息内容
		std::wstring trayNotify_msg_afterAddWhitelist_end;

		LanguageResouces() { init(); }

		void init()
		{
			// 获取当前界面语音
			LANGID lang = GetUserDefaultUILanguage();
			// 根据语言加载不同的文本
			if (PRIMARYLANGID(lang) == LANG_CHINESE)
				loadChinese();
			else
				loadEnglish();
		}

	private:
		void loadChinese()
		{
			trayMenu_pause = L"暂停";
			trayMenu_enable = L"启动";
			trayMenu_startOnStartup = L"开机自启动";
			trayMenu_addToWhitelist = L"将前台窗口进程添加到白名单";
			trayMenu_filterSameProcess = L"排除相同进程的窗口";
			trayMenu_exit = L"退出";

			WHITELIST_WINDOW_NAME = L"白名单管理";
			whitelist_ListView_Col0_Title = L"白名单进程";
			whitelist_delete = L"删除选择";
			whitelist_clear = L"清空";

			trayNotify_type_info = L"提示";
			trayNotify_msg_afterRun = L"OneWinDesk 正在运行";

			trayNotify_msg_afterAddWhitelist_start = L"已添加进程 ";
			trayNotify_msg_afterAddWhitelist_end = L" 到白名单";
		}

		void loadEnglish()
		{
			trayMenu_pause = L"Pause";
			trayMenu_enable = L"Start";
			trayMenu_startOnStartup = L"Run at startup";
			trayMenu_addToWhitelist = L"Add foreground process to whitelist";
			trayMenu_filterSameProcess = L"Exclude windows of the same process";
			trayMenu_exit = L"Exit";

			WHITELIST_WINDOW_NAME = L"Whitelist Manager";
			whitelist_ListView_Col0_Title = L"Whitelisted Processes";
			whitelist_delete = L"Delete Selected";
			whitelist_clear = L"Clear";

			trayNotify_type_info = L"Info";
			trayNotify_msg_afterRun = L"OneWinDesk is running";

			trayNotify_msg_afterAddWhitelist_start = L"Added ";
			trayNotify_msg_afterAddWhitelist_end = L" to whitelist sucessfully.";
		}
	};

public:
	/// <summary>
	/// 多语言资源
	/// </summary>
	inline static const LanguageResouces res;
};
#pragma once
#include<string>
#include<mutex>
#include <unordered_set>

/// <summary>
/// 用户配置类, 记录所有软件配置
/// </summary>
class UserConfig {
public:
	/// <summary>
	/// 白名单集合
	/// </summary>
	std::unordered_set<std::string> whitelist;

	/// <summary>
	/// 是否排除掉相同进程的窗口
	/// </summary>
	bool filterSameProcessWindow = true;
};


/// <summary>
/// 配置服务
/// </summary>
class ConfigService
{
public:
	/// <summary>
	/// 添加 进程名称 到白名单
	/// </summary>
	/// <param name="processName">进程名称</param>
	static void addWhitelist(std::wstring processName);
	/// <summary>
	/// 该进程是否被添加进白名单
	/// </summary>
	/// <returns></returns>
	static bool isProcessNameInWhitelist(std::wstring processName);
	/// <summary>
	/// 获取白名单列表
	/// </summary>
	/// <returns></returns>
	static std::unordered_set<std::string> getWhitelist();
	/// <summary>
	/// 移除白名单
	/// </summary>
	static void removeWhitelist(std::string removeItem);
	/// <summary>
	/// 清空白名单
	/// </summary>
	static void clearWhitelist();

	/// <summary>
	/// 保存配置到文件
	/// </summary>
	static void saveToFile();
	/// <summary>
	/// 加载配置
	/// </summary>
	static void load();
	
	/// <summary>
	/// 设置 是否排除掉相同进程的窗口
	/// </summary>
	static void setFilterSameProcessWindow(bool val);
	/// <summary>
	/// 当前是否排除掉相同进程的窗口
	/// </summary>
	static bool isFilterSameProcessWindow();

private:
	/// <summary>
	/// 用户配置对象
	/// </summary>
	inline static UserConfig userConfig;

	/// <summary>
	/// 操作用户配置对象的锁
	/// </summary>
	inline static std::mutex g_actionMutex;

	/// <summary>
	/// 获取用户配置文件路径
	/// </summary>
	/// <returns></returns>
	static std::wstring getConfigPath();

	
};

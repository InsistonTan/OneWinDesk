#include "ConfigService.h"
#include "../utils/MyUtils.h"
#include <shlobj.h>
#include <filesystem>
#include <fstream>
#include "../window/MainWindow.h"


void ConfigService::saveToFile()
{
	std::ofstream file(getConfigPath(), std::ios::out | std::ios::trunc);

	if (!file.is_open())
	{
		return;
	}

	// 白名单
	for (const auto& item : userConfig.whitelist)
	{
		file << string_whitelist << "=" << item << '\n';
	}

	file.close();
}

void ConfigService::load()
{
	// 加锁
	std::lock_guard<std::mutex> lock(g_actionMutex);

	std::ifstream file(getConfigPath());

	if (!file.is_open())
	{
		return;
	}

	// 重置白名单集合
	userConfig.whitelist.clear();

	std::string line;
	while (std::getline(file, line))
	{
		auto pos = line.find('=');

		if (pos == std::string::npos)
			continue;

		std::string key = MyUtils::trim(line.substr(0, pos));
		std::string value = MyUtils::trim(line.substr(pos + 1));

		if (key == string_whitelist && value.length() > 0) {
			userConfig.whitelist.insert(value);
		}
	}

	file.close();
}

std::wstring ConfigService::getConfigPath()
{
	PWSTR path = nullptr;

	// 获取 localAppData
	SHGetKnownFolderPath(
		FOLDERID_LocalAppData,
		0,
		nullptr,
		&path);

	std::wstring localAppData = path;

	CoTaskMemFree(path);

	std::filesystem::path configDir = std::filesystem::path(localAppData) / APP_NAME;

	// 不存在则创建
	std::filesystem::create_directories(configDir);

	return (configDir / L"config.ini").wstring();
}

bool ConfigService::isProcessNameInWhitelist(std::wstring processName)
{
	// 加锁
	std::lock_guard<std::mutex> lock(g_actionMutex);
	return userConfig.whitelist.contains(MyUtils::wstringToString(processName));
}

std::unordered_set<std::string> ConfigService::getWhitelist()
{
	// 加锁
	std::lock_guard<std::mutex> lock(g_actionMutex);
	return userConfig.whitelist;
}

void ConfigService::addWhitelist(std::wstring processName)
{
	if (processName.length() <= 0)
		return;

	{
		// 加锁
		std::lock_guard<std::mutex> lock(g_actionMutex);

		// 添加到白名单
		userConfig.whitelist.insert(MyUtils::wstringToString(processName));

		// 保存到文件
		saveToFile();
	}
}

void ConfigService::removeWhitelist(std::string removeItem)
{
	// 加锁
	std::lock_guard<std::mutex> lock(g_actionMutex);
	userConfig.whitelist.erase(removeItem);

	saveToFile();
}

void ConfigService::clearWhitelist()
{
	// 加锁
	std::lock_guard<std::mutex> lock(g_actionMutex);
	userConfig.whitelist.clear();

	saveToFile();
}



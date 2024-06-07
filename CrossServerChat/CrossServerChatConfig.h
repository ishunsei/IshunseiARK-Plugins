#pragma once
#include <fstream>
#include <json.hpp>
#include <Logger/spdlog/fmt/bundled/format.h>

#include "CrossServerChatConst.h"
#include "Tools.h"
#include "API/UE/BasicTypes.h"
#include "Logger/Logger.h"

using nlohmann::json;
using std::pair;

inline bool CheckPermissionsByPluginInfo()
{
    const std::string filePath = fmt::format("{}/ArkApi/Plugins/{}/PluginInfo.json", ArkApi::Tools::GetCurrentDir(), PROJECTNAME);
    std::ifstream file{ filePath };
    if (!file.is_open()) throw std::runtime_error("Can't open PluginInfo.json");

    const auto& config = json::parse(file);
    file.close();
	
    if (config.contains("Dependencies"))
    {
        for (auto& item : config["Dependencies"].items())
        {
            if (auto name = item.value().get<std::string>();
            	name._Equal("Permissions"))
                return true;
        }
    }
    return false;
}

inline std::string ReadServerName()
{
    const auto& path = ArkApi::Tools::GetCurrentDir();
    size_t rootIndex = std::string::npos;
    size_t serverDataIndex = std::string::npos;
    for (int i = 0; i < 5; i++) {
        if (i != 0) rootIndex--;
        rootIndex = path.find_last_of('\\', rootIndex);
        if (i < 3) serverDataIndex = rootIndex;
    }

    const auto& serverDataDirectory = path.substr(0, serverDataIndex);
    const auto& rootDirectory = path.substr(0, rootIndex) + "\\Profiles\\";
    _finddata_t findData;

    const auto& handle = _findfirst((rootDirectory + "*.profile").c_str(), &findData);
    if (handle == -1)
    {
        throw std::runtime_error(fmt::format("_findfirst error path ->  {}*.profile", rootDirectory));
    }

    do
    {
        if (findData.attrib)
        {
            std::ifstream file { rootDirectory + findData.name };
            if (!file.is_open())
            {
                throw std::runtime_error(fmt::format("Can't open {}{}", rootDirectory, findData.name));
            }
            json j = json::parse(file);
            file.close();

            auto installDirectory = j.value("InstallDirectory", "");
            if (installDirectory._Equal(serverDataDirectory))
            {
                return j.value("ProfileName", "");
            }
        }
    } while (_findnext(handle, &findData) == 0);

    return "";
}

inline void from_json(const json& j, PluginConfig& p)
{
	auto defaultConfig = CrossServerChatConfig();

	if (j.contains("Default")) {
		const auto& defaultValue = j["Default"];
		defaultConfig.name = defaultValue.value("Name", "");
		defaultConfig.icon = defaultValue.value("Icon", "");
		defaultConfig.message = defaultValue.value("Message", "{}");
		defaultConfig.richText = defaultValue.value("RichText", false);
		defaultConfig.disabled = defaultValue.value("Disabled", false);
	}
	else
	{
		throw std::runtime_error("config.json default config not find");
	}

	auto playerMap = map<uint64, CrossServerChatConfig>();
	if (j.contains("Players"))
	{
		for (auto& item : j["Players"].items())
		{
			const auto& steamId = item.value().value<uint64>("SteamId", 0);
			auto configEntity = CrossServerChatConfig();
			const auto& itemValue = item.value();
			configEntity.name = itemValue.value("Name", "");
			configEntity.icon = itemValue.value("Icon", "");
			configEntity.message = itemValue.value("Message", "{}");
			configEntity.richText = itemValue.value("RichText", false);
			configEntity.disabled = itemValue.value("Disabled", false);

			playerMap.insert(pair(steamId, configEntity));
		}
	}

	auto groupMap = map<string, CrossServerChatGroupConfig>();
	if (j.contains("Groups"))
	{
		int priority = 0;
		for (auto& item : j["Groups"].items())
		{
			priority++;
			const auto& key = item.value().value("GroupName", "");
			auto configEntity = CrossServerChatGroupConfig();
			const auto& itemValue = item.value();
			configEntity.name = itemValue.value("Name", "");
			configEntity.icon = itemValue.value("Icon", "");
			configEntity.message = itemValue.value("Message", "{}");
			configEntity.richText = itemValue.value("RichText", false);
			configEntity.priority = priority;
			configEntity.disabled = itemValue.value("Disabled", false);

			groupMap.insert(pair(key, configEntity));
		}
	}

	auto emojiEscape = map<string, string>();
	if (j.contains("EmojiEscape"))
	{
		for (auto& item : j["EmojiEscape"].items())
		{
			emojiEscape.insert(pair<string, string>(item.key(), item.value()));
		}
	}

	p.replaceLocalMessage = j.value("ReplaceLocalMessage", false);
	p.serverName = j.value("ServerName", "");
	p.defaultConfig = defaultConfig;
	p.playerConfig = playerMap;
	p.emojiEscape = emojiEscape;
	p.groupConfig = groupMap;
}

inline void ReadConfig()
{
    const std::string filePath = fmt::format("{}/ArkApi/Plugins/{}/config.json",
    	ArkApi::Tools::GetCurrentDir(), PROJECTNAME);
    std::ifstream file { filePath };
    if (!file.is_open()) throw std::runtime_error("Can't open config.json");
    const auto& j = json::parse(file);
    file.close();

    auto config = j.get<PluginConfig>();

    if (config.serverName.empty())
    {
        config.serverName = ReadServerName();
    }
    if (config.serverName.empty())
    {
        throw std::runtime_error("ServerName is empty!");
    }
    
    Log::GetLog()->info("configs -> debug is [{}], replaceLocalMessage is [{}], ServerName is [{}]",
#if NDEBUG
    false,
#else
    true,
#endif
    	config.replaceLocalMessage, config.serverName);

#ifndef NDEBUG
    std::ranges::for_each(config.emojiEscape, [](const auto& kv)
        {
			Log::GetLog()->info("config.json emojiEscape config -> Key is [{}], Value is [{}]", kv.first, kv.second);
        });

    Log::GetLog()->info("config.json default config -> Name is [{}], Icon is [{}], Message is [{}], Disabled is [{}]",
        config.defaultConfig.name, config.defaultConfig.icon, config.defaultConfig.message, config.defaultConfig.disabled);

    std::ranges::for_each(config.playerConfig, [](const auto& kv)
        {
            Log::GetLog()->info("config.json players config -> SteamId is [{}], Name is [{}], Icon is [{}], Message is [{}], Disabled is [{}]",
            	kv.first, kv.second.name, kv.second.icon, kv.second.message, kv.second.disabled);
        });

    std::ranges::for_each(config.groupConfig, [](const auto& kv)
        {
            Log::GetLog()->info("config.json groups config -> Key is {}, Name is [{}], Icon is [{}], Message is [{}], Disabled is [{}]",
            	kv.first, kv.second.name, kv.second.icon, kv.second.message, kv.second.disabled);
        });
#endif
	
    CurrentPluginConfig = config;
}
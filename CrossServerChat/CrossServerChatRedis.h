#pragma once

#include <event2/event.h>
#include <hiredis/adapters/libevent.h>
#include <Permissions/ArkPermissions.h>

#include "CrossServerChatConst.h"
#include "API/UE/Containers/FString.h"

#pragma comment(lib, "event.lib")
#pragma comment(lib, "hiredis.lib")
#pragma comment(lib, "Permissions.lib")

inline void FormatMessageByTag(std::string& message, const std::string& tagStart, const std::string& tagEnd, const std::map<std::string, std::string>& emojiEscape)
{
	for (size_t index = 0; index < message.length(); )
	{
		auto startIndex = std::string::npos;

		size_t emojiLength = 0;
		std::string v;

		for (const auto& [key, value] : emojiEscape)
		{
			const auto& findIndex = message.find(key, index);
			if (findIndex == std::string::npos) continue;
			
			if (startIndex == std::string::npos || findIndex < startIndex)
			{
				startIndex = findIndex;
				emojiLength = key.length();
				v = value;
			}
		}

		if (startIndex == std::string::npos)
		{
			message.insert(message.length(), tagEnd);
			message.insert(index, tagStart);
			break;
		}

		if (startIndex != index)
		{
			message.insert(startIndex, tagEnd);
			message.insert(index, tagStart);
			startIndex += tagEnd.length() + tagStart.length();
		}
        
		message.replace(startIndex, emojiLength, v);
        
		startIndex += v.length();
		
		if (startIndex < message.length())
		{
			bool flag = true;
			while (flag)
			{
				flag = false;
				for (const auto& [key, value] : emojiEscape)
				{
					const auto& findIndex = message.find(key, startIndex);
					if (findIndex == startIndex)
					{
						message.replace(findIndex, key.length(), value);
						startIndex += value.length();
						flag = true;
						break;
					}
				}
			}
		}
		index = startIndex;
	}
}

inline void OnRedisConnected(const redisAsyncContext* c, const int status)
{
    if (status != REDIS_OK)
    {
        Log::GetLog()->warn("OnRedisConnected error -> {}", c->errstr);
    	return;
    }
    try
    {
        auto& token = CurrentPluginToken;
        token.redisContext = redisConnect("127.0.0.1", 6379);
        Log::GetLog()->info("redis connect success");
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error("OnRedisConnected error -> {}", error.what());
    }
    catch (const std::string& error)
    {
        Log::GetLog()->error("OnRedisConnected error -> {}", error);
    }
    catch (const char* error)
    {
        Log::GetLog()->error("OnRedisConnected error -> {}", error);
    }
    catch (...)
    {
        Log::GetLog()->error("OnRedisConnected unknown error");
    }
}

inline void OnRedisDisConnected(const redisAsyncContext* c, const int status)
{
    if (status != REDIS_OK)
    {
        Log::GetLog()->warn("OnRedisDisConnected error -> {}", c->errstr);
    	return;
    }
	
    try
    {
        const auto& token = CurrentPluginToken;
        redisFree(token.redisContext);
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error("OnRedisDisConnected error -> {}", error.what());
    }
    catch (const std::string& error)
    {
        Log::GetLog()->error("OnRedisDisConnected error -> {}", error);
    }
    catch (const char* error)
    {
        Log::GetLog()->error("OnRedisDisConnected error -> {}", error);
    }
    catch (...)
    {
        Log::GetLog()->error("OnRedisDisConnected unknown error");
    }

    Log::GetLog()->warn("redis is disconnect");
}

inline void OnRedisMessage(redisAsyncContext* c, void* r, void* p)
{
    try
    {
		const auto& config = CurrentPluginConfig;
		const auto& token = CurrentPluginToken;

		if (token.isWorking == false) return;

		if (config.serverName.empty()) {
			throw std::runtime_error("ServerName is Empty!");
		}
    	const auto& reply = static_cast<redisReply*>(r);
    	if (reply == nullptr) return;
    	if (reply->type != REDIS_REPLY_ARRAY || reply->elements != 3) return;
		if (strcmp(reply->element[0]->str, "subscribe") == 0) return;

		const std::string message = reply->element[2]->str;
    	const auto& world = ArkApi::GetApiUtils().GetWorld();
    	if (world == nullptr) return;
    	
    	auto index = message.find('\r');
		if (index < 0 || index == std::string::npos) throw std::runtime_error("not find serverName in message!");
		const auto& serverName = message.substr(0, index);

    	if (serverName.empty()) throw std::runtime_error("serverName empty in message!");
#if NDEBUG
		if (!config.replaceLocalMessage && strcmp(config.serverName.data(), serverName.data()) == 0) return;
#endif

		auto startIndex = index + 1;
		index = message.find('\r', startIndex);
		if (index < 0 || index == std::string::npos) throw std::runtime_error("not find steamId in message!");
		const auto& steamIdStr = message.substr(startIndex, index - startIndex);
		const auto& senderSteamId = std::stoull(steamIdStr);

		startIndex = index + 1;
		index = message.find('\r', startIndex);
		if (index < 0 || index == std::string::npos) throw std::runtime_error("not find characterName in message!");
		const auto& characterName = message.substr(startIndex, index - startIndex);
		if (characterName.empty()) throw std::runtime_error("characterName empty in message!");
		
		startIndex = index + 1;
		index = message.find('\r', startIndex);
		if (index < 0 || index == std::string::npos) throw std::runtime_error("not find tribeName in message!");
		const auto& tribeName = message.substr(startIndex, index - startIndex);
		auto playerMessage = message.substr(index + 1);

		if (playerMessage.empty()) throw std::runtime_error("playerMessage empty!");
		if (tribeName.empty())
		{
			Log::GetLog()->info("<{}> {}: {}", serverName, characterName, playerMessage);
		}
		else
		{
			Log::GetLog()->info("<{}> {}({}): {}", serverName, characterName, tribeName, playerMessage);
		}
		
		std::string formatName;
		std::string iconPath;
		std::string formatMessage; 
		bool richText = false;

		const auto& playerConfig = config.playerConfig;
		if (const auto& playerIterator = playerConfig.find(senderSteamId);
			playerIterator != playerConfig.end())
		{
			const auto& second = playerIterator->second;
			formatName = second.name;
			iconPath = second.icon;
			formatMessage = second.message;
			richText = second.richText;
		}
    	else
    	{
			const auto& defaultConfig = config.defaultConfig;
			formatName = defaultConfig.name;
			iconPath = defaultConfig.icon;
			formatMessage = defaultConfig.message;
			richText = defaultConfig.richText;

			if (usePermissions)
			{
				const auto& groupConfig = config.groupConfig;
				int currentPriority = 0;
				for (auto& groupName : Permissions::GetPlayerGroups(senderSteamId))
				{
					const auto& key = groupName.ToString();
					const auto& groupIterator = groupConfig.find(key);
					if (groupIterator == groupConfig.end()) continue;
					
					const auto& second = groupIterator->second;
					if (second.priority <= currentPriority) continue;
					
					currentPriority = second.priority;
					formatName = second.name;
					iconPath = second.icon;
					formatMessage = second.message;
					richText = second.richText;
				}
			}
		}

		auto chat_message = FChatMessage();
		chat_message.ChatType = EChatType::Type::AllianceChat;
		if (!formatName.empty())
		{
			const auto& senderName = fmt::format("<{}>{}({})", serverName, characterName, formatName);
			chat_message.SenderName = FString(ArkApi::Tools::Utf8Decode(senderName));
		}
    	else
    	{
			const auto& senderName = fmt::format("<{}>{}", serverName, characterName);
			chat_message.SenderName = FString(ArkApi::Tools::Utf8Decode(senderName));
		}
		
		if (!tribeName.empty()) chat_message.SenderTribeName = FString(ArkApi::Tools::Utf8Decode(tribeName));
		

		const auto& placeholderIndex = formatMessage.find("{}", 0);
		if (placeholderIndex == std::string::npos) return;

		if (richText)
		{
			FormatMessageByTag(playerMessage,
				formatMessage.substr(0, placeholderIndex),
				formatMessage.substr(placeholderIndex + 2), config.emojiEscape);
		}
    	else
    	{
			playerMessage = formatMessage.replace(placeholderIndex, 2, playerMessage);
		}
		chat_message.Message = FString(ArkApi::Tools::Utf8Decode(playerMessage));

		if (!iconPath.empty())
		{
			const auto& icon = reinterpret_cast<UTexture2D*>(Globals::StaticLoadObject(UTexture2D::StaticClass(),
				nullptr, ArkApi::Tools::Utf8Decode(iconPath).c_str(), nullptr, 0, 0, true));
			chat_message.SenderIcon = icon;
		}
		
		const auto& playerControllers = world->PlayerControllerListField();
		for (auto playerController : playerControllers)
		{
			const auto& shooter = static_cast<AShooterPlayerController*>(playerController.Get());
			shooter->ClientChatMessage(chat_message);
		}
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error("HandlerRedisMessage error -> {}", error.what());
    }
    catch (const std::string& error)
    {
        Log::GetLog()->error("HandlerRedisMessage error -> {}", error);
    }
    catch (const char* error)
    {
        Log::GetLog()->error("HandlerRedisMessage error -> {}", error);
    }
    catch (...)
    {
        Log::GetLog()->error("HandlerRedisMessage unknown error");
    }
}

inline void dispatch(PluginToken& token) {
    try {
        while (token.isWorking) {
            const auto& redisAsyncContext = redisAsyncConnect("127.0.0.1", 6379);
            if (redisAsyncContext->err) {
                Log::GetLog()->error("error to connect redis -> {}", redisAsyncContext->errstr);
                redisAsyncFree(redisAsyncContext);
                continue;
            }
            token.redisAsyncContext = redisAsyncContext;

            const auto& base = event_base_new();
            redisLibeventAttach(redisAsyncContext, base);
            redisAsyncSetConnectCallback(redisAsyncContext, OnRedisConnected);
            redisAsyncSetDisconnectCallback(redisAsyncContext, OnRedisDisConnected);
            redisAsyncCommand(redisAsyncContext, OnRedisMessage, nullptr, "SUBSCRIBE %s", ChatMessageRedisChannel);
            event_base_dispatch(base);

            event_base_free(base);
            token.redisAsyncContext = nullptr;
            redisAsyncFree(redisAsyncContext);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error("redis thread error -> {}", error.what());
    }
    catch (const std::string& error)
    {
        Log::GetLog()->error("redis thread error -> {}", error);
    }
    catch (const char* error)
    {
        Log::GetLog()->error("redis thread error -> {}", error);
    }
    catch (...)
    {
        Log::GetLog()->error("redis thread unknown error");
    }
    Log::GetLog()->warn("redis thread is close!");
}
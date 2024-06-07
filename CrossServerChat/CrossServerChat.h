#pragma once

#include <winsock2.h>
#include <windows.h>

#include <API/ARK/Ark.h>
#include "CrossServerChatConfig.h"
#include "CrossServerChatRedis.h"

DECLARE_HOOK(AShooterPlayerController_ClientChatMessage, void, AShooterPlayerController*, const FChatMessage&);

inline bool HandlerPlayerMessage(AShooterPlayerController* _this, const FChatMessage& msg)
{
	const auto& token = CurrentPluginToken;
	const auto& config = CurrentPluginConfig;

	if (token.isWorking == false)
	{
#ifndef NDEBUG
		Log::GetLog()->info("HandlerPlayerMessage error! token.isWorking is false!");
#endif
		return true;
	}

	const auto& serverName = config.serverName;
	if (serverName.empty())
	{
		Log::GetLog()->warn("ChatMessageCallback error! config.ServerName.empty() is true!");
		return false;
	}

	const auto& senderSteamId = std::stoull(msg.UserId.ToString());
	const auto& playerConfig = config.playerConfig;
	
	if (const auto& playerIter = playerConfig.find(senderSteamId); playerIter != playerConfig.end())
	{
		if (playerIter->second.disabled)
		{
#ifndef NDEBUG
			Log::GetLog()->info("intercept player message, because playerConfig[steamId].disabled is true!");
#endif
			return true;
		}
	}
	else
	{
		const auto& defaultConfig = config.defaultConfig;
		auto disable = defaultConfig.disabled;

		if (usePermissions)
		{
			const auto& groupConfig = config.groupConfig;
			int currentPriority = 0;
			for (const auto& groupName : Permissions::GetPlayerGroups(senderSteamId))
			{
				const auto& key = groupName.ToString();
				if (const auto& groupIter = groupConfig.find(key); groupIter != groupConfig.end())
				{
					const auto& second = groupIter->second;
					if (second.priority > currentPriority)
					{
						currentPriority = second.priority;
						disable = second.disabled;
					}
				}
			}
		}
		if (disable)
		{
#ifndef NDEBUG
			Log::GetLog()->info("intercept player message, disable is true!");
#endif
			return true;
		}
	}

	if (msg.SendMode != EChatSendMode::Type::GlobalChat) return true;

	const auto& thisSteamId = ArkApi::IApiUtils::GetSteamIdFromController(_this);
	if (thisSteamId != senderSteamId)
	{
		return !config.replaceLocalMessage;
	}

	try {
		if (token.redisContext == nullptr || token.redisContext->err != REDIS_OK)
		{
			if (token.redisContext == nullptr)
			{
				Log::GetLog()->warn("HandlerPlayerMessage error! token.redisContext is null!");
			}
			else
			{
				Log::GetLog()->warn("HandlerPlayerMessage error! token.redisContext->err is {}!", token.redisContext->err);
			}
			return true;
		}

		const auto& playerState = _this->GetShooterPlayerState();
		const auto& tribeData = playerState->MyTribeDataField();
		const auto& tribeName = tribeData->TribeNameField();
		const auto& senderName = msg.SenderName;
		const auto& message = msg.Message;

		const auto& redisMessage = fmt::format("{}\r{}\r{}\r{}\r{}", serverName, senderSteamId, senderName.ToString(), tribeName.ToString(), message.ToString());
		const auto& reply = static_cast<redisReply*>(redisCommand(token.redisContext, "PUBLISH %s %b", ChatMessageRedisChannel, redisMessage.data(), redisMessage.size()));
		if (reply == nullptr)
		{
			Log::GetLog()->warn("redis publish error! reply is NULL");
		}
		else
		{
			if (reply->type == REDIS_REPLY_ERROR)
			{
				Log::GetLog()->warn("redis publish error! {}", reply->str);
			}
			freeReplyObject(reply);
		}
		return !config.replaceLocalMessage;
	}
	catch (const std::exception& error)
	{
		Log::GetLog()->error("PUBLISH error -> {}", error.what());
	}
	catch (const std::string& error)
	{
		Log::GetLog()->error("PUBLISH error -> {}", error);
	}
	catch (const char* error)
	{
		Log::GetLog()->error("PUBLISH error -> {}", error);
	}
	catch (...)
	{
		Log::GetLog()->error("PUBLISH unknown error");
	}
	return true;
}

inline void Hook_AShooterPlayerController_ClientChatMessage(AShooterPlayerController* _this, const FChatMessage& msg)
{
	if (_this == nullptr || msg.UserId.IsEmpty())
	{
		AShooterPlayerController_ClientChatMessage_original(_this, msg);
		return;
	}

	try
	{
		if (HandlerPlayerMessage(_this, msg))
		{
			AShooterPlayerController_ClientChatMessage_original(_this, msg);
		}
	}
	catch (const std::exception& error)
	{
		Log::GetLog()->error("{} error -> {}", __FUNCTION__, error.what());
	}
	catch (const std::string& error)
	{
		Log::GetLog()->error("{} error -> {}", __FUNCTION__, error);
	}
	catch (const char* error)
	{
		Log::GetLog()->error("{} error -> {}", __FUNCTION__, error);
	}
	catch (...)
	{
		Log::GetLog()->error("{} unknown error", __FUNCTION__);
	}
}

inline void OnReloadConsoleCommand(APlayerController* _this, FString*, bool)
{
	ReadConfig();

	if (_this)
	{
		const auto& playerController = static_cast<AShooterPlayerController*>(_this);
		ArkApi::GetApiUtils().SendServerMessage(playerController, FColorList::Green, "{} Plugin reloaded!", PROJECTNAME);
	}
}

inline void UnLoad()
{
	auto& token = CurrentPluginToken;
	token.isWorking = false;
	if (token.redisAsyncContext != nullptr)
	{
		redisAsyncFree(token.redisAsyncContext);
	}

	ArkApi::GetCommands().RemoveConsoleCommand(FString(fmt::format(L"{}.Reload", PROJECTNAME)));
	ArkApi::GetHooks().DisableHook("AShooterPlayerController.ClientChatMessage", &Hook_AShooterPlayerController_ClientChatMessage);
}

inline void Load()
{
	Log::Get().Init(PROJECTNAME);
	
	usePermissions = CheckPermissionsByPluginInfo();
	if (usePermissions)
	{
		Log::GetLog()->info("plugin will depend on Permissions!");
	}
	else
	{
		Log::GetLog()->info("plugin will not depend on Permissions!");
	}
	ReadConfig();

	CurrentPluginToken = PluginToken{true, nullptr, nullptr};
	std::thread thread(dispatch, std::ref(CurrentPluginToken));
	thread.detach();

	ArkApi::GetCommands().AddConsoleCommand(FString(fmt::format(L"{}.Reload", PROJECTNAME)), &OnReloadConsoleCommand);
	ArkApi::GetHooks().SetHook("AShooterPlayerController.ClientChatMessage", &Hook_AShooterPlayerController_ClientChatMessage, &AShooterPlayerController_ClientChatMessage_original);
}
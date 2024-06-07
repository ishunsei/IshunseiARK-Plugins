#pragma once

#include "API/Ark/Ark.h"
#include "Util.h"

inline nlohmann::json config;

inline void Suicide(AShooterPlayerController* _this, FString*, EChatSendMode::Type)
{
	if (!_this)
	{
		Log::GetLog()->error("{} function AShooterPlayerController* is nullptr!", __FUNCTION__);
		return;
	}

	const auto& playerCharacter = _this->GetPlayerCharacter();
	if (!playerCharacter)
	{
		SendUTF8ChatMessage(_this, config["CharacterDeathError"].get<std::string>());
		return;
	}
	
	if (playerCharacter->IsSitting(false))
	{
		SendUTF8ChatMessage(_this, config["CharacterSittingError"].get<std::string>());
	}
    else if (ArkApi::IApiUtils::IsRidingDino(_this))
	{
		SendUTF8ChatMessage(_this, config["CharacterRidingError"].get<std::string>());
	}
	else
	{
		const auto& successMessage = config["SuicideMessage"].get<std::string>();
		if (successMessage.find("{x}") != std::string::npos ||
			successMessage.find("{y}") != std::string::npos)
		{
			const auto& playerRelativeLocation = ArkApi::IApiUtils::GetPosition(_this);
			const auto& playerLocation = FVectorToCoords(playerRelativeLocation);
			
			auto replacedStr = successMessage;
			LoopReplace(replacedStr, "{x}", std::format("{:.2f}", playerLocation.x));
			LoopReplace(replacedStr, "{y}", std::format("{:.2f}", playerLocation.y));
			
			SendUTF8ChatMessage(_this, replacedStr);
		}
		else
		{
			SendUTF8ChatMessage(_this, successMessage);
		}
		_this->GetPlayerCharacter()->Suicide();
	}
}

inline void OnReloadConsoleCommand(APlayerController* _this, FString*, bool)
{
	if (config != nullptr)
	{
		for (auto& command : config.at("SuicideCommands"))
		{
			ArkApi::GetCommands().RemoveChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str());
		}
	}
	config = LoadConfig();
	
	for (auto& command : config.at("SuicideCommands"))
	{
		ArkApi::GetCommands().AddChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str(), &Suicide);
	}
	
	if (_this)
	{
		const auto& playerController = static_cast<AShooterPlayerController*>(_this);
		ArkApi::GetApiUtils().SendServerMessage(playerController, FColorList::Green, "{} Plugin reloaded!", PROJECTNAME);
	}
}

inline void UnLoad()
{
	if (config != nullptr)
	{
		for (auto& command : config.at("SuicideCommands"))
		{
			ArkApi::GetCommands().RemoveChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str());
		}
	}
	
	ArkApi::GetCommands().RemoveConsoleCommand(FString(fmt::format(L"{}.Reload", PROJECTNAME)));
}

inline void Load()
{
    Log::Get().Init(PROJECTNAME);
	config = LoadConfig();

	for (auto& command : config.at("SuicideCommands"))
	{
		ArkApi::GetCommands().AddChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str(), &Suicide);
	}
	
	ArkApi::GetCommands().AddConsoleCommand(FString(fmt::format(L"{}.Reload", PROJECTNAME)), &OnReloadConsoleCommand);
}
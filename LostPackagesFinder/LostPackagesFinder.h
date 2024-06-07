#pragma once

#include "API/Ark/Ark.h"
#include "Util.h"

inline nlohmann::json config;

inline void FindDeathItemCache(AShooterPlayerController* _this, FString*, EChatSendMode::Type)
{
    if (!_this)
    {
        Log::GetLog()->error("{} function AShooterPlayerController* is nullptr!", __FUNCTION__);
        return;
    }
    
    const auto& playerCharacter = _this->GetPlayerCharacter();
    if (playerCharacter == nullptr)
    {
        SendUTF8ChatMessage(_this, config["CharacterDeathError"].get<std::string>());
        return;
    }
    
    const auto& world = ArkApi::GetApiUtils().GetWorld();
    const auto& actors = world->PersistentLevelField()->GetActorsField();
    const auto& playerId = playerCharacter->LinkedPlayerDataIDField();

    auto foundCount = 0;
    const auto& packageFoundMessage = config["PackageFoundMessage"].get<std::string>();
    const auto& deadCharacterFoundMessage = config["DeadCharacterFoundMessage"].get<std::string>();
    const auto& packageNotFoundError = config["PackageNotFoundError"].get<std::string>();
    const auto& packageFoundTips = config["PackageFoundTips"].get<std::string>();
    for (AActor* actor : actors)
    {
        if (actor == nullptr) continue;
        if (actor->NameField().ToString().Contains("DeathItemCache_PlayerDeath_C"))
        {
            if (!actor->IsA(APrimalStructureItemContainer::GetPrivateStaticClass())) continue;
            
            const auto& structure = static_cast<APrimalStructureItemContainer*>(actor);
            if (structure->DeathCacheCharacterIDField() != playerId) continue;
            
            foundCount++;
            if (packageFoundMessage.find("{x}") != std::string::npos ||
                packageFoundMessage.find("{y}") != std::string::npos)
            {
                const auto& structureRelativeLocation = structure->RootComponentField()->RelativeLocationField();
                const auto& structureCoords = FVectorToCoords(structureRelativeLocation);

                auto replacedStr = packageFoundMessage;
                LoopReplace(replacedStr, "{x}", std::format("{:.2f}", structureCoords.x));
                LoopReplace(replacedStr, "{y}", std::format("{:.2f}", structureCoords.y));

                SendUTF8ChatMessage(_this, replacedStr);
            }
            else
            {
                SendUTF8ChatMessage(_this, packageFoundMessage);
            }
        }
        else if (actor->IsA(AShooterCharacter::GetPrivateStaticClass()))
        {
            const auto& findShooterCharacter = static_cast<AShooterCharacter*>(actor);
            if (!findShooterCharacter->IsDead() || findShooterCharacter->LinkedPlayerDataIDField() != playerId) continue;
            foundCount++;
            
            if (deadCharacterFoundMessage.find("{x}") != std::string::npos ||
                deadCharacterFoundMessage.find("{y}") != std::string::npos)
            {
                const auto& structureRelativeLocation = findShooterCharacter->RootComponentField()->RelativeLocationField();
                const auto& structureCoords = FVectorToCoords(structureRelativeLocation);

                auto replacedStr = deadCharacterFoundMessage;
                LoopReplace(replacedStr, "{x}", std::format("{:.2f}", structureCoords.x));
                LoopReplace(replacedStr, "{y}", std::format("{:.2f}", structureCoords.y));

                SendUTF8ChatMessage(_this, replacedStr);
            }
            else
            {
                SendUTF8ChatMessage(_this, deadCharacterFoundMessage);
            }
        }
    }

    if (foundCount > 0)
    {
        if (packageFoundTips.find("{}") != std::string::npos)
        {
            SendUTF8ChatMessage(_this, fmt::format(packageFoundTips, foundCount));
        }
        else
        {
            SendUTF8ChatMessage(_this, packageFoundTips);
        }
    }
    else
    {
        SendUTF8ChatMessage(_this, packageNotFoundError);
    }
}

inline void PullDeathItemCache(AShooterPlayerController* _this, FString*, EChatSendMode::Type)
{
    if (!_this)
    {
        Log::GetLog()->error("{} function AShooterPlayerController* is nullptr!", __FUNCTION__);
        return;
    }
    
    const auto& playerCharacter = _this->GetPlayerCharacter();
    if (playerCharacter == nullptr)
    {
        SendUTF8ChatMessage(_this, config["CharacterDeathError"].get<std::string>());
        return;
    }
    
    const auto& world = ArkApi::GetApiUtils().GetWorld();
    const auto& actors = world->PersistentLevelField()->GetActorsField();
    const auto& playerId = playerCharacter->LinkedPlayerDataIDField();
    
    auto foundCount = 0;
    const auto& packagePulledMessage = config["PackagePulledMessage"].get<std::string>();
    const auto& deadCharacterPulledMessage = config["DeadCharacterPulledMessage"].get<std::string>();
    const auto& packageNotFoundError = config["PackageNotFoundError"].get<std::string>();
    const auto& packageFoundTips = config["PackageFoundTips"].get<std::string>();

    for (AActor* actor : actors)
    {
        if (actor == nullptr) continue;
        if (actor->NameField().ToString().Contains("DeathItemCache_PlayerDeath_C"))
        {
            if (!actor->IsA(APrimalStructureItemContainer::GetPrivateStaticClass())) continue;
            
            const auto& structure = static_cast<APrimalStructureItemContainer*>(actor);
            if (structure->DeathCacheCharacterIDField() != playerId) continue;

            foundCount++;
            const auto& inventory = structure->MyInventoryComponentField();
            if (inventory == nullptr) continue;
            
            if (packagePulledMessage.find("{x}") != std::string::npos ||
                packagePulledMessage.find("{y}") != std::string::npos)
            {
                const auto& structureRelativeLocation = structure->RootComponentField()->RelativeLocationField();
                const auto& structureCoords = FVectorToCoords(structureRelativeLocation);

                auto replacedStr = packagePulledMessage;
                LoopReplace(replacedStr, "{x}", std::format("{:.2f}", structureCoords.x));
                LoopReplace(replacedStr, "{y}", std::format("{:.2f}", structureCoords.y));
                
                SendUTF8ChatMessage(_this, replacedStr);
            }
            else
            {
                SendUTF8ChatMessage(_this, packagePulledMessage);
            }
            
            auto playerRelativeLocation = ArkApi::IApiUtils::GetPosition(_this);
            APrimalStructureItemContainer* deathCache = nullptr;
            inventory->DropInventoryDeposit(ArkApi::GetApiUtils().GetWorld()->TimeSecondsField() + config["PackageSurvivalTime"].get<int>(),
                false, false, nullptr, nullptr,
                &deathCache, nullptr, FString(""), FString(""),
                -1, 300, true, -1,
                true, &playerRelativeLocation, true);
        }
        else if (actor->IsA(AShooterCharacter::GetPrivateStaticClass()))
        {
            const auto& findShooterCharacter = static_cast<AShooterCharacter*>(actor);
            if (!findShooterCharacter->IsDead() || findShooterCharacter->LinkedPlayerDataIDField() != playerId) continue;
            
            foundCount++;
            const auto& inventory = findShooterCharacter->MyInventoryComponentField();
            if (inventory == nullptr) continue;
            if (deadCharacterPulledMessage.find("{x}") != std::string::npos ||
                deadCharacterPulledMessage.find("{y}") != std::string::npos)
            {
                const auto& structureRelativeLocation = findShooterCharacter->RootComponentField()->RelativeLocationField();
                const auto& structureCoords = FVectorToCoords(structureRelativeLocation);

                auto replacedStr = deadCharacterPulledMessage;
                LoopReplace(replacedStr, "{x}", std::format("{:.2f}", structureCoords.x));
                LoopReplace(replacedStr, "{y}", std::format("{:.2f}", structureCoords.y));

                SendUTF8ChatMessage(_this, replacedStr);
            }
            else
            {
                SendUTF8ChatMessage(_this, deadCharacterPulledMessage);
            }
            auto playerRelativeLocation = ArkApi::IApiUtils::GetPosition(_this);

            for (const auto& items = inventory->InventoryItemsField();
                const auto& item : items)
            {
                const auto& slotIndex = item->SlotIndexField();
                if (slotIndex >= 0)
                {
                    item->SlotIndexField() = -1;
                    item->UpdatedItem(false);
                }
            }
            
            APrimalStructureItemContainer* deathCache = nullptr;
            inventory->DropInventoryDeposit(ArkApi::GetApiUtils().GetWorld()->TimeSecondsField() + 3600,
                false, false, nullptr, nullptr,
                &deathCache, nullptr, FString(""), FString(""),
                -1, 300, true, -1,
                true, &playerRelativeLocation, true);

            findShooterCharacter->Destroy(false, false);
        }
    }
    
    if (foundCount > 0)
    {
        if (packageFoundTips.find("{}") != std::string::npos)
        {
            SendUTF8ChatMessage(_this, fmt::format(packageFoundTips, foundCount));
        }
        else
        {
            SendUTF8ChatMessage(_this, packageFoundTips);
        }
    }
    else
    {
        SendUTF8ChatMessage(_this, packageNotFoundError);
    }
}

inline void OnReloadConsoleCommand(APlayerController* _this, FString*, bool)
{
    if (config != nullptr)
    {
        for (auto& command : config.at("FindCommands"))
        {
            ArkApi::GetCommands().RemoveChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str());
        }
        for (auto& command : config.at("PullCommands"))
        {
            ArkApi::GetCommands().RemoveChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str());
        }
    }
    config = LoadConfig();
	
    for (auto& command : config.at("FindCommands"))
    {
        ArkApi::GetCommands().AddChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str(), &FindDeathItemCache);
    }
    for (auto& command : config.at("PullCommands"))
    {
        ArkApi::GetCommands().AddChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str(), &PullDeathItemCache);
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
        for (auto& command : config.at("FindCommands"))
        {
            ArkApi::GetCommands().RemoveChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str());
        }
        for (auto& command : config.at("PullCommands"))
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

    for (auto& command : config.at("FindCommands"))
    {
        ArkApi::GetCommands().AddChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str(), &FindDeathItemCache);
    }
    for (auto& command : config.at("PullCommands"))
    {
        ArkApi::GetCommands().AddChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str(), &PullDeathItemCache);
    }
	
    ArkApi::GetCommands().AddConsoleCommand(FString(fmt::format(L"{}.Reload", PROJECTNAME)), &OnReloadConsoleCommand);
}
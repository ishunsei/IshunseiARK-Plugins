#pragma once

#include "API/Ark/Ark.h"
#include "Util.h"

inline nlohmann::json config;

struct DinoInfo
{
    int level;
    std::string biologyName;
    std::string dinoName;
    float distance;
    float x;
    float y;
};

inline void TrackDinosaurs(AShooterPlayerController* _this, const FString* msg, EChatSendMode::Type)
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
    
    const auto& playerPosition = ArkApi::IApiUtils::GetPosition(_this);

    TArray<FString> parameters;
    msg->ParseIntoArray(parameters, L" ", true);
    if (!parameters.IsValidIndex(1))
    {
	    SendUTF8ChatMessage(_this, config["FormatError"].get<std::string>());
        return;
    }

    TArray<FString> filters;
    parameters[1].ParseIntoArray(filters, L"|", true);
    
    const auto& teamId = _this->TargetingTeamField();
    TArray<DinoInfo> dinosaurs;
    TArray<AActor*> allDinosaurs;
    UGameplayStatics::GetAllActorsOfClass(ArkApi::GetApiUtils().GetWorld(), APrimalDinoCharacter::GetPrivateStaticClass(), &allDinosaurs);

    for (const auto& dinoActor : allDinosaurs)
    {
        if (!dinoActor || dinoActor->TargetingTeamField() != teamId) continue;
        
        const auto& dino = static_cast<APrimalDinoCharacter*>(dinoActor);

        FString dinoDescriptiveName;
        const auto& dinoDescriptiveNameLocalization = dino->GetDinoDescriptiveName(&dinoDescriptiveName);
        const auto& descriptiveNameString = dinoDescriptiveNameLocalization->ToString();
        const auto& end = descriptiveNameString.find_last_of(')');
        const auto& start = descriptiveNameString.find_last_of('(') + 1;
        const auto& status = dino->MyCharacterStatusComponentField();
        const auto& level = status->GetCharacterLevel();
        const auto& biologyName = descriptiveNameString.substr(start, end - start);
        
        FString currentDinoName;
        const auto& currentDinoNameLocalization = dino->GetCurrentDinoName(&currentDinoName, _this);

        const std::string levelString = std::to_string(level);
        
        bool isTarget = true;
        const int& filterSize = filters.Num();
        for (int i = 0; i < filterSize; i++)
        {
            const auto& word = filters[i].ToString();
            if (!currentDinoNameLocalization->Contains(filters[i]) &&
                biologyName.find(word) == std::string::npos &&
                levelString.find(word) == std::string::npos)
            {
                isTarget = false;
                break;
            }
        }
        if (!isTarget) continue;
        const auto& dinoPosition = dino->RootComponentField()->RelativeLocationField();
        const auto& dinoCoords = FVectorToCoords(dinoPosition);
        const auto& distance = FVector::Distance(playerPosition, dinoPosition);
        
        DinoInfo info;
        info.level = level;
        info.distance = distance;
        info.x = dinoCoords.x;
        info.y = dinoCoords.y;
        info.biologyName = biologyName;
        info.dinoName = currentDinoNameLocalization->ToString();
        
        dinosaurs.Add(info);
    }

    if (dinosaurs.Num() > 0)
    {
        dinosaurs.Sort([](const auto& dinoA, const auto& dinoB)
        {
           return dinoA.distance < dinoB.distance;
        });
    }
    else
    {
        SendUTF8ChatMessage(_this, config["DinoNotFoundError"].get<std::string>());
        return;
    }
    
    int currentPage = 0;
    try
    {
        if (parameters.IsValidIndex(2))
        {
            currentPage = std::stoi(parameters[2].ToString()) - 1;
        }
    } catch (...) {
    }

    const int& pageSize = config["PageSize"].get<int>();
    const int& allCount = dinosaurs.Num();

    std::stringstream result;
    const auto& title = config["Title"].get<std::string>();
    if (!title.empty())
    {
        result << title << '\n';
    }

    const auto& content = config["Content"].get<std::string>();
    const auto& distanceCoefficient = config["DistanceCoefficient"].get<float>();
    const auto& endIndex = min(allCount, (currentPage + 1) * pageSize);
    for (auto index = currentPage * pageSize; index < endIndex; index++)
    {
        const auto& dino = dinosaurs[index];
        auto currentContent = content;
        LoopReplace(currentContent, "{number}", std::format("{}", index + 1));
        LoopReplace(currentContent, "{name}", dino.dinoName);
        LoopReplace(currentContent, "{biologyName}", dino.biologyName);
        LoopReplace(currentContent, "{level}", std::format("{}", dino.level));
        LoopReplace(currentContent, "{y}", std::format("{:.2f}", dino.y));
        LoopReplace(currentContent, "{x}", std::format("{:.2f}", dino.x));
        LoopReplace(currentContent, "{distance}", std::format("{:.0f}", dino.distance / distanceCoefficient));
        
        result << currentContent << '\n';
    }
    
    const auto& footer = config["Footer"].get<std::string>();
    if (!footer.empty())
    {
        auto currentFooter = footer;
        LoopReplace(currentFooter, "{totalCount}", std::format("{}", allCount));
        LoopReplace(currentFooter, "{totalPage}", std::format("{}", ceil(static_cast<float>(allCount) / static_cast<float>(pageSize))));
        LoopReplace(currentFooter, "{currentPage}", std::format("{}", currentPage + 1));
        
        result << currentFooter;
    }

    const auto& notificationMessage = FString(ArkApi::Tools::Utf8Decode(result.str()));
    const auto& notificationIcon = config["NotificationIcon"].get<std::string>();
    
    ArkApi::GetApiUtils().SendNotification(_this, FColorList::Green,
        config["DisplayScale"].get<float>(),
        config["DisplayTime"].get<float>(),
        notificationIcon.empty() ? nullptr :
            reinterpret_cast<UTexture2D*>(Globals::StaticLoadObject(UTexture2D::StaticClass(),
                nullptr,ArkApi::Tools::Utf8Decode(notificationIcon).c_str(),
                nullptr, 0, 0, true)),
        *notificationMessage);
}

inline void OnReloadConsoleCommand(APlayerController* _this, FString*, bool)
{
    if (config != nullptr)
    {
        for (auto& command : config.at("TraceCommands"))
        {
            ArkApi::GetCommands().RemoveChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str());
        }
    }
    config = LoadConfig();
	
    for (auto& command : config.at("TraceCommands"))
    {
        ArkApi::GetCommands().AddChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str(), &TrackDinosaurs);
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
        for (auto& command : config.at("TraceCommands"))
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
    for (auto& command : config.at("TraceCommands"))
    {
        ArkApi::GetCommands().AddChatCommand(ArkApi::Tools::Utf8Decode(command.get<std::string>()).c_str(), &TrackDinosaurs);
    }
	
    ArkApi::GetCommands().AddConsoleCommand(FString(fmt::format(L"{}.Reload", PROJECTNAME)), &OnReloadConsoleCommand);
}
#pragma once

#include <string>
#include "API/Ark/Ark.h"
#include <fstream>
#include <json.hpp>

inline void LoopReplace(std::string& str, const std::string& target, const std::string& replacement)
{
    size_t pos = str.find(target);
    while (pos != std::string::npos) {
        str.replace(pos, target.length(), replacement);
        pos = str.find(target, pos + replacement.length());
    }
}

inline ArkApi::MapCoords FVectorToCoords(FVector actor_position)
{
    AWorldSettings* world_settings = ArkApi::GetApiUtils().GetWorld()->GetWorldSettings(false, true);
    APrimalWorldSettings* p_world_settings = static_cast<APrimalWorldSettings*>(world_settings);
    ArkApi::MapCoords coords;

    float lat_scale = p_world_settings->LatitudeScaleField() != 0 ? p_world_settings->LatitudeScaleField() : 800.0f;
    float lon_scale = p_world_settings->LongitudeScaleField() != 0 ? p_world_settings->LongitudeScaleField() : 800.0f;

    float lat_origin = p_world_settings->LatitudeOriginField() != 0 ? p_world_settings->LatitudeOriginField() : -400000.0f;
    float lon_origin = p_world_settings->LongitudeOriginField() != 0 ? p_world_settings->LongitudeOriginField() : -400000.0f;

    float lat_div = 100.f / lat_scale;
    float lat = (lat_div * actor_position.Y + lat_div * abs(lat_origin)) / 1000.f;

    float lon_div = 100.f / lon_scale;
    float lon = (lon_div * actor_position.X + lon_div * abs(lon_origin)) / 1000.f;

    coords.x = std::floor(lon * 100.0f) / 100.0f;
    coords.y = std::floor(lat * 100.0f) / 100.0f;

    return coords;
}

inline nlohmann::json LoadConfig()
{
    const std::string config_path = fmt::format("{}/ArkApi/Plugins/{}/config.json", ArkApi::Tools::GetCurrentDir(), PROJECTNAME);
    std::ifstream file{config_path};
    if (!file.is_open()) throw std::runtime_error(fmt::format("Can't open config.json from {}", config_path));
    const auto& result = nlohmann::json::parse(file);
    file.close();
    return result;
}

inline void SendChatMessage(AShooterPlayerController* player_controller, FString message)
{
    FChatMessage chat_message = FChatMessage();
    chat_message.SenderName = PROJECTNAME;
    chat_message.Message = message;
    player_controller->ClientChatMessage(chat_message);
}

inline void SendUTF8ChatMessage(AShooterPlayerController* player_controller, std::string message)
{
    FChatMessage chat_message = FChatMessage();
    chat_message.SenderName = PROJECTNAME;
    chat_message.Message = ArkApi::Tools::Utf8Decode(message).c_str();
    player_controller->ClientChatMessage(chat_message);

}
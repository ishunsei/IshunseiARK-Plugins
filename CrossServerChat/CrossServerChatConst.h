#pragma once

#define ChatMessageRedisChannel "ARK:ChatMessage"

#include <hiredis/async.h>
#include "API/UE/BasicTypes.h"

using std::string;
using std::map;

struct CrossServerChatConfig
{
    string name;
    string icon;
    string message;
    bool richText;
    bool disabled;
};

struct CrossServerChatGroupConfig
{
    string name;
    string icon;
    string message;
    bool richText;
    int priority;
    bool disabled;
};

struct PluginConfig
{
    bool replaceLocalMessage;
    string serverName;
    std::map<std::string, std::string> emojiEscape;
    CrossServerChatConfig defaultConfig;
    map<uint64, CrossServerChatConfig> playerConfig;
    map<string, CrossServerChatGroupConfig> groupConfig;
} inline CurrentPluginConfig;

struct PluginToken
{
    bool isWorking;
    redisAsyncContext* redisAsyncContext;
    redisContext* redisContext;
} inline CurrentPluginToken;

inline bool usePermissions;
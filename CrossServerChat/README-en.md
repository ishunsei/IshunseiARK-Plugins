<h1 align="center"> CrossServerChat </h1>

<a href="README-cn.md">中文</a>

A server message synchronization plugin for [ARK: Survival Evolved](https://store.steampowered.com/app/346110/ARK_Survival_Evolved/) based on Redis, with the following features compared to other plugins:
- Supports colored text with emojis
- Supports muting specific users, user groups, or all users on the server. Muted users can only see their own messages.
- No need for complex configuration, can easily share the same configuration across all servers, reducing maintenance costs.
- Naturally supports third-party plugins, just execute `PUBLISH ARK:ChatMessage "ServerName\rSteamId\rPlayerName\rTribeName\rPlayerMessage"` in Redis to send a cross-server message, for example: `PUBLISH ARK:ChatMessage "01Island\r0\rishunsei\rishunsei's Tribe\rHelloWorld"`.

# Special Notes
- This plugin depends on Redis, please install Redis yourself before using it.
- This plugin does **not support** configuring Redis connection information, it will always use `127.0.0.1:6379` to connect. If you need to customize the connection information, please modify the relevant code and recompile the plugin.
**Note: Exposing the Redis port to the public network may cause security issues, so it is not recommended to open port 6379 in the firewall.**

- The [Permissions](https://gameservershub.com/forums/resources/ark-permissions.20/) plugin from `GameServerHub` is an **optional feature** of this plugin. You can choose whether to enable support for it through configuration, but you need to install the Permissions plugin beforehand.
- Please use the **Debug** version for the first-time use, modify the configurations in the template according to your needs, and then replace the other files with the **Release** version after ensuring that the configuration is fully consistent with your requirements.

# Usage

## Self-compiling
- Download the source code and compile the CrossServerChat project (you can replace the Version project to update the [ArkServerApi](https://gameservershub.com/forums/resources/ark-server-api.12/) if needed).
- Place all the compiled files in the server's plugin directory.
- Modify the configuration file.
- Put the event.dll and hiredis.dll files in the ShooterGame\Binaries\Win64 directory.
Start the server.

## Using the published files
- Download the latest released version resources and copy the entire CrossServerChat folder to the server's plugin directory.
- Modify the configuration file.
- Put the event.dll and hiredis.dll files from the CrossServerChat folder in the ShooterGame\Binaries\Win64 directory.
- Start the server.

The final correct file directory should look like this (bold for new files, other plugins can be analogized):
- ShooterGame
  - Binaries
    - Win64
      - **event.dll**
      - **hiredis.dll**
      - ArkApi
        - Plugins
          - **CrossServerChat**
            - **config.json**
            - **CrossServerChat.dll**
            - **CrossServerChat.exp**
            - **CrossServerChat.lib**
            - **CrossServerChat.pdb**
            - **PluginInfo.json**

# How the Plugin Works
- The plugin will connect to Redis when it loads. If the Redis connection fails or is disconnected during runtime, the plugin will retry every 1 second until the connection is successful.
 - If `ReplaceLocalMessage` is set to `true` in **config.json**, all messages will fail to be sent when the Redis connection is disconnected.
 - If `ReplaceLocalMessage` is set to `false` in **config.json**, messages within the same server will not be affected, but cross-server message synchronization will fail.
- The plugin will distinguish messages from the same server based on **the final read server name**. If there are multiple server configurations with the same server name, they will be treated as messages from the same server.
- All players will be matched against the configurations in **config.json** in the following order:
 - If the player's SteamId is found in the **Players section**, their configuration will be used directly. Otherwise, it will continue to the next match.
 - If `Support for Permissions Plugin is enabled`, the player's permissions will be matched against the `Groups` configurations, with the configurations at the bottom having higher priority. If no match is found, it will continue to the next match.
- The `Default` configuration will be used if no other match is found.

The following is a detailed explanation of the `config.json` configuration:

```json
{
  // Replace local messages
  // When set to true, the original white local message will be blocked, and the configured text format will replace it.
  // Configuring this value will cause the same-server messages to not be displayed when the Redis connection is abnormal.
  // When set to false, the plugin will not intervene in the display of the same-server messages; when configuring this value, the same-server will not display the configured message format.
  "ReplaceLocalMessage": false,

  // Server name, setting this item will start the server using this name.
  // If this item is empty or does not exist, the configuration name in ArkServerManager will be extracted.
  // If neither is found, the server name will be blank, and the plugin will be invalid (equivalent to the state of the plugin not being installed).
  // It is recommended to leave it blank and use the configuration name in ArkServerManager, so that you can avoid the step of configuring each server separately.
  "ServerName": "Test",

  // Text emoticon escape
  // The key is the emoticon symbol, and the value is the emoticon code. This configuration is used to fix the bug that the emoticon will be invalid when the message format contains colors.
  // If ARK official does not add or remove text emoticons, this item can be left unchanged.
  "EmojiEscape": {
    ">:)": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Evil.Emo_Evil\"/>",
    ":)": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Smile.Emo_Smile\"/>",
    ";)": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Wink.Emo_Wink\"/>",
    ":D": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Laugh.Emo_Laugh\"/>",
    ":d": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Laugh.Emo_Laugh\"/>",
    ":(": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Sad.Emo_Sad\"/>",
    ":|": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Blank.Emo_Blank\"/>",
    ":O": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Eyes.Emo_Eyes\"/>",
    ":o": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Eyes.Emo_Eyes\"/>",
    ":P": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Tongue.Emo_Tongue\"/>",
    ":p": "<img src=\"Chat.Image\" path=\"/Game/PrimalEarth/Emo/Emo_Tongue.Emo_Tongue\"/>"
  },

  // Default configuration
  // If the player cannot find a matching configuration in other items, this configuration will be directly adopted.
  "Default": {
    
    // Player title, the final format will be <server name>player name(player title)[player tribe]:player's message format
    "Name": "Default",

    // Player's icon
    "Icon": "",

    // Player's message format, the final player message will be replaced by the content in the curly braces, please make sure the format is correct and includes curly braces;
    "Message": "<RichColor Color=\"0,225,225,1\">{}</>",

    // Whether to disable the chat function, when this item is set to true, the player will not be able to send messages; can be used as a ban function;
    "Disabled": false
  },

  // Permission group configuration
  // This item will only be enabled when the [PluginInfo.json] -> Dependencies in the configuration plugin directory includes Permissions, otherwise this item will be ignored.
  // This item needs to pay attention to the order of configuration, the lower the configuration, the higher the priority, and the highest priority configuration will be used when matching the player.
  // Note: Configuring the above content means that you need to enable support for the GameServerHub permission plugin, so please be sure to install the permission plugin.
  "Groups": [
    {
      // Permission group name, the player will be matched based on the permissions they hold, and the match will be successful when they have the permission with this name.
      "GroupName": "Default",
      "Name": "",
      "Icon": "/Game/PrimalEarth/UI/Textures/CraftingBrokenIcon.CraftingBrokenIcon",
      "Message": "<RichColor Color=\"0,225,225,1\">{}</>",
      "Disabled": true
    },
    {
      "GroupName": "VIPX",
      "Name": "Sponsor",
      "Icon": "/Game/PrimalEarth/UI/Textures/CraftingBrokenIcon.CraftingBrokenIcon",
      "Message": "{}",
      "Disabled": false
    },
    {
      "GroupName": "VIP1",
      "Name": "VIP1",
      "Icon": "/Game/PrimalEarth/UI/Textures/CraftingBrokenIcon.CraftingBrokenIcon",
      "Message": "{}",
      "Disabled": false
    }
  ],

  // Player configuration
  // Here you can configure the format for a specific player, this item has the highest priority, and when the player finds the corresponding configuration here, that configuration will be used directly.
  "Players": [
    {
      // Player's SteamId
      // Note: Do not configure duplicate SteamIds, as the program may malfunction if you do.
      "SteamId": 76561198323369576,
      "Name": "ishunsei",
      "Icon": "/Game/PrimalEarth/UI/Textures/CraftingBrokenIcon.CraftingBrokenIcon",
      "Message": "<RichColor Color=\"0,225,225,1\">{}</>",
      "Disabled": false
    }
  ]
}
```
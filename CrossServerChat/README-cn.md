<h1 align="center"> CrossServerChat </h1>

<a href="README-en.md">English</a>

基于Redis的[《方舟：生存进化》](https://store.steampowered.com/app/346110/ARK_Survival_Evolved/) 服务器消息同步插件，相比于其他插件，此插件有以下几个特点
- 支持在彩色文字中带有表情
- 支持对某个用户、某类用户、全服用户禁言,被禁言的用户发言只有自己能看见
- 无需繁琐的配置，可以轻松实现所有服务器共享同一份配置，降低维护成本
- 天然支持第三方插件，仅需在Redis中执行 `PUBLISH ARK:ChatMessage "服务器名称\rSteamId\r玩家名称\r玩家部落\r玩家消息"`即可实现第三方插件发送跨服消息，例如 `PUBLISH ARK:ChatMessage "01孤岛\r0\r李舜生\r李舜生的部落\rHelloWorld"`

# 特别说明
- 此插件将依赖Redis，使用前请自行安装Redis
- 此插件**不支持**配置Redis链接信息，链接时将会固定使用 `127.0.0.1:6379` 进行链接，如需自定义链接信息请自行修改相关代码并重新编译
**注意: 将Redis端口对外网公开将会引发安全问题,所以不建议在防火墙中打开6379端口**
- `GameServerHub` 的 [Permissions](https://gameservershub.com/forums/resources/ark-permissions.20/) 插件是本插件的**可选项**，通过配置可以通过配置选择是否启用对此插件的支持，**启用支持需要提前安装 [Permissions](https://gameservershub.com/forums/resources/ark-permissions.20/) 插件**
- 初次使用请先使用 **Debug** 版本测试，按照需求修改模板中的配置，确保配置与需求完全一致后保留配置文件并将其他文件更换为 **Release** 版本

# 使用方法

## 自编译
- 下载源码并编译CrossServerChat工程 (有需要可自行替换Version工程以更新[ArkServerApi](https://gameservershub.com/forums/resources/ark-server-api.12/))
- 将编译生成好的所有文件放入服务器插件目录中
- 修改配置文件
- 将event.dll与hiredis.dll放入ShooterGame\Binaries\Win64目录中
- 启动服务器即可

## 使用发布文件
- 下载最新发布版本资源并将CrossServerChat文件夹完整拷贝到服务器插件目录
- 修改配置文件
- 将CrossServerChat文件夹中的event.dll与hiredis.dll放入ShooterGame\Binaries\Win64目录中
- 启动服务器即可

最终正确的文件目录应该是这样的 (粗体为新增文件)
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

# 插件的工作方式
- 此插件将在加载时链接Redis，若Redis链接失败或在运行中Redis链接断开，插件将会在`1秒`后重试，直到链接成功为止
  - 若`config.json`中`ReplaceLocalMessage`的配置为`true`，则redis链接断开时所有消息都无法成功发送
  - 若`config.json`中`ReplaceLocalMessage`的配置为`false`，则redis链接断开时同服中的消息将不会被影响，跨服消息同步将失效
- 插件将根据**最终读取到的服务器名称**来区分同服的消息，如果同时存在多个服务器配置的服务器名称完全一致，他们将会视作同服消息处理
- 所有玩家将按照`config.json`进行匹配对应的配置，匹配顺序如下
  - 若`Players`中找到此`SteamId`的配置，将直接采用此配置，否则继续往下匹配
  - 若`启用对权限插件的支持`则根据玩家所拥有的权限进行匹配`Groups`，配置的越靠后优先级越高，若未找到匹配项则继续往下匹配
  - 使用`Default`中的配置

以下为`config.json`配置详解
```json
{
  // 替换本地消息
  // 当设置为true时，原白色本地消息将会被屏蔽，取而代之的将是配置的文本格式
  //                  配置此值在Redis链接异常时会导致同服消息也会无法显示
  // 当设置为false时，插件将不会干预同服的消息显示;配置此值时同服将不会显示配置的消息格式
  "ReplaceLocalMessage": false,

  // 服务器名称，设置此项将会使用此名称启动服务器
  // 若此项为空或不存在此项时将会提取ArkServerManager中的配置名称
  // 若均未找到则服务器名称将会是空白，插件将会失效（等同于未安装插件的状态）
  // 推荐留空使用ArkServerManager中的配置名称，这样可以省去给每个服务器单独配置的步骤
  "ServerName": "测试",

  // 文字表情转义
  // key为表情符号，value为表情代码，此配置用于修复消息格式带有颜色时表情会失效的bug
  // 若方舟官方未添加或删除文字表情，此项可以不做修改
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

  // 默认配置
  // 若玩家在其他项找不到与其匹配的配置时，将直接采用此配置
  "Default": {

    // 玩家头衔，最终格式将会是 <服务器名称>玩家名称(玩家头衔)[玩家部落]:玩家的消息格式
    "Name": "默认",

    // 玩家的图标
    "Icon": "",

    // 玩家的消息格式，最终的玩家消息将会填入花括号中代替，请务必保证格式正确且包含花括号;
    "Message": "<RichColor Color=\"0,225,225,1\">{}</>",

    // 是否禁用聊天功能，此项设置为true时玩家将无法发送消息;可用做禁言功能;
    "Disabled": false
  },

  // 权限组配置
  // 仅当配置插件目录下[PluginInfo.json]->Dependencies中带有Permissions时此项将会启用,否则将会忽略此项
  // 此项需要关注配置顺序，越往下的配置优先级越高，在匹配玩家时将会使用优先级最高的配置
  // 注意:配置上述内容时意味着您需要启用对GameServerHub权限插件的支持，所以请务必安装权限插件
  "Groups": [
    {
      // 权限组名称，玩家将会根据所持有的权限匹配此项，当拥有该名称的权限时将会匹配成功
      "GroupName": "Default",
      "Name": "",
      "Icon": "/Game/PrimalEarth/UI/Textures/CraftingBrokenIcon.CraftingBrokenIcon",
      "Message": "<RichColor Color=\"0,225,225,1\">{}</>",
      "Disabled": true
    },
    {
      "GroupName": "VIPX",
      "Name": "金主",
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

  // 玩家配置
  // 此处可单独配置某个玩家的格式，此项的优先级是最高的，当玩家在此处找到了对应的配置，将会直接使用该配置
  "Players": [
    {
      // 玩家的SteamId
      // 注意:请勿配置重复的SteamId，若配置重复的SteamId程序可能会发生异常
      "SteamId": 76561198323369576,
      "Name": "李舜生",
      "Icon": "/Game/PrimalEarth/UI/Textures/CraftingBrokenIcon.CraftingBrokenIcon",
      "Message": "<RichColor Color=\"0,225,225,1\">{}</>",
      "Disabled": false
    }
  ]
}
```
<h1 align="center"> 李舜生的方舟生存进化插件集合 </h1>

<a href="README-en.md">English</a>

本仓库为李舜生开发的[《方舟：生存进化》](https://store.steampowered.com/app/346110/ARK_Survival_Evolved/)的服务器插件集合，包含以下几个插件（点击对应插件名跳转至对应插件介绍）

- 基于Redis的跨服聊天插件 (<a href="CrossServerChat/README-cn.md">CrossServerChat</a>)
- 自杀插件 (<a href="Suicide/README-cn.md">Suicide</a>)
- 找包插件 (<a href="LostPackagesFinder/README-cn.md">LostPackagesFinder</a>)
- 寻龙插件 (<a href="DinosaurTracker/README-cn.md">CrossServerChat</a>)

如插件详情页中未出现与本说明重复内容，则以本说明为准。

# 特别说明
- 如果您对插件有任何疑问，欢迎提交 <a href="issues">issues</a>
- 所有插件基于 [GameServerHub](https://gameservershub.com/) 的 [ArkServerApi](https://gameservershub.com/forums/resources/ark-server-api.12/) 并 **均支持文字提示最大程度自定义**
- 所有插件不带有任何的广告及后门
- 目前插件仅实现了WIN64平台，其他平台请自行修改代码编译
- 所有插件均完美支持 `ArkServerApi` 的**热加载**、**热卸载**、**热重载**，通过这两个命令可以在不关闭服务器的情况下卸载、安装、更新此插件
  - 卸载命令为 `cheat plugins.unload 插件名`
  - 加载命令为 `cheat plugins.load 插件名`
  - 重载命令为 `cheat 插件名.Reload` 重新加载插件的 **config.json** 配置文件
- 所有插件不允许商用！！！
- 所有插件不允许商用！！！
- 所有插件不允许商用！！！


# 使用方法
- 注意! 跨服聊天插件需要额外放置其他文件，具体请看对应插件介绍页！
- 注意! 跨服聊天插件需要额外放置其他文件，具体请看对应插件介绍页！
- 注意! 跨服聊天插件需要额外放置其他文件，具体请看对应插件介绍页！

## 自编译
- 下载源码并编译需要的插件工程 (有需要可自行替换Version工程以更新[ArkServerApi](https://gameservershub.com/forums/resources/ark-server-api.12/))
- 将编译生成好的插件文件放入服务器插件目录中
- 修改配置文件
- 启动服务器即可

## 使用已发布文件
- 下载最新发布版本资源并将需要的插件文件夹完整拷贝到服务器插件目录
- 修改配置文件
- 启动服务器即可

最终正确的文件目录应该是这样的 (粗体为新增文件，其他插件请举一反三)
- ShooterGame
  - Binaries
    - Win64
      - ArkApi
        - Plugins
          - **LostPackagesFinder**
            - **config.json**
            - **LostPackagesFinder.dll**
            - **LostPackagesFinder.exp**
            - **LostPackagesFinder.lib**
            - **LostPackagesFinder.pdb**
            - **PluginInfo.json**

# License
[GPL License](https://opensource.org/license/gpl-3-0)
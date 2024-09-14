<h1 align="center"> ishunsei's ARK: Survival Evolved Plugin Collection </h1>

<a href="README-cn.md">中文</a>

This repository contains a collection of server plugins developed by ishunsei for the game [ARK: Survival Evolved](https://store.steampowered.com/app/346110/ARK_Survival_Evolved/). The following plugins are included (click on the plugin name to go to the corresponding plugin introduction):

- Cross-Server Chat Plugin based on Redis (<a href="CrossServerChat/README-en.md">CrossServerChat</a>)
- Suicide Plugin (<a href="Suicide/README-en.md">Suicide</a>)
- Lost Packages Finder Plugin (<a href="LostPackagesFinder/README-en.md">LostPackagesFinder</a>)
- Dinosaur Tracker Plugin (<a href="DinosaurTracker/README-en.md">DinosaurTracker</a>)

If there is no content in the plugin details page that repeats this explanation, then this explanation takes precedence.

# Special Notes
- If you have any questions about the plugin, feel free to submit <a href="issues">issues</a>.
- All plugins are based on the [ArkServerApi](https://gameservershub.com/forums/resources/ark-server-api.12/) from [GameServerHub](https://gameservershub.com/) and **support maximum customization of text prompts**.
- All plugins do not contain any advertisements or backdoors.
- Currently, the plugins are only implemented for the WIN64 platform, and other platforms need to be compiled manually.
- All plugins fully support **hot loading**, **hot unloading**, and **hot reloading** of the `ArkServerApi`, allowing you to install, uninstall, and update the plugins without restarting the server.
  - Unload command `cheat plugins.unload [PluginName]`
  - Load command `cheat plugins.load [PluginName]`
  - Reload command `cheat [PluginName].Reload`.Reload to reload the **config.json** configuration file
- All plugins are not allowed to be used commercially!!!
- All plugins are not allowed to be used commercially!!!
- All plugins are not allowed to be used commercially!!!


# Usage
- Note! The Cross-Server Chat plugin requires additional files to be placed, please check the corresponding plugin introduction page for details!
- Note! The Cross-Server Chat plugin requires additional files to be placed, please check the corresponding plugin introduction page for details!
- Note! The Cross-Server Chat plugin requires additional files to be placed, please check the corresponding plugin introduction page for details!

## Self-Compilation
- Download the source code and compile the required plugin projects (you can replace the Version project to update the [ArkServerApi](https://gameservershub.com/forums/resources/ark-server-api.12/) if needed)
- Place the compiled plugin files in the server plugin directory
- Modify the configuration file
- Start the server

## Use Published Files
- Download the latest released resources and copy the required plugin folders completely to the server plugin directory
- Modify the configuration file
- Start the server

The final correct file directory should look like this (bold for new files, other plugins can be analogized):
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
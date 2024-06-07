<h1 align="center"> LostPackagesFinder </h1>

<a href="README-cn.md">中文</a>

[ARK: Survival Evolved](https://store.steampowered.com/app/346110/ARK_Survival_Evolved/) Lost Packages Finder Plugin

# Special Instructions
- The **PackageSurvivalTime** in the **config.json** file of the plugin is the existence time (in seconds) for the retrieved packages.
- This plugin has two types of commands:
 - Find Package: This command will output the location of the package created by death to the chat window, and can be used repeatedly until the package is retrieved or picked up.
 - Pull Package: This command will immediately move the package created by death to the player's location (regardless of whether the corpse has been destroyed), and the retrieved package will have a **PackageSurvivalTime** existence time. If the package has been retrieved before, it can no longer be identified through **Pull** or **Find**.
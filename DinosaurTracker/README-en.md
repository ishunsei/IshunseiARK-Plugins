<h1 align="center"> DinosaurTracker </h1>

<a href="README-cn.md">中文</a>

[ARK: Survival Evolved](https://store.steampowered.com/app/346110/ARK_Survival_Evolved/) Dragon Finder Plugin, This plugin has the following features:
- Supports searching for dinosaurs using multiple compound conditions.

# Special Instructions
- The **DistanceCoefficient** in the **config.json** file of the plugin is a distance multiplier, as the distance values in the game are quite large and have no units, so a coefficient is used to calculate a suitable distance value.
- The **DisplayTime** in the **config.json** file of the plugin is the duration (in seconds) for displaying the notification text.
- The **DisplayScale** in the **config.json** file of the plugin is the scaling factor for the size of the notification text.
- The purpose of the **NotificationIcon** in the **config.json** file of the plugin is currently unknown.
- The **PageSize** in the **config.json** file of the plugin is the number of dinosaurs to display per page in the dragon finding feature.

# How the Plugin Works
- [Find Dragon Command] Rex|246|MyRex [Page Number]
The above command will perform a compound search for dinosaurs that have `name, category, or level containing "Rex"`, and `name, category, or level containing "246"`, and `name, category, or level containing "MyRex"`.
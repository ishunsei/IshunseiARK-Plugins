<h1 align="center"> DinosaurTracker </h1>

<a href="README-en.md">English</a>

[《方舟：生存进化》](https://store.steampowered.com/app/346110/ARK_Survival_Evolved/) 找龙插件，此插件有以下几个特点
- 支持使用多重复合条件搜索恐龙

# 特别说明
- 插件配置 **config.json** 的 **DistanceCoefficient** 为距离乘数，因游戏中的距离数值较大且无单位，故使用一个系数进行乘算得到一个适合的距离值
- 插件配置 **config.json** 的 **DisplayTime** 为提示文字展示的时长（秒）
- 插件配置 **config.json** 的 **DisplayScale** 为提示文字展示的大小倍率
- 插件配置 **config.json** 的 **NotificationIcon** 暂时未知作用
- 插件配置 **config.json** 的 **PageSize** 为找龙每页展示的数量

# 插件的工作方式
- [找龙命令] 霸王龙|246|小霸 [页码]
以上命令将会复合搜索三组条件，即：`名称或类别或等级包含霸王龙` 且 `名称或类别或等级包含246` 且 `名称或类别或等级包含小霸` 的恐龙。
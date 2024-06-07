<h1 align="center"> LostPackagesFinder </h1>

<a href="README-en.md">English</a>

[《方舟：生存进化》](https://store.steampowered.com/app/346110/ARK_Survival_Evolved/) 找包插件

# 特别说明
- 插件配置 **config.json** 的 **PackageSurvivalTime** 为被拉取包裹的存在时间（秒）
- 此插件有两种类型的命令
 - 寻找包裹：将死亡产生的包裹位置输出到聊天窗口，此命令可重复使用，直至包裹被拉取或被拾取。
 - 拉取包裹：将死亡产生的包裹立即移动到玩家位置（无论尸体是否已被销毁），被移动的包裹将会有 **PackageSurvivalTime** 的存在时间，且若包裹被拉取过则无法再通过 **拉取** 或 **寻找** 识别。
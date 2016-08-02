#ifndef _MESSAGEDEF_H_
#define _MESSAGEDEF_H_
#pragma once
//该文件为使用工具自动生成，请不要在文件中修改，如需要修改，请与sskylin联系。
enum S2C_EVENT
{
	S2C_EVENT_BASE = 511,
	S2C_RseAuthState,		//^s^RseAuthState#
	S2C_RseCustomizedActivity,		//^s^RseCustomizedActivity#
	S2C_RseDeployUnits,		//^s^RseDeployUnits#
	S2C_RseError,		//^s^RseError#
	S2C_RseHeartbeat,		//^s^RseHeartbeat#
	S2C_RseItemDamaged,		//^s^RseItemDamaged#
	S2C_RseLogin,		//^s^RseLogin#
	S2C_RseObtainAskForHelp,		//^s^RseObtainAskForHelp#
	S2C_RseObtainAttackerList,		//^s^RseObtainAttackerList#
	S2C_RseObtainBattleReplay,		//^s^RseObtainBattleReplay#
	S2C_RseObtainCustomizer,		//^s^RseObtainCustomizer#
	S2C_RseObtainHangarsHelp,		//^s^RseObtainHangarsHelp#
	S2C_RseObtainMessagesList,		//^s^RseObtainMessagesList#
	S2C_RseObtainNeighborsList,		//^s^RseObtainNeighborsList#
	S2C_RseObtainNpcList,		//^s^RseObtainNpcList#
	S2C_RseObtainPayUrlParame,		//^s^RseObtainPayUrlParame#
	S2C_RseObtainRandomTargets,		//^s^RseObtainRandomTargets#
	S2C_RseObtainSocialItems,		//^s^RseObtainSocialItems#
	S2C_RseObtainSocialUserInfo,		//^s^RseObtainSocialUserInfo#
	S2C_RseObtainUniverse,		//^s^RseObtainUniverse#
	S2C_RseObtainVisitHelps,		//^s^RseObtainVisitHelps#
	S2C_RseQueryCheckAndLockAccountIfPossible,		//^s^RseQueryCheckAndLockAccountIfPossible#
	S2C_RseQueryGalaxyWindow,		//^s^RseQueryGalaxyWindow#
	S2C_RseQueryReceiveWishItem,		//^s^RseQueryReceiveWishItem#
	S2C_RseQueryRefuseReceiveWishItem,		//^s^RseQueryRefuseReceiveWishItem#
	S2C_RseQuerySendWishItem,		//^s^RseQuerySendWishItem#
	S2C_RseQueryStarInfo,		//^s^RseQueryStarInfo#
	S2C_RseQueryStarsBookmarks,		//^s^RseQueryStarsBookmarks#
	S2C_RseQueryVisitHelpsGiftUnitsOnBunker,		//^s^RseQueryVisitHelpsGiftUnitsOnBunker#
	S2C_RseUnitDamaged,		//^s^RseUnitDamaged#
	S2C_RseUpdateAlliances,		//^s^RseUpdateAlliances#
	S2C_RseUpdateGameUnits,		//^s^RseUpdateGameUnits#
	S2C_RseUpdateItem,		//^s^RseUpdateItem#
	S2C_RseUpdateMisc,		//^s^RseUpdateMisc#
	S2C_RseUpdateMissions,		//^s^RseUpdateMissions#
	S2C_RseUpdateProfile,		//^s^RseUpdateProfile#
	S2C_RseUpdateShips,		//^s^RseUpdateShips#
	S2C_RseUpdateSocialItem,		//^s^RseUpdateSocialItem#
	S2C_RseUpdateStarsBookmarks,		//^s^RseUpdateStarsBookmarks#
	S2C_RseUpdateTargets,		//^s^RseUpdateTargets#
	S2C_RseBattleDamagesPack,		//^s^RseBattleDamagesPack#
	S2C_RseItemMineExploded,		//^s^RseItemMineExploded#
	S2C_RseFinished,		//^s^RseFinished#
	S2C_RseQueryPvE,		//^s^RseQueryPvE#
};
enum C2S_EVENT
{
	C2S_EVENT_BASE = 1023,
	C2S_RceBattleDamagesPack,		//^c^RceBattleDamagesPack#
	C2S_RceCustomizedActivity,		//^c^RceCustomizedActivity#
	C2S_RceFinished,		//^c^RceFinished#
	C2S_RceHeartbeat,		//^c^RceHeartbeat#
	C2S_RceItemMineExploded,		//^c^RceItemMineExploded#
	C2S_RceLogin,		//^c^RceLogin#
	C2S_RceNpcAttackStart,		//^c^RceNpcAttackStart#
	C2S_RceObtainAskForHelp,		//^c^RceObtainAskForHelp#
	C2S_RceObtainAttackerList,		//^c^RceObtainAttackerList#
	C2S_RceObtainBattleReplay,		//^c^RceObtainBattleReplay#
	C2S_RceObtainCustomizer,		//^c^RceObtainCustomizer#
	C2S_RceObtainHangarsHelp,		//^c^RceObtainHangarsHelp#
	C2S_RceObtainMessagesList,		//^c^RceObtainMessagesList#
	C2S_RceObtainNeighborsList,		//^c^RceObtainNeighborsList#
	C2S_RceObtainNpcList,		//^c^RceObtainNpcList#
	C2S_RceObtainPayUrlParame,		//^c^RceObtainPayUrlParame#
	C2S_RceObtainRandomTargets,		//^c^RceObtainRandomTargets#
	C2S_RceObtainSocialItems,		//^c^RceObtainSocialItems#
	C2S_RceObtainSocialUserInfo,		//^c^RceObtainSocialUserInfo#
	C2S_RceObtainUniverse,		//^c^RceObtainUniverse#
	C2S_RceObtainVisitHelps,		//^c^RceObtainVisitHelps#
	C2S_RceOnlineReward,		//^c^RceOnlineReward#
	C2S_RceQueryCheckAndLockAccountIfPossible,		//^c^RceQueryCheckAndLockAccountIfPossible#
	C2S_RceQueryGalaxyWindow,		//^c^RceQueryGalaxyWindow#
	C2S_RceQueryReceiveWishItem,		//^c^RceQueryReceiveWishItem#
	C2S_RceQueryRefuseReceiveWishItem,		//^c^RceQueryRefuseReceiveWishItem#
	C2S_RceQuerySendWishItem,		//^c^RceQuerySendWishItem#
	C2S_RceQueryStarInfo,		//^c^RceQueryStarInfo#
	C2S_RceQueryStarsBookmarks,		//^c^RceQueryStarsBookmarks#
	C2S_RceQueryVisitHelpsGiftUnitsOnBunker,		//^c^RceQueryVisitHelpsGiftUnitsOnBunker#
	C2S_RceUpdateAlliances,		//^c^RceUpdateAlliances#
	C2S_RceUpdateGameUnits,		//^c^RceUpdateGameUnits#
	C2S_RceUpdateItem,		//^c^RceUpdateItem#
	C2S_RceUpdateMisc,		//^c^RceUpdateMisc#
	C2S_RceUpdateMissions,		//^c^RceUpdateMissions#
	C2S_RceUpdateProfile,		//^c^RceUpdateProfile#
	C2S_RceUpdateShips,		//^c^RceUpdateShips#
	C2S_RceUpdateSocialItem,		//^c^RceUpdateSocialItem#
	C2S_RceUpdateStarsBookmarks,		//^c^RceUpdateStarsBookmarks#
	C2S_RceUpdateTargets,		//^c^RceUpdateTargets#
	C2S_RceUpdateVisitHelp,		//^c^RceUpdateVisitHelp#
	C2S_RceQueryPvE,		//^c^RceQueryPvE#
	C2S_EVENT_MAX,
};
#endif


ProtocolMgr = class("ProtocolMgr")

require("redwar/logic/net/PkgMap.lua")
require("redwar/logic/net/protocol/RceBattleDamagesPack_pb.lua")
require("redwar/logic/net/protocol/RceCustomizedActivity_pb.lua")
require("redwar/logic/net/protocol/RceFinished_pb.lua")
require("redwar/logic/net/protocol/RceHeartbeat_pb.lua")
require("redwar/logic/net/protocol/RceItemMineExploded_pb.lua")
require("redwar/logic/net/protocol/RceLogin_pb.lua")
require("redwar/logic/net/protocol/RceNpcAttackStart_pb.lua")
require("redwar/logic/net/protocol/RceObtainAskForHelp_pb.lua")
require("redwar/logic/net/protocol/RceObtainAttackerList_pb.lua")
require("redwar/logic/net/protocol/RceObtainBattleReplay_pb.lua")
require("redwar/logic/net/protocol/RceObtainCustomizer_pb.lua")
require("redwar/logic/net/protocol/RceObtainHangarsHelp_pb.lua")
require("redwar/logic/net/protocol/RceObtainMessagesList_pb.lua")
require("redwar/logic/net/protocol/RceObtainNeighborsList_pb.lua")
require("redwar/logic/net/protocol/RceObtainNpcList_pb.lua")
require("redwar/logic/net/protocol/RceObtainPayUrlParame_pb.lua")
require("redwar/logic/net/protocol/RceObtainRandomTargets_pb.lua")
require("redwar/logic/net/protocol/RceObtainSocialItems_pb.lua")
require("redwar/logic/net/protocol/RceObtainSocialUserInfo_pb.lua")
require("redwar/logic/net/protocol/RceObtainUniverse_pb.lua")
require("redwar/logic/net/protocol/RceObtainVisitHelps_pb.lua")
require("redwar/logic/net/protocol/RceOnlineReward_pb.lua")
require("redwar/logic/net/protocol/RceQueryCheckAndLockAccountIfPossible_pb.lua")
require("redwar/logic/net/protocol/RceQueryGalaxyWindow_pb.lua")
require("redwar/logic/net/protocol/RceQueryPvE_pb.lua")
require("redwar/logic/net/protocol/RceQueryReceiveWishItem_pb.lua")
require("redwar/logic/net/protocol/RceQueryRefuseReceiveWishItem_pb.lua")
require("redwar/logic/net/protocol/RceQuerySendWishItem_pb.lua")
require("redwar/logic/net/protocol/RceQueryStarInfo_pb.lua")
require("redwar/logic/net/protocol/RceQueryStarsBookmarks_pb.lua")
require("redwar/logic/net/protocol/RceQueryVisitHelpsGiftUnitsOnBunker_pb.lua")
require("redwar/logic/net/protocol/RceUpdateAlliances_pb.lua")
require("redwar/logic/net/protocol/RceUpdateGameUnits_pb.lua")
require("redwar/logic/net/protocol/RceUpdateItem_pb.lua")
require("redwar/logic/net/protocol/RceUpdateMisc_pb.lua")
require("redwar/logic/net/protocol/RceUpdateMissions_pb.lua")
require("redwar/logic/net/protocol/RceUpdateProfile_pb.lua")
require("redwar/logic/net/protocol/RceUpdateShips_pb.lua")
require("redwar/logic/net/protocol/RceUpdateSocialItem_pb.lua")
require("redwar/logic/net/protocol/RceUpdateStarsBookmarks_pb.lua")
require("redwar/logic/net/protocol/RceUpdateTargets_pb.lua")
require("redwar/logic/net/protocol/RceUpdateVisitHelp_pb.lua")

require("redwar/logic/net/protocol/RseAuthState_pb.lua")
require("redwar/logic/net/protocol/RseBattleDamagesPack_pb.lua")
require("redwar/logic/net/protocol/RseCustomizedActivity_pb.lua")
require("redwar/logic/net/protocol/RseError_pb.lua")
require("redwar/logic/net/protocol/RseFinished_pb.lua")
require("redwar/logic/net/protocol/RseHeartbeat_pb.lua")
require("redwar/logic/net/protocol/RseItemMineExploded_pb.lua")
require("redwar/logic/net/protocol/RseLogin_pb.lua")
require("redwar/logic/net/protocol/RseObtainAskForHelp_pb.lua")
require("redwar/logic/net/protocol/RseObtainAttackerList_pb.lua")
require("redwar/logic/net/protocol/RseObtainBattleReplay_pb.lua")
require("redwar/logic/net/protocol/RseObtainCustomizer_pb.lua")
require("redwar/logic/net/protocol/RseObtainHangarsHelp_pb.lua")
require("redwar/logic/net/protocol/RseObtainMessagesList_pb.lua")
require("redwar/logic/net/protocol/RseObtainNeighborsList_pb.lua")
require("redwar/logic/net/protocol/RseObtainNpcList_pb.lua")
require("redwar/logic/net/protocol/RseObtainPayUrlParame_pb.lua")
require("redwar/logic/net/protocol/RseObtainRandomTargets_pb.lua")
require("redwar/logic/net/protocol/RseObtainSocialItems_pb.lua")
require("redwar/logic/net/protocol/RseObtainSocialUserInfo_pb.lua")
require("redwar/logic/net/protocol/RseObtainUniverse_pb.lua")
require("redwar/logic/net/protocol/RseObtainVisitHelps_pb.lua")
require("redwar/logic/net/protocol/RseQueryCheckAndLockAccountIfPossible_pb.lua")
require("redwar/logic/net/protocol/RseQueryGalaxyWindow_pb.lua")
require("redwar/logic/net/protocol/RseQueryPvE_pb.lua")
require("redwar/logic/net/protocol/RseQueryReceiveWishItem_pb.lua")
require("redwar/logic/net/protocol/RseQueryRefuseReceiveWishItem_pb.lua")
require("redwar/logic/net/protocol/RseQuerySendWishItem_pb.lua")
require("redwar/logic/net/protocol/RseQueryStarInfo_pb.lua")
require("redwar/logic/net/protocol/RseQueryStarsBookmarks_pb.lua")
require("redwar/logic/net/protocol/RseQueryVisitHelpsGiftUnitsOnBunker_pb.lua")
require("redwar/logic/net/protocol/RseUpdateAlliances_pb.lua")
require("redwar/logic/net/protocol/RseUpdateGameUnits_pb.lua")
require("redwar/logic/net/protocol/RseUpdateItem_pb.lua")
require("redwar/logic/net/protocol/RseUpdateMisc_pb.lua")
require("redwar/logic/net/protocol/RseUpdateMissions_pb.lua")
require("redwar/logic/net/protocol/RseUpdateProfile_pb.lua")
require("redwar/logic/net/protocol/RseUpdateShips_pb.lua")
require("redwar/logic/net/protocol/RseUpdateSocialItem_pb.lua")
require("redwar/logic/net/protocol/RseUpdateStarsBookmarks_pb.lua")
require("redwar/logic/net/protocol/RseUpdateTargets_pb.lua")


ID_RceBattleDamagesPack = 1024 +    0
ID_RceCustomizedActivity = 1024 +    1
ID_RceFinished = 1024 +    2
ID_RceHeartbeat = 1024 +    3
ID_RceItemMineExploded = 1024 +    4
ID_RceLogin = 1024 +    5
ID_RceNpcAttackStart = 1024 +    6
ID_RceObtainAskForHelp = 1024 +    7
ID_RceObtainAttackerList = 1024 +    8
ID_RceObtainBattleReplay = 1024 +    9
ID_RceObtainCustomizer = 1024 +   10
ID_RceObtainHangarsHelp = 1024 +   11
ID_RceObtainMessagesList = 1024 +   12
ID_RceObtainNeighborsList = 1024 +   13
ID_RceObtainNpcList = 1024 +   14
ID_RceObtainPayUrlParame = 1024 +   15
ID_RceObtainRandomTargets = 1024 +   16
ID_RceObtainSocialItems = 1024 +   17
ID_RceObtainSocialUserInfo = 1024 +   18
ID_RceObtainUniverse = 1024 +   19
ID_RceObtainVisitHelps = 1024 +   20
ID_RceOnlineReward = 1024 +   21
ID_RceQueryCheckAndLockAccountIfPossible = 1024 +   22
ID_RceQueryGalaxyWindow = 1024 +   23
ID_RceQueryPvE = 1024 +   24
ID_RceQueryReceiveWishItem = 1024 +   25
ID_RceQueryRefuseReceiveWishItem = 1024 +   26
ID_RceQuerySendWishItem = 1024 +   27
ID_RceQueryStarInfo = 1024 +   28
ID_RceQueryStarsBookmarks = 1024 +   29
ID_RceQueryVisitHelpsGiftUnitsOnBunker = 1024 +   30
ID_RceUpdateAlliances = 1024 +   31
ID_RceUpdateGameUnits = 1024 +   32
ID_RceUpdateItem = 1024 +   33
ID_RceUpdateMisc = 1024 +   34
ID_RceUpdateMissions = 1024 +   35
ID_RceUpdateProfile = 1024 +   36
ID_RceUpdateShips = 1024 +   37
ID_RceUpdateSocialItem = 1024 +   38
ID_RceUpdateStarsBookmarks = 1024 +   39
ID_RceUpdateTargets = 1024 +   40
ID_RceUpdateVisitHelp = 1024 +   41

ID_RseAuthState = 512 +    0
ID_RseBattleDamagesPack = 512 +    1
ID_RseCustomizedActivity = 512 +    2
ID_RseError = 512 +    3
ID_RseFinished = 512 +    4
ID_RseHeartbeat = 512 +    5
ID_RseItemMineExploded = 512 +    6
ID_RseLogin = 512 +    7
ID_RseObtainAskForHelp = 512 +    8
ID_RseObtainAttackerList = 512 +    9
ID_RseObtainBattleReplay = 512 +   10
ID_RseObtainCustomizer = 512 +   11
ID_RseObtainHangarsHelp = 512 +   12
ID_RseObtainMessagesList = 512 +   13
ID_RseObtainNeighborsList = 512 +   14
ID_RseObtainNpcList = 512 +   15
ID_RseObtainPayUrlParame = 512 +   16
ID_RseObtainRandomTargets = 512 +   17
ID_RseObtainSocialItems = 512 +   18
ID_RseObtainSocialUserInfo = 512 +   19
ID_RseObtainUniverse = 512 +   20
ID_RseObtainVisitHelps = 512 +   21
ID_RseQueryCheckAndLockAccountIfPossible = 512 +   22
ID_RseQueryGalaxyWindow = 512 +   23
ID_RseQueryPvE = 512 +   24
ID_RseQueryReceiveWishItem = 512 +   25
ID_RseQueryRefuseReceiveWishItem = 512 +   26
ID_RseQuerySendWishItem = 512 +   27
ID_RseQueryStarInfo = 512 +   28
ID_RseQueryStarsBookmarks = 512 +   29
ID_RseQueryVisitHelpsGiftUnitsOnBunker = 512 +   30
ID_RseUpdateAlliances = 512 +   31
ID_RseUpdateGameUnits = 512 +   32
ID_RseUpdateItem = 512 +   33
ID_RseUpdateMisc = 512 +   34
ID_RseUpdateMissions = 512 +   35
ID_RseUpdateProfile = 512 +   36
ID_RseUpdateShips = 512 +   37
ID_RseUpdateSocialItem = 512 +   38
ID_RseUpdateStarsBookmarks = 512 +   39
ID_RseUpdateTargets = 512 +   40


local _pkgMap = nil

function ProtocolMgr:initialize()
	_pkgMap = PkgMap:new()
	self:InitPkgMap()
end

function ProtocolMgr:GetPkgMap()
	 return _pkgMap
end

function ProtocolMgr:InitPkgMap()
	_pkgMap:AddPkg(ID_RceBattleDamagesPack, RceBattleDamagesPack_pb.RceBattleDamagesPack())
	_pkgMap:AddPkg(ID_RceCustomizedActivity, RceCustomizedActivity_pb.RceCustomizedActivity())
	_pkgMap:AddPkg(ID_RceFinished, RceFinished_pb.RceFinished())
	_pkgMap:AddPkg(ID_RceHeartbeat, RceHeartbeat_pb.RceHeartbeat())
	_pkgMap:AddPkg(ID_RceItemMineExploded, RceItemMineExploded_pb.RceItemMineExploded())
	_pkgMap:AddPkg(ID_RceLogin, RceLogin_pb.RceLogin())
	_pkgMap:AddPkg(ID_RceNpcAttackStart, RceNpcAttackStart_pb.RceNpcAttackStart())
	_pkgMap:AddPkg(ID_RceObtainAskForHelp, RceObtainAskForHelp_pb.RceObtainAskForHelp())
	_pkgMap:AddPkg(ID_RceObtainAttackerList, RceObtainAttackerList_pb.RceObtainAttackerList())
	_pkgMap:AddPkg(ID_RceObtainBattleReplay, RceObtainBattleReplay_pb.RceObtainBattleReplay())
	_pkgMap:AddPkg(ID_RceObtainCustomizer, RceObtainCustomizer_pb.RceObtainCustomizer())
	_pkgMap:AddPkg(ID_RceObtainHangarsHelp, RceObtainHangarsHelp_pb.RceObtainHangarsHelp())
	_pkgMap:AddPkg(ID_RceObtainMessagesList, RceObtainMessagesList_pb.RceObtainMessagesList())
	_pkgMap:AddPkg(ID_RceObtainNeighborsList, RceObtainNeighborsList_pb.RceObtainNeighborsList())
	_pkgMap:AddPkg(ID_RceObtainNpcList, RceObtainNpcList_pb.RceObtainNpcList())
	_pkgMap:AddPkg(ID_RceObtainPayUrlParame, RceObtainPayUrlParame_pb.RceObtainPayUrlParame())
	_pkgMap:AddPkg(ID_RceObtainRandomTargets, RceObtainRandomTargets_pb.RceObtainRandomTargets())
	_pkgMap:AddPkg(ID_RceObtainSocialItems, RceObtainSocialItems_pb.RceObtainSocialItems())
	_pkgMap:AddPkg(ID_RceObtainSocialUserInfo, RceObtainSocialUserInfo_pb.RceObtainSocialUserInfo())
	_pkgMap:AddPkg(ID_RceObtainUniverse, RceObtainUniverse_pb.RceObtainUniverse())
	_pkgMap:AddPkg(ID_RceObtainVisitHelps, RceObtainVisitHelps_pb.RceObtainVisitHelps())
	_pkgMap:AddPkg(ID_RceOnlineReward, RceOnlineReward_pb.RceOnlineReward())
	_pkgMap:AddPkg(ID_RceQueryCheckAndLockAccountIfPossible, RceQueryCheckAndLockAccountIfPossible_pb.RceQueryCheckAndLockAccountIfPossible())
	_pkgMap:AddPkg(ID_RceQueryGalaxyWindow, RceQueryGalaxyWindow_pb.RceQueryGalaxyWindow())
	_pkgMap:AddPkg(ID_RceQueryPvE, RceQueryPvE_pb.RceQueryPvE())
	_pkgMap:AddPkg(ID_RceQueryReceiveWishItem, RceQueryReceiveWishItem_pb.RceQueryReceiveWishItem())
	_pkgMap:AddPkg(ID_RceQueryRefuseReceiveWishItem, RceQueryRefuseReceiveWishItem_pb.RceQueryRefuseReceiveWishItem())
	_pkgMap:AddPkg(ID_RceQuerySendWishItem, RceQuerySendWishItem_pb.RceQuerySendWishItem())
	_pkgMap:AddPkg(ID_RceQueryStarInfo, RceQueryStarInfo_pb.RceQueryStarInfo())
	_pkgMap:AddPkg(ID_RceQueryStarsBookmarks, RceQueryStarsBookmarks_pb.RceQueryStarsBookmarks())
	_pkgMap:AddPkg(ID_RceQueryVisitHelpsGiftUnitsOnBunker, RceQueryVisitHelpsGiftUnitsOnBunker_pb.RceQueryVisitHelpsGiftUnitsOnBunker())
	_pkgMap:AddPkg(ID_RceUpdateAlliances, RceUpdateAlliances_pb.RceUpdateAlliances())
	_pkgMap:AddPkg(ID_RceUpdateGameUnits, RceUpdateGameUnits_pb.RceUpdateGameUnits())
	_pkgMap:AddPkg(ID_RceUpdateItem, RceUpdateItem_pb.RceUpdateItem())
	_pkgMap:AddPkg(ID_RceUpdateMisc, RceUpdateMisc_pb.RceUpdateMisc())
	_pkgMap:AddPkg(ID_RceUpdateMissions, RceUpdateMissions_pb.RceUpdateMissions())
	_pkgMap:AddPkg(ID_RceUpdateProfile, RceUpdateProfile_pb.RceUpdateProfile())
	_pkgMap:AddPkg(ID_RceUpdateShips, RceUpdateShips_pb.RceUpdateShips())
	_pkgMap:AddPkg(ID_RceUpdateSocialItem, RceUpdateSocialItem_pb.RceUpdateSocialItem())
	_pkgMap:AddPkg(ID_RceUpdateStarsBookmarks, RceUpdateStarsBookmarks_pb.RceUpdateStarsBookmarks())
	_pkgMap:AddPkg(ID_RceUpdateTargets, RceUpdateTargets_pb.RceUpdateTargets())
	_pkgMap:AddPkg(ID_RceUpdateVisitHelp, RceUpdateVisitHelp_pb.RceUpdateVisitHelp())

	_pkgMap:AddPkg(ID_RseAuthState, RseAuthState_pb.RseAuthState())
	_pkgMap:AddPkg(ID_RseBattleDamagesPack, RseBattleDamagesPack_pb.RseBattleDamagesPack())
	_pkgMap:AddPkg(ID_RseCustomizedActivity, RseCustomizedActivity_pb.RseCustomizedActivity())
	_pkgMap:AddPkg(ID_RseError, RseError_pb.RseError())
	_pkgMap:AddPkg(ID_RseFinished, RseFinished_pb.RseFinished())
	_pkgMap:AddPkg(ID_RseHeartbeat, RseHeartbeat_pb.RseHeartbeat())
	_pkgMap:AddPkg(ID_RseItemMineExploded, RseItemMineExploded_pb.RseItemMineExploded())
	_pkgMap:AddPkg(ID_RseLogin, RseLogin_pb.RseLogin())
	_pkgMap:AddPkg(ID_RseObtainAskForHelp, RseObtainAskForHelp_pb.RseObtainAskForHelp())
	_pkgMap:AddPkg(ID_RseObtainAttackerList, RseObtainAttackerList_pb.RseObtainAttackerList())
	_pkgMap:AddPkg(ID_RseObtainBattleReplay, RseObtainBattleReplay_pb.RseObtainBattleReplay())
	_pkgMap:AddPkg(ID_RseObtainCustomizer, RseObtainCustomizer_pb.RseObtainCustomizer())
	_pkgMap:AddPkg(ID_RseObtainHangarsHelp, RseObtainHangarsHelp_pb.RseObtainHangarsHelp())
	_pkgMap:AddPkg(ID_RseObtainMessagesList, RseObtainMessagesList_pb.RseObtainMessagesList())
	_pkgMap:AddPkg(ID_RseObtainNeighborsList, RseObtainNeighborsList_pb.RseObtainNeighborsList())
	_pkgMap:AddPkg(ID_RseObtainNpcList, RseObtainNpcList_pb.RseObtainNpcList())
	_pkgMap:AddPkg(ID_RseObtainPayUrlParame, RseObtainPayUrlParame_pb.RseObtainPayUrlParame())
	_pkgMap:AddPkg(ID_RseObtainRandomTargets, RseObtainRandomTargets_pb.RseObtainRandomTargets())
	_pkgMap:AddPkg(ID_RseObtainSocialItems, RseObtainSocialItems_pb.RseObtainSocialItems())
	_pkgMap:AddPkg(ID_RseObtainSocialUserInfo, RseObtainSocialUserInfo_pb.RseObtainSocialUserInfo())
	_pkgMap:AddPkg(ID_RseObtainUniverse, RseObtainUniverse_pb.RseObtainUniverse())
	_pkgMap:AddPkg(ID_RseObtainVisitHelps, RseObtainVisitHelps_pb.RseObtainVisitHelps())
	_pkgMap:AddPkg(ID_RseQueryCheckAndLockAccountIfPossible, RseQueryCheckAndLockAccountIfPossible_pb.RseQueryCheckAndLockAccountIfPossible())
	_pkgMap:AddPkg(ID_RseQueryGalaxyWindow, RseQueryGalaxyWindow_pb.RseQueryGalaxyWindow())
	_pkgMap:AddPkg(ID_RseQueryPvE, RseQueryPvE_pb.RseQueryPvE())
	_pkgMap:AddPkg(ID_RseQueryReceiveWishItem, RseQueryReceiveWishItem_pb.RseQueryReceiveWishItem())
	_pkgMap:AddPkg(ID_RseQueryRefuseReceiveWishItem, RseQueryRefuseReceiveWishItem_pb.RseQueryRefuseReceiveWishItem())
	_pkgMap:AddPkg(ID_RseQuerySendWishItem, RseQuerySendWishItem_pb.RseQuerySendWishItem())
	_pkgMap:AddPkg(ID_RseQueryStarInfo, RseQueryStarInfo_pb.RseQueryStarInfo())
	_pkgMap:AddPkg(ID_RseQueryStarsBookmarks, RseQueryStarsBookmarks_pb.RseQueryStarsBookmarks())
	_pkgMap:AddPkg(ID_RseQueryVisitHelpsGiftUnitsOnBunker, RseQueryVisitHelpsGiftUnitsOnBunker_pb.RseQueryVisitHelpsGiftUnitsOnBunker())
	_pkgMap:AddPkg(ID_RseUpdateAlliances, RseUpdateAlliances_pb.RseUpdateAlliances())
	_pkgMap:AddPkg(ID_RseUpdateGameUnits, RseUpdateGameUnits_pb.RseUpdateGameUnits())
	_pkgMap:AddPkg(ID_RseUpdateItem, RseUpdateItem_pb.RseUpdateItem())
	_pkgMap:AddPkg(ID_RseUpdateMisc, RseUpdateMisc_pb.RseUpdateMisc())
	_pkgMap:AddPkg(ID_RseUpdateMissions, RseUpdateMissions_pb.RseUpdateMissions())
	_pkgMap:AddPkg(ID_RseUpdateProfile, RseUpdateProfile_pb.RseUpdateProfile())
	_pkgMap:AddPkg(ID_RseUpdateShips, RseUpdateShips_pb.RseUpdateShips())
	_pkgMap:AddPkg(ID_RseUpdateSocialItem, RseUpdateSocialItem_pb.RseUpdateSocialItem())
	_pkgMap:AddPkg(ID_RseUpdateStarsBookmarks, RseUpdateStarsBookmarks_pb.RseUpdateStarsBookmarks())
	_pkgMap:AddPkg(ID_RseUpdateTargets, RseUpdateTargets_pb.RseUpdateTargets())
end

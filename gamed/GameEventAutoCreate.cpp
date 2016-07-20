#include "event/AutoEventHead.h"
#include "GameNetHandler.h"
#include "GameEventHandler.h"
//该文件为使用工具自动生成，请不要在文件中修改，如需要修改，请与sskylin联系。


void GameEventHandler::initAutoEventProcessors()
{
	RceCustomizedActivityHandle::createInstance(this);
	RceLoginHandle::createInstance(this);
	RceObtainAttackerListHandle::createInstance(this);
	RceObtainBattleReplayHandle::createInstance(this);
	RceObtainCustomizerHandle::createInstance(this);
	RceObtainMessagesListHandle::createInstance(this);
	RceObtainNeighborsListHandle::createInstance(this);
	RceObtainNpcListHandle::createInstance(this);
	RceObtainSocialItemsHandle::createInstance(this);
	RceObtainSocialUserInfoHandle::createInstance(this);
	RceObtainUniverseHandle::createInstance(this);
	RceUpdateAlliancesHandle::createInstance(this);
	RceDeployUnitsHandle::createInstance(this);
	RceItemDamagedHandle::createInstance(this);
	RceObtainAskForHelpHandle::createInstance(this);
	RceObtainHangarsHelpHandle::createInstance(this);
	RceObtainPayUrlParameHandle::createInstance(this);
	RceObtainRandomTargetsHandle::createInstance(this);
	RceObtainVisitHelpsHandle::createInstance(this);
	RceOnlineRewardHandle::createInstance(this);
	RceQueryCheckAndLockAccountIfPossibleHandle::createInstance(this);
	RceQueryGalaxyWindowHandle::createInstance(this);
	RceQueryReceiveWishItemHandle::createInstance(this);
	RceQueryRefuseReceiveWishItemHandle::createInstance(this);
	RceQuerySendWishItemHandle::createInstance(this);
	RceQueryStarInfoHandle::createInstance(this);
	RceQueryStarsBookmarksHandle::createInstance(this);
	RceQueryVisitHelpsGiftUnitsOnBunkerHandle::createInstance(this);
	RceUnitDamagedHandle::createInstance(this);
	RceUpdateGameUnitsHandle::createInstance(this);
	RceUpdateItemHandle::createInstance(this);
	RceUpdateMiscHandle::createInstance(this);
	RceUpdateMissionsHandle::createInstance(this);
	RceUpdateProfileHandle::createInstance(this);
	RceUpdateShipsHandle::createInstance(this);
	RceUpdateSocialItemHandle::createInstance(this);
	RceUpdateStarsBookmarksHandle::createInstance(this);
	RceUpdateTargetsHandle::createInstance(this);
	RceUpdateVisitHelpHandle::createInstance(this);
}

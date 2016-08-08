package com.sincetimes.redwar.game.comnunicate.protocol {
    import com.sincetimes.redwar.game.comnunicate.PkgMap;
    import flash.utils.getQualifiedClassName;
    /**
     * ...
     * @author Foreverflying
     */
    public class ProtocolMgr {
        public function ProtocolMgr() {
            _pkgMap = new PkgMap;
            InitPkgMap();
        }

        public function GetPkgMap() : PkgMap {
            return _pkgMap;
        }

        private function InitPkgMap() : void {
            _pkgMap.AddPkg(  1024 +    0, RceBattleDamagesPack );
            _pkgMap.AddPkg(  1024 +    1, RceCustomizedActivity );
            _pkgMap.AddPkg(  1024 +    2, RceFinished );
            _pkgMap.AddPkg(  1024 +    3, RceHeartbeat );
            _pkgMap.AddPkg(  1024 +    4, RceItemMineExploded );
            _pkgMap.AddPkg(  1024 +    5, RceLogin );
            _pkgMap.AddPkg(  1024 +    6, RceNpcAttackStart );
            _pkgMap.AddPkg(  1024 +    7, RceObtainAskForHelp );
            _pkgMap.AddPkg(  1024 +    8, RceObtainAttackerList );
            _pkgMap.AddPkg(  1024 +    9, RceObtainBattleReplay );
            _pkgMap.AddPkg(  1024 +   10, RceObtainCustomizer );
            _pkgMap.AddPkg(  1024 +   11, RceObtainHangarsHelp );
            _pkgMap.AddPkg(  1024 +   12, RceObtainMessagesList );
            _pkgMap.AddPkg(  1024 +   13, RceObtainNeighborsList );
            _pkgMap.AddPkg(  1024 +   14, RceObtainNpcList );
            _pkgMap.AddPkg(  1024 +   15, RceObtainPayUrlParame );
            _pkgMap.AddPkg(  1024 +   16, RceObtainRandomTargets );
            _pkgMap.AddPkg(  1024 +   17, RceObtainSocialItems );
            _pkgMap.AddPkg(  1024 +   18, RceObtainSocialUserInfo );
            _pkgMap.AddPkg(  1024 +   19, RceObtainUniverse );
            _pkgMap.AddPkg(  1024 +   20, RceObtainVisitHelps );
            _pkgMap.AddPkg(  1024 +   21, RceOnlineReward );
            _pkgMap.AddPkg(  1024 +   22, RceQueryCheckAndLockAccountIfPossible );
            _pkgMap.AddPkg(  1024 +   23, RceQueryGalaxyWindow );
            _pkgMap.AddPkg(  1024 +   24, RceQueryPvE );
            _pkgMap.AddPkg(  1024 +   25, RceQueryReceiveWishItem );
            _pkgMap.AddPkg(  1024 +   26, RceQueryRefuseReceiveWishItem );
            _pkgMap.AddPkg(  1024 +   27, RceQuerySendWishItem );
            _pkgMap.AddPkg(  1024 +   28, RceQueryStarInfo );
            _pkgMap.AddPkg(  1024 +   29, RceQueryStarsBookmarks );
            _pkgMap.AddPkg(  1024 +   30, RceQueryVisitHelpsGiftUnitsOnBunker );
            _pkgMap.AddPkg(  1024 +   31, RceUpdateAlliances );
            _pkgMap.AddPkg(  1024 +   32, RceUpdateGameUnits );
            _pkgMap.AddPkg(  1024 +   33, RceUpdateItem );
            _pkgMap.AddPkg(  1024 +   34, RceUpdateMisc );
            _pkgMap.AddPkg(  1024 +   35, RceUpdateMissions );
            _pkgMap.AddPkg(  1024 +   36, RceUpdateProfile );
            _pkgMap.AddPkg(  1024 +   37, RceUpdateShips );
            _pkgMap.AddPkg(  1024 +   38, RceUpdateSocialItem );
            _pkgMap.AddPkg(  1024 +   39, RceUpdateStarsBookmarks );
            _pkgMap.AddPkg(  1024 +   40, RceUpdateTargets );
            _pkgMap.AddPkg(  1024 +   41, RceUpdateVisitHelp );

            _pkgMap.AddPkg(  512 +    0, RseAuthState );
            _pkgMap.AddPkg(  512 +    1, RseBattleDamagesPack );
            _pkgMap.AddPkg(  512 +    2, RseCustomizedActivity );
            _pkgMap.AddPkg(  512 +    3, RseError );
            _pkgMap.AddPkg(  512 +    4, RseFinished );
            _pkgMap.AddPkg(  512 +    5, RseHeartbeat );
            _pkgMap.AddPkg(  512 +    6, RseItemMineExploded );
            _pkgMap.AddPkg(  512 +    7, RseLogin );
            _pkgMap.AddPkg(  512 +    8, RseObtainAskForHelp );
            _pkgMap.AddPkg(  512 +    9, RseObtainAttackerList );
            _pkgMap.AddPkg(  512 +   10, RseObtainBattleReplay );
            _pkgMap.AddPkg(  512 +   11, RseObtainCustomizer );
            _pkgMap.AddPkg(  512 +   12, RseObtainHangarsHelp );
            _pkgMap.AddPkg(  512 +   13, RseObtainMessagesList );
            _pkgMap.AddPkg(  512 +   14, RseObtainNeighborsList );
            _pkgMap.AddPkg(  512 +   15, RseObtainNpcList );
            _pkgMap.AddPkg(  512 +   16, RseObtainPayUrlParame );
            _pkgMap.AddPkg(  512 +   17, RseObtainRandomTargets );
            _pkgMap.AddPkg(  512 +   18, RseObtainSocialItems );
            _pkgMap.AddPkg(  512 +   19, RseObtainSocialUserInfo );
            _pkgMap.AddPkg(  512 +   20, RseObtainUniverse );
            _pkgMap.AddPkg(  512 +   21, RseObtainVisitHelps );
            _pkgMap.AddPkg(  512 +   22, RseQueryCheckAndLockAccountIfPossible );
            _pkgMap.AddPkg(  512 +   23, RseQueryGalaxyWindow );
            _pkgMap.AddPkg(  512 +   24, RseQueryPvE );
            _pkgMap.AddPkg(  512 +   25, RseQueryReceiveWishItem );
            _pkgMap.AddPkg(  512 +   26, RseQueryRefuseReceiveWishItem );
            _pkgMap.AddPkg(  512 +   27, RseQuerySendWishItem );
            _pkgMap.AddPkg(  512 +   28, RseQueryStarInfo );
            _pkgMap.AddPkg(  512 +   29, RseQueryStarsBookmarks );
            _pkgMap.AddPkg(  512 +   30, RseQueryVisitHelpsGiftUnitsOnBunker );
            _pkgMap.AddPkg(  512 +   31, RseUpdateAlliances );
            _pkgMap.AddPkg(  512 +   32, RseUpdateGameUnits );
            _pkgMap.AddPkg(  512 +   33, RseUpdateItem );
            _pkgMap.AddPkg(  512 +   34, RseUpdateMisc );
            _pkgMap.AddPkg(  512 +   35, RseUpdateMissions );
            _pkgMap.AddPkg(  512 +   36, RseUpdateProfile );
            _pkgMap.AddPkg(  512 +   37, RseUpdateShips );
            _pkgMap.AddPkg(  512 +   38, RseUpdateSocialItem );
            _pkgMap.AddPkg(  512 +   39, RseUpdateStarsBookmarks );
            _pkgMap.AddPkg(  512 +   40, RseUpdateTargets );
        }

        private var _pkgMap : PkgMap;
    }
}

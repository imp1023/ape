#include "../CountryEventHandler.h"
#include "../CountryDataHandler.h"
#include "DealResourceEvent.h"
#include "../CountryRgnDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../common/const_def.h"
#include "../../event/EventDefine.h"

#include <math.h>
#include "../../event/EventQueue.h"
#include "../../common/SysLog.h"
#include "../CountryConfigMgr.h"
#include "../logic/CountryInfoCfg.h"
#include "../CountryDefine.h"
#include "../logic/ResourceCfg.h"

extern int G_CountrySrvD_ID; 

//const int STAR_STAT_TIME = 60*1000;

DealResourceEvent ::DealResourceEvent ()
:CBaseEvent()
{
	//debug_idx = -1;
}
DealResourceEvent ::~DealResourceEvent ()
{

}

void DealResourceEvent ::registHandler()
{
	eh_->getEventHandler()->registHandler(C2S_RceResourceOpt, ((ProcessRoutine) DealResourceEvent ::handle_));
}

void DealResourceEvent ::handle(Event* e)
{	
	GCG_CountryNeed* pNeedArgs = e->mutable_countryneed();
	int nGameID = pNeedArgs->gamesrvid();
	int nRegion = pNeedArgs->region();

	CountryDataHandler* pDH= eh_->getDataHandler();
	if(!pDH->IsRightRegion(nRegion))
	{
		return;//分区错误
	}
	
	CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
	if(pRDH==NULL)
	{
		return;
	}

	switch(e->cmd())
	{
	case C2S_RceResourceOpt:
		{
			HandleRceResourceOpt(e,pRDH,nGameID);
		}
		break;
	default:
		break;
	}
}

void DealResourceEvent::HandleRceResourceOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if ( pRDH == NULL )
	{
		return;
	}
	e->set_state(Status_Country_To_Game);
	RceResourceOpt* pRceOpt = e->mutable_ce_rceresourceopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseResourceOpt* pRseOpt = e->mutable_se_rseresourceopt();
	pRseOpt->set_ret(0);

	int nType		= pRceOpt->type();

	switch( nType )
	{
	case ROT_Open:
		{
			HandleOpen(e,pRDH,nGameID);
		}
		break;
	case ROT_Search:
		{
			HandleSearch(e,pRDH,nGameID);
		}
		break;
	case ROT_Attack:
		{
			HandleAttackOver(e,pRDH,nGameID);
		}
		break;
	case ROT_Leave:
		{
			HandleLeave(e,pRDH,nGameID);
		}
		break;
	case ROT_Update:
		{
			HandleUpdate(e,pRDH,nGameID);
		}
		break;
	case ROT_AttackOver:
		{
			HandleAttackOver(e,pRDH,nGameID);
		}
		break;
	default: 
		break;
	}
}


void DealResourceEvent::HandleOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceResourceOpt* pRceOpt = e->mutable_ce_rceresourceopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseResourceOpt* pRseOpt = e->mutable_se_rseresourceopt();
	time_t tNow = time(NULL);

	if ( pRceOpt->resourceid() != 0 )
	{
		//玩家有数据
		CResource* pResource = pRDH->GetCResource( pRceOpt->resourceid() );
		if ( pResource == NULL )
		{
			//未知资源ID
			return;
		}

		if ( pResource->IsGetByUser( toString(e->uid()), tNow ) )
		{
			pRseOpt->set_resourceid( pRceOpt->resourceid() );
			pRseOpt->set_resourceendtime( pResource->m_nEndTime );
		}
		else
		{
			pRseOpt->set_resourceid( 0 );
			pRseOpt->set_resourceendtime( 0 );
		}
	}
	
	for ( int i = 0; i < pRceOpt->getid_size() && i < RESOURCE_SEARCH_CNT; i++ )
	{
		int nGetID = pRceOpt->getid(i);
		CResource* pResource = pRDH->GetCResource( nGetID );
		if ( pResource == NULL )
		{
			//未知资源ID
			continue;
		}

		DB_C_Resource* pDBResource = pResource->m_pDBResource;
		if ( pDBResource == NULL )
		{
			return;
		}

		ResourceMsg* pMsg = pRseOpt->add_resourcemsg();
		pMsg->set_id( pResource->m_nID );
		int64 uid;
		safe_atoll(pResource->m_sUid.c_str(),uid);

		if ( uid == e->uid() )
		{
			pResource->PlayerLeave();
		}
		if ( !pResource->HasEnd( tNow ) )
		{
			//玩家控制的资源
			pMsg->set_uid( pResource->m_sUid );
			pMsg->set_name( pResource->m_sName );
			//将领信息
			for ( int j = 0; j < pDBResource->admiralid_size() && j < pDBResource->admiraltrainlv_size(); j++ )
			{
				pMsg->add_admiralid( pDBResource->admiralid(j) );
				pMsg->add_admiraltrainlv( pDBResource->admiraltrainlv(j) );
			}
			pMsg->set_power(pDBResource->pow());
		}
	}
	
	eh_->sendEventToGamed(e,nGameID);
}

void DealResourceEvent::HandleSearch(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceResourceOpt* pRceOpt = e->mutable_ce_rceresourceopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseResourceOpt* pRseOpt = e->mutable_se_rseresourceopt();
	time_t tNow = time(NULL);

	int nSearchType = pRceOpt->searchtype();
	if ( nSearchType > MAX_RESOURSE_SEARCH_TYPE || nSearchType <= 0 )
	{
		pRseOpt->set_ret( 5 );
		eh_->sendEventToGamed(e,nGameID);
	}
	int nGetList[RESOURCE_SEARCH_CNT];
	memset(nGetList,0,sizeof(int)*RESOURCE_SEARCH_CNT);
	int nCnt = 0;

	while ( pRseOpt->resourcemsg_size() < RESOURCE_SEARCH_CNT )
	{
		nCnt++;
		if ( nCnt > 20 )
		{
			//强制跳出
			pRseOpt->clear_resourcemsg();
			for ( int i = 0; i < RESOURCE_SEARCH_CNT; i++ )
			{
				CResource* pResource = pRDH->GetCResourceByNum( i, nSearchType );
				if ( pResource == NULL )
				{
					continue;
				}
				DB_C_Resource* pDBResource = pResource->m_pDBResource;
				if ( pDBResource == NULL )
				{
					continue;
				}

				ResourceMsg* pMsg = pRseOpt->add_resourcemsg();
				pMsg->set_id(pResource->m_nID);

				int64 uid;
				safe_atoll(pResource->m_sUid.c_str(),uid);

				if ( uid == e->uid() )
				{
					pResource->PlayerLeave();
				}

				if ( !pResource->HasEnd( tNow ) )
				{
					//玩家控制的资源
					pMsg->set_uid( pResource->m_sUid );
					pMsg->set_name( pResource->m_sName );
					//将领信息
					for ( int j = 0; j < pDBResource->admiralid_size() && j < pDBResource->admiraltrainlv_size(); j++ )
					{
						pMsg->add_admiralid( pDBResource->admiralid(j) );
						pMsg->add_admiraltrainlv( pDBResource->admiraltrainlv(j) );
					}
					pMsg->set_power(pDBResource->pow());
				}
			}
			break;
		}

		CResource* pResource = pRDH->GetRandCResource( nSearchType );
		if ( pResource == NULL )
		{
			continue;
		}
		DB_C_Resource* pDBResource = pResource->m_pDBResource;
		if ( pDBResource == NULL )
		{
			continue;
		}

		bool bFind = false;
		for ( int i = 0; i < RESOURCE_SEARCH_CNT; i++ )
		{
			if ( pResource->m_nID == nGetList[i] )
			{
				bFind = true;
				break;
			}
		}

		if ( bFind )
		{
			continue;
		}

		nGetList[pRseOpt->resourcemsg_size()] = pResource->m_nID;

		ResourceMsg* pMsg = pRseOpt->add_resourcemsg();
		pMsg->set_id(pResource->m_nID);

		int64 uid;
		safe_atoll(pResource->m_sUid.c_str(),uid);

		if ( uid == e->uid() )
		{
			pResource->PlayerLeave();
		}

		if ( !pResource->HasEnd( tNow ) )
		{
			//玩家控制的资源
			pMsg->set_uid( pResource->m_sUid );
			pMsg->set_name( pResource->m_sName );
			//将领信息
			for ( int j = 0; j < pDBResource->admiralid_size() && j < pDBResource->admiraltrainlv_size(); j++ )
			{
				pMsg->add_admiralid( pDBResource->admiralid(j) );
				pMsg->add_admiraltrainlv( pDBResource->admiraltrainlv(j) );
			}
			pMsg->set_power(pDBResource->pow());
		}
	}
	
	eh_->sendEventToGamed(e,nGameID);
}

void DealResourceEvent::HandleAttackOver(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceResourceOpt* pRceOpt = e->mutable_ce_rceresourceopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseResourceOpt* pRseOpt = e->mutable_se_rseresourceopt();
	pRceOpt->set_type(ROT_AttackOverCountry);
	time_t tNow = time(NULL);

	GCG_ResourceOpt* pResourceMsg = e->mutable_resourceopt();

	int nResourceID = pResourceMsg->resourceid();
	CResource* pResource = pRDH->GetCResource(nResourceID);
	if ( pResource == NULL )
	{
		return;
	}
	
	string sAtkID = pResourceMsg->selfuid();
	string sDefID = pResourceMsg->enemyuid();
	string sName = pResourceMsg->selfname();

	int nLv = nResourceID%MAX_RESOURSE_TYPE_ID;
	nLv /= MAX_RESOURSE_SAVE_ID;
	const ResourceTypeTbl* pTbl = ResourceCfg::Instance().GetResourceTypeTbl( nLv );
	if ( pTbl == NULL )
	{
		return;
	}
	
	if ( sDefID.size() == 0 )
	{
		if ( !pResource->IsEmpty(tNow) )
		{
			pRseOpt->set_ret(7);

			for ( int i = 0; i < pRceOpt->getid_size() && i < RESOURCE_SEARCH_CNT; i++ )
			{
				int nGetID = pRceOpt->getid(i);
				CResource* pResource = pRDH->GetCResource( nGetID );
				if ( pResource == NULL )
				{
					//未知资源ID
					continue;
				}

				DB_C_Resource* pDBResource = pResource->m_pDBResource;
				if ( pDBResource == NULL )
				{
					return;
				}

				ResourceMsg* pMsg = pRseOpt->add_resourcemsg();
				pMsg->set_id( pResource->m_nID );
				pMsg->set_uid( pResource->m_sUid );
				pMsg->set_name( pResource->m_sName );
				//将领信息
				for ( int j = 0; j < pDBResource->admiralid_size() && j < pDBResource->admiraltrainlv_size(); j++ )
				{
					pMsg->add_admiralid( pDBResource->admiralid(j) );
					pMsg->add_admiraltrainlv( pDBResource->admiraltrainlv(j) );
				}
				pMsg->set_power(pDBResource->pow());
			}

			eh_->sendEventToGamed(e,nGameID);
			return;
		}

		pResource->PlayerChange( sAtkID, sName, tNow + pTbl->m_nControlTime );
		pResourceMsg->set_endtime( tNow + pTbl->m_nControlTime );
		FillResourceData(pResource,pResourceMsg,tNow + pTbl->m_nControlTime);

		pRseOpt->set_resourceid( pRceOpt->resourceid() );
		pRseOpt->set_resourceendtime( pResource->m_nEndTime );

		pRDH->markResourceInfoDirty();
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	else
	{
		if ( pResource->IsEmpty(tNow) )
		{
			pResource->PlayerChange( sAtkID, sName, tNow + pTbl->m_nControlTime );
			pResourceMsg->set_endtime( tNow + pTbl->m_nControlTime );
			FillResourceData(pResource,pResourceMsg,tNow + pTbl->m_nControlTime);

			pRseOpt->set_resourceid( pRceOpt->resourceid() );
			pRseOpt->set_resourceendtime( pResource->m_nEndTime );

			pRDH->markResourceInfoDirty();
			eh_->sendEventToGamed(e,nGameID);
			return;
		}

		if ( !pResource->IsGetByUser(sDefID,tNow) )
		{
			pRseOpt->set_ret(8);

			for ( int i = 0; i < pRceOpt->getid_size() && i < RESOURCE_SEARCH_CNT; i++ )
			{
				int nGetID = pRceOpt->getid(i);
				CResource* pResource = pRDH->GetCResource( nGetID );
				if ( pResource == NULL )
				{
					//未知资源ID
					continue;
				}

				DB_C_Resource* pDBResource = pResource->m_pDBResource;
				if ( pDBResource == NULL )
				{
					return;
				}

				ResourceMsg* pMsg = pRseOpt->add_resourcemsg();
				pMsg->set_id( pResource->m_nID );
				pMsg->set_uid( pResource->m_sUid );
				pMsg->set_name( pResource->m_sName );
				//将领信息
				for ( int j = 0; j < pDBResource->admiralid_size() && j < pDBResource->admiraltrainlv_size(); j++ )
				{
					pMsg->add_admiralid( pDBResource->admiralid(j) );
					pMsg->add_admiraltrainlv( pDBResource->admiraltrainlv(j) );
				}
				pMsg->set_power(pDBResource->pow());
			}

			eh_->sendEventToGamed(e,nGameID);
			return;
		}

		if ( pResourceMsg->batkwin() )
		{
			pResource->PlayerChange( sAtkID, sName, tNow + pTbl->m_nControlTime );
			pResourceMsg->set_endtime( tNow + pTbl->m_nControlTime );
			FillResourceData(pResource,pResourceMsg,tNow + pTbl->m_nControlTime);

			pRseOpt->set_resourceid( pRceOpt->resourceid() );
			pRseOpt->set_resourceendtime( pResource->m_nEndTime );
		}
		else
		{
			//战败,更新被攻击者资料
			FillResourceData(pResource,pResourceMsg,tNow + pTbl->m_nControlTime,false);
			pRseOpt->set_resourceid( pRceOpt->resourceid() );
			pRseOpt->set_resourceendtime( pResource->m_nEndTime );
		}

		eh_->sendEventToGamed(e,nGameID);

		//发送给被攻击者
		int64 enemyUid = 0;
		safe_atoll(sDefID,enemyUid);

		pRceOpt->set_type(ROT_AttackOverCountryEnemy);
		e->set_uid(enemyUid);

		pRDH->markResourceInfoDirty();
		eh_->sendEventToGamed(e,eh_->getGamedIdByUserId(enemyUid));

		return;
	}
	
}

void DealResourceEvent::HandleLeave(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceResourceOpt* pRceOpt = e->mutable_ce_rceresourceopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseResourceOpt* pRseOpt = e->mutable_se_rseresourceopt();
	time_t tNow = time(NULL);

	if ( pRceOpt->resourceid() == 0 )
	{
		return;
	}

	CResource* pResource = pRDH->GetCResource( pRceOpt->resourceid() );
	if ( pResource == NULL )
	{
		//未知资源ID
		return;
	}

	if ( pResource->IsGetByUser( toString(e->uid()), tNow ) )
	{
		pResource->PlayerLeave();
		pRseOpt->set_resourceid( 0 );
		pRseOpt->set_resourceendtime( 0 );
	}
	else
	{
		pRseOpt->set_ret(6);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	eh_->sendEventToGamed(e,nGameID);
}

void DealResourceEvent::HandleUpdate(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceResourceOpt* pRceOpt = e->mutable_ce_rceresourceopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseResourceOpt* pRseOpt = e->mutable_se_rseresourceopt();
	time_t tNow = time(NULL);

	if ( pRceOpt->resourceid() != 0 )
	{
		//玩家有数据
		CResource* pResource = pRDH->GetCResource( pRceOpt->resourceid() );
		if ( pResource == NULL )
		{
			//未知资源ID
			return;
		}

		if ( pResource->IsGetByUser( toString(e->uid()), tNow ) )
		{
			pRseOpt->set_resourceid( pRceOpt->resourceid() );
			pRseOpt->set_resourceendtime( pResource->m_nEndTime );
		}
		else
		{
			pRseOpt->set_resourceid( 0 );
			pRseOpt->set_resourceendtime( 0 );
		}
	}

	for ( int i = 0; i < pRceOpt->getid_size() && i < RESOURCE_SEARCH_CNT; i++ )
	{
		int nGetID = pRceOpt->getid(i);
		CResource* pResource = pRDH->GetCResource( nGetID );
		if ( pResource == NULL )
		{
			//未知资源ID
			continue;
		}

		DB_C_Resource* pDBResource = pResource->m_pDBResource;
		if ( pDBResource == NULL )
		{
			return;
		}

		ResourceMsg* pMsg = pRseOpt->add_resourcemsg();
		pMsg->set_id( pResource->m_nID );
		pMsg->set_uid( pResource->m_sUid );
		pMsg->set_name( pResource->m_sName );
		//将领信息
		for ( int j = 0; j < pDBResource->admiralid_size() && j < pDBResource->admiraltrainlv_size(); j++ )
		{
			pMsg->add_admiralid( pDBResource->admiralid(j) );
			pMsg->add_admiraltrainlv( pDBResource->admiraltrainlv(j) );
		}
		pMsg->set_power(pDBResource->pow());
		//将领信息
	}

	eh_->sendEventToGamed(e,nGameID);
}

void DealResourceEvent::FillResourceData( CResource* pResource, GCG_ResourceOpt* pResourceMsg, int nTime, bool bWin )
{
	if ( pResource == NULL || pResourceMsg == NULL )
	{
		return;
	}
	DB_C_Resource* pDBResource = pResource->m_pDBResource;
	if ( pDBResource == NULL )
	{
		return;
	}

	if ( !bWin )
	{
		pDBResource->clear_admiralid();
		pDBResource->clear_admiraltrainlv();
		for ( int i = 0; i < pResourceMsg->admiralidenemy_size() && pResourceMsg->admirallevelenemy_size() && pResourceMsg->admiraltrainlvenemy_size(); i++ )
		{
			pDBResource->add_admiralid(pResourceMsg->admiralidenemy(i));
			pDBResource->add_admiraltrainlv(pResourceMsg->admiraltrainlvenemy(i));
		}
		pDBResource->set_pow(pResourceMsg->powenemy());
		return;
	}

	pResource->m_sUid = pResourceMsg->selfuid();
	pResource->m_sName = pResourceMsg->selfname();;
	pResource->m_nEndTime = nTime;

	pDBResource->clear_admiralid();
	pDBResource->clear_admiraltrainlv();
	for ( int i = 0; i < pResourceMsg->admiralid_size() && pResourceMsg->admirallevel_size() && pResourceMsg->admiraltrainlv_size(); i++ )
	{
		pDBResource->add_admiralid(pResourceMsg->admiralid(i));
		pDBResource->add_admiraltrainlv(pResourceMsg->admiraltrainlv(i));
	}
	pDBResource->set_pow(pResourceMsg->pow());
	
}
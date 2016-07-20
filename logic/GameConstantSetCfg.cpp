#include "GameConstantSetCfg.h"
#include "../common/DBC.h"
#include "../common/string-util.h"

const char* GameConstantSetCfg::szGameConstantSet		= "Config/GameConstantSet.dat";

GameConstantSetCfg::GameConstantSetCfg()
{
	m_mapGameConstantSet.clear();
}

GameConstantSetCfg::~GameConstantSetCfg()
{
	ClearTbl();
}

bool GameConstantSetCfg::LoadTbl()
{
	ClearTbl();

	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szGameConstantSet);
	int nRow = fileDBC.GetRecordsNum();
	for (int i = 0; i < nRow; i++)
	{
		int nID =  fileDBC.Search_Posistion(i, E_GAMECONSTANTSET_LOAD_ID	)->iValue;
		bool bInUse = fileDBC.Search_Posistion(i, E_GAMECONSTANTSET_LOAD_INUSE )->iValue >0 ? true : false;

		if(bInUse == false)
		{
			continue;
		}

		ConstantSetUnit* pNewSet = new ConstantSetUnit();
		if(pNewSet == NULL)
		{
			printf("Load %s cnt:%d,New Failed\n", szGameConstantSet, nRow);
			return false;
		}

		for(int nIndex = 0;nIndex < __GAME_CONSTANTSET_NUM__;nIndex ++)
		{
			pNewSet->m_strArgs[nIndex] = fileDBC.Search_Posistion(i, E_GAMECONSTANTSET_LOAD_STRARG + nIndex*2	)->pString;
			//pNewSet->m_nArgs[nIndex] = fileDBC.Search_Posistion(i, E_GAMECONSTANTSET_LOAD_INTARG + nIndex*2	)->iValue;
		}

		for(int nIndex = 0;nIndex < __GAME_CONSTANTSET_NUMN__;nIndex ++)
		{
			//pNewSet->m_strArgs[nIndex] = fileDBC.Search_Posistion(i, E_GAMECONSTANTSET_LOAD_STRARG + nIndex*2	)->pString;
			pNewSet->m_nArgs[nIndex] = fileDBC.Search_Posistion(i, E_GAMECONSTANTSET_LOAD_INTARG + nIndex*2	)->iValue;
		}
		
		switch(nID)
		{
		case 	E_GAMECONSTANTSET_DEF_NONE:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic(nID);
			}
			break;
		case	E_GAMECONSTANTSET_DEF_FIGHTVALUE_FILTER:
		case	E_GAMECONSTANTSET_DEF_ADMIRALSCORE_FILTER:
		case	E_GAMECONSTANTSET_DEF_ALLUSEADMIRALSCORE_FILTER:
		case    E_GAMECONSTANTSET_DEF_PVERANK_FILTER:
		case    E_GAMECONSTANTSET_DEF_HEROCOMBAT_FILTER:
		case    E_GAMECONSTANTSET_DEF_ALLUSEHEROCOMBAT_FILTER:

			{
				pNewSet->m_pLogic = new GameConstantSetLogic_FightValueFilter(nID);
			}
			break;
		case   E_GAMECONSTANTSET_DEF_WAIGUAKICKNUM:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_WaiGuaKick(nID);
			}
			break;
		case  E_GAMECONSTANTSET_DEF_STRATEGICPATA:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_PVEPaTa(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_STRATEGICGENERAL:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_StrategicGeneral(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_EXPLORE:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_Explore(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_ARENACFG:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_ArenaConfig(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_WORLDARENACFG:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_WorldArena(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_HEROARENACFG:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_HeroArena(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_OFFICER:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_Officer(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_REGIONARENACFG:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_RegionArena(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_NPCDEFIENCE:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_PlayerDefenseNpc(nID);
			}
			break;
        case E_GAMECONSTANTSET_DEF_EXPLOREASRENAL:
        case E_GAMECONSTANTSET_DEF_ALLPEOPLELOTTERY:
            {
                pNewSet->m_pLogic = new GameConstantSetLogic_ExploreAsrenal(nID);
            }
            break;
        case E_GAMECONSTANTSET_DEF_COSTCREDITGIFT:
            {
                pNewSet->m_pLogic = new GameConstantSetLogic_CostCreditGift(nID);
            }
            break;
        case E_GAMECONSTANTSET_DEF_ADMIRALVISITFULL:
            {
                pNewSet->m_pLogic = new GameConstantSetLogic_AdmiralSoulBuy(nID);
            }
            break;
		case E_GAMECONSTANTSET_DEF_REBATE:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_CreditRebate(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_HAPPY_CARD:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_HappyCard(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_GET_ADMIRAL:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_GetAdmiral(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_WEAPON_SEARCH:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_WeaponSearch(nID);
			}
			break;
		case E_GAMECONSTANTSET_OPEN_BOX:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_OpenBox(nID);
			}
			break;
		case E_GAMECONSTANTSET_DEF_NEW_RECALL:
			{
				pNewSet->m_pLogic = new GameConstantSetLogic_NewReCall(nID);
			}
			break;
		default:
			{
				pNewSet->m_pLogic = NULL;
			}
			break;
		}

		if(pNewSet->m_pLogic)
		{
			pNewSet->m_pLogic->Init(pNewSet->m_strArgs,pNewSet->m_nArgs);
		}

		m_mapGameConstantSet[nID] = pNewSet;
	}
	printf("Load %s cnt:%d\n", szGameConstantSet, nRow);
	return true;
}

void GameConstantSetCfg::ClearTbl()
{
	map<int,ConstantSetUnit*>::iterator itr = m_mapGameConstantSet.begin();
	for(;itr != m_mapGameConstantSet.end();itr++)
	{
		ConstantSetUnit* pUnit = itr->second;
		if(pUnit != NULL)
		{
			if(pUnit->m_pLogic != NULL)
			{
				delete pUnit->m_pLogic;
				pUnit->m_pLogic = NULL;
			}

			delete pUnit;
			pUnit = NULL;
		}
	}
	m_mapGameConstantSet.clear();
}

ConstantSetUnit* GameConstantSetCfg::GetGameConstantSet(int nID)
{
	map<int,ConstantSetUnit*>::iterator itr = m_mapGameConstantSet.find(nID);

	if ( itr == m_mapGameConstantSet.end() )
	{
		return NULL;
	}

	return m_mapGameConstantSet[nID];
}

int GameConstantSetCfg::GetDoubleElevenOfficerReward(int nShopID, int nDay)
{
	if(nDay<=0 || nDay>5)
		return 0;

	ConstantSetUnit* pGameInfo = NULL;
	for(int i=E_GAMECONSTANTSET_DEF_DOUBLEELEVENOFFICER_1; i<E_GAMECONSTANTSET_DEF_DOUBLEELEVENOFFICER_3+1; i++)
	{
		ConstantSetUnit* pConstant = GetGameConstantSet(i);
		if(pConstant == NULL)
			return 0;

		if(pConstant->m_nArgs[__GAME_CONSTANTSET_NUMN__-1] == nShopID)
			pGameInfo = GetGameConstantSet(i);
	}

	if(pGameInfo == NULL)
		return 0;

	return pGameInfo->m_nArgs[nDay-1];
	
}


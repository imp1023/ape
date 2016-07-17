#include "NpcInitItemTbl.h"
#include "../common/DBC.h"
#include "../common/string-util.h"

const char* NPCInitItemTbl::szConfigFile = "Config/NpcItem.dat";

NPCInitItemTbl::NPCInitItemTbl(void)
{
}

NPCInitItemTbl::~NPCInitItemTbl(void)
{
	Clear();
}

void NPCInitItemTbl::LoadInfo()
{
	Clear();

	DBCFile fileDBC(0);
	if( fileDBC.OpenFromTXT(szConfigFile) == false)
	{
		return;
	}

	int nRow = fileDBC.GetRecordsNum();
	if( nRow <= 0 )
	{
		return;
	}

	for (int i = 0; i < nRow; i++)
	{
		CFG_NPCInitItem *pItem = new CFG_NPCInitItem();
		int nId = fileDBC.Search_Posistion(i, 0)->iValue;
		pItem->npcId = fileDBC.Search_Posistion(i, 1)->pString;
		pItem->sid = fileDBC.Search_Posistion(i, 2)->iValue;
		pItem->sku = fileDBC.Search_Posistion(i, 3)->iValue;
		pItem->type = fileDBC.Search_Posistion(i, 4)->iValue;
		pItem->upgradeId = fileDBC.Search_Posistion(i, 5)->iValue;
		pItem->isFlipped = fileDBC.Search_Posistion(i, 6)->iValue;
		pItem->energy = fileDBC.Search_Posistion(i, 7)->iValue;
		pItem->x = fileDBC.Search_Posistion(i, 8)->iValue;
		pItem->y = fileDBC.Search_Posistion(i, 9)->iValue;
		pItem->state = fileDBC.Search_Posistion(i, 10)->iValue;
		pItem->time = fileDBC.Search_Posistion(i, 11)->iValue;

		string strNpc = pItem->npcId.substr(0, 5);
		int nNpcId = 0;
		safe_atoi(pItem->npcId.substr(5,1), nNpcId);
		if("npc_B" == strNpc){
			map<int, vector<CFG_NPCInitItem*>>::iterator iter = m_mNpcBInitItems.find(nNpcId);
			if(iter == m_mNpcBInitItems.end()){
				vector<CFG_NPCInitItem*> vec;
				vec.push_back(pItem);
				m_mNpcBInitItems.insert(make_pair(nNpcId, vec));
			}else{
				iter->second.push_back(pItem);
			}
		}else if("npc_C" == strNpc){
			map<int, vector<CFG_NPCInitItem*>>::iterator iter = m_mNpcCInitItems.find(nNpcId);
			if(iter == m_mNpcCInitItems.end()){
				vector<CFG_NPCInitItem*> vec;
				vec.push_back(pItem);
				m_mNpcCInitItems.insert(make_pair(nNpcId, vec));
			}else{
				iter->second.push_back(pItem);
			}
		}else if("npc_D" == strNpc){
			map<int, vector<CFG_NPCInitItem*>>::iterator iter = m_mNpcDInitItems.find(nNpcId);
			if(iter == m_mNpcDInitItems.end()){
				vector<CFG_NPCInitItem*> vec;
				vec.push_back(pItem);
				m_mNpcDInitItems.insert(make_pair(nNpcId, vec));
			}else{
				iter->second.push_back(pItem);
			}
		}
	}
	printf("Load %s cnt:%d\n", NPCInitItemTbl::szConfigFile, nRow);
}

void NPCInitItemTbl::Clear()
{
	for(map<int, vector<CFG_NPCInitItem*>>::iterator iter = m_mNpcBInitItems.begin(); iter != m_mNpcBInitItems.end(); iter++){
		vector<CFG_NPCInitItem*>& vec = iter->second;
		for(int i = 0; i < vec.size(); i++){
			delete vec[i];
		}
	}
	m_mNpcBInitItems.clear();
	for(map<int, vector<CFG_NPCInitItem*>>::iterator iter = m_mNpcCInitItems.begin(); iter != m_mNpcCInitItems.end(); iter++){
		vector<CFG_NPCInitItem*>& vec = iter->second;
		for(int i = 0; i < vec.size(); i++){
			delete vec[i];
		}
	}
	m_mNpcCInitItems.clear();
	for(map<int, vector<CFG_NPCInitItem*>>::iterator iter = m_mNpcDInitItems.begin(); iter != m_mNpcDInitItems.end(); iter++){
		vector<CFG_NPCInitItem*>& vec = iter->second;
		for(int i = 0; i < vec.size(); i++){
			delete vec[i];
		}
	}
	m_mNpcDInitItems.clear();
}

bool NPCInitItemTbl::GetItems(string npc, int npcId, vector<CFG_NPCInitItem*>& items)
{
	if("npc_B" == npc){
		if(npcId < 0 || npcId > NPC_B_MAX_NPCID){
			return false;
		}
		map<int, vector<CFG_NPCInitItem*>>::iterator iter = m_mNpcBInitItems.find(npcId);
		if(iter == m_mNpcBInitItems.end()){
			return false;
		}
		items = iter->second;
		return true;
	}else if("npc_C" == npc){
		if(npcId < 0 || npcId > NPC_C_MAX_NPCID){
			return false;
		}
		map<int, vector<CFG_NPCInitItem*>>::iterator iter = m_mNpcCInitItems.find(npcId);
		if(iter == m_mNpcCInitItems.end()){
			return false;
		}
		items = iter->second;
		return true;
	}else if("npc_D" == npc){
		if(npcId < 0 || npcId > NPC_D_MAX_NPCID){
			return false;
		}
		map<int, vector<CFG_NPCInitItem*>>::iterator iter = m_mNpcDInitItems.find(npcId);
		if(iter == m_mNpcDInitItems.end()){
			return false;
		}
		items = iter->second;
		return true;
	}
	return false;
}
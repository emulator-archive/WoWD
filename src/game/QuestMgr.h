// Copyright (C) 2004 WoW Daemon
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef __QUESTMGR_H
#define __QUESTMGR_H

struct QuestRelation
{
    Quest *qst;
    uint8 type;
};

class Item;

typedef std::list<QuestRelation *> QuestRelationList;


class WOWD_SERVER_DECL QuestMgr :  public Singleton < QuestMgr >
{
public:

    ~QuestMgr();

    void AddQuest(Quest* quest)
    {
        m_quests[quest->id] = quest;
    }

    Quest* FindQuest(uint32 questid)
    {
        HM_NAMESPACE::hash_map<uint32, Quest*>::iterator itr = m_quests.find(questid);
        if(itr == m_quests.end())
            return NULL;
        else
            return itr->second;
    }

    // Quest Loading
    bool LoadSQLQuests();

    uint32 PlayerMeetsReqs(Player* plr, Quest* qst);

    uint32 CalcStatus(Object* quest_giver, Player* plr);
    uint32 CalcQuestStatus(Object* quest_giver, Player* plr, QuestRelation* qst);
    uint32 CalcQuestStatus(Object* quest_giver, Player* plr, Quest* qst, uint8 type);
    uint32 ActiveQuestsCount(Object* quest_giver, Player* plr);

    //Packet Forging...
    void BuildOfferReward(WorldPacket* data,Quest* qst, Object* qst_giver, uint32 menutype);
    void BuildQuestDetails(WorldPacket* data, Quest* qst, Object* qst_giver, uint32 menutype);    
    void BuildRequestItems(WorldPacket* data, Quest* qst, Object* qst_giver, uint32 menutype);
    void BuildQuestComplete(WorldPacket* data, Quest* qst);
    void BuildQuestList(WorldPacket* data, Object* qst_giver, Player* plr);
    bool OnActivateQuestGiver(Object *qst_giver, Player *plr);

    void SendQuestUpdateAddKill(Player* plr, uint32 questid, uint32 entry, uint32 count, uint32 tcount, uint64 guid);
    void BuildQuestUpdateAddItem(WorldPacket* data, uint32 itemid, uint32 count);
    void BuildQuestUpdateComplete(WorldPacket* data, Quest* qst);
    void BuildQuestFailed(WorldPacket* data, uint32 questid);

    bool OnGameObjectActivate(Player *plr, GameObject *go);
    void OnPlayerKill(Player* plr, Creature* victim);
    void OnPlayerItemPickup(Player* plr, Item* item);
    void OnPlayerExploreArea(Player* plr, uint32 AreaID);
    void OnQuestFinished(Player* plr, Quest* qst, Object *qst_giver, uint32 reward_slot);

    uint32 GenerateQuestXP(Player *pl, Quest *qst);

    void SendQuestInvalid( INVALID_REASON reason, Player *plyr);
    void SendQuestFailed(FAILED_REASON failed, Player *plyr);
    void SendQuestUpdateFailed(Quest *pQuest, Player *plyr);
    void SendQuestUpdateFailedTimer(Quest *pQuest, Player *plyr);
    void SendQuestLogFull(Player *plyr);
    
    void LoadNPCQuests(Creature *qst_giver);
    void LoadGOQuests(GameObject *go);

    QuestRelationList* GetCreatureQuestList(uint32 entryid);
       QuestRelationList* GetGOQuestList(uint32 entryid);
    uint32 GetGameObjectLootQuest(uint32 GO_Entry);
    void SetGameObjectLootQuest(uint32 GO_Entry, uint32 Item_Entry);
    inline bool IsQuestRepeatable(Quest *qst) { return qst->is_repeatable; }

    inline int32 QuestHasMob(Quest* qst, uint32 mob)
    {
        for(uint32 i = 0; i < 4; ++i)
            if(qst->required_mob[i] == mob)
                return qst->required_mobcount[i];
        return -1;
    }

    inline int32 GetOffsetForMob(Quest *qst, uint32 mob)
    {
        for(uint32 i = 0; i < 4; ++i)
            if(qst->required_mob[i] == mob)
                return i;

        return -1;
    }

    inline int32 GetOffsetForItem(Quest *qst, uint32 itm)
    {
        for(uint32 i = 0; i < 4; ++i)
            if(qst->required_item[i] == itm)
                return i;

        return -1;
    }

    inline HM_NAMESPACE::hash_map<uint32, Quest*>::iterator Begin() { return m_quests.begin(); }
    inline HM_NAMESPACE::hash_map<uint32, Quest*>::iterator End() { return m_quests.end(); }

private:

    HM_NAMESPACE::hash_map<uint32, Quest*> m_quests;

    HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* > m_npc_quests;
    HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* > m_obj_quests;
    HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* > m_itm_quests;

    HM_NAMESPACE::hash_map<uint32, uint32>          m_ObjectLootQuestList;

    template <class T> void _AddQuest(uint32 entryid, Quest *qst, uint8 type);

    template <class T> HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* >& _GetList();

    // Quest Loading
    void _RemoveChar(char* c, std::string *str);
    void _CleanLine(std::string *str);
};

template<> inline HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* >& QuestMgr::_GetList<Creature>()
    {return m_npc_quests;}
template<> inline HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* >& QuestMgr::_GetList<GameObject>()
    {return m_obj_quests;}
template<> inline HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* >& QuestMgr::_GetList<Item>()
    {return m_itm_quests;}


#define sQuestMgr QuestMgr::getSingleton()

#endif

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

#ifndef WOWSERVER_QUEST_H
#define WOWSERVER_QUEST_H

using namespace std;

enum QUEST_STATUS
{
    QMGR_QUEST_NOT_AVAILABLE            = 0x00,    // There aren't quests avaiable.                   | "No Mark"
    QMGR_QUEST_AVAILABLELOW_LEVEL       = 0x01,    // Quest avaiable, and your level isnt enough.     | "Gray Quotation Mark !"
    QMGR_QUEST_CHAT                     = 0x02,    // Quest avaiable it shows a talk baloon.          | "No Mark"
    QMGR_QUEST_NOT_FINISHED             = 0x03,    // Quest isnt finished yet.                        | "Gray Question ? Mark"
    QMGR_QUEST_REPEATABLE               = 0x04,    // Quest repeatable                                | "Blue Question ? Mark" 
    QMGR_QUEST_AVAILABLE                = 0x05,    // Quest avaiable, and your level is enough        | "Yellow Quotation ! Mark" 
    QMGR_QUEST_FINISHED                 = 0x06,    // Quest has been finished.                        | "Yellow Question  ? Mark"
    //QUEST_ITEM_UPDATE                 = 0x06     // Yellow Question "?" Mark. //Unknown
};

enum QUESTGIVER_QUEST_TYPE
{
    QUESTGIVER_QUEST_START  = 0x01,
    QUESTGIVER_QUEST_END    = 0x02,
};

enum QUEST_TYPE
{
    QUEST_GATHER    = 0x01,
    QUEST_SLAY      = 0x02,
};

enum QUEST_FLAG
{
    QUEST_FLAG_NONE          = 0,   
    QUEST_FLAG_DELIVER       = 1,   
    QUEST_FLAG_KILL          = 2,   
    QUEST_FLAG_SPEAKTO       = 4,
    QUEST_FLAG_REPEATABLE    = 8,   
    QUEST_FLAG_EXPLORATION   = 16,
    QUEST_FLAG_TIMED         = 32,  
    QUEST_FLAG_REPUTATION    = 128,
};

enum FAILED_REASON
{
    FAILED_REASON_FAILED            = 0,
    FAILED_REASON_INV_FULL            = 4,
    FAILED_REASON_DUPE_ITEM_FOUND   = 0x10,
};

enum INVALID_REASON
{
    INVALID_REASON_DONT_HAVE_REQ            = 0,
    INVALID_REASON_DONT_HAVE_LEVEL          = 1,
    INVALID_REASON_DONT_HAVE_RACE           = 6,
    INVALID_REASON_HAVE_TIMED_QUEST         = 12,
    INVALID_REASON_HAVE_QUEST               = 13,
    INVALID_REASON_DONT_HAVE_REQ_ITEMS      = 0x13,
    INVALID_REASON_DONT_HAVE_REQ_MONEY      = 0x15,
};

struct Quest
{
    uint32 id;
    uint32 quest_sort;
    uint32 zone_id;
    uint32 quest_flags;
    uint32 min_level;
    uint32 max_level;
    uint32 type;
    uint32 required_races;
    uint32 required_class;
    uint32 required_tradeskill;
    uint32 required_rep_faction;
    uint32 required_rep_value;

    uint32 time;
    uint32 special_flags;
    
    uint32 previous_quest_id;
    uint32 next_quest_id;

    uint32 srcitem;
    uint32 srcitemcount;

    string title;
    string details;
    string objectives;
    string completiontext;
    string incompletetext;
    string endtext;

    string objectivetexts[4];

    uint32 required_item[4];
    uint32 required_itemcount[4];
    uint32 count_required_item;

    uint32 required_mob[4];
    uint32 required_mobtype[4];
    uint32 required_mobcount[4];
    uint32 count_required_mob;

    uint32 reward_choiceitem[6];
    uint32 reward_choiceitemcount[6];
    uint32 count_reward_choiceitem;

    uint32 reward_item[4];
    uint32 reward_itemcount[4];
    uint32 count_reward_item;

    uint32 reward_repfaction[2];
    uint32 reward_repvalue[2];

    uint32 reward_money;
    uint32 reward_xp_as_money;
    uint32 effect_on_player;
    uint32 reward_xp;
    uint32 reward_spell;

    uint32 point_mapid;
    uint32 point_x;
    uint32 point_y;
    uint32 point_opt;

    uint32 required_money;

    uint32 required_quests[4];
    uint32 required_triggers[4];
    uint32 count_requiredquests;
    uint32 count_requiredtriggers;

    uint32 receive_items[4];
    uint32 receive_itemcount[4];
    uint32 count_receiveitems;

    int is_repeatable;
};

enum QUEST_MOB_TYPES
{
    QUEST_MOB_TYPE_CREATURE = 1,
    QUEST_MOB_TYPE_GAMEOBJECT = 2,
};

class QuestLogEntry
{
    friend class QuestMgr;

public:

    QuestLogEntry();
    ~QuestLogEntry();

    inline Quest* GetQuest() { return m_quest; };
    void Init(Quest* quest, Player* plr, uint32 slot);

    bool CanBeFinished();
    void SubtractTime(uint32 value);
    void SaveToDB();
    bool LoadFromDB(Field *fields);
    void UpdatePlayerFields();

    void SetTrigger(uint32 i);
    void SetMobCount(uint32 i, uint32 count);

    void SetSlot(int32 i);
    void Finish();

    void SendQuestComplete();
    void SendUpdateAddKill(uint32 i);
    inline uint32 GetMobCount(uint32 i) { return m_mobcount[i]; }
    inline uint32 GetExploredAreas(uint32 i) { return m_explored_areas[i]; }

    inline uint32 GetBaseField(uint32 slot)
    {
        return PLAYER_QUEST_LOG_1_1 + (m_slot * 3);
    }

private:

    bool mInitialized;
    bool mDirty;

    Quest *m_quest;
    Player *m_plr;
    
    uint32 m_mobcount[4];
    uint32 m_explored_areas[4];

    uint32 m_time_left;
    int32 m_slot;
};

WorldPacket* BuildQuestQueryResponse(Quest *qst);

#endif

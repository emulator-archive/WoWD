// script_src.cpp : Defines the entry point for the DLL application.
//

#include <vector>
#include "game/StdAfx.h"

extern "C" WOWD_SCRIPT_DECL GossipScript* CreateGossipScript(uint32 uEntryId);
extern "C" WOWD_SCRIPT_DECL void ScriptInitialize(ScriptModule *mod);

// Build version info function
BUILD_VERSIONINFO_DATA(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);

class WOWD_SCRIPT_DECL WarsongGulchBattlemaster : public GossipScript
{
public:
    void GossipHello(Creature * pCreature, Player * plr, bool AutoSend)
    {
        script_debuglog("Gossip-Battlemaster.cpp : WSG Hello by %s", plr->GetName());

        GossipMenu *Menu;
        uint32 Team = plr->GetTeam();
        if(Team > 1) Team = 1;
        
        // Check if the player can be entered into the bg or not.
        if(plr->getLevel() < 10)
        {
            uint32 FactMessages[2] = { 7599, 7688 };

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);

            if(Rand(30.0f))    // 30% chance to send message
            {
                char message[200];
                sprintf(message, "Sorry, %s, you are not skilled enough to enter the hazards of Warsong Gulch. Come back when you have trained some more.", plr->GetName());
                pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message);
            }
        }
        else
        {
            uint32 FactMessages[2] = { 7689, 7705 }; // need to find the second one

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);
            Menu->AddItem( 0, "I would like to enter the battleground.", 1);
        }
        
        if(AutoSend)
            Menu->SendTo(plr);
    }

    void GossipSelectOption(Creature * pCreature, Player * plr, uint32 Id, uint32 IntId)
    {
        // Check the player is capable.
        if(plr->getLevel() < 10)
        {
            // We shouldn't actually get here.. it means they've cheated with packets or
            // are using a bot :p
            pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You cannot enter the battleground." );
        }

        // Send battleground list.
        plr->GetSession()->SendBattlegroundList(pCreature, 2);  // WSG = 2
    }

    void Destroy()
    {
        delete this;
    }
};

class WOWD_SCRIPT_DECL ArathiBasinBattlemaster : public GossipScript
{
public:
    void GossipHello(Creature * pCreature, Player * plr, bool AutoSend)
    {
        script_debuglog("Gossip-Battlemaster.cpp : AB Hello by %s", plr->GetName());

        GossipMenu *Menu;
        uint32 Team = plr->GetTeam();
        if(Team > 1) Team = 1;

        // Check if the player can be entered into the bg or not.
        if(plr->getLevel() < 20)
        {
            uint32 FactMessages[2] = { 7700, 7667 };

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);

            if(Rand(30.0f))    // 30% chance to send message
            {
                char message[200];
                sprintf(message, "You cannot help our struggle yet, %s. Once you are stronger come back and join the fight!", plr->GetName());
                pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message);
            }
        }
        else
        {
            uint32 FactMessages[2] = { 7700, 7555 }; // need to find the second one

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);
            Menu->AddItem( 0, "I would like to enter the battleground.", 1);
        }

        if(AutoSend)
            Menu->SendTo(plr);
    }

    void GossipSelectOption(Creature * pCreature, Player * plr, uint32 Id, uint32 IntId)
    {
        // Check the player is capable.
        if(plr->getLevel() < 20)
        {
            // We shouldn't actually get here.. it means they've cheated with packets or
            // are using a bot :p
            pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You cannot enter the battleground." );
        }

        // Send battleground list.
        plr->GetSession()->SendBattlegroundList(pCreature, 3);  // WSG = 3
    }

    void Destroy()
    {
        delete this;
    }
};

class WOWD_SCRIPT_DECL AlteracValleyBattlemaster : public GossipScript
{
public:
    void GossipHello(Creature * pCreature, Player * plr, bool AutoSend)
    {
        script_debuglog("Gossip-Battlemaster.cpp : AV Hello by %s", plr->GetName());

        GossipMenu *Menu;
        uint32 Team = plr->GetTeam();
        if(Team > 1) Team = 1;

        // Check if the player can be entered into the bg or not.
        if(plr->getLevel() < 60)
        {
            uint32 FactMessages[2] = { 7658, 7658 };

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);

            if(Rand(30.0f))    // 30% chance to send message
            {
                char message[200];
                sprintf(message, "Only our best warriors can enter Alterac Valley, %s. Come back when you are more experienced.", plr->GetName());
                pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message);
            }
        }
        else
        {
            uint32 FactMessages[2] = { 7658, 7659 }; // need to find the second one

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);
            Menu->AddItem( 0, "I would like to enter the battleground.", 1);
        }

        if(AutoSend)
            Menu->SendTo(plr);
    }

    void GossipSelectOption(Creature * pCreature, Player * plr, uint32 Id, uint32 IntId)
    {
        // Check the player is capable.
        if(plr->getLevel() < 60)
        {
            // We shouldn't actually get here.. it means they've cheated with packets or
            // are using a bot :p
            pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You cannot enter the battleground." );
        }

        // Send battleground list.
        plr->GetSession()->SendBattlegroundList(pCreature, 1);  // AV = 2
    }

    void Destroy()
    {
        delete this;
    }
};

// class WOWD_SCRIPT_DECL ArenaBattlemaster : public GossipScript

GossipScript* CreateGossipScript(uint32 uEntryId)
{
    switch(uEntryId)
    {
    // WSG battlemasters
    case 15102:
    case 14981:
    case 14982:
    case 2302:
    case 10360:
    case 3890:
        {
            return new WarsongGulchBattlemaster;
        }break;

        // AB battlemasters
    case 15008:
    case 857:
    case 907:
    case 12198:
    case 14990:
    case 15006:
        {
            return new ArathiBasinBattlemaster;
        }break;

        // AV battlemasters
    case 7410:
    case 12197:
    case 5118:
    case 15106:
    case 7247:
    case 14942:
        {
            return new AlteracValleyBattlemaster;
        }break;

    /*    // arena battlemasters
    case 4: 
        {
            return new ArenaBattlemaster;
        }break;*/
    }
}
void ScriptInitialize(ScriptModule *mod)
{
    /* Battlemaster List */
    sScriptMgr.ScriptAddGossipModule(15102, mod);
    sScriptMgr.ScriptAddGossipModule(14981, mod);
    sScriptMgr.ScriptAddGossipModule(14982, mod);
    sScriptMgr.ScriptAddGossipModule(2302, mod);
    sScriptMgr.ScriptAddGossipModule(10360, mod);
    sScriptMgr.ScriptAddGossipModule(3890, mod);
    sScriptMgr.ScriptAddGossipModule(15008, mod);
    sScriptMgr.ScriptAddGossipModule(857, mod);
    sScriptMgr.ScriptAddGossipModule(907, mod);
    sScriptMgr.ScriptAddGossipModule(12198, mod);
    sScriptMgr.ScriptAddGossipModule(14990, mod);
    sScriptMgr.ScriptAddGossipModule(15006, mod);
    sScriptMgr.ScriptAddGossipModule(7410, mod);
    sScriptMgr.ScriptAddGossipModule(12197, mod);
    sScriptMgr.ScriptAddGossipModule(5118, mod);
    sScriptMgr.ScriptAddGossipModule(15106, mod);
    sScriptMgr.ScriptAddGossipModule(7247, mod);
    sScriptMgr.ScriptAddGossipModule(14942, mod);
}
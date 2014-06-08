// script_src.cpp : Defines the entry point for the DLL application.
//

#include <vector>
#include "game/StdAfx.h"

extern "C" WOWD_SCRIPT_DECL GossipScript* CreateGossipScript(uint32 uEntryId);
extern "C" WOWD_SCRIPT_DECL void DestroyGossipScript(GossipScript* Script);
extern "C" WOWD_SCRIPT_DECL void ScriptInitialize(ScriptModule *mod);

// Build version info function
BUILD_VERSIONINFO_DATA(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);

class WOWD_SCRIPT_DECL InnkeeperGossip : public GossipScript
{
public:
    void GossipHello(Creature* pCreature, Player* Plr, bool AutoSend);
    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId);
    void GossipEnd(Creature* pCreature, Player* Plr);
};

#ifdef WIN32

BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
    return TRUE;
}

#endif

void InnkeeperGossip::GossipHello(Creature* pCreature, Player* Plr, bool AutoSend)
{
    script_debuglog("Gossip-Innkeepers.cpp : Hello by %s", Plr->GetName());
    GossipMenu *Menu;
    uint32 TextID = 820;
    // Hello (First) Menu.
    objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), TextID, Plr);

    if( pCreature->GetUInt32Value(UNIT_NPC_FLAGS) & UNIT_NPC_FLAG_VENDOR )
        Menu->AddItem( 1, "I would like to browse your goods.", 1 );
    
    Menu->AddItem( 5, "Make this inn your home.", 2 );
    Menu->AddItem( 0, "What can I do at an inn?", 3 );
    if(AutoSend)
        Menu->SendTo(Plr);
}

#define SendQuickMenu(textid) objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), textid, Plr); \
    Menu->SendTo(Plr);

void InnkeeperGossip::GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
{
    script_debuglog("Gossip-Innkeepers.cpp : Select option %u by %s", IntId, Plr->GetName());
    switch(IntId)
    {
    case 1:     // VENDOR
        Plr->GetSession()->SendInventoryList(pCreature);
        break;
    case 2:     // BINDER
        Plr->GetSession()->SendInnkeeperBind(pCreature);
        break;
    case 3:     // WHAT CAN I DO ?
        // Prepare second menu
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 1853, Plr);
        Menu->AddItem(5, "Make this inn your home.", 2);
        Menu->SendTo(Plr);
        break;
    }
}

void InnkeeperGossip::GossipEnd(Creature* pCreature, Player* Plr)
{
    GossipScript::GossipEnd(pCreature, Plr);
}

GossipScript* CreateGossipScript(uint32 uEntryId)
{
    InnkeeperGossip * SGuard = new InnkeeperGossip();
    return SGuard;
}

void DestroyGossipScript(GossipScript* Script)
{
    delete ((InnkeeperGossip*)Script);
}

void ScriptInitialize(ScriptModule *mod)
{
    /* Innkeeper List */
    sScriptMgr.ScriptAddGossipModule(295, mod);      // Innkeeper Farley
    sScriptMgr.ScriptAddGossipModule(1247, mod);     // Innkeeper Belm
    sScriptMgr.ScriptAddGossipModule(1464, mod);     // Innkeeper Helbrek
    sScriptMgr.ScriptAddGossipModule(2352, mod);     // Innkeeper Anderson
    sScriptMgr.ScriptAddGossipModule(2388, mod);     // Innkeeper Shay
    sScriptMgr.ScriptAddGossipModule(3934, mod);     // Innkeeper Boorand Plainswind
    sScriptMgr.ScriptAddGossipModule(5111, mod);     // Innkeeper Firebrew
    sScriptMgr.ScriptAddGossipModule(5688, mod);     // Innkeeper Renee
    sScriptMgr.ScriptAddGossipModule(5814, mod);     // Innkeeper Thulbek
    sScriptMgr.ScriptAddGossipModule(6272, mod);     // Innkeeper Janene
    sScriptMgr.ScriptAddGossipModule(6727, mod);     // Innkeeper Brianna
    sScriptMgr.ScriptAddGossipModule(6734, mod);     // Innkeeper Hearthstove
    sScriptMgr.ScriptAddGossipModule(6735, mod);     // Innkeeper Saelienne
    sScriptMgr.ScriptAddGossipModule(6736, mod);     // Innkeeper Keldamyr
    sScriptMgr.ScriptAddGossipModule(6737, mod);     // Innkeeper Shaussiy
    sScriptMgr.ScriptAddGossipModule(6738, mod);     // Innkeeper Kimlya
    sScriptMgr.ScriptAddGossipModule(6739, mod);     // Innkeeper Bates
    sScriptMgr.ScriptAddGossipModule(6740, mod);     // Innkeeper Allison
    sScriptMgr.ScriptAddGossipModule(6741, mod);     // Innkeeper Norman
    sScriptMgr.ScriptAddGossipModule(6746, mod);     // Innkeeper Pala
    sScriptMgr.ScriptAddGossipModule(6747, mod);     // Innkeeper Kauth
    sScriptMgr.ScriptAddGossipModule(6778, mod);     // Melika Isenstrider
    sScriptMgr.ScriptAddGossipModule(6790, mod);     // Innkeeper Trelayne
    sScriptMgr.ScriptAddGossipModule(6791, mod);     // Innkeeper Wiley
    sScriptMgr.ScriptAddGossipModule(6806, mod);     // Tannok Frosthammer
    sScriptMgr.ScriptAddGossipModule(6807, mod);     // Innkeeper Skindle
    sScriptMgr.ScriptAddGossipModule(6928, mod);     // Innkeeper Grosk
    sScriptMgr.ScriptAddGossipModule(6929, mod);     // Innkeeper Gryshka
    sScriptMgr.ScriptAddGossipModule(6930, mod);     // Innkeeper Karakul
    sScriptMgr.ScriptAddGossipModule(7714, mod);     // Innkeeper Byula
    sScriptMgr.ScriptAddGossipModule(7731, mod);     // Innkeeper Jayka
    sScriptMgr.ScriptAddGossipModule(7733, mod);     // Innkeeper Fizzgrimble
    sScriptMgr.ScriptAddGossipModule(7736, mod);     // Innkeeper Shyria
    sScriptMgr.ScriptAddGossipModule(7737, mod);     // Innkeeper Greul
    sScriptMgr.ScriptAddGossipModule(7744, mod);     // Innkeeper Thulfram
    sScriptMgr.ScriptAddGossipModule(8931, mod);     // Innkeeper Heather
    sScriptMgr.ScriptAddGossipModule(9356, mod);     // Innkeeper Shul'kar
    sScriptMgr.ScriptAddGossipModule(9501, mod);     // Innkeeper Adegwa
    sScriptMgr.ScriptAddGossipModule(11103, mod);    // Innkeeper Lyshaerya
    sScriptMgr.ScriptAddGossipModule(11106, mod);    // Innkeeper Sikewa
    sScriptMgr.ScriptAddGossipModule(11116, mod);    // Innkeeper Abeqwa
    sScriptMgr.ScriptAddGossipModule(11118, mod);    // Innkeeper Vizzie
    sScriptMgr.ScriptAddGossipModule(12196, mod);    // Innkeeper Kaylisk
    sScriptMgr.ScriptAddGossipModule(14731, mod);    // Lard
    sScriptMgr.ScriptAddGossipModule(15174, mod);    // Calandrath
}
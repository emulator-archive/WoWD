/************************************************************************/
/* GENERAL GUARD SCRIPT - WoWD Project                                  */
/************************************************************************/

// Covers *all* guard types, scripting their texts to guide players around.
// Enable this define to make all gossip texts have a "back" / "I was looking
// for somethinge else" button added.

#define BACK_BUTTON

#include "game/StdAfx.h"

extern "C" WOWD_SCRIPT_DECL GossipScript* CreateGossipScript(uint32 uEntryId);
extern "C" WOWD_SCRIPT_DECL void DestroyGossipScript(GossipScript* Script);
extern "C" WOWD_SCRIPT_DECL void ScriptInitialize(ScriptModule *mod);

// Build version info function
BUILD_VERSIONINFO_DATA(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);

#ifndef BACK_BUTTON

// Make code neater with this define.
#define SendQuickMenu(textid) objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), textid, Plr); \
    Menu->SendTo(Plr);

#else

// Make code neater with this define.
#define SendQuickMenu(textid) objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), textid, Plr); \
    Menu->AddItem(0, "I was looking for something else.", 0); \
    Menu->SendTo(Plr);

#endif

/************************************************************************/
/* Stormwind CITY Guards                                                */
/************************************************************************/

class WOWD_SCRIPT_DECL StormwindGuard : public GossipScript
{
public:
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2593, Plr);

        Menu->AddItem(0, "Auction House", 1);
        Menu->AddItem(0, "Bank of Stormwind", 2);
        Menu->AddItem(0, "Deeprun Tram", 3);
        Menu->AddItem(0, "The Inn", 4);
        Menu->AddItem(0, "Gryphon Master", 5);    
        Menu->AddItem(0, "Guild Master", 6);
        Menu->AddItem(0, "Mailbox", 7);
        Menu->AddItem(0, "Stable Master", 8);
        Menu->AddItem(0, "Weapon Trainer", 9);
        Menu->AddItem(0, "Officers` Lounge", 10);
        Menu->AddItem(0, "Battlemaster", 11);
        Menu->AddItem(0, "Class Trainer", 12);
        Menu->AddItem(0, "Profession Trainer", 13);

        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // Auction House
            SendQuickMenu(3834);
            Plr->Gossip_SendPOI(-8811.46, 667.46, 6, 6, 0, "Stormwind Auction House");
            break;

        case 2:     // Bank
            SendQuickMenu(764);
            Plr->Gossip_SendPOI(-8916.87, 622.87, 6, 6, 0, "Stormwind Bank");
            break;

        case 3:     // Tram
            SendQuickMenu(3813);
            Plr->Gossip_SendPOI(-8378.88, 554.23, 6, 6, 0, "The Deeprun Tram");
            break;

        case 4:     // Inn
            SendQuickMenu(3860);
            Plr->Gossip_SendPOI(-8869.0, 675.4, 6, 6, 0, "The Gilded Rose");
            break;

        case 5:     // Gryphon
            SendQuickMenu(879);
            Plr->Gossip_SendPOI(-8837.0, 493.5, 6, 6, 0, "Stormwind Gryphon Master");
            break;

        case 6:     // Guildmaster
            SendQuickMenu(882);
            Plr->Gossip_SendPOI(-8894.0, 611.2, 6, 6, 0, "Stormwind Vistor`s Center");
            break;

        case 7:     // Mailbox
            SendQuickMenu(3518);
            Plr->Gossip_SendPOI(-8876.48, 649.18, 6, 6, 0, "Stormwind Mailbox");
            break;

        case 8:     // Stable master
            SendQuickMenu(5984);
            Plr->Gossip_SendPOI(-8433.0, 554.7, 6, 6, 0, "Jenova Stoneshield");
            break;

        case 9:     // weapon master
            SendQuickMenu(4516);
            Plr->Gossip_SendPOI(-8797.0, 612.8, 6, 6, 0, "Woo Ping");
            break;

        case 10:    // officer lounge
            SendQuickMenu(7047);
            Plr->Gossip_SendPOI(-8759.92, 399.69, 6, 6, 0, "Champions` Hall");
            break;

        case 11:    // Battlemaster submenu
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 7499, Plr);
                Menu->AddItem(0, "Alterac Valley", 14);
                Menu->AddItem(0, "Arathi Basin", 15);
                Menu->AddItem(0, "Warsong Gulch", 16);
                Menu->SendTo(Plr);
            }break;

        case 12:    // Class Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);
                Menu->AddItem( 0, "Mage"       , 17);
                Menu->AddItem( 0, "Rogue"      , 18);
                Menu->AddItem( 0, "Warrior"    , 19);
                Menu->AddItem( 0, "Druid"      , 20);
                Menu->AddItem( 0, "Priest"     , 21);
                Menu->AddItem( 0, "Shaman"     , 37);
                Menu->AddItem( 0, "Paladin"    , 22);
                Menu->AddItem( 0, "Hunter"     , 23);
                Menu->AddItem( 0, "Warlock"    , 24);
                Menu->SendTo(Plr);
            }break;

        case 13:    // Profession Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);
                Menu->AddItem( 0, "Alchemy"           , 25);
                Menu->AddItem( 0, "Blacksmithing"     , 26);
                Menu->AddItem( 0, "Cooking"           , 27);
                Menu->AddItem( 0, "Enchanting"        , 28);
                Menu->AddItem( 0, "Engineering"       , 29);
                Menu->AddItem( 0, "First Aid"         , 30);
                Menu->AddItem( 0, "Fishing"           , 31);
                Menu->AddItem( 0, "Herbalism"         , 32);
                Menu->AddItem( 0, "Leatherworking"    , 33);
                Menu->AddItem( 0, "Mining"            , 34);
                Menu->AddItem( 0, "Skinning"          , 35);
                Menu->AddItem( 0, "Tailoring"         , 36);
                Menu->SendTo(Plr);
            }break;

            //////////////////
            // Battlemaster submenu
            ////////

        case 14://AV
            {
                Plr->Gossip_SendPOI(-8443.88, 335.99, 6, 6, 0, "Thelman Slatefist");
                SendQuickMenu(7500);
            }break;

        case 15://AB
            {
                Plr->Gossip_SendPOI(-8443.88, 335.99, 6, 6, 0, "Lady Hoteshem");
                SendQuickMenu(7650);
            }break;

        case 16://WSG
            {
                Plr->Gossip_SendPOI(-8443.88, 335.99, 6, 6, 0, "Elfarran");
                SendQuickMenu(7501);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 17: //Mage
            {
                Plr->Gossip_SendPOI(-9012.0, 867.6, 6, 6, 0, "Wizard`s Sanctum");
                SendQuickMenu(899);
            }break;

        case 18: //Rogue
            {
                Plr->Gossip_SendPOI(-8753.0, 367.8, 6, 6, 0, "Stormwind - Rogue House");
                SendQuickMenu(900);
            }break;

        case 19: //Warrior
            {
                Plr->Gossip_SendPOI(-8624.54, 402.61, 6, 6, 0, "Stormwind Barracks");
                SendQuickMenu(901);
            }break;

        case 20: //Druid
            {
                Plr->Gossip_SendPOI(-8751.0, 1124.5, 6, 6, 0, "The Park");
                SendQuickMenu(902);
            }break;

        case 21: //Priest
            {
                Plr->Gossip_SendPOI(-8512.0, 862.4, 6, 6, 0, "Cathedral Of Light");
                SendQuickMenu(903);
            }break;

        case 22: //Paladin
            {
                Plr->Gossip_SendPOI(-8577.0, 881.7, 6, 6, 0, "Cathedral Of Light");
                SendQuickMenu(904);
            }break;

        case 23: //Hunter
            {
                Plr->Gossip_SendPOI(-8413.0, 541.5, 6, 6, 0, "Hunter Lodge");
                SendQuickMenu(905);
            }break;

        case 24: //Warlock
            {
                Plr->Gossip_SendPOI(-8948.91, 998.35, 6, 6, 0, "The Slaughtered Lamb");
                SendQuickMenu(906);
            }break;

        case 37: // Shaman
            {
                SendQuickMenu(10106);
            }break;

        case 25: //Alchemy
            {
                Plr->Gossip_SendPOI(-8988.0, 759.60, 6, 6, 0, "Alchemy Needs");
                SendQuickMenu(919);
            }break;

        case 26: //Blacksmithing
            {
                Plr->Gossip_SendPOI(-8424.0, 616.9, 6, 6, 0, "Therum Deepforge");
                SendQuickMenu(920);
            }break;

        case 27: //Cooking
            {
                Plr->Gossip_SendPOI(-8611.0, 364.6, 6, 6, 0, "Pig and Whistle Tavern");
                SendQuickMenu(921);
            }break;

        case 28: //Enchanting
            {
                Plr->Gossip_SendPOI(-8858.0, 803.7, 6, 6, 0, "Lucan Cordell");
                SendQuickMenu(941);
            }break;

        case 29: //Engineering
            {
                Plr->Gossip_SendPOI(-8347.0, 644.1, 6, 6, 0, "Lilliam Sparkspindle");
                SendQuickMenu(922);
            }break;

        case 30: //First Aid
            {
                Plr->Gossip_SendPOI(-8513.0, 801.8, 6, 6, 0, "Shaina Fuller");
                SendQuickMenu(923);
            }break;

        case 31: //Fishing
            {
                Plr->Gossip_SendPOI(-8803.0, 767.5, 6, 6, 0, "Arnold Leland");
                SendQuickMenu(940);
            }break;

        case 32: //Herbalism
            {
                Plr->Gossip_SendPOI(-8967.0, 779.5, 6, 6, 0, "Alchemy Needs");
                SendQuickMenu(924);
            }break;

        case 33: //Leatherworking
            {
                Plr->Gossip_SendPOI(-8726.0, 477.4, 6, 6, 0, "The Protective Hide");
                SendQuickMenu(925);
            }break;

        case 34: //Mining
            {
                Plr->Gossip_SendPOI(-8434.0, 692.8, 6, 6, 0, "Gelman Stonehand");
                SendQuickMenu(927);
            }break;

        case 35: //Skinning
            {
                Plr->Gossip_SendPOI(-8716.0, 469.4, 6, 6, 0, "The Protective Hide");
                SendQuickMenu(928);
            }break;

        case 36: //Tailoring
            {
                Plr->Gossip_SendPOI(-8938.0, 800.7, 6, 6, 0, "Duncan`s Textiles");
                SendQuickMenu(929);
            }break;
        }
    }
};

class WOWD_SCRIPT_DECL DarnassusGuard : public GossipScript
{
public:
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3016, Plr);

        Menu->AddItem( 0, "Auction House"      , 1);
        Menu->AddItem( 0, "The Bank"           , 2);
        Menu->AddItem( 0, "Hippogryph Master"  , 3);
        Menu->AddItem( 0, "Guild Master"       , 4);
        Menu->AddItem( 0, "The Inn"            , 5);
        Menu->AddItem( 0, "Mailbox"            , 6);
        Menu->AddItem( 0, "Stable Master"      , 7);
        Menu->AddItem( 0, "Weapons Trainer"    , 8);
        Menu->AddItem( 0, "Battlemaster"       , 9);
        Menu->AddItem( 0, "Class Trainer"      , 10);
	 	Menu->AddItem( 0, "Profession Trainer" , 11);

        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers (Most/All 'borrowed' from scriptdev)
            /////

        case 1:     // Auction House
			SendQuickMenu(3833);
			Plr->Gossip_SendPOI(9861.23, 2334.55, 6, 6, 0, "Darnassus Auction House");
			break;
			
		case 2:		// Bank
			SendQuickMenu(3017);
			Plr->Gossip_SendPOI(9938.45, 2512.35, 6, 6, 0, "Darnassus Bank");
			break;
		
		case 3:		// Wind Master
			SendQuickMenu(3018);
			Plr->Gossip_SendPOI(9945.65, 2618.94, 6, 6, 0, "Rut'theran Village");
			break;
			
		case 4:		// Guild Master
			SendQuickMenu(3019);
			Plr->Gossip_SendPOI(10076.40, 2199.59, 6, 6, 0, "Darnassus Guild Master");
			break;
			
		case 5:		// Inn
			SendQuickMenu(3020);
			Plr->Gossip_SendPOI(10133.29, 2222.52, 6, 6, 0, "Darnassus Inn");
			break;
			
		case 6:		// Mail
			SendQuickMenu(3021);
			Plr->Gossip_SendPOI(9942.17, 2495.48, 6, 6, 0, "Darnassus Mailbox");
			break;
		
		case 7:		// Stables
			SendQuickMenu(5980);
			Plr->Gossip_SendPOI(10167.20, 2522.66, 6, 6, 0, "Alassin");
			break;
			
		case 8:		// Weapons Trainer
			SendQuickMenu(4517);
			Plr->Gossip_SendPOI(9907.11, 2329.70, 6, 6, 0, "Ilyenia Moonfire");
			break;
			
		  case 9:    // Battlemaster submenu
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 7519, Plr);
                Menu->AddItem(0, "Alterac Valley", 12);
                Menu->AddItem(0, "Arathi Basin", 13);
                Menu->AddItem(0, "Warsong Gulch", 14);
                Menu->SendTo(Plr);
            }break;
			
			case 10:    // Class Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);
                Menu->AddItem( 0, "Mage"       , 15);
                Menu->AddItem( 0, "Rogue"      , 16);
                Menu->AddItem( 0, "Warrior"    , 17);
                Menu->AddItem( 0, "Druid"      , 18);
                Menu->AddItem( 0, "Priest"     , 19);
                Menu->AddItem( 0, "Shaman"     , 20);
                Menu->AddItem( 0, "Paladin"    , 21);
                Menu->AddItem( 0, "Hunter"     , 22);
                Menu->AddItem( 0, "Warlock"    , 23);
                Menu->SendTo(Plr);
            }break;

        case 11:    // Profession Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4273, Plr);
                Menu->AddItem( 0, "Alchemy"           , 24);
                Menu->AddItem( 0, "Blacksmithing"     , 25);
                Menu->AddItem( 0, "Cooking"           , 26);
                Menu->AddItem( 0, "Enchanting"        , 27);
                Menu->AddItem( 0, "Engineering"       , 28);
                Menu->AddItem( 0, "First Aid"         , 29);
                Menu->AddItem( 0, "Fishing"           , 30);
                Menu->AddItem( 0, "Herbalism"         , 31);
                Menu->AddItem( 0, "Leatherworking"    , 32);
                Menu->AddItem( 0, "Mining"            , 33);
                Menu->AddItem( 0, "Skinning"          , 34);
                Menu->AddItem( 0, "Tailoring"         , 35);
                Menu->SendTo(Plr);
            }break;
		
			//////////////////
            // Battlemaster subsubmenu (Yes, there's a sub in a sub :<)
            ////////

        case 12://AV
            {
                Plr->Gossip_SendPOI(9923.61, 2327.43, 6, 6, 0, "Brogun Stoneshield");
                SendQuickMenu(7518);
            }break;

        case 13://AB
            {
                Plr->Gossip_SendPOI(9977.37, 2324.39, 6, 6, 0, "Keras Wolfheart");
                SendQuickMenu(7651);
            }break;

        case 14://WSG
            {
                Plr->Gossip_SendPOI(9979.84, 2315.79, 6, 6, 0, "Aethalas");
                SendQuickMenu(7482);
            }break;

			//////////////////
            // Trainer subsubmenu (Yes, there's a sub in a sub :<)
            ////////
		
		case 22:	// Hunter
            {
                Plr->Gossip_SendPOI(10177.29, 2511.10, 6, 6, 0, "Darnassus Hunter Trainer");
                SendQuickMenu(3023);
            }break;
		
		case 18:	// Druid
            {
                Plr->Gossip_SendPOI(10186, 2570.46, 6, 6, 0, "Darnassus Druid Trainer");
                SendQuickMenu(3024);
            }break;

        case 19:	// Priest
            {
                Plr->Gossip_SendPOI(9659.12, 2524.88, 6, 6, 0, "Temple of the Moon");
                SendQuickMenu(3025);
            }break;
	
        case 16:	// Rogue
            {
                Plr->Gossip_SendPOI(10122, 2599.12, 6, 6, 0, "Darnassus Rogue Trainer");
                SendQuickMenu(3026);
            }break;

        case 17:	// Warrior
            {
                Plr->Gossip_SendPOI(9951.91, 2280.38, 6, 6, 0, "Warrior's Terrace");
                SendQuickMenu(3033);
            }break;
		//// Those skill things submenu
			
		case 24: //Alchemy
            {
                Plr->Gossip_SendPOI(10075.90, 2356.76, 6, 6, 0, "Darnassus Alchemy Trainer");
                SendQuickMenu(3035);
            }break;

        case 26: //Cooking
            {
                Plr->Gossip_SendPOI(10088.59, 2419.21, 6, 6, 0, "Darnassus Cooking Trainer");
                SendQuickMenu(3036);
            }break;
			
        case 27: //Enchanting
            {
                Plr->Gossip_SendPOI(10146.09, 2313.42, 6, 6, 0, "Darnassus Enchanting Trainer");
                SendQuickMenu(3337);
            }break;

        case 29: //First Aid
            {
                Plr->Gossip_SendPOI(10150.09, 2390.43, 6, 6, 0, "Darnassus First Aid Trainer");
                SendQuickMenu(3037);
            }break;

        case 30: //Fishing
            {
                Plr->Gossip_SendPOI(9836.20, 2432.17, 6, 6, 0, "Darnassus Fishing Trainer");
                SendQuickMenu(3038);
            }break;

        case 31: //Herbalism
            {
                Plr->Gossip_SendPOI(9757.17, 2430.16, 6, 6, 0, "Darnassus Herbalism Trainer");
                SendQuickMenu(3039);
            }break;

        case 32: //Leatherworking
            {
                Plr->Gossip_SendPOI(10086.59, 2255.77, 6, 6, 0, "Darnassus Leatherworking Trainer");
                SendQuickMenu(3040);
            }break;

        case 34: //Skinning
            {
                Plr->Gossip_SendPOI(10081.40, 2257.18, 6, 6, 0, "Darnassus Skinning Trainer");
                SendQuickMenu(3042);
            }break;

        case 35: //Tailoring
            {
                Plr->Gossip_SendPOI(10079.70, 2268.19, 6, 6, 0, "Darnassus Tailor");
                SendQuickMenu(3044);
            }break;


		}
	}
    void GossipEnd(Creature* pCreature, Player* Plr)
    {

    }
};

class WOWD_SCRIPT_DECL NorthshireGuard : public GossipScript
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

void NorthshireGuard::GossipHello(Creature* pCreature, Player* Plr, bool AutoSend)
{
    script_debuglog("Gossip-Northshire-Guard.cpp : Hello by %s", Plr->GetName());
    GossipMenu *Menu;
    // Hello (First) Menu.
    objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2593, Plr);

    Menu->AddItem( 0, "Bank"                 , 1);
    Menu->AddItem( 0, "Gryphon master"       , 2);
    Menu->AddItem( 0, "Guild master"         , 3);
    Menu->AddItem( 0, "Inn"                  , 4);
    Menu->AddItem( 0, "Stable master"        , 5);
    Menu->AddItem( 0, "Class trainer"        , 6);
    Menu->AddItem( 0, "Profession trainer"   , 7);

    if(AutoSend)
        Menu->SendTo(Plr);
}

void NorthshireGuard::GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
{
    GossipMenu *Menu; 
    script_debuglog("Gossip-Northshir-Guard.cpp : Select option %u by %s", IntId, Plr->GetName());
    switch(IntId)
    {
    case 1:     // bank
        
        SendQuickMenu(4260);
        break;

    case 2:     // gryphon master
        
        SendQuickMenu(4261);
        break;

    case 3:     // guild master
        
        SendQuickMenu(4262);
        break;

    case 4:     // inn
        
        SendQuickMenu(4263);
        Plr->Gossip_SendPOI(-9459.34, 42.08, 99, 6, 0, "Lion's Pride Inn");
        break;

    case 5:     // stable master

        SendQuickMenu(5983);
        Plr->Gossip_SendPOI(-9466.62, 45.87, 99, 6, 0, "Erma");
        break;

    case 6:     // class trainer list

        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);

        Menu->AddItem( 0, "Druid",                8);
        Menu->AddItem( 0, "Hunter",               9);
        Menu->AddItem( 0, "Mage",                 10);
        Menu->AddItem( 0, "Paladin",              11);
        Menu->AddItem( 0, "Priest",               12);
        Menu->AddItem( 0, "Rogue",                13);
        Menu->AddItem( 0, "Warlock",              14);
        Menu->AddItem( 0, "Warrior",              15);

        Menu->SendTo(Plr);

    case 7:     // profession trainer list

        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4273, Plr);

        Menu->AddItem( 0, "Alchemy"              ,16);
        Menu->AddItem( 0, "Blacksmithing"        ,17);
        Menu->AddItem( 0, "Cooking"              ,18);
        Menu->AddItem( 0, "Enchanting"           ,19);
        Menu->AddItem( 0, "Engineering"          ,20);
        Menu->AddItem( 0, "First Aid"            ,21);
        Menu->AddItem( 0, "Fishing"              ,22);
        Menu->AddItem( 0, "Herbalism"            ,23);
        Menu->AddItem( 0, "Leatherworking"       ,24);
        Menu->AddItem( 0, "Mining"               ,25);
        Menu->AddItem( 0, "Skinning"             ,26);
        Menu->AddItem( 0, "Tailoring"            ,27);

        Menu->SendTo(Plr);

    default:

        script_errorlog("unknown val in northshire guard script: %u", IntId);
    }
    
}

void NorthshireGuard::GossipEnd(Creature* pCreature, Player* Plr)
{
    GossipScript::GossipEnd(pCreature, Plr);
}

/************************************************************************/
/* Undercity Guards Working? Maybe not implemented?                     */
/************************************************************************/

class WOWD_SCRIPT_DECL UndercityGuard : public GossipScript
{
public:
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3543, Plr);

        Menu->AddItem(0, "Auction House", 1);
        Menu->AddItem(0, "Bank of Undercity", 2);
        Menu->AddItem(0, "Zeppelin", 3);
        Menu->AddItem(0, "The Inn", 4);
        Menu->AddItem(0, "Bat Handler", 5);    
        Menu->AddItem(0, "Guild Master", 6);
        Menu->AddItem(0, "Mailbox", 7);
        Menu->AddItem(0, "Stable Master", 8);
        Menu->AddItem(0, "Weapon Trainer", 9);
      //  Menu->AddItem(0, "Officers` Lounge", 10);
        Menu->AddItem(0, "Battlemaster", 11);
        Menu->AddItem(0, "Class Trainer", 12);
        Menu->AddItem(0, "Profession Trainer", 13);

        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // Auction House
            SendQuickMenu(3520);
            Plr->Gossip_SendPOI(1647.9, 258.49, 6, 6, 0, "Undercity Auction House");
            break;

        case 2:     // Bank
            SendQuickMenu(3514);
            Plr->Gossip_SendPOI(1595.64, 232.45, 6, 6, 0, "Undercity Bank");
            break;

        case 3:     // Tram
            SendQuickMenu(3519);
            Plr->Gossip_SendPOI(2059, 274.86, 6, 6, 0, "Undercity Zeppelin");
            break;

        case 4:     // Inn
            SendQuickMenu(3517);
            Plr->Gossip_SendPOI(1639.43, 220.99, 6, 6, 0, "Undercity Inn");
            break;

        case 5:     // OMG BATS.
            SendQuickMenu(3515);
            Plr->Gossip_SendPOI(1565.9, 271.43, 6, 6, 0, "Undercity Bat Handler");
            break;

        case 6:     // Guildmaster
            SendQuickMenu(3516);
            Plr->Gossip_SendPOI(1594.17, 205.57, 6, 6, 0, "Undercity Guild Master");
            break;

        case 7:     // Mailbox
            SendQuickMenu(3518);
            Plr->Gossip_SendPOI(1632.68, 219.4, 6, 6, 0, "Undercity Mailbox");
            break;

        case 8:     // Stable master
            SendQuickMenu(5979);
            Plr->Gossip_SendPOI(1634.18, 226.76, 6, 6, 0, "Anya Maulray");
            break;

        case 9:     // weapon master
            SendQuickMenu(4521);
            Plr->Gossip_SendPOI(1670.31, 324.66, 6, 6, 0, "Archibald");
            break;

        case 11:    // Battlemaster submenu
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 7527, Plr);
                Menu->AddItem(0, "Alterac Valley", 14);
                Menu->AddItem(0, "Arathi Basin", 15);
                Menu->AddItem(0, "Warsong Gulch", 16);
                Menu->SendTo(Plr);
            }break;

        case 12:    // Class Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3542, Plr);
                Menu->AddItem( 0, "Mage"       , 17);
                Menu->AddItem( 0, "Rogue"      , 18);
                Menu->AddItem( 0, "Warrior"    , 19);
                Menu->AddItem( 0, "Druid"      , 20);
                Menu->AddItem( 0, "Priest"     , 21);
                Menu->AddItem( 0, "Shaman"     , 37);
                Menu->AddItem( 0, "Paladin"    , 22);
                Menu->AddItem( 0, "Hunter"     , 23);
                Menu->AddItem( 0, "Warlock"    , 24);
                Menu->SendTo(Plr);
            }break;

        case 13:    // Profession Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3541, Plr);
                Menu->AddItem( 0, "Alchemy"           , 25);
                Menu->AddItem( 0, "Blacksmithing"     , 26);
                Menu->AddItem( 0, "Cooking"           , 27);
                Menu->AddItem( 0, "Enchanting"        , 28);
                Menu->AddItem( 0, "Engineering"       , 29);
                Menu->AddItem( 0, "First Aid"         , 30);
                Menu->AddItem( 0, "Fishing"           , 31);
                Menu->AddItem( 0, "Herbalism"         , 32);
                Menu->AddItem( 0, "Leatherworking"    , 33);
                Menu->AddItem( 0, "Mining"            , 34);
                Menu->AddItem( 0, "Skinning"          , 35);
                Menu->AddItem( 0, "Tailoring"         , 36);
                Menu->SendTo(Plr);
            }break;

            //////////////////
            // Battlemaster submenu
            ////////

        case 14://Alterac Valley
            {
                Plr->Gossip_SendPOI(1329, 333.92, 6, 6, 0, "Grizzle Halfmane");
                SendQuickMenu(7525);
            }break;

        case 15://AB
            {
                Plr->Gossip_SendPOI(1283.3, 287.16, 6, 6, 0, "Sir Malory Wheeler");
                SendQuickMenu(7646);
            }break;

        case 16://War Song Gulch
            {
                Plr->Gossip_SendPOI(1265, 351.18, 6, 6, 0, "Kurden Bloodclaw");
                SendQuickMenu(7526);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 17: //Mage
            {
                Plr->Gossip_SendPOI(1781, 53, 6, 6, 0, "Undercity Mage Trainers");
                SendQuickMenu(3513);
            }break;

        case 18: //Rogue
            {
                Plr->Gossip_SendPOI(1418.56, 65, 6, 6, 0, "Undercity Rogue Trainers");
                SendQuickMenu(3526);
            }break;

        case 19: //Warrior
            {
                Plr->Gossip_SendPOI(1775.59, 418.19, 6, 6, 0, "Undercity Warrior Trainers");
                SendQuickMenu(3527);
            }break;

        case 21: //Priest
            {
                Plr->Gossip_SendPOI(1758.33, 401.5, 6, 6, 0, "Undercity Priest Trainers");
                SendQuickMenu(3521);
            }break;

        case 24: //Warlock
            {
                Plr->Gossip_SendPOI(1780.92, 53.16, 6, 6, 0, "Undercity Warlock Trainers");
                SendQuickMenu(3526);
            }break;


///// Professions submenu
        case 25: //Alchemy
            {
                Plr->Gossip_SendPOI(1419.82, 417.19, 6, 6, 0, "The Apothecarium");
                SendQuickMenu(3528);
            }break;

        case 26: //Blacksmithing
            {
                Plr->Gossip_SendPOI(1696, 285, 6, 6, 0, "Undercity Blacksmithing Trainer");
                SendQuickMenu(3529);
            }break;

        case 27: //Cooking
            {
                Plr->Gossip_SendPOI(1596.34, 274.68, 6, 6, 0, "Undercity Cooking Trainer");
                SendQuickMenu(3530);
            }break;

        case 28: //Enchanting
            {
                Plr->Gossip_SendPOI(1488.54, 280.19, 6, 6, 0, "Undercity Enchanting Trainer");
                SendQuickMenu(3531);
            }break;

        case 29: //Engineering
            {
                Plr->Gossip_SendPOI(1408.58, 143.43, 6, 6, 0, "Undercity Engineering Trainer");
                SendQuickMenu(3532);
            }break;

        case 30: //First Aid
            {
                Plr->Gossip_SendPOI(1519.65, 167.19, 6, 6, 0, "Undercity First Aid Trainer");
                SendQuickMenu(3533);
            }break;

        case 31: //Fishing
            {
                Plr->Gossip_SendPOI(1679.9, 89, 6, 6, 0, "Undercity Fishing Trainer");
                SendQuickMenu(3534);
            }break;

        case 32: //Herbalism
            {
                Plr->Gossip_SendPOI(1558, 349.36, 6, 6, 0, "Undercity Herbalism Trainer");
                SendQuickMenu(3535);
            }break;

        case 33: //Leatherworking
            {
                Plr->Gossip_SendPOI(1498.76, 196.43, 6, 6, 0, "Undercity Leatherworking Trainer");
                SendQuickMenu(3536);
            }break;

        case 34: //Mining
            {
                Plr->Gossip_SendPOI(1642.88, 335.58, 6, 6, 0, "Undercity Mining Trainer");
                SendQuickMenu(3537);
            }break;

        case 35: //Skinning
            {
                Plr->Gossip_SendPOI(1498.6, 196.46, 6, 6, 0, "Undercity Skinning Trainer");
                SendQuickMenu(3538);
            }break;

        case 36: //Tailoring
            {
                Plr->Gossip_SendPOI(1689.55, 193, 6, 6, 0, "Undercity Tailoring Trainer");
                SendQuickMenu(3539);
            }break;
        }
    }
};


// Very Basic Teldrassil...
class WOWD_SCRIPT_DECL TeldrassilGuard : public GossipScript
{
public:
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4316, Plr);

        Menu->AddItem(0, "Bank", 1);
		Menu->AddItem(0, "Ferry", 2);
		Menu->AddItem(0, "Guild Master", 3);
		Menu->AddItem(0, "Inn", 4);
		Menu->AddItem(0, "Stable Master", 5);
		Menu->AddItem(0, "Class Trainer", 6);
		Menu->AddItem(0, "Profession Trainer", 7);

        if(AutoSend)
            Menu->SendTo(Plr);
    }
	
    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // Bank
            SendQuickMenu(4317);
            break;
		
		case 2:		// Ferry 
			SendQuickMenu(4318);
			break;
		
		case 3:		// Guild Master
			SendQuickMenu(4319);
			break;
		
		case 4:		// Inn
			Plr->Gossip_SendPOI(9821.49, 960.13, 6, 6, 0, "Dolanaar Inn");
			SendQuickMenu(4320);
			
		case 5:		// Stable Master
			Plr->Gossip_SendPOI(9808.37, 931.1, 6, 6, 0, "Seriadne");
			SendQuickMenu(5982);


        case 6:    // Class Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);
                Menu->AddItem( 0, "Druid"       , 8);
                Menu->AddItem( 0, "Hunter"      , 9);
                Menu->AddItem( 0, "Priest"    , 10);
                Menu->AddItem( 0, "Rogue"      , 11);
                Menu->AddItem( 0, "Warrior"     , 12);
                Menu->SendTo(Plr);
            }break;

        case 7:    // Profession Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4273, Plr);
                Menu->AddItem( 0, "Alchemy"           , 13);
                Menu->AddItem( 0, "Blacksmithing"     , 14);
                Menu->AddItem( 0, "Cooking"           , 15);
                Menu->AddItem( 0, "Enchanting"        , 16);
                Menu->AddItem( 0, "Engineering"       , 17);
                Menu->AddItem( 0, "First Aid"         , 18);
                Menu->AddItem( 0, "Fishing"           , 19);
                Menu->AddItem( 0, "Herbalism"         , 20);
                Menu->AddItem( 0, "Leatherworking"    , 21);
                Menu->AddItem( 0, "Mining"            , 22);
                Menu->AddItem( 0, "Skinning"          , 23);
                Menu->AddItem( 0, "Tailoring"         , 24);
                Menu->SendTo(Plr);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 8: //Druid
            {
                Plr->Gossip_SendPOI(9741.58, 963.7, 6, 6, 0, "Kal");
                SendQuickMenu(4323);
            }break;
		
		case 9: // Hunter
			{
				Plr->Gossip_SendPOI(9815.12, 926.28, 6, 6, 0, "Dazalar");
				SendQuickMenu(4324);
			}break;
		
		case 10: // Priest
			{
				Plr->Gossip_SendPOI(9906.16, 986.63, 6, 6, 0, "Laurna Morninglight");
				SendQuickMenu(4325);
			}break;

		case 11: // Rogue
			{
				Plr->Gossip_SendPOI(9789, 942.86, 6, 6, 0, "Jannok Breezesong");
				SendQuickMenu(4327);
			}break;
			
		case 12: // Warrior
			{
				Plr->Gossip_SendPOI(9821.96, 950.61, 6, 6, 0, "Kyra Windblade");
				SendQuickMenu(4327);
			}break;

///// Professions submenu
        case 13: // Alchemy
            {
                Plr->Gossip_SendPOI(9767.59, 878.81, 6, 6, 0, "Cyndra Kindwhisper");
                SendQuickMenu(4329);
            }break;
		
		case 14: // Cooking, Yummy :D
			{
				Plr->Gossip_SendPOI(9751.19, 906.13, 6, 6, 0, "Zarrin");
				SendQuickMenu(4330);
			}break;
		
		case 15: // Enchanting
			{
				Plr->Gossip_SendPOI(10677.59, 1946.56, 6, 6, 0, "Alanna Raveneye");
				SendQuickMenu(4331);
			}break;
			
		case 16: // First Aid
			{
				Plr->Gossip_SendPOI(9903.12, 999, 6, 6, 0, "Byancie");
				SendQuickMenu(4332);
			}break;
			
		case 17: // Fishing
			{
				SendQuickMenu(4333);
			}break;
			
		case 18: // Herbalism
			{
				Plr->Gossip_SendPOI(9773.78, 875.88, 6, 6, 0, "Malorne Bladeleaf");
				SendQuickMenu(4334);
			}break;
			
		case 19: // Leatherworking
			{
				Plr->Gossip_SendPOI(10152.59, 1681.46, 6, 6, 0, "Nadyia Maneweaver");
				SendQuickMenu(4335);
			}break;
			
		case 20: // Skinning
			{
				Plr->Gossip_SendPOI(10135.59, 1673.18, 6, 6, 0, "Radnaal Maneweaver");
				SendQuickMenu(4336);
			}break;
			
		case 21: // Tailoring
			{
				SendQuickMenu(4337);
			}break;

        }
    }
};

GossipScript* CreateGossipScript(uint32 uEntryId)
{
    switch(uEntryId)
    {
    case 1423:      // Northshire Guard
        return new NorthshireGuard;
        break;

    case 68:        // Stormwind CITY guards
    case 1976:      
        return new StormwindGuard;
        break;

    case 4262:      // Darnassus sentinel
        return new DarnassusGuard;
        break;

    case 5624:		// Undercity
	 return new UndercityGuard;
        break;

    case 3571:
        return new TeldrassilGuard;
        break;
    }
    return 0;
}

void DestroyGossipScript(GossipScript* Script)
{
    delete Script;
}

void ScriptInitialize(ScriptModule *mod)
{
    /* Guard List */
    sScriptMgr.ScriptAddGossipModule(1423, mod);        // Northshire Guard 
    sScriptMgr.ScriptAddGossipModule(68, mod);          // Stormwind City Guard
    sScriptMgr.ScriptAddGossipModule(1976, mod);        // Stormwind City Patroller
    sScriptMgr.ScriptAddGossipModule(4262, mod);        // Darnassus sentinal
    sScriptMgr.ScriptAddGossipModule(5624, mod);	// Undercity
    sScriptMgr.ScriptAddGossipModule(3571, mod);	// Teldrassil
}

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

#include "StdAfx.h"

initialiseSingleton( ObjectMgr );

#ifdef WIN32
#define ToLower(yourstring) transform (yourstring.begin(),yourstring.end(), yourstring.begin(), tolower);
#else
#define ToLower(str) for(unsigned int i=0;i<str.size();i++) tolower(str[i]);
#endif
#define ToUpper(yourstring) transform (yourstring.begin(),yourstring.end(), yourstring.begin(), toupper);

ObjectMgr::ObjectMgr()
{
    m_hiCharGuid = 1;
    m_hiCreatureGuid = 1;
    m_hiItemGuid = 1;
    m_hiGoGuid = 1;
    m_hiDoGuid = 1;
    m_hiNameGuid = 1;
    m_hiGoNameGuid = 1;

    m_auctionid = 0;
    m_mailid = 0;

}


ObjectMgr::~ObjectMgr()
{
    sLog.outString("    Removing Corpses from Object Holder");
    CorpseCollectorUnload();

    CleanupObjectHolders();
    sLog.outString("    Deleting ItemSets...");
    for(ItemSetContentMap::iterator i = mItemSets.begin(); i != mItemSets.end(); ++i)
    {
        delete i->second;
    }

    sLog.outString("    Deleting CreatureNames...");
    for( CreatureNameMap::iterator i = mCreatureNames.begin( ); i != mCreatureNames.end( ); ++ i ) {
        delete i->second;
    }
    mCreatureNames.clear();

    sLog.outString("    Deleting CreatureSpawnTemplates...");
    for( CreatureSpawnTemplateMap::iterator i = mCreatureSpawnTemplates.begin( ); i != mCreatureSpawnTemplates.end( ); ++ i ) {
        delete i->second;
    }
    mCreatureSpawnTemplates.clear( );

    sLog.outString("    Deleting ItemPrototypes...");
    for( ItemPrototypeMap::iterator i = mItemPrototypes.begin( ); i != mItemPrototypes.end( ); ++ i ) {
        delete i->second;
    }
    mItemPrototypes.clear( );

    sLog.outString("    Deleting CreateInfo...");
    for( PlayerCreateInfoMap::iterator i = mPlayerCreateInfo.begin( ); i != mPlayerCreateInfo.end( ); ++ i ) {
        delete i->second;
    }
    mPlayerCreateInfo.clear( );

    sLog.outString("    Deleting Guilds...");
    for ( GuildMap::iterator i = mGuild.begin(); i != mGuild.end(); ++i ) {
        delete i->second;
    }
    mGuild.clear();

    sLog.outString("    Deleting NPC Texts...");
    for( GossipTextMap::iterator i = mGossipText.begin( ); i != mGossipText.end( ); ++ i ) {
        delete i->second;
    }
    mGossipText.clear( );

    sLog.outString("    Deleting Graveyards...");
    for( GraveyardMap::iterator i = mGraveyards.begin( ); i != mGraveyards.end( ); ++ i ) {
        delete i->second;
    }
    mGraveyards.clear( );

    sLog.outString("    Deleting GameObjectNames...");
    for( GameObjectNameMap::iterator i = mGameObjectNames.begin( ); i != mGameObjectNames.end( ); ++ i ) {
        delete i->second;
    }
    mGameObjectNames.clear();

    sLog.outString("    Deleting and Saving Charters...");
    for( std::list<Charter*>::iterator i = Guild_CharterList.begin( ); i != Guild_CharterList.end( ); ++ i ) 
    {
        Charter *gc=(*i);
        sDatabase.Execute("DELETE FROM charters WHERE leaderguid = %u", gc->leaderGuid);
        sDatabase.Execute("INSERT INTO charters VALUES(%u,'%s',%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)",
        (uint32)gc->leaderGuid,gc->guildName.c_str(),(uint32)gc->itemGuid,(uint32)gc->signer[0],(uint32)gc->signer[1],(uint32)gc->signer[2],
        (uint32)gc->signer[3],(uint32)gc->signer[4],(uint32)gc->signer[5],(uint32)gc->signer[6],(uint32)gc->signer[7],(uint32)gc->signer[8]    );

        delete gc;
    }
    Guild_CharterList.clear();

    sLog.outString("    Deleting PvPAreas...");
    for( PvPAreaMap::iterator i = mPvPAreas.begin( ); i != mPvPAreas.end( ); ++ i ) {
        delete i->second;
    }
    mPvPAreas.clear();

    sLog.outString("    Deleting Vendors...");
    for( VendorMap::iterator i = mVendors.begin( ); i != mVendors.end( ); ++ i ) 
    {
        delete i->second;
    }
    mVendors.clear();

    sLog.outString("    Deleting Item Pages...");
    for(ItemPageMap::iterator itr = mItemPages.begin(); itr != mItemPages.end(); ++itr)
        delete itr->second;
    mItemPages.clear();

    sLog.outString("    Deleting AI Threat Spells...");
    for( std::list<ThreatToSpellId*>::iterator i = threatToSpells.begin( ); i != threatToSpells.end( ); ++ i ) 
    {
        ThreatToSpellId *gc=(*i);
        delete gc;
    }
    threatToSpells.clear();

    sLog.outString("    Deleting Totem Spells...");
    m_totemSpells.clear();

    sLog.outString("    Deleting Teleport Coords...");
    for( TeleportMap::iterator i = mTeleports.begin( ); i != mTeleports.end( ); ++ i ) {
        delete i->second;
    }
    mTeleports.clear( );

    sLog.outString("    Deleting Fishing Zones...");
    for( FishingZoneMap::iterator i = mFishingZones.begin( ); i != mFishingZones.end( ); ++ i ) {
        delete i->second;
    }
    mFishingZones.clear( );

    for( TrainerMap::iterator i = mTrainers.begin( ); i != mTrainers.end( ); ++ i) {
        Trainer * t = i->second;
        delete [] t->TalkMessage;
        delete [] t->SpellList;
        delete t;
    }
    mTrainers.clear( );

    for(TrainerSpellMap::iterator i = mNormalSpells.begin(); i != mNormalSpells.end(); ++i)
    {
        vector<TrainerSpell*>::iterator itr = i->second.begin();
        for(; itr != i->second.end(); ++itr)
        {
            delete (*itr);
        }
        i->second.clear();
    }
    mNormalSpells.clear();

    for(TrainerSpellMap::iterator i = mPetSpells.begin(); i != mPetSpells.end(); ++i)
    {
        vector<TrainerSpell*>::iterator itr = i->second.begin();
        for(; itr != i->second.end(); ++itr)
        {
            delete (*itr);
        }
        i->second.clear();
    }
    mPetSpells.clear();

    for(SpellExtraInfoMap::iterator i = mExtraSpellData.begin(); i != mExtraSpellData.end(); ++i)
    {
        delete i->second;
    }
    mExtraSpellData.clear();

    for( LevelInfoMap::iterator i = mLevelInfo.begin(); i != mLevelInfo.end(); ++i)
    {
        LevelMap * l = i->second;
        for(LevelMap::iterator i2 = l->begin(); i2 != l->end(); ++i2)
        {
            delete i2->second;
        }
        l->clear();
        delete l;
    }
    mLevelInfo.clear();
}

void ObjectMgr::CleanupObjectHolders()
{
    sLog.outString("    Deleting corpses and any left over objects inside ObjectHolder...");
    sLog.outString("      Corpse"); CleanupObjectHolder<Corpse>();
    sLog.outString("      Creature"); sObjHolder.CleanupContainer<Creature>();
    sLog.outString("      GameObject"); sObjHolder.CleanupContainer<GameObject>();
    sLog.outString("      Player"); CleanupObjectHolder<Player>();
    sLog.outString("      Pet"); CleanupObjectHolder<Pet>();
    sLog.outString("      Transporter"); CleanupObjectHolder<Transporter>();
    sLog.outString("      DynamicObject"); sObjHolder.CleanupContainer<DynamicObject>();
    sLog.outString("    Deleting ObjectHolder\n");
    delete ObjectHolder::getSingletonPtr();
}

template<class T>
void ObjectMgr::CleanupObjectHolder()
{
// Someone fix this on linux pls!
#ifdef WIN32
    HM_NAMESPACE::hash_map<uint32, T*>::iterator itr;
    for(itr = sObjHolder.Begin<T>(); itr != sObjHolder.End<T>();)
    {
        HM_NAMESPACE::hash_map<uint32, T*>::iterator it2 = itr;
        ++itr;
        ASSERT(sObjHolder.Delete<T>(it2->second));
    }
#endif
}
//
// Groups
//

Group * ObjectMgr::GetGroupByLeader(Player* pPlayer)
{
    GroupSet::const_iterator itr;
    for (itr = mGroupSet.begin(); itr != mGroupSet.end(); itr++)
    {
        if ((*itr)->GetLeader() == pPlayer)
        {
            return *itr;
        }
    }

    return NULL;
}

//
// Player names
//
PlayerInfo *ObjectMgr::GetPlayerInfo( uint64 guid )
{
    HM_NAMESPACE::hash_map<uint32,PlayerInfo>::iterator itr = m_playersinfo.find( GUID_LOPART(guid) );
    if( itr !=m_playersinfo.end( ) )
        return &(itr->second);

    //returning unknown player if no data found
    
    //i think we should return NULL, to allow caller to know that this player
    //does not exist, instead of adding some shit to entries,
    //let's suppose some1 needs info for player 123
    //that does not exist we add info on this player
    //but later we create player with such guid, that would lead
    //to situation where client and server has different info and leads to bug
    //(c) Phantomas
    
    return NULL;
    
    /*
    PlayerInfo *pn = new PlayerInfo;
    pn->guid = guid;
    pn->name = "Unknown Player";
    pn->race = 0;
    pn->gender = 0;
    pn->cl = 0;
    AddPlayerName(pn);
    return pn;*/
}

void ObjectMgr::AddPlayerInfo(PlayerInfo *pn)
{
    m_playersinfo[(uint32)pn->guid]=*pn;
}

//
// Creature names
//
CreatureInfo *ObjectMgr::GetCreatureName(uint32 id)
{
    CreatureNameMap::const_iterator itr = mCreatureNames.find( id );
    if( itr != mCreatureNames.end( ) )
        return itr->second;

    //returning unknown creature if no data found
    return NULL;    
}

uint32 ObjectMgr::AddCreatureName(const char* name)
{
    for( CreatureNameMap::iterator i = mCreatureNames.begin( );
        i != mCreatureNames.end( ); ++ i )
    {
        if (strcmp(i->second->Name.c_str(),name) == 0)
            return i->second->Id;
    }

    uint32 id = m_hiNameGuid++;
    AddCreatureName(id, name);

    std::stringstream ss;
    ss << "INSERT INTO creature_names (entry,creature_name) VALUES (" << id << ", '" << name << "')";
    sDatabase.Execute( ss.str( ).c_str( ) );

    return id;
}

uint32 ObjectMgr::AddCreatureName(const char* name, uint32 displayid)
{
    for( CreatureNameMap::iterator i = mCreatureNames.begin( );
        i != mCreatureNames.end( ); ++ i )
    {
        if (strcmp(i->second->Name.c_str(),name) == 0)
            return i->second->Id;
    }

    uint32 id = m_hiNameGuid++;
    AddCreatureName(id, name, displayid);

    std::stringstream ss;
    ss << "INSERT INTO creature_names (entry,creature_name,displayid) VALUES (" << id << ", '" << name << "', '" << displayid << "')";
    sDatabase.Execute( ss.str( ).c_str( ) );

    return id;
}

uint32 ObjectMgr::AddCreatureSubName(const char* name, const char* subname, uint32 displayid)
{
    for( CreatureNameMap::iterator i = mCreatureNames.begin( );
        i != mCreatureNames.end( ); ++ i )
    {
        if (strcmp(i->second->Name.c_str(),name) == 0)
            if (i->second->DisplayID == displayid)
                if (strcmp(i->second->SubName.c_str(),subname) == 0)
                    return i->second->Id;
    }

    uint32 id = m_hiNameGuid++;

    CreatureInfo *cInfo = new CreatureInfo;
    cInfo->DisplayID = displayid;
    cInfo->Id = id;
    cInfo->Name = name;
    cInfo->SubName = subname;
    cInfo->Flags1 = 0;
    cInfo->Type = 0;
    cInfo->Family = 0;
    cInfo->Rank = 0;
    cInfo->Unknown1 = 0;
    cInfo->SpellDataID = 0;
    cInfo->Civilian = 0;
    cInfo->Leader = 0;

    AddCreatureName(cInfo);

    std::stringstream ss;
    ss << "INSERT INTO creature_names (entry,creature_name,Subname,displayid) VALUES (" << id << ", '" << name;
    ss << "', '" << subname << "', '" << displayid << "')";
    sDatabase.Execute( ss.str( ).c_str( ) );

    return id;
}

void ObjectMgr::AddCreatureName(CreatureInfo *cinfo)
{
    ASSERT( mCreatureNames.find(cinfo->Id) == mCreatureNames.end() );
    //verifying if info for that creature already exists(need some cleaning here some time)
    CreatureNameMap::iterator itr = mCreatureNames.find( cinfo->Id );
    //if found we delete the old creature info
    if( itr != mCreatureNames.end( ) )
        mCreatureNames.erase(itr);
    mCreatureNames[cinfo->Id] = cinfo;
}

void ObjectMgr::AddCreatureName(uint32 id, const char* name)
{
    CreatureInfo *cinfo;
    cinfo = new CreatureInfo;
    cinfo->Id = id;
    cinfo->Name = name;
    cinfo->SubName = "";
    cinfo->Flags1 = 0;
    cinfo->Type = 0; 
    cinfo->Family = 0;
    cinfo->Rank = 0;
    cinfo->Unknown1 = 0;
    cinfo->SpellDataID = 0;
    cinfo->DisplayID = 0;
    cinfo->Civilian = 0;
    cinfo->Leader = 0;

    ASSERT( name );
    ASSERT( mCreatureNames.find(id) == mCreatureNames.end() );
    mCreatureNames[id] = cinfo;
}
void ObjectMgr::LoadSpellSkills()
{
    uint32 i;
    int total = sSkillStore.GetNumRows();

    for(i = 0; i < sSkillStore.GetNumRows(); i++)
    {
        skilllinespell *sp = sSkillStore.LookupEntry(i);
        if (sp)
        {
            mSpellSkills[sp->spell] = sp;
        }
        if(!(i % 50)) SetProgressBar(i, total, "Skills");
    }
    ClearProgressBar();
}
skilllinespell* ObjectMgr::GetSpellSkill(uint32 id)
{
    return mSpellSkills[id];
}
void ObjectMgr::AddCreatureName(uint32 id, const char* name, uint32 displayid)
{
    CreatureInfo *cinfo;
    cinfo = new CreatureInfo;
    cinfo->Id = id;
    cinfo->Name = name;
    cinfo->SubName = "";
    cinfo->Type = 0;
    cinfo->Flags1 = 0;
    cinfo->Family = 0;
    cinfo->Rank = 0;
    cinfo->Unknown1 = 0;
    cinfo->SpellDataID = 0;
    cinfo->DisplayID = displayid;
    cinfo->Civilian = 0;
    cinfo->Leader = 0;

    ASSERT( name );
    ASSERT( mCreatureNames.find(id) == mCreatureNames.end() );
    mCreatureNames[id] = cinfo;
}

void ObjectMgr::LoadCreatureSpawnTemplates()
{
    CreatureSpawnTemplate *ct;
    QueryResult *result = sDatabase.Query( "SELECT * FROM creaturespawntemplate" );
    if(result)
    {
        uint32 total =(uint32) result->GetRowCount();
        int num = 0;
        do
        {
            Field *fields = result->Fetch();
            ct = new CreatureSpawnTemplate;
            int column = 0;
            ct->EntryID = fields[column].GetUInt32();
            column += 1;
            /*
            ct->ModelID = fields[column].GetUInt32();
            column += 1;
            ct->Name = fields[column].GetString();
            column += 1;
            ct->SubName = fields[column].GetString();
            column += 1;
            */
            ct->MaxHealth = fields[column].GetUInt32();
            column += 1;
            ct->MaxMana = fields[column].GetUInt32();
            column += 1;
            ct->Armor = fields[column].GetUInt32();
            column += 1;
            ct->Level = fields[column].GetUInt32();
            column += 1;
            ct->Faction = fields[column].GetUInt32();
            column += 1;
            ct->Flag = fields[column].GetUInt32();
            column += 1;
            ct->Scale = fields[column].GetFloat();
            column += 1;
            ct->Speed = fields[column].GetFloat();
            column += 1;
            //ct->Rank = fields[column].GetUInt32();
            //column += 1;
            ct->MinDamage = fields[column].GetFloat();
            column += 1;
            ct->MaxDamage = fields[column].GetFloat();
            column += 1;
            ct->MinRangedDamage = fields[column].GetFloat();
            column += 1;
            ct->MaxRangedDamage = fields[column].GetFloat();
            column += 1;
            ct->BaseAttackTime = fields[column].GetUInt32();
            column += 1;
            ct->RangedAttackTime = fields[column].GetUInt32();
            column += 1;
            ct->BoundingRadius = fields[column].GetFloat();
            column += 1;
            ct->CombatReach = fields[column].GetFloat();
            column += 1;
            ct->Slot1Model = fields[column].GetUInt32();
            column += 1;
            ct->Slot1Info1 = fields[column].GetUInt8();
            column += 1;
            ct->Slot1Info2 = fields[column].GetUInt8();
            column += 1;
            ct->Slot1Info3 = fields[column].GetUInt8();
            column += 1;
            ct->Slot1Info4 = fields[column].GetUInt8();
            column += 1;
            ct->Slot1Info5 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Model = fields[column].GetUInt32();
            column += 1;
            ct->Slot2Info1 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Info2 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Info3 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Info4 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Info5 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Model = fields[column].GetUInt32();
            column += 1;
            ct->Slot3Info1 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Info2 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Info3 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Info4 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Info5 = fields[column].GetUInt8();
            column += 1;
            //ct->Type = fields[column].GetUInt32();
            //column += 1;
            ct->MountModelID = fields[column].GetUInt32();
            column += 1;


            AddCreatureSpawnTemplate(ct);
            ++num;
            if(!(num % 50)) SetProgressBar(num, total, "Creature Spawn Templates");
        } while( result->NextRow() );
        ClearProgressBar();
        delete result;
    }
}

void ObjectMgr::LoadPlayersInfo()
{
    PlayerInfo pn;
    QueryResult *result = sDatabase.Query("SELECT guid,name,data,zoneId,timestamp,publicNote,officerNote,guildRank FROM characters");
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();
           
            pn.guid = fields[0].GetUInt32();
            pn.name = fields[1].GetString();
            const char * data= fields[2].GetString();
            const char * p=data;
            uint32 x=0;
            for(;x <UNIT_FIELD_LEVEL;x++)
            {
                p=strstr(p," ");
                p++;
            }
            pn.lastLevel=atol(p);

            for(;x <UNIT_FIELD_BYTES_0;x++)
            {
                p=strstr(p," ");
                p++;
            }
            uint32 v=atol(p);
            
            //SetUInt32Value(UNIT_FIELD_BYTES_0, ( ( race ) | ( class_ << 8 ) | ( gender << 16 ) | ( powertype << 24 ) ) );

            pn.race = v%256;
            pn.cl = (v&0x0000ff00)>>8;
            pn.gender = (v&0x00ff0000)>>16;
            pn.lastZone=fields[3].GetUInt32();
            pn.lastOnline=fields[4].GetUInt32();
            pn.publicNote=fields[5].GetString();
            pn.officerNote= fields[6].GetString();
            
            pn.Rank=fields[7].GetUInt32();

            if(pn.race==RACE_HUMAN||pn.race==RACE_DWARF||pn.race==RACE_GNOME||pn.race==RACE_NIGHTELF||pn.race==RACE_DRAENEI)
                pn.team =0;
            else pn.team=1;
          
            AddPlayerInfo(&pn);
        } while( result->NextRow() );

        delete result;
    }
}

void ObjectMgr::LoadCreatureNames()
{
    CreatureInfo *cn;
    QueryResult *result = sDatabase.Query( "SELECT * FROM creature_names" );
    if(result)
    {
        uint32 total =(uint32) result->GetRowCount();
        int num = 0;
        do
        {
            Field *fields = result->Fetch();

            cn = new CreatureInfo;
            cn->Id = fields[0].GetUInt32();
            cn->Name = fields[1].GetString();
            cn->SubName = fields[2].GetString();
            cn->Flags1 = fields[3].GetUInt32();
            cn->Type = fields[4].GetUInt32();
            cn->Family = fields[5].GetUInt32();
            cn->Rank = fields[6].GetUInt32();
            cn->Unknown1 = fields[7].GetUInt32();
            cn->SpellDataID = fields[8].GetUInt32();
            cn->DisplayID = fields[9].GetUInt32();
            cn->Civilian = fields[10].GetUInt8();
            cn->Leader = fields[11].GetUInt8();

            AddCreatureName( cn );
            ++num;
            if(!(num % 30)) SetProgressBar(num, total, "Creature Names");
        } while( result->NextRow() );
        ClearProgressBar();

        delete result;
    }

    result = sDatabase.Query( "SELECT MAX(entry) FROM creature_names" );
    if(result)
    {
        m_hiNameGuid = result->Fetch()[0].GetUInt32();

        delete result;
    }
}

PlayerInfo* ObjectMgr::GetPlayerInfoByName(const char *name)
{
    HM_NAMESPACE::hash_map<uint32,PlayerInfo>::const_iterator i;
    for(i=m_playersinfo.begin();i!=m_playersinfo.end();i++)
    if(!stricmp(i->second.name.c_str(),name))
     return (PlayerInfo* )&(i->second);
    return NULL;
}



void ObjectMgr::LoadAuctions()
{

}
void ObjectMgr::LoadItemPrototypes()
{
    QueryResult *result = sDatabase.Query( "SELECT * FROM items" );

    if( !result )
        return;
    ItemPrototype *pItemPrototype;
    int i;
    if( result->GetFieldCount() < 117 )
    {
        sLog.outError("DB: Items table has incorrect number of fields");
        delete result;
        return;
    }

    uint32 total =(uint32) result->GetRowCount();
    int pos = 0;
    do
    {
        Field *fields = result->Fetch();

        if( !fields[0].GetUInt32() )
        {
            sLog.outBasic("DB: Incorrect item id found");
            continue;
        }

        pItemPrototype = new ItemPrototype;

        pItemPrototype->ItemId = fields[0].GetUInt32();
        pItemPrototype->Class = fields[1].GetUInt32();
        pItemPrototype->SubClass = fields[2].GetUInt32();
        pItemPrototype->unknown_bc = fields[3].GetUInt32();
        pItemPrototype->Name1 = fields[4].GetString();
        pItemPrototype->Name2 = fields[5].GetString();
        pItemPrototype->Name3 = fields[6].GetString();
        pItemPrototype->Name4 = fields[7].GetString();
        pItemPrototype->DisplayInfoID = fields[8].GetUInt32();
        pItemPrototype->Quality = fields[9].GetUInt32();
        pItemPrototype->Flags = fields[10].GetUInt32();
        pItemPrototype->BuyPrice = fields[11].GetUInt32();
        pItemPrototype->SellPrice = fields[12].GetUInt32();
        pItemPrototype->InventoryType = fields[13].GetUInt32();
        pItemPrototype->AllowableClass = fields[14].GetUInt32();
        pItemPrototype->AllowableRace = fields[15].GetUInt32();
        pItemPrototype->ItemLevel = fields[16].GetUInt32();
        pItemPrototype->RequiredLevel = fields[17].GetUInt32();
        pItemPrototype->RequiredSkill = fields[18].GetUInt32();
        pItemPrototype->RequiredSkillRank = fields[19].GetUInt32();
        pItemPrototype->RequiredSkillSubRank = fields[20].GetUInt32();
        pItemPrototype->RequiredPlayerRank1 = fields[21].GetUInt32();
        pItemPrototype->RequiredPlayerRank2 = fields[22].GetUInt32();
        pItemPrototype->RequiredFaction = fields[23].GetUInt32();
        pItemPrototype->RequiredFactionStanding = fields[24].GetUInt32();
        pItemPrototype->Unique = fields[25].GetUInt32();
        pItemPrototype->MaxCount = fields[26].GetUInt32();
        pItemPrototype->ContainerSlots = fields[27].GetUInt32();
        for(i = 0; i < 20; i+=2)
        {
            pItemPrototype->ItemStatType[i/2] = fields[28 + i].GetUInt32();
            pItemPrototype->ItemStatValue[i/2] = fields[29 + i].GetUInt32();
        }
        for(i = 0; i < 15; i+=3)
        {
            // Stupid items.sql
          //  int *a=(int *)malloc(sizeof(int)); *a=fields[48 + i].GetUInt32();
            //int *b=(int *)malloc(sizeof(int)); *b=fields[49 + i].GetUInt32();

            pItemPrototype->DamageMin[i/3] = fields[48 + i].GetFloat();
            pItemPrototype->DamageMax[i/3] = fields[49 + i].GetFloat();
            /*
            */           
            //pItemPrototype->DamageMin[i/3] = fields[46 + +i].GetFloat();
            //pItemPrototype->DamageMax[i/3] = fields[47 +i].GetFloat();
            pItemPrototype->DamageType[i/3] = fields[50 + i].GetUInt32();
//            free(a);free(b);
        }
        pItemPrototype->Armor = fields[63].GetUInt32();
        pItemPrototype->HolyRes = fields[64].GetUInt32();
        pItemPrototype->FireRes = fields[65].GetUInt32();
        pItemPrototype->NatureRes = fields[66].GetUInt32();
        pItemPrototype->FrostRes = fields[67].GetUInt32();
        pItemPrototype->ShadowRes = fields[68].GetUInt32();
        pItemPrototype->ArcaneRes = fields[69].GetUInt32();
        pItemPrototype->Delay = fields[70].GetUInt32();
        pItemPrototype->AmmoType = fields[71].GetUInt32();
        
        pItemPrototype->Range = fields[72].GetFloat();
       
        for(i = 0; i < 30; i+=6)
        {
            pItemPrototype->SpellId[i/6] = fields[73+i].GetUInt32();
            pItemPrototype->SpellTrigger[i/6] = fields[74+i].GetUInt32();
            pItemPrototype->SpellCharges[i/6] = fields[75+i].GetUInt32();
            pItemPrototype->SpellCooldown[i/6] = fields[76+i].GetUInt32();
            pItemPrototype->SpellCategory[i/6] = fields[77+i].GetUInt32();
            pItemPrototype->SpellCategoryCooldown[i/6] = fields[78+i].GetUInt32();
        }
        pItemPrototype->Bonding = fields[103].GetUInt32();
        pItemPrototype->Description = fields[104].GetString();
        pItemPrototype->PageId = fields[105].GetUInt32();
        pItemPrototype->PageLanguage = fields[106].GetUInt32();
        pItemPrototype->PageMaterial = fields[107].GetUInt32();
        pItemPrototype->QuestId = fields[108].GetUInt32();
        pItemPrototype->LockId = fields[109].GetUInt32();
        pItemPrototype->LockMaterial = fields[110].GetUInt32();
        pItemPrototype->Field108 = fields[111].GetUInt32();
        pItemPrototype->RandomPropId = fields[112].GetUInt32();
        pItemPrototype->RandomPropId_2 = fields[113].GetUInt32();
        pItemPrototype->Block = fields[114].GetUInt32();
        pItemPrototype->ItemSet = fields[115].GetUInt32();
        pItemPrototype->MaxDurability = fields[116].GetUInt32();
        pItemPrototype->ZoneNameID = fields[117].GetUInt32();
        pItemPrototype->Field114 = fields[118].GetUInt32();
        pItemPrototype->BagFamily = fields[119].GetUInt32();
        pItemPrototype->ToolCategory = fields[120].GetUInt32();
        pItemPrototype->SocketColor1 = fields[121].GetUInt32();
        pItemPrototype->Unk201_3 = fields[122].GetUInt32();
        pItemPrototype->SocketColor2 = fields[123].GetUInt32();
        pItemPrototype->Unk201_5 = fields[124].GetUInt32();
        pItemPrototype->SocketColor3 = fields[125].GetUInt32();
        pItemPrototype->Unk201_7 = fields[126].GetUInt32();
        pItemPrototype->SocketBonus = fields[127].GetUInt32();
        pItemPrototype->GemProperties = fields[128].GetUInt32();
        pItemPrototype->ItemExtendedCost = fields[129].GetUInt32();
        pItemPrototype->DisenchantReqSkill = fields[130].GetUInt32();

        // item sets
        if(pItemPrototype->ItemSet > 0)
        {
            ItemSetContentMap::iterator itr = mItemSets.find(pItemPrototype->ItemSet);
            std::list<ItemPrototype*>* l;
            if(itr == mItemSets.end())
            {
                l = new std::list<ItemPrototype*>;                
                mItemSets.insert( ItemSetContentMap::value_type( pItemPrototype->ItemSet, l) );
            } else {
                l = itr->second;
            }
            l->push_back(pItemPrototype);
        }

        AddItemPrototype(pItemPrototype);
        ++pos;
        if(!(pos % 100)) SetProgressBar(pos, total, "Item Prototypes");
    } while( result->NextRow() );
    delete result;
    ClearProgressBar();
}


void ObjectMgr::LoadQuests()
{
    sQuestMgr.LoadSQLQuests();
}

Creature* ObjectMgr::LoadCreature(uint32 guid, int32 instanceid)
{
    Creature* unit;

    unit = sObjHolder.Create<Creature>();
    ASSERT(unit);

    if(unit->LoadFromDB(guid))
    {
        unit->_Activated = true;
        unit->SetInstanceID(instanceid);
        unit->AddToWorld();
    } else {
        sLog.outError("Creature with guid %u failed to load from db.", unit->GetSQL_id());
        //sObjHolder.Delete<Creature>(unit);
        return NULL;
    }
    
    unit->SetSpawnedObject(true);

    return unit;
}

Creature* ObjectMgr::LoadCreature(CreatureTemplate *t, int32 instanceid)
{
    Creature* unit;

    unit = sObjHolder.Create<Creature>();
    ASSERT(unit);

    if(unit->LoadFromDB(t))
    {
        unit->_Activated = true;
        unit->SetInstanceID(instanceid);
        unit->AddToWorld();
    } else {
        sLog.outError("Creature with guid %u failed to load from template.", unit->GetSQL_id());
        //sObjHolder.Delete<Creature>(unit);
        return NULL;
    }

    unit->SetSpawnedObject(true);

    return unit;
}

GameObject* ObjectMgr::LoadGameObject(uint32 guid, int32 instanceid)
{
    GameObject* go = sObjHolder.Create<GameObject>();
    ASSERT(go);

    if(!go->LoadFromDB(guid))
    {
        sLog.outError("gameobject failed to load from db.");
        return NULL;
    }
    go->SetSpawnedObject(true);
    go->SetInstanceID(instanceid);
    go->_Activated = true;
    go->AddToWorld();
    CALL_GO_SCRIPT_EVENT(go, OnCreate)();
    CALL_GO_SCRIPT_EVENT(go, OnSpawn)();

    return go;
}

GameObject* ObjectMgr::LoadGameObject(GameObjectTemplate *t, int32 instanceid)
{
    GameObject* go = sObjHolder.Create<GameObject>();
    ASSERT(go);

    go->LoadFromDB(t);
    go->SetSpawnedObject(true);
    go->SetInstanceID(instanceid);
    go->_Activated = true;
    go->AddToWorld();
    CALL_GO_SCRIPT_EVENT(go, OnCreate)();
    CALL_GO_SCRIPT_EVENT(go, OnSpawn)();

    return go;
}

void ObjectMgr::LoadPlayerCreateInfo()
{
    QueryResult *result = sDatabase.Query( "SELECT * FROM playercreateinfo" );

    if( !result )
    {
        sLog.outString("  Query failed: SELECT * FROM playercreateinfo");
        return;
    }

    PlayerCreateInfo *pPlayerCreateInfo;

    int num = 0;
    int total = (int)result->GetRowCount();
    do
    {
        Field *fields = result->Fetch();

        pPlayerCreateInfo = new PlayerCreateInfo;

        pPlayerCreateInfo->index = fields[0].GetUInt8();
        pPlayerCreateInfo->race = fields[1].GetUInt8();
        pPlayerCreateInfo->factiontemplate = fields[2].GetUInt32();
        pPlayerCreateInfo->class_ = fields[3].GetUInt8();
        pPlayerCreateInfo->mapId = fields[4].GetUInt32();
        pPlayerCreateInfo->zoneId = fields[5].GetUInt32();
        pPlayerCreateInfo->positionX = fields[6].GetFloat();
        pPlayerCreateInfo->positionY = fields[7].GetFloat();
        pPlayerCreateInfo->positionZ = fields[8].GetFloat();
        pPlayerCreateInfo->displayId = fields[9].GetUInt16();
        pPlayerCreateInfo->strength = fields[10].GetUInt8();
        pPlayerCreateInfo->ability = fields[11].GetUInt8();
        pPlayerCreateInfo->stamina = fields[12].GetUInt8();
        pPlayerCreateInfo->intellect = fields[13].GetUInt8();
        pPlayerCreateInfo->spirit = fields[14].GetUInt8();
        pPlayerCreateInfo->health = fields[15].GetUInt32();
        pPlayerCreateInfo->mana = fields[16].GetUInt32();
        pPlayerCreateInfo->rage = fields[17].GetUInt32();
        pPlayerCreateInfo->focus = fields[18].GetUInt32();
        pPlayerCreateInfo->energy = fields[19].GetUInt32();
        pPlayerCreateInfo->attackpower = fields[20].GetUInt32();
        pPlayerCreateInfo->mindmg = fields[21].GetFloat();
        pPlayerCreateInfo->maxdmg = fields[22].GetFloat();
        //for (i=0; i<10; i++)
        //{
            //pPlayerCreateInfo->item[i] = fields[22+i*2].GetUInt32();
            //pPlayerCreateInfo->item_slot[i] = fields[23+i*2].GetUInt8();
            /*std::stringstream ss;
            ss << "INSERT INTO playercreateinfo_items VALUES("
                << uint32(pPlayerCreateInfo->index) << ","
                << uint32(pPlayerCreateInfo->item_slot[i]) << ","
                << uint32(pPlayerCreateInfo->item[i]) << ","
                << "0)";
            sDatabase.Execute(ss.str().c_str());*/
        //}
       // for (i=0; i<10; i++)
        //{
            //pPlayerCreateInfo->spell[i] = fields[42+i].GetUInt16();
            /*std::stringstream ss;
            ss << "INSERT INTO playercreateinfo_spells VALUES("
                << uint32(pPlayerCreateInfo->index) << ","
                << uint32(pPlayerCreateInfo->spell[i]) << ");";
            sDatabase.Execute(ss.str().c_str());*/
        //}

        std::stringstream sql;
        sql << "SELECT * FROM playercreateinfo_skills WHERE indexid=" << uint32(pPlayerCreateInfo->index) << ";";
        QueryResult *sk_sql = sDatabase.Query(sql.str().c_str());

        if(sk_sql)
        {
            do 
            {
                Field *fields = sk_sql->Fetch();
                CreateInfo_SkillStruct tsk;
                tsk.skillid = fields[1].GetUInt32();
                tsk.currentval = fields[2].GetUInt32();
                tsk.maxval = fields[3].GetUInt32();
                pPlayerCreateInfo->skills.push_back(tsk);
            } while(sk_sql->NextRow());
        }
        delete sk_sql;
        sql.rdbuf()->str("");

        sql << "SELECT * FROM playercreateinfo_spells WHERE indexid=" << uint32(pPlayerCreateInfo->index) << ";";
        QueryResult *sp_sql = sDatabase.Query(sql.str().c_str());

        if(sp_sql)
        {
            do 
            {
                pPlayerCreateInfo->spell_list.push_back(sp_sql->Fetch()[1].GetUInt16());
            } while(sp_sql->NextRow());
        }

        delete sp_sql;
        sql.rdbuf()->str("");

        sql << "SELECT * FROM playercreateinfo_items WHERE indexid=" << uint32(pPlayerCreateInfo->index) << ";";
        QueryResult *items_sql = sDatabase.Query(sql.str().c_str());
        
        if(items_sql)
        {
            do 
            {
                Field *fields = items_sql->Fetch();
                CreateInfo_ItemStruct itm;
                itm.protoid = fields[1].GetUInt32();
                itm.slot = fields[2].GetUInt8();
                itm.amount = fields[3].GetUInt32();
                pPlayerCreateInfo->items.push_back(itm);
            } while(items_sql->NextRow());
        }
        delete items_sql;
        sql.rdbuf()->str("");

        sql << "SELECT * FROM playercreateinfo_bars WHERE class=" << uint32(pPlayerCreateInfo->class_) << ";";
        QueryResult *bars_sql = sDatabase.Query(sql.str().c_str());

        if(bars_sql)
        {
            do 
            {
                Field *fields = bars_sql->Fetch();
                CreateInfo_ActionBarStruct bar;
                bar.button = fields[2].GetUInt32();
                bar.action = fields[3].GetUInt32();
                bar.type = fields[4].GetUInt32();
                bar.misc = fields[5].GetUInt32();
                pPlayerCreateInfo->actionbars.push_back(bar);
            } while(bars_sql->NextRow());            
        }
        delete bars_sql;
        sql.rdbuf()->str("");

        AddPlayerCreateInfo(pPlayerCreateInfo);
        ++num;
        SetProgressBar(num, total, "Player Create Info");

    } while( result->NextRow() );

    delete result;
    ClearProgressBar();
}

// DK:LoadGuilds()
void ObjectMgr::LoadGuilds()
{
    std::stringstream query;
    QueryResult *result = sDatabase.Query( "SELECT * FROM guilds" );
    QueryResult *result2;    
    QueryResult *result3;

    if(!result)
    {
        sLog.outString("  Query failed: SELECT * FROM guilds");
        return;
    }

    Guild *pGuild;
    struct RankInfo rankList;

    do
    {
        Field *fields = result->Fetch();

        pGuild = new Guild;

        pGuild->SetGuildId( fields[0].GetUInt32() );
        pGuild->SetGuildName( fields[1].GetString() );
        pGuild->SetGuildLeaderGuid(fields[2].GetUInt64() );
        pGuild->SetGuildEmblemStyle( fields[3].GetUInt32() );
        pGuild->SetGuildEmblemColor(fields[4].GetUInt32() );
        pGuild->SetGuildBorderStyle( fields[5].GetUInt32() );
        pGuild->SetGuildBorderColor( fields[6].GetUInt32() );
        pGuild->SetGuildBackgroundColor( fields[7].GetUInt32() );
        pGuild->SetGuildInfo( fields[8].GetString() );
        pGuild->SetGuildMotd( fields[9].GetString() );

        result2 = sDatabase.Query( "SELECT guid FROM characters WHERE guildId=%u",pGuild->GetGuildId());
        if(result2)
        {
            do
            {
                PlayerInfo *pi=objmgr.GetPlayerInfo(result2->Fetch()->GetUInt32());
                if(pi)
                pGuild->AddGuildMember( pi );
            }while( result2->NextRow() );
            delete result2;
        }

        query.str("");
        query << "SELECT * FROM guild_ranks WHERE guildId=" << pGuild->GetGuildId() << " ORDER BY rankId";
        result3 = sDatabase.Query( query.str().c_str() );
        if(result3)
        { 
            do
            {
                Field *fields3 = result3->Fetch();

                rankList.name = fields3[2].GetString();
                rankList.rights = fields3[3].GetUInt32();

                pGuild->CreateRank(rankList.name, rankList.rights );
            }while( result3->NextRow() );
            delete result3;
        }
        pGuild->LoadGuildCreationDate();

        AddGuild(pGuild);

    }while( result->NextRow() );

    delete result;
}
    
bool ObjectMgr::SignedCharter(uint64 guid)
{
    std::list<Charter*>::iterator i;
    for(i=Guild_CharterList.begin();i!= Guild_CharterList.end();i++)
    {
        Charter * p=(*i);
        if(p->leaderGuid==guid)
            return true;

        if(p->signCount > 9)
            p->signCount = 9;
            
        for(uint32 x=0;x<p->signCount ;x++)
            if(p->signer[x]==guid)
                return true;
    
    }
    return false;
}

void ObjectMgr::LoadSpellExtraData()
{
    SpellExtraInfo *spelldata;
    QueryResult *result = sDatabase.Query( "SELECT * FROM spellextra" );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();

            spelldata = new SpellExtraInfo;
            spelldata->Id = fields[0].GetUInt32();
            spelldata->specialtype = fields[1].GetUInt32();
            spelldata->enchantableslots = fields[2].GetUInt32();
            spelldata->ExtraFlags = fields[3].GetUInt32();

            mExtraSpellData[spelldata->Id] = spelldata;
        } while( result->NextRow() );
        delete result;
    }
}

SpellExtraInfo* ObjectMgr::GetSpellExtraData(uint32 id)
{
    return mExtraSpellData[id];    
}

void ObjectMgr::SaveCharter(Charter *gc)
{
    std::stringstream ss2;
    
    if(!gc)
    {
        return;
    }
    sDatabase.Execute("DELETE FROM charters WHERE leaderguid = %u" , gc->leaderGuid);
    
    ss2 << "INSERT INTO charters (leaderGuid, guildName, itemGuid, signer1, signer2, signer3, signer4, signer5, signer6, signer7, signer8, signer9) VALUES(";
    ss2 << gc->leaderGuid << ", ";
    ss2 << "'" << gc->guildName << "', ";
    ss2 << gc->itemGuid << ", ";
    for(uint32 i=0; i <= 7; i++)
    {
         ss2 << gc->signer[i] << ", ";
    }
    ss2 <<  gc->signer[8] << ");";
    sDatabase.Execute(ss2.str( ).c_str( ));
}

void ObjectMgr::LoadCharters()
{
    QueryResult *result = sDatabase.Query( "SELECT * FROM charters" );

    if(!result)
        return;

    do
    {
        Field *fields = result->Fetch();
        Charter *pt=new Charter;
        
        pt->leaderGuid = fields[0].GetUInt64();
        pt->guildName = fields[1].GetString();
        pt->itemGuid = fields[2].GetUInt32();
        pt->signCount = 0;
        memset(pt->signer,0,sizeof(uint64)*9);

        for(int i = 0;i<9;i++)
        {
            uint64 guid=fields[i+3].GetUInt64();
            if(!guid || !this->GetPlayerInfo(guid))
                continue;
        
            pt->signer[pt->signCount++]=guid;
        }
        Guild_CharterList.push_back(pt);

    } while( result->NextRow() );

    delete result;
}



void ObjectMgr::DeletePlayerSignature(uint64 guid)
{
    //check if charter leader-> remove petion
    //if signed in charter-> remove signature
    for(std::list<Charter*>::iterator i=Guild_CharterList.begin();i!=Guild_CharterList.end();i++)
    {
        Charter *p=(*i);
        if(p->leaderGuid==guid)//charter leader
        {
            for(uint32 x=0;x<p->signCount;x++)
            {
                Player *m=GetPlayer(p->signer[x]);
                if(m)//player is online
                    m->UnSetCharterSigned();

            }
            sDatabase.Execute ("DELETE FROM charters WHERE leaderGuid=%u",(uint32)guid);
            Guild_CharterList.erase(i);
            delete p;
            return;
        }
        //check if player is regular signerz
        
        for(uint32 x=0;x<p->signCount++;x++)
        {
            if(p->signer[x]!=guid)continue;
            //ok got it
            
            p->signCount--;
            uint32 y;
            for( y=x;y<p->signCount;y++)
            p->signer[y]=p->signer[y+1];
            
            p->signer[y]=0;
            
            return;
        }
    }


}

Corpse* ObjectMgr::LoadCorpse(uint32 guid)
{
    Corpse *pCorpse;
    std::stringstream query;
    query << "SELECT * FROM Corpses WHERE guid = " << guid;

    QueryResult *result = sDatabase.Query( query.str().c_str() );

    if( !result )
        return NULL;

    do
    {
        Field *fields = result->Fetch();
        pCorpse = sObjHolder.Create<Corpse>(fields[0].GetUInt32());
        pCorpse->Create();

        pCorpse->SetPosition(fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat(), fields[4].GetFloat());
        pCorpse->SetZoneId(fields[5].GetUInt32());
        pCorpse->SetMapId(fields[6].GetUInt32());
        pCorpse->LoadValues( fields[7].GetString());
        pCorpse->SetSpawnedObject(true);
        pCorpse->SetLoadedFromDB(true);
        pCorpse->SetInstanceID(fields[8].GetUInt32());
        pCorpse->AddToWorld();
    } while( result->NextRow() );

    delete result;

    return pCorpse;
}

Corpse* ObjectMgr::LoadCorpse(CorpseTemplate *t)
{
    Corpse *pCorpse;

    pCorpse = sObjHolder.Create<Corpse>(t->_guid);
    pCorpse->Create();
    pCorpse->SetPosition(t->_x, t->_y, t->_z, t->_o);
    pCorpse->SetZoneId(t->_zone);
    pCorpse->SetMapId(t->_map);
    pCorpse->LoadValues(t->_fields);
    pCorpse->SetLoadedFromDB(t->_loadedfromdb);
    pCorpse->SetSpawnedObject(true);
    pCorpse->AddToWorld();
    
    return pCorpse;
}

//------------------------------------------------------
// Live corpse retreival.
// comments: I use the same tricky method to start from the last corpse instead of the first
//------------------------------------------------------
Corpse *ObjectMgr::GetCorpseByOwner(Player *pOwner)
{
    ASSERT(pOwner->GetGUIDLow());   // something very bad went wrong if thats 0.

    HM_NAMESPACE::hash_map<uint32, Corpse*>::iterator itr;
    for (itr = sObjHolder.Begin<Corpse>(); itr != sObjHolder.End<Corpse>(); ++itr)
    {
        if(itr->second->GetUInt64Value(CORPSE_FIELD_OWNER) == pOwner->GetGUID())
            return itr->second;
    }


    return NULL;
}

void ObjectMgr::DelinkPlayerCorpses(Player *pOwner)
{
    //dupe protection agaisnt crashs
    HM_NAMESPACE::hash_map<uint32, Corpse*>::iterator itr;
    for (itr = sObjHolder.Begin<Corpse>(); itr != sObjHolder.End<Corpse>(); ++itr)
    {
        if(itr->second->GetUInt64Value(CORPSE_FIELD_OWNER) == pOwner->GetGUID())
        {
           itr->second->SetUInt64Value(CORPSE_FIELD_OWNER, 0);
           itr->second->SetUInt32Value(CORPSE_FIELD_FLAGS, 5);
           itr->second->SetCorpseState(CORPSE_STATE_BONES);
           itr->second->DeleteFromDB();
           CorpseAddEventDespawn(itr->second);
        }
    }


}

void ObjectMgr::AddGossipText(GossipText *pGText)
{
    ASSERT( pGText->ID );
    ASSERT( mGossipText.find(pGText->ID) == mGossipText.end() );
    mGossipText[pGText->ID] = pGText;
}

GossipText *ObjectMgr::GetGossipText(uint32 ID)
{
    GossipTextMap::const_iterator itr;
    for (itr = mGossipText.begin(); itr != mGossipText.end(); itr++)
    {
        if(itr->second->ID == ID)
            return itr->second;
    }
    return NULL;
}

void ObjectMgr::LoadGossipText()
{
    GossipText *pGText;

    QueryResult *result = sDatabase.Query( "SELECT * FROM npc_text" );
    if(!result)
    {
        sLog.outString("  Query failed: SELECT * FROM npc_text");
        return;
    }

    uint32 total =(uint32)result->GetRowCount();
    int num = 0;
    do
    {
        Field *fields = result->Fetch();
        pGText = new GossipText;
        uint32 index = 0;
        pGText->ID = fields[index].GetUInt32();                         index++;
               
        for(uint32 i=0;i<8;i++)
        {
            pGText->Texts[i].Prob = fields[index].GetFloat();           index++;
            pGText->Texts[i].Text[0] = fields[index].GetString();       index++;
            pGText->Texts[i].Text[1] = fields[index].GetString();       index++;
            pGText->Texts[i].Lang = fields[index].GetUInt32();          index++;
            for(uint32 e=0;e<6;e++)
            {
                pGText->Texts[i].Emote[e] = fields[index].GetUInt32();  index++;
            }
        }
        ++num;
        if(!(num % 30)) SetProgressBar(num, total, "NPC Text");
        AddGossipText(pGText);

    }while( result->NextRow() );

    ClearProgressBar();
    delete result;

    result = sDatabase.Query("SELECT * FROM npc_gossip_textid");
    if(result)
    {
        uint32 entry, text;
        do 
        {
            entry = result->Fetch()[0].GetUInt32();
            text  = result->Fetch()[1].GetUInt32();

            mNpcToGossipText[entry] = text;

        } while(result->NextRow());
        delete result;
    }
}

void ObjectMgr::AddGraveyard(GraveyardTeleport *pgrave)
{
    ASSERT( pgrave );
    ASSERT( mGraveyards.find(pgrave->ID) == mGraveyards.end() );
    mGraveyards[pgrave->ID] = pgrave;
}

void ObjectMgr::LoadGraveyards()
{
    GraveyardTeleport *pgrave;
    QueryResult *result = sDatabase.Query( "SELECT * FROM graveyards" );
    if( !result )
    {
        sLog.outString("  Query failed: SELECT * FROM graveyards");
        return;
    }

    uint32 total = (uint32)result->GetRowCount();
    int num = 0;
    do
    {
        Field *fields = result->Fetch();
        pgrave = new GraveyardTeleport;
        pgrave->ID = fields[0].GetUInt32();
        pgrave->X = fields[1].GetFloat();
        pgrave->Y = fields[2].GetFloat();
        pgrave->Z = fields[3].GetFloat();
        pgrave->O = fields[4].GetFloat();
        pgrave->ZoneId = fields[5].GetUInt32();
        pgrave->AdjacentZoneId = fields[6].GetUInt32();
        pgrave->MapId = fields[7].GetUInt32();
        pgrave->FactionID = fields[8].GetUInt32();

        AddGraveyard(pgrave);
        ++num;
        if(!(num % 10)) SetProgressBar(num, total, "Graveyards");

    }while( result->NextRow() );

    ClearProgressBar();
    delete result;
}

void ObjectMgr::LoadTeleportCoords()
{
    TeleportCoords *tc = NULL;
    QueryResult *result = sDatabase.Query( "SELECT * FROM teleport_coords" );
    if( !result )
    {
        sLog.outString("  Query failed: SELECT * FROM teleport_coords");
        return;
    }

    uint32 total =(uint32) result->GetRowCount();
    int num = 0;
    do
    {
        Field *fields = result->Fetch();
        tc = new TeleportCoords;
        tc->id = fields[0].GetUInt32();
        tc->mapId = fields[2].GetUInt32();
        tc->x = fields[3].GetFloat();
        tc->y = fields[4].GetFloat();
        tc->z = fields[5].GetFloat();

        AddTeleportCoords(tc);
        ++num;
        if(!(num % 5)) SetProgressBar(num, total, "Teleport Locations");

    }while( result->NextRow() );
    ClearProgressBar();
    delete result;
}

void ObjectMgr::LoadGMTickets()
{
    QueryResult *result = sDatabase.Query( "SELECT * FROM gm_tickets" );

    GM_Ticket *ticket;
    if(result == 0)
        return;

    do
    {
        Field *fields = result->Fetch();

        ticket = new GM_Ticket;
        ticket->guid = fields[0].GetUInt32();
        ticket->name = fields[1].GetString();
        ticket->level = fields[2].GetUInt32();
        ticket->type = fields[3].GetUInt32();
        ticket->posX = fields[4].GetFloat();
        ticket->posY = fields[5].GetFloat();
        ticket->posZ = fields[6].GetFloat();
        ticket->message = fields[7].GetString();
        ticket->timestamp = fields[8].GetUInt32();

        AddGMTicket(ticket);

    } while( result->NextRow() );

    delete result;
}

void ObjectMgr::SaveGMTicket(uint64 guid)
{
    std::stringstream ss1;
    std::stringstream ss2;
    ss1 << "DELETE FROM gm_tickets WHERE guid = " << guid << ";";
    sDatabase.Execute(ss1.str( ).c_str( ));
    GM_Ticket* ticket = GetGMTicket(guid);
    if(!ticket)
    {
        return;
    }

    ss2 << "INSERT INTO gm_tickets (guid, name, level, type, posX, posY, posZ, message, timestamp) VALUES(";
    ss2 << ticket->guid << ", '";
    ss2 << ticket->name << "', ";
    ss2 << ticket->level << ", ";
    ss2 << ticket->type << ", ";
    ss2 << ticket->posX << ", ";
    ss2 << ticket->posY << ", ";
    ss2 << ticket->posZ << ", '";
    ss2 << ticket->message << "', ";
    ss2 << ticket->timestamp << ");";
    sDatabase.Execute(ss2.str( ).c_str( ));
}

void ObjectMgr::SetHighestGuids()
{
    QueryResult *result;
    uint32 corpseshi=0;
    uint32 gameobjectshi=0;

    result = sDatabase.Query( "SELECT MAX(guid) FROM characters" );
    if( result )
    {
        m_hiCharGuid = result->Fetch()[0].GetUInt32()+1;

        delete result;
    }

    result = sDatabase.Query( "SELECT MAX(id) FROM creatures" );
    if( result )
    {
        m_hiCreatureGuid = result->Fetch()[0].GetUInt32()+1;

        delete result;
    }

    result = sDatabase.Query("SELECT MAX(guid) FROM playeritems");
    if( result )
    {
        m_hiItemGuid = result->Fetch()[0].GetUInt32() + 1;
        delete result;
    }

    result = sDatabase.Query( "SELECT MAX(entry) FROM creature_names" );
    if( result )
    {
        m_hiNameGuid = result->Fetch()[0].GetUInt32()+1;

        delete result;
    }

    result = sDatabase.Query( "SELECT MAX(entry) FROM gameobject_names" );
    if( result )
    {
        m_hiGoNameGuid = result->Fetch()[0].GetUInt32()+1;

        delete result;
    }

    result = sDatabase.Query( "SELECT MAX(id) FROM gameobjects" );
    if( result )
    {
        gameobjectshi = result->Fetch()[0].GetUInt32()+1;

        delete result;
    }

    //FIXME: Corpses are Gameobjects. IF we add other gameobjects we need a unified table
    result = sDatabase.Query( "SELECT MAX(guid) FROM corpses" );
    if( result )
    {
        corpseshi = result->Fetch()[0].GetUInt32()+1;

        delete result;
    }
    if(corpseshi > gameobjectshi )
    {
        m_hiGoGuid = corpseshi;
    }
    else
    {
        m_hiGoGuid = gameobjectshi;
    }

    /*result = sDatabase.Query("SELECT MAX(id) FROM raids");
    m_hiGroupId = 0;    // because it is incremented
    if(result)
    {
        m_hiGroupId = result->Fetch()[0].GetUInt32();
        delete result;
    }*/

    //sLog.outString( "  Ch:%d Cr:%d Go:%d It:%d Grp:%d", m_hiCharGuid, m_hiCreatureGuid, m_hiGoGuid, m_hiItemGuid, m_hiGroupId);
}
uint32 ObjectMgr::GenerateAuctionID()
{
    objmgr.m_auctionid++;
    return objmgr.m_auctionid;
}
uint32 ObjectMgr::GenerateMailID()
{
    return m_mailid++;
}
uint32 ObjectMgr::GenerateLowGuid(uint32 guidhigh)
{
    Guard guard(m_guidGenMutex);
    switch(guidhigh)
    {
    case HIGHGUID_CONTAINER     :
    case HIGHGUID_ITEM          : return m_hiItemGuid++;     break;
    case HIGHGUID_UNIT          : return m_hiCreatureGuid++; break;
    case HIGHGUID_PLAYER        : return m_hiCharGuid++;     break;
    case HIGHGUID_GAMEOBJECT    : return m_hiGoGuid++;       break;
    case HIGHGUID_CORPSE        : return m_hiGoGuid++;       break;
    case HIGHGUID_DYNAMICOBJECT : return m_hiDoGuid++;       break;
    default                     : ASSERT(0); return 0;
    }   
}

void ObjectMgr::AddGameObjectName(GameObjectInfo *goinfo)
{
    ASSERT( mGameObjectNames.find(goinfo->ID) == mGameObjectNames.end() );
    //verifying if info for that gameobject already exists(need some cleaning here some time)
    GameObjectNameMap::iterator itr = mGameObjectNames.find( goinfo->ID );
    //if found we delete the old gameobject info
    if( itr != mGameObjectNames.end( ) )
        mGameObjectNames.erase(itr);
    mGameObjectNames[goinfo->ID] = goinfo;

    /*std::stringstream ss;
    ss << "INSERT INTO gameobject_names VALUES (";
    ss << goinfo->ID << ", ";
    ss << goinfo->Type << ", ";
    ss << goinfo->DisplayID << ", '";
    ss << goinfo->Name << "', ";
    ss << goinfo->sound0 << ", ";
    ss << goinfo->sound1 << ", ";
    ss << goinfo->sound2 << ", ";
    ss << goinfo->sound3 << ", ";
    ss << goinfo->sound4 << ", ";
    ss << goinfo->sound5 << ", ";
    ss << goinfo->sound6 << ", ";
    ss << goinfo->sound7 << ", ";
    ss << goinfo->sound8 << ", ";
    ss << goinfo->sound9 << ", ";
    ss << goinfo->Unknown1 << ", ";
    ss << goinfo->Unknown2 << ", ";
    ss << goinfo->Unknown3 << ", ";
    ss << goinfo->Unknown4 << ", ";
    ss << goinfo->Unknown5 << ", ";
    ss << goinfo->Unknown6 << ", ";
    ss << goinfo->Size << ")";
    sDatabase.Execute( ss.str( ).c_str( ) );*/
}

GameObjectInfo *ObjectMgr::GetGameObjectName(uint32 ID)
{
    GameObjectNameMap::const_iterator itr = mGameObjectNames.find( ID );
    if( itr != mGameObjectNames.end( ) )
        return itr->second;


    GameObjectInfo *goi;
    goi = new GameObjectInfo;
    goi->Type = 0;
    goi->ID = ID;
    goi->Name = "Unknown GameObject";
    goi->DisplayID = 0;
    goi->SpellFocus = 0;
    goi->sound1 = 0;
    goi->sound2 = 0;
    goi->sound3 = 0;
    goi->sound4 = 0;
    goi->sound5 = 0;
    goi->sound6 = 0;
    goi->sound7 = 0;
    goi->sound8 = 0;
    goi->sound9 = 0;
    goi->Unknown1 = 0;
    goi->Unknown2 = 0;
    goi->Unknown3 = 0;
    goi->Unknown4 = 0;
    goi->Unknown5 = 0;
    goi->Unknown6 = 0;
    
    AddGameObjectName(goi);
    return goi;
}

GameObjectInfo *ObjectMgr::GetGameObjectNameNULL(uint32 ID)
{
    GameObjectNameMap::const_iterator itr = mGameObjectNames.find( ID );
    if( itr != mGameObjectNames.end( ) )
        return itr->second;
    else
        return NULL;
}

void ObjectMgr::LoadGameObjectNames()
{
    GameObjectInfo *gon;
    QueryResult *result = sDatabase.Query( "SELECT * FROM gameobject_names" );
    if(result)
    {
        uint32 total =(uint32) result->GetRowCount();
        int num = 0;
        do
        {
            Field *fields = result->Fetch();

            gon = new GameObjectInfo;

            gon->ID = fields[0].GetUInt32();
            gon->Type =  fields[1].GetUInt32();
            gon->DisplayID =  fields[2].GetUInt32();
            gon->Name =  fields[3].GetString();
            gon->SpellFocus =  fields[4].GetUInt32();
            gon->sound1 =  fields[5].GetUInt32();
            gon->sound2 = fields[6].GetUInt32();
            gon->sound3 = fields[7].GetUInt32();
            gon->sound4 =  fields[8].GetUInt32();
            gon->sound5 =  fields[9].GetUInt32();
            gon->sound6 =  fields[10].GetUInt32();
            gon->sound7 =   fields[11].GetUInt32();
            gon->sound8 =  fields[12].GetUInt32();
            gon->sound9 =  fields[13].GetUInt32();
            gon->Unknown1 =  fields[14].GetUInt32();
            gon->Unknown2 =  fields[15].GetUInt32();
            gon->Unknown3 =  fields[16].GetUInt32();
            gon->Unknown4 =  fields[17].GetUInt32();
            gon->Unknown5 =  fields[18].GetUInt32();
            gon->Unknown6 =  fields[19].GetUInt32();
            gon->Unknown7 =  fields[20].GetUInt32();
            gon->Unknown8 =  fields[21].GetUInt32();
            gon->Unknown9 =  fields[22].GetUInt32();
            gon->Unknown10 =  fields[23].GetUInt32();
            gon->Unknown11 =  fields[24].GetUInt32();
            gon->Unknown12 =  fields[25].GetUInt32();
            gon->Unknown13 =  fields[26].GetUInt32();
            gon->Unknown14 =  fields[27].GetUInt32();
            gon->Size = fields[28].GetFloat();
            AddGameObjectName(gon);
            ++num;
            if(!(num % 350)) SetProgressBar(num, total, "Gameobject Names");
        } while( result->NextRow() );

        ClearProgressBar();
        delete result;
    }

    result = sDatabase.Query( "SELECT MAX(entry) FROM gameobject_names" );
    if(result)
    {
        m_hiGoNameGuid = result->Fetch()[0].GetUInt32();

        delete result;
    }
}

void ObjectMgr::ProcessGameobjectQuests()
{
    /*if(!mGameObjectNames.size())
        return;

    int total = mGameObjectNames.size();
    std::set<Quest*> tmp;
    for(HM_NAMESPACE::hash_map<uint32, Quest*>::iterator itr = sQuestMgr.Begin(); itr != sQuestMgr.End(); ++itr)
    {
        Quest *qst = itr->second;
        if(qst->count_required_item > 0 || 
            qst->required_mobtype[0] == QUEST_MOB_TYPE_GAMEOBJECT ||
            qst->required_mobtype[1] == QUEST_MOB_TYPE_GAMEOBJECT ||
            qst->required_mobtype[2] == QUEST_MOB_TYPE_GAMEOBJECT ||
            qst->required_mobtype[3] == QUEST_MOB_TYPE_GAMEOBJECT )
        {
            tmp.insert(qst);
        }
    }
    std::vector<GameObjectInfo*> gos;
    gos.reserve(5000);
    for(GameObjectNameMap::iterator it0 = mGameObjectNames.begin(); it0 != mGameObjectNames.end(); ++it0)
    {
        GameObjectInfo *gon = it0->second;
        gos.push_back(it0->second);
    }

    int c = 0;
    total = gos.size();
    for(std::vector<GameObjectInfo*>::iterator it0 = gos.begin(); it0 != gos.end(); ++it0)
    {
        GameObjectInfo *gon = (*it0);

        map<uint32, std::set<uint32> >* golootlist = &(LootMgr::getSingleton().quest_loot_go);
        map<uint32, std::set<uint32> >::iterator it2 = golootlist->find(gon->ID);
        //// QUEST PARSING
        for(std::set<Quest*>::iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
        {
            Quest *qst = *itr;
            int max = qst->count_required_mob;
            if(qst->count_required_item > max)
                max = qst->count_required_item;

            if(max > 0)
            {
                for(int i = 0; i < max; ++i)
                {
                    if(qst->required_mob[i] > 0)
                    {
                        if(qst->required_mob[i] == gon->ID &&
                        qst->required_mobtype[i] == QUEST_MOB_TYPE_GAMEOBJECT)
                        {
                            gon->goMap.insert( GameObjectGOMap::value_type( qst, qst->required_mobcount[i] ) );
                            sDatabase.WaitExecute("INSERT INTO gameobject_quest_pickup_binding VALUES(%u, %u, %u)",
                                gon->ID, qst->id, qst->required_mobcount[i]);
                        }
                    }
                    if(qst->required_itemcount[i] > 0 && it2 != golootlist->end())
                    {
                        // check our loot template for this item
                        for(std::set<uint32>::iterator it4 = it2->second.begin(); it4 != it2->second.end(); ++it4)
                        {
                            if((*it4) == qst->required_item[i])
                            {
                                //GOQuestItem it;
                                //it.itemid = qst->required_item[i];
                                //it.requiredcount = qst->required_itemcount[i];
                                //gon->itemMap.insert( GameObjectItemMap::value_type( qst, it ) );
                                //gon->itemMap[qst].insert(it);
                                gon->itemMap[qst].insert( std::map<uint32, uint32>::value_type( qst->required_item[i], qst->required_itemcount[i]) );
                                sDatabase.WaitExecute("INSERT INTO gameobject_quest_item_binding VALUES(%u, %u, %u, %u)",
                                    gon->ID, qst->id, qst->required_item[i], qst->required_itemcount[i]);
                            }
                        }
                    }
                }
            }
        }
        c++;
        if(!(c % 150))
            SetProgressBar(c, total, "Binding");
    }
    ClearProgressBar();*/
    int total = 0;
    QueryResult * result  = sDatabase.Query("SELECT * FROM gameobject_quest_item_binding");
    QueryResult * result2 = sDatabase.Query("SELECT * FROM gameobject_quest_pickup_binding");

    if(result)
        total += result->GetRowCount();
    if(result2)
        total += result2->GetRowCount();

    int pos = 0;
    GameObjectInfo * gon;
    Quest * qst;

    if(result)
    {
        do 
        {
            ++pos;
            Field * fields = result->Fetch();
            gon = GetGameObjectNameNULL(fields[0].GetUInt32());
            qst = sQuestMgr.FindQuest(fields[1].GetUInt32());
            if(gon && qst)
                gon->itemMap[qst].insert( make_pair( fields[2].GetUInt32(), fields[3].GetUInt32() ) );            

            if(!(pos % 10))
                SetProgressBar(pos, total, "Bindings");

        } while(result->NextRow());
        delete result;
    }


    if(result2)
    {
        do 
        {
            ++pos;
            Field * fields = result2->Fetch();
            gon = GetGameObjectNameNULL(fields[0].GetUInt32());
            qst = sQuestMgr.FindQuest(fields[1].GetUInt32());
            if(gon && qst)
                gon->goMap.insert( make_pair( qst, fields[2].GetUInt32() ) );

            if(!(pos % 10))
                SetProgressBar(pos, total, "Bindings");

        } while(result2->NextRow());
        delete result2;
    }
    
    ClearProgressBar();    
}

void ObjectMgr::LoadPvPAreas()
{
    std::stringstream query;
    QueryResult *result = sDatabase.Query( "SELECT * FROM pvpareas" );

    if(!result)
    {
        sLog.outString("  Query failed: SELECT * FROM pvpareas");
        return;
    }

    PvPArea *pPvPArea;
    int num = 0;
    uint32 total =(uint32) result->GetRowCount();

    do
    {
        Field *fields = result->Fetch();

        pPvPArea = new PvPArea;

        pPvPArea->AreaId = fields[0].GetUInt32();
        pPvPArea->AreaName = fields[1].GetString();
        pPvPArea->PvPType = fields[2].GetUInt16();

        AddPvPArea(pPvPArea);
        ++num;
        if(!(num % 20)) SetProgressBar(num, total, "PvP Zones");
    }while( result->NextRow() );

    ClearProgressBar();
    delete result;
}

Player* ObjectMgr::GetPlayer(const char* name, bool caseSensitive)
{
    HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
    if(!caseSensitive)
    {
        std::string targetName;
        std::string strName = name;
        std::transform(strName.begin(),strName.end(),strName.begin(),towlower);
        for (itr = sObjHolder.Begin<Player>(); itr != sObjHolder.End<Player>(); itr++)
        {
            targetName = itr->second->GetNameString();
            if(stricmp(targetName.c_str(), strName.c_str()) == 0)
                return itr->second;
        }        
    }
    else
    {
        for (itr = sObjHolder.Begin<Player>(); itr != sObjHolder.End<Player>(); itr++)
        {
            if(strcmp(itr->second->GetName(), name) == 0)
                return itr->second;
        }
    }

    return NULL;
}

Player* ObjectMgr::GetPlayer(uint64 guid)
{
    return sObjHolder.GetObject<Player>(guid);
}

Creature* ObjectMgr::GetCreature(uint64 guid)
{
    switch(UINT32_LOPART(GUID_HIPART(guid)))
    {
    case HIGHGUID_PET:
        return sObjHolder.GetObject<Pet>(guid);
        break;
    case HIGHGUID_UNIT:
        return sObjHolder.GetObject<Creature>(guid);
        break;
    }

    return NULL;
}

void ObjectMgr::AddCreatureSpawnTemplate(CreatureSpawnTemplate *ct)
{
    ASSERT( ct );
    ASSERT( mCreatureSpawnTemplates.find(ct->EntryID) == mCreatureSpawnTemplates.end() );
    mCreatureSpawnTemplates[ct->EntryID] = ct;
}

CreatureSpawnTemplate* ObjectMgr::GetCreatureSpawnTemplate(uint32 entryid) const
{
    CreatureSpawnTemplateMap::const_iterator itr = mCreatureSpawnTemplates.find( entryid );
    if( itr != mCreatureSpawnTemplates.end( ) )
        return itr->second;
    return NULL;
}

bool ObjectMgr::RemoveCreatureSpawnTemplate(uint32 entryid)
{
    CreatureSpawnTemplateMap::iterator i = mCreatureSpawnTemplates.find( entryid );
    if (i == mCreatureSpawnTemplates.end())
    {
        return false;
    }
    mCreatureSpawnTemplates.erase(i);
    return true;
}

void ObjectMgr::AddAuction(AuctionEntry *ah)
{
    ASSERT( ah );
    ASSERT( mAuctions.find(ah->Id) == mAuctions.end() );
    mAuctions[ah->Id] = ah;
}

AuctionEntry* ObjectMgr::GetAuction(uint32 id) const
{
    AuctionEntryMap::const_iterator itr = mAuctions.find( id );
    if( itr != mAuctions.end( ) )
        return itr->second;
    return NULL;
}

bool ObjectMgr::RemoveAuction(uint32 id)
{
    AuctionEntryMap::iterator i = mAuctions.find(id);
    if (i == mAuctions.end())
    {
        return false;
    }
    mAuctions.erase(i);
    return true;
}

ItemPrototype* ObjectMgr::GetItemPrototype(uint32 id) const
{
    ItemPrototypeMap::const_iterator itr = mItemPrototypes.find( id );
    if( itr != mItemPrototypes.end( ) )
        return itr->second;
    return NULL;
}

void ObjectMgr::AddItemPrototype(ItemPrototype *itemProto)
{
    ASSERT( itemProto );
    ASSERT( mItemPrototypes.find(itemProto->ItemId) == mItemPrototypes.end() );
    mItemPrototypes[itemProto->ItemId] = itemProto;
}


Item* ObjectMgr::GetAItem(uint64 id)
{
    ItemMap::const_iterator itr = mAitems.find(id);
    if (itr != mAitems.end())
    {
        return itr->second;
    }
    return NULL;
}

void ObjectMgr::AddAItem(Item* it)
{
    ASSERT( it );
    ASSERT( mAitems.find(it->GetGUID()) == mAitems.end());
    mAitems[it->GetGUID()] = it;
}

bool ObjectMgr::RemoveAItem(uint64 id)
{
    ItemMap::iterator i = mAitems.find(id);
    if (i == mAitems.end())
    {
        return false;
    }
    mAitems.erase(i);
    return true;
}

/*
Trainerspell* ObjectMgr::GetTrainerspell(uint32 id) const
{
TrainerspellMap::const_iterator itr = mTrainerspells.find( id );
if( itr != mTrainerspells.end( ) )
return itr->second;
return NULL;
}

void ObjectMgr::AddTrainerspell(Trainerspell *trainspell)
{
ASSERT( trainspell );
ASSERT( mTrainerspells.find(trainspell->Id) == mTrainerspells.end() );
mTrainerspells[trainspell->Id] = trainspell;
}*/

void ObjectMgr::AddPlayerCreateInfo(PlayerCreateInfo *playerCreate)
{
    ASSERT( playerCreate );
    mPlayerCreateInfo[playerCreate->index] = playerCreate;
}

PlayerCreateInfo* ObjectMgr::GetPlayerCreateInfo(uint8 race, uint8 class_) const
{
    PlayerCreateInfoMap::const_iterator itr;
    for (itr = mPlayerCreateInfo.begin(); itr != mPlayerCreateInfo.end(); itr++)
    {
        if( (itr->second->race == race) && (itr->second->class_ == class_) )
            return itr->second;
    }
    return NULL;
}

void ObjectMgr::AddGuild(Guild *pGuild)
{
    ASSERT( pGuild );
    mGuild[pGuild->GetGuildId()] = pGuild;
}

uint32 ObjectMgr::GetTotalGuildCount()
{
    uint32 count=0;
    GuildMap::const_iterator itr;
    for (itr = mGuild.begin();itr != mGuild.end(); itr++)
    {
        count++;
    }
    return count;
}

bool ObjectMgr::RemoveGuild(uint32 guildId)
{
    GuildMap::iterator i = mGuild.find(guildId);
    if (i == mGuild.end())
    {
        return false;
    }

    i->second->RemoveFromDb();

    mGuild.erase(i);

    //Guild *pGuild = GetGuild(guildId);
    //pGuild->RemoveFromDb();

    return true;
}

Guild* ObjectMgr::GetGuild(uint32 guildId)
{
    GuildMap::const_iterator itr = mGuild.find(guildId);
    if(itr == mGuild.end())
        return 0;
    return itr->second;
    //sLog.outError("guildId: %d",guildId);
    /*for (itr = mGuild.begin();itr != mGuild.end(); itr++)
    {
        if( itr->second->GetGuildId() == guildId )
            return itr->second;
    }
    return NULL;*/
}

Guild* ObjectMgr::GetGuildByLeaderGuid(uint64 leaderGuid)
{
    GuildMap::const_iterator itr;
    //sLog.outError("leaderGuid: %d",leaderGuid);
    for (itr = mGuild.begin();itr != mGuild.end(); itr++)
    {
        if( itr->second->GetGuildLeaderGuid() == leaderGuid )
            return itr->second;
    }
    return NULL;
}

Guild* ObjectMgr::GetGuildByGuildName(std::string guildName)
{
    GuildMap::const_iterator itr;
    //sLog.outError("guildname: %s",guildName.c_str());
    for (itr = mGuild.begin();itr != mGuild.end(); itr++)
    {
        if( itr->second->GetGuildName() == guildName )
            return itr->second;
    }
    return NULL;
}

void ObjectMgr::AddCharter(Charter* gc)
{

    Guild_CharterList.push_back(gc);
    SaveCharter(gc);
}

void ObjectMgr::DeleteCharter(uint64 leaderGuid)
{
    std::list<Charter*>::iterator i;
    for(i = Guild_CharterList.begin(); i != Guild_CharterList.end(); i++)
    {
        if((*i)->leaderGuid == leaderGuid)
        {
            sDatabase.Execute(
                "DELETE FROM charters WHERE leaderGuid=%u",(uint32)leaderGuid);
            delete (*i);
            Guild_CharterList.erase(i);
            return;
        }
    }
}

Charter *ObjectMgr::GetGuildCharter(uint64 leaderGuid)
{
    std::list<Charter*>::iterator i;
    for(i = Guild_CharterList.begin(); i != Guild_CharterList.end(); i++)
    {
        if((*i)->leaderGuid == leaderGuid)
        {
            return (*i);
        }
    }
    return NULL;
}

Charter *ObjectMgr::GetGuildCharterByCharterGuid(uint32 itemGuid)
{
    std::list<Charter*>::iterator i;
    for(i = Guild_CharterList.begin(); i != Guild_CharterList.end(); i++)
    {
        if((*i)->itemGuid == itemGuid)
        {
            return (*i);
        }
    }
    return NULL;
}

void ObjectMgr::AddTeleportCoords(TeleportCoords* TC)
{
    ASSERT( TC );
    mTeleports[TC->id] = TC;  
}

TeleportCoords* ObjectMgr::GetTeleportCoords(uint32 id) const
{
    TeleportMap::const_iterator itr = mTeleports.find( id );
    if( itr != mTeleports.end( ) )
        return itr->second;
    return NULL;
}

void ObjectMgr::AddGMTicket(GM_Ticket *ticket)
{
    ASSERT( ticket );
    GM_TicketList.push_back(ticket);

    // save
    SaveGMTicket(ticket->guid);
}

//modified for vs2005 compatibility
void ObjectMgr::remGMTicket(uint64 guid)
{
    for(std::list<GM_Ticket*>::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
    {
        if((*i)->guid == guid)
        {
            i = GM_TicketList.erase(i);
        }
        else
        {
            ++i;
        }
    }

    // kill from db
    sDatabase.Execute("DELETE FROM gm_tickets WHERE guid=%u", guid);
}

GM_Ticket* ObjectMgr::GetGMTicket(uint64 guid)
{
    for(std::list<GM_Ticket*>::iterator i = GM_TicketList.begin(); i != GM_TicketList.end(); i++)
    {
        if((*i)->guid == guid)
        {
            return (*i);
        }
    }
    return NULL;
}

void ObjectMgr::AddPvPArea(PvPArea* pvparea)
{
    ASSERT( pvparea );
    mPvPAreas[pvparea->AreaId] = pvparea;
}

PvPArea* ObjectMgr::GetPvPArea(uint32 AreaId)
{
    PvPAreaMap::const_iterator itr;
    for (itr = mPvPAreas.begin();itr != mPvPAreas.end(); itr++)
    {
        if( itr->second->AreaId == AreaId )
            return itr->second;
    }
    return NULL;
}

void ObjectMgr::LoadVendors()
{
    HM_NAMESPACE::hash_map<uint32, std::vector<CreatureItem>*>::iterator itr;
    std::vector<CreatureItem> *items;
    CreatureItem itm;

    std::stringstream query;
    query << "SELECT * FROM vendors";

    QueryResult *result = sDatabase.Query( query.str().c_str() );
    if(result)
    {
        int total = result->GetRowCount();
        int num = 0;
        do
        {
            Field *fields = result->Fetch();

            itr = mVendors.find(fields[0].GetUInt32());
            if (itr == mVendors.end())
            {
                items = new std::vector<CreatureItem>;
                mVendors[fields[0].GetUInt32()] = items;
            }
            else
            {
                items = itr->second;
            }
            
            itm.itemid = fields[1].GetUInt32();
            itm.amount = fields[2].GetUInt32();
            items->push_back(itm);

            ++num;
            if(!(num % 30)) SetProgressBar(num, total, "Vendors");

        }
        while( result->NextRow() );

        delete result;
    }
    ClearProgressBar();
}

std::vector<CreatureItem>* ObjectMgr::GetVendorList(uint32 entry)
{
    VendorMap::iterator itr;
    
    itr = mVendors.find(entry);
    
    if (itr == mVendors.end())
        return NULL;
    else
        return itr->second;
}

void ObjectMgr::LoadTotemSpells()
{
    int i = 0;
    std::stringstream query;
    QueryResult *result = sDatabase.Query( "SELECT * FROM totemspells" );

    if(!result)
    {
        sLog.outString("Query failed: SELECT * FROM totemspells");
        return;
    }

    TotemSpells *ts = NULL;

    int num = 0;
    uint32 total =(uint32) result->GetRowCount();
    SpellEntry * sp;
    uint32 spellid;

    do
    {
        Field *fields = result->Fetch();
        spellid = fields[1].GetUInt32();
        sp = sSpellStore.LookupEntry(spellid);
        if(!spellid || !sp) continue;

        m_totemSpells.insert( TotemSpellMap::value_type( fields[0].GetUInt32(), sp ));

        ++num;
        if(!(num % 5)) SetProgressBar(num, total, "Totem Spell Binding");
    } while( result->NextRow() );

    ClearProgressBar();
    delete result;
}

SpellEntry* ObjectMgr::GetTotemSpell(uint32 spellId)
{
    TotemSpellMap::const_iterator itr = m_totemSpells.find(spellId);
    if(itr == m_totemSpells.end()) return 0;
    return itr->second;
}

void ObjectMgr::LoadAIThreatToSpellId()
{
    std::stringstream query;
    QueryResult *result = sDatabase.Query( "SELECT * FROM ai_threattospellid" );

    if(!result)
    {
        sLog.outString("  Query failed: SELECT * FROM ai_threattospellid");
        return;
    }

    ThreatToSpellId *t2s = NULL;

    uint32 total =(uint32) result->GetRowCount();
    int num = 0;
    do
    {
        Field *fields = result->Fetch();

        t2s = new ThreatToSpellId;
        t2s->spellId = fields[0].GetUInt32();
        t2s->mod = atoi(fields[1].GetString());
        threatToSpells.push_back(t2s);
        ++num;
        if(!(num % 20)) SetProgressBar(num, total, "Spell Threat Modifiers");

    } while( result->NextRow() );

    ClearProgressBar();
    delete result;

}

int32 ObjectMgr::GetAIThreatToSpellId(uint32 spellId)
{
    for(std::list<ThreatToSpellId*>::iterator i = threatToSpells.begin(); i != threatToSpells.end(); i++)
    {
        if((*i)->spellId == spellId)
        {
            return (*i)->mod;
        }
    }
    return 0;
}

void ObjectMgr::LoadFishingZones()
{
    QueryResult *result = sDatabase.Query("SELECT * FROM fishing");
    if(!result)
    {
        sLog.outError("Query failed: SELECT * FROM fishing");
        return;
    }
    uint32 total =(uint32) result->GetRowCount();
    int num =0;

    FishingZoneEntry *entry = NULL;

    do 
    {
        Field *fields = result->Fetch();
        entry = new FishingZoneEntry;
        entry->MinSkill = fields[1].GetUInt32();
        entry->MaxSkill = fields[2].GetUInt32();
        uint32 id = fields[0].GetUInt32();

        mFishingZones[id] = entry;  // Add to hashmap
        ++num;
        if(!(num % 20)) SetProgressBar(num, total, "Fishing Zone Info");
    } while(result->NextRow());

    ClearProgressBar();
    delete result;  // now.. a memleak at the very start of server load would be baaaad :P
}

FishingZoneEntry* ObjectMgr::GetFishingZone(uint32 zoneid)
{
    FishingZoneMap::iterator i = mFishingZones.find(zoneid);
    if(i == mFishingZones.end()) return NULL;
    return i->second;
}

Unit* ObjectMgr::GetUnit(uint64 guid)
{
    switch(UINT32_LOPART(GUID_HIPART(guid)))
    {
    case HIGHGUID_UNIT:
        return sObjHolder.GetObject<Creature>(guid);
        break;
    case HIGHGUID_PLAYER:
        return sObjHolder.GetObject<Player>(guid);
        break;
    case HIGHGUID_PET:
        return sObjHolder.GetObject<Pet>(guid);
        break;
    }
    return NULL;
}

Unit* ObjectMgr::GetUnit(WoWGuid guid)
{
    return GetUnit(guid.GetOldGuid());
}

void ObjectMgr::LoadItemPages()
{
    QueryResult *result = sDatabase.Query("SELECT * FROM itempages");
    if(!result)
    {
        sLog.outString("  Query failed: SELECT * FROM itempages");
        return;
    }
    uint32 total = (uint32)result->GetRowCount();
    int num = 0;
    ItemPage *page = NULL;
    do 
    {
        Field *fields = result->Fetch();
        page = new ItemPage;
        page->text = fields[1].GetString();
        page->next_page = fields[2].GetUInt32();
        AddItemPage(fields[0].GetUInt32(), page);
        ++num;
        if(!(num % 50)) SetProgressBar(num, total, "Item Pages");
    } while(result->NextRow());
    ClearProgressBar();
    delete result;
}

Item * ObjectMgr::CreateItem(uint32 entry,Player * owner)
{
    ItemPrototype * proto = GetItemPrototype(entry);
    if(proto == 0) return 0;

    if(proto->InventoryType == INVTYPE_BAG)
    {
        Container * pContainer = new Container;
        pContainer->Create(GenerateLowGuid(HIGHGUID_CONTAINER), entry, owner);
        return pContainer;
    }
    else
    {
        Item * pItem = new Item;
        pItem->Create(GenerateLowGuid(HIGHGUID_ITEM), entry, owner);
        return pItem;
    }
}

Item * ObjectMgr::LoadItem(uint32 guid)
{
    QueryResult * result = sDatabase.Query("SELECT * FROM playeritems WHERE guid = %u", guid);
    Item * pReturn = 0;

    if(result)
    {
        ItemPrototype * pProto = GetItemPrototype(result->Fetch()[2].GetUInt32());
        if(pProto->InventoryType == INVTYPE_BAG)
        {
            Container * pContainer = new Container;
            pContainer->LoadFromDB(result->Fetch());
            pReturn = pContainer;
        }
        else
        {
            Item * pItem = new Item;
            pItem->LoadFromDB(result->Fetch(), 0, false);
            pReturn = pItem;
        }
        delete result;
    }
    
    return pReturn;
}

void SetProgressBar(int val, int mx, const char* label)
{
    printf ("[%s] ",label);

#ifndef WIN32
    printf("[");
    int barPos = val * 40/*30*/ / mx + 1;
    int p;
    for (p = 0; p < barPos; p++) putchar ('#');
    for (; p < 40/*30*/; p++) putchar (' ');

    printf ("] %d%%\r", val * 100 / mx);
#else
    printf("\xba");
    int l;
    if(label)
        l = strlen(label);
    else
        l = 0;
    if(l > 65)
        l = 65;

    int barPos = val * (65 - l) / mx + 1;
    int p;
    for (p = 0; p < barPos; p++) putchar (0xb1);
    for (; p <(65 - l); p++) putchar (' ');

    printf ("\xba %d%%\r", val * 100 / mx);
#endif
    fflush(stdout);
}

void ClearProgressBar()
{
    for(int p = 0; p<78;p++)
        putchar(' ');
    printf("\r");
}

//------------------------------------------------------------
// Corpse Collector Loading
// comments:
//------------------------------------------------------------
void ObjectMgr::CorpseCollectorLoad()
{
    Corpse *pCorpse = NULL;
 
    QueryResult *result = sDatabase.Query("SELECT * FROM corpses");

    if(result)
    {
        do
        {
          Field *fields = result->Fetch();
          pCorpse = sObjHolder.Create<Corpse>(fields[0].GetUInt32());
          pCorpse->Create();
          pCorpse->SetPosition(fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat(), fields[4].GetFloat());
          pCorpse->SetZoneId(fields[5].GetUInt32());
          pCorpse->SetMapId(fields[6].GetUInt32());
          pCorpse->LoadValues( fields[7].GetString());
          pCorpse->SetInstanceID(fields[8].GetUInt32());
          pCorpse->SetSpawnedObject(true);
          if(sWorldCreator.GetMap(pCorpse->GetMapId()) && sWorldCreator.GetMap(pCorpse->GetMapId())->InstanceExists(pCorpse->GetInstanceID()))
          {
              pCorpse->AddToWorld();
          }
          pCorpse = NULL;
        } while( result->NextRow() );
        
        delete result;
    }
}

std::list<ItemPrototype*>* ObjectMgr::GetListForItemSet(uint32 setid)
{
    ItemSetContentMap::iterator it = mItemSets.find(setid);
    if(it == mItemSets.end())
        return NULL;
    else
        return it->second;
}

void ObjectMgr::CorpseAddEventDespawn(Corpse *pCorpse)
{
    sEventMgr.AddEvent(this, &ObjectMgr::DespawnCorpse, pCorpse->GetGUID(), EVENT_CORPSE_DESPAWN, 600000, 1);
}

void ObjectMgr::DespawnCorpse(uint64 Guid)
{
    Corpse * pCorpse = sObjHolder.GetObject<Corpse>(Guid);
    if(pCorpse == 0)    // Already Deleted
        return;
    
    pCorpse->Despawn();
    sObjHolder.Delete<Corpse>(pCorpse);
}

void ObjectMgr::CorpseCollectorUnload()
{
    HM_NAMESPACE::hash_map<uint32, Corpse*>::iterator itr, it2;
    for (itr = sObjHolder.Begin<Corpse>(); itr != sObjHolder.End<Corpse>();)
    {
        it2 = itr;
        ++itr;
        sObjHolder.Delete<Corpse>(it2->second);
    }
}

GossipMenu::GossipMenu(uint64 Creature_Guid, uint32 Text_Id) : CreatureGuid(Creature_Guid), TextId(Text_Id)
{
    
}

void GossipMenu::AddItem(uint16 Icon, const char* Text, int32 Id)
{
    GossipMenuItem Item;
    Item.Icon = Icon;
    Item.Text = Text;
    Item.Id = Menu.size();
    if(Id > 0)
        Item.IntId = Id;
    else
        Item.IntId = Item.Id;        

    Menu.push_back(Item);
}

void GossipMenu::AddItem(GossipMenuItem* GossipItem)
{
    Menu.push_back(*GossipItem);
}

void GossipMenu::BuildPacket(WorldPacket& Packet)
{
    Packet << CreatureGuid;
    Packet << TextId;
    Packet << uint32(Menu.size());

    for(std::vector<GossipMenuItem>::iterator iter = Menu.begin();
        iter != Menu.end(); ++iter)
    {
        Packet << iter->Id;
        Packet << iter->Icon;
        Packet << uint32(0);    // something new in tbc. maybe gold requirement or smth?
        Packet << iter->Text;
        Packet << uint8(0); // ?
    }
}

void GossipMenu::SendTo(Player* Plr)
{
    WorldPacket data(SMSG_GOSSIP_MESSAGE, Menu.size() * 50 + 12);
    BuildPacket(data);
    data << uint32(0);  // 0 quests obviously
    Plr->GetSession()->SendPacket(&data);
}

void ObjectMgr::CreateGossipMenuForPlayer(GossipMenu** Location, uint64 Guid, uint32 TextID, Player* Plr)
{
    GossipMenu *Menu = new GossipMenu(Guid, TextID);
    ASSERT(Menu);

    if(Plr->CurrentGossipMenu != NULL)
        delete Plr->CurrentGossipMenu;

    Plr->CurrentGossipMenu = Menu;
    *Location = Menu;
}

GossipMenuItem GossipMenu::GetItem(uint32 Id)
{
    if(Id >= Menu.size())
    {
        GossipMenuItem k;
        k.IntId = 1;
        return k;
    } else {
        return Menu[Id];
    }
}

uint32 ObjectMgr::GetGossipTextForNpc(uint32 ID)
{
    NpcToGossipTextMap::iterator itr = mNpcToGossipText.find(ID);
    if(itr == mNpcToGossipText.end()) return 0;
    return itr->second;
}

void ObjectMgr::GenerateTrainerSpells()
{
    std::map<uint32, TrainerSpellOverride> OMap;
    
    QueryResult * result = sDatabase.Query("SELECT * FROM trainerspelloverride");
    if(result != 0)
    {
        uint32 mx = result->GetRowCount(), c = 0;
        do 
        {
            Field * f = result->Fetch();
            TrainerSpellOverride ov;
            ov.Cost = f[1].GetUInt32();
            ov.RequiredSpell = f[2].GetUInt32();
            ov.DeleteSpell = f[3].GetUInt32();
            ov.RequiredSkill = f[4].GetUInt32();
            ov.RequiredSkillValue = f[5].GetUInt32();
            ov.RequiredLevel = f[6].GetUInt32();
            ov.RequiredClass = f[7].GetUInt32();

            OMap[f[0].GetUInt32()] = ov;
            c++;
            if(!(c % 20))
                SetProgressBar(c, mx,"Spell Overrides");
        } while(result->NextRow());
        delete result;
        ClearProgressBar();
    }
    std::vector<uint32> itemSpell;

    // Lets take item spell learn list so we can remove recipe from trainers
    result = sDatabase.Query("SELECT spellid_1,spellid_2,spellid_3,spellid_4,spellid_5 FROM items");
    if(result != 0)
    {
        do 
        {
            Field * f = result->Fetch();
            uint32 s1,s2,s3,s4,s5;
            if((s1 = f[0].GetUInt32()) != 0)
            {
                SpellEntry *sp = sSpellStore.LookupEntry(s1);
                for(int i = 0; i < 3; i++)
                {
                    if(sp->Effect[i] == SPELL_EFFECT_LEARN_SPELL
                        || sp->Effect[i] == SPELL_EFFECT_LEARN_PET_SPELL)
                        itemSpell.push_back(sp->EffectTriggerSpell[i]);
                }
            }
            if((s2 = f[1].GetUInt32()) != 0)
            {
                SpellEntry *sp = sSpellStore.LookupEntry(s2);
                for(int i = 0; i < 3; i++)
                {
                    if(sp->Effect[i] == SPELL_EFFECT_LEARN_SPELL
                        || sp->Effect[i] == SPELL_EFFECT_LEARN_PET_SPELL)
                        itemSpell.push_back(sp->EffectTriggerSpell[i]);
                }
            }
            if((s3 = f[2].GetUInt32()) != 0)
            {
                SpellEntry *sp = sSpellStore.LookupEntry(s3);
                for(int i = 0; i < 3; i++)
                {
                    if(sp->Effect[i] == SPELL_EFFECT_LEARN_SPELL
                        || sp->Effect[i] == SPELL_EFFECT_LEARN_PET_SPELL)
                        itemSpell.push_back(sp->EffectTriggerSpell[i]);
                }
            }
            if((s4 = f[3].GetUInt32()) != 0)
            {
                SpellEntry *sp = sSpellStore.LookupEntry(s4);
                for(int i = 0; i < 3; i++)
                {
                    if(sp->Effect[i] == SPELL_EFFECT_LEARN_SPELL
                        || sp->Effect[i] == SPELL_EFFECT_LEARN_PET_SPELL)
                        itemSpell.push_back(sp->EffectTriggerSpell[i]);
                }
            }
            if((s5 = f[4].GetUInt32()) != 0)
            {
                SpellEntry *sp = sSpellStore.LookupEntry(s5);
                for(int i = 0; i < 3; i++)
                {
                    if(sp->Effect[i] == SPELL_EFFECT_LEARN_SPELL
                        || sp->Effect[i] == SPELL_EFFECT_LEARN_PET_SPELL)
                        itemSpell.push_back(sp->EffectTriggerSpell[i]);
                }
            }
        } while(result->NextRow());
        delete result;
    }

    // Convert ranks into our temp map.

    // i _hate_ string indexes :p
    map<string, map<uint32, uint32>* > SpellRankMap;
    map<uint32, uint32> SpellRanks;
    map<string, map<uint32, uint32>* >::iterator it1;
    map<uint32, uint32>::iterator it2;
    map<uint32, uint32> TeachingSpellMap;

    uint32 mx = sSpellStore.GetNumRows();
    for(uint32 i = 0; i < mx; ++i)
    {
        // Get Spell
        SpellEntry *Sp = static_cast<FastIndexedDataStore<SpellEntry>*>(SpellStore::getSingletonPtr())->LookupRow(i);
        if(Sp != NULL)
        {
            // Skip learning spells ;)
            // Skip any spells that are obselete
            if(Sp->SpellVisual == 222)
                continue;

            uint32 j;
            bool check = false;
            for(j = 0; j < 3; j++)
            {
                if(check)
                    break;
                check = false;
                if(Sp->Effect[j] == SPELL_EFFECT_LEARN_SPELL ||
                    Sp->Effect[j] == SPELL_EFFECT_LEARN_PET_SPELL)
                {
                    // BANNED SPELLS CUZ THEY'RE FUCKED.. GG BLIZZARD
                    if(Sp->Id == 21085 || Sp->Id == 1872)
                        continue;

                    const char* SpellName = sSpellStore.LookupString(Sp->Name);
                    const char* RankName = sSpellStore.LookupString(Sp->Rank);
                    if(!(TeachingSpellMap.find(Sp->EffectTriggerSpell[j]) == TeachingSpellMap.end()))
                    {
                        //printf("Duplicate training spell %s %s\n", SpellName, RankName);
                    } else {
                        for(std::vector<uint32>::iterator itr = itemSpell.begin(); itr != itemSpell.end(); ++itr)
                        {
                            if((*itr) == Sp->Id)
                            {
                                check = true;
                                break;
                            }
                        }
                        if(check)
                            continue;
                        TeachingSpellMap.insert( map<uint32, uint32>::value_type( Sp->EffectTriggerSpell[j], Sp->Id ) );
                    }
                    break;
                }
            }

            if(j != 3)
                continue;

            // Get our row name
            if(!Sp->Rank)
                continue;

            const char* SpellName = sSpellStore.LookupString(Sp->Name);
            const char* RankName = sSpellStore.LookupString(Sp->Rank);

            // Skip old spells
            if(SpellName[0] == 'z' && SpellName[1] == 'z' &&
                SpellName[2] == 'O' && SpellName[3] == 'L' &&
                SpellName[4] == 'D')
            {
                continue;
            }

            // Convert rank name into a number
            int32 RankNumber = -1;

            if(sscanf(RankName, "Rank %d", &RankNumber) != 1)  // Not a ranked spell
                continue;

            SpellRanks[Sp->Id] = RankNumber;

            // Insert into our map if we don't have one
            string Sp_Name = SpellName;
            it1 = SpellRankMap.find(Sp_Name);
            map<uint32, uint32> *mapPtr;
            if(it1 == SpellRankMap.end())
            {
                mapPtr = new map<uint32, uint32>;
                SpellRankMap.insert( map<string, map<uint32, uint32>* >::value_type( Sp_Name, mapPtr ) );
                it1 = SpellRankMap.find(Sp_Name);
                ASSERT(it1 != SpellRankMap.end());
            } else {
                mapPtr = it1->second;
            }

            if(!(mapPtr->find(RankNumber) == mapPtr->end()))
            {

                uint32 o = mapPtr->find(RankNumber)->second;
                SpellEntry *p = sSpellStore.LookupEntry(o);
                const char* SpellName2 = sSpellStore.LookupString(p->Name);
                const char* RankName2 = sSpellStore.LookupString(p->Rank);

                // For enchants, override the aura spell with casting spell.
                if(Sp->Effect[0] == 54 ||
                    Sp->Effect[1] == 54 ||
                    Sp->Effect[2] == 54)
                {
                    if(p->Effect[0] != 54 && p->Effect[1] != 54 && p->Effect[2] != 54)
                        mapPtr->find(RankNumber)->second = Sp->Id;
                }
            } else {
                mapPtr->insert( map<uint32, uint32>::value_type( (uint32)RankNumber, Sp->Id ) );
            }
        }
        if(!(i % 50))
        {
            SetProgressBar(i, mx, "Calculating Rank Trees");
        }
    }
    ClearProgressBar();

    skilllinespell *sp;
    SpellEntry * spell;
    TrainerSpellMap * destmap;

    mx = sSkillStore.GetNumRows();

    for(uint32 i = 0; i < mx; ++i)
    {
        sp = sSkillStore.LookupEntry(i);

        // Check if we're a learning spell :)
        spell = sSpellStore.LookupEntry(sp->spell);
        
        skilllineentry *skill = sSkillLineStore.LookupEntry(sp->skilline);
        ASSERT(skill);

        for(uint32 j = 0; j < 3; j++)
        {
            destmap = NULL;
            uint32 TeachingSpellId = TeachingSpellMap[sp->spell];
            
            const char* SpellName = sSpellStore.LookupString(spell->Name);
            const char* RankName = sSpellStore.LookupString(spell->Rank);

            if(!TeachingSpellId)
            {
                continue;
            }

            SpellEntry * TeachingSpell = sSpellStore.LookupEntry(TeachingSpellId);
            if(TeachingSpell->Effect[j] == SPELL_EFFECT_LEARN_SPELL)
                destmap = &mNormalSpells;
            else
                destmap = &mPetSpells;

            if(destmap == NULL)
                continue;

            // Create our trainer spell.
            uint32 SpellID = TeachingSpell->EffectTriggerSpell[j];

            // TODO: Check for quest reward spells, and shit like that.

            TrainerSpell * TS = new TrainerSpell;
            TS->pSpell = spell;
            TS->pTrainingSpell = TeachingSpell;
            TS->SpellID = SpellID;
            TS->TeachingSpellID = TeachingSpellId;
            TS->DeleteSpell = 0;
            TS->RequiredSpell = 0;
            TS->TeachingLine = 0;
            TS->IsProfession = 0;
            TS->RequiredClass = -1;

            // Find out our spell rank.
            string Sp_Name = SpellName;
            it1 = SpellRankMap.find(Sp_Name);
            if(it1 != SpellRankMap.end())
            {
                // We're a ranked spell.
                uint32 SpellRank = SpellRanks[SpellID];
                // Grab any ranks lower than ours
                if(SpellRank > 1)
                {
                    vector<uint32> lowerspells;
                    lowerspells.reserve(15);

                    // Assign required spells
                    uint32 crank = SpellRank - 1;
                    if(crank > 0)
                    {
                        it2 = it1->second->find(crank);
                        if((it2 != it1->second->end()))
                        {
                            uint32 rspell = it2->second;
                            ASSERT(rspell);
                            TS->RequiredSpell = rspell;

                            uint32 flags = spell->SpellFamilyName;
                            if(flags == 0x4 || flags == 0x10 || flags == 0x8 || flags == 0xA)
                                TS->DeleteSpell = rspell;
                        }
                    }
                }
            }

            // Profession checks.. la di da...
            TS->RequiredSkillLine = 0;
            TS->RequiredSkillLineValue = 0;
            
            TS->RequiredLevel = spell->spellLevel;
            TS->Cost = sWorld.mPrices[spell->spellLevel];

            if(skill->type == SKILL_TYPE_PROFESSION)
            {
                // Check if we're a profession learner. If we are, we need to have the previous
                // rank. If we're a profession spell, we need to have an amount

                uint32 l;
                for(l = 0; l < 3; ++l)
                {
                    if(TeachingSpell->Effect[l] == SPELL_EFFECT_SKILL_STEP)        // Rank
                    {
                        break;
                    }
                }
                if(l == 3)
                {
                    TS->RequiredSkillLineValue = 1;
                    TS->RequiredSkillLine = skill->id;
                }
                else
                {
                    //Here handles general profession learns
                    TS->TeachingLine = skill->id;
                    uint32 rval = 0;
                    // hack hack hack!
                    if(!stricmp(RankName, "Journeyman"))
                        rval = 75;
                    else if(!stricmp(RankName, "Expert"))
                        rval = 150;
                    else if(!stricmp(RankName, "Artisan"))
                        rval = 225;
                    else if(!stricmp(RankName, "Master"))
                        rval = 300;
                    else
                        TS->IsProfession = 1;

                    if(rval != 0)
                    {
                        TS->RequiredSkillLine = skill->id;
                        TS->RequiredSkillLineValue = rval;
                    }
                }
            }
            if(skill->type == SKILL_TYPE_SECONDARY)
            {
                uint32 rval = 0;
                //Riding skills again
                if(strstr(SpellName, "Riding") || strstr(SpellName, "Piloting")
                    || strstr(SpellName, "Horsemanship"))
                {                    
                    if(!stricmp(RankName, "Apprentice"))
                    {
                        TS->Cost = 900000;
                        TS->RequiredLevel = 40;
                    }
                    else if(!stricmp(RankName, "Journeyman"))
                    {
                        rval = 75;
                        TS->Cost = 6000000;
                        TS->RequiredLevel = 60;
                    }
                    else if(!stricmp(RankName, "Expert"))
                    {
                        rval = 150;
                        TS->Cost = 8000000;
                        TS->RequiredLevel = 70;
                    }
                    else if(!stricmp(RankName, "Artisan"))
                    {
                        rval = 225;
                        TS->Cost = 50000000;
                        TS->RequiredLevel = 70;
                    }
                    //No Master for now but if we have later
                    else if(!stricmp(RankName, "Master"))
                    {
                        rval = 300;
                        TS->RequiredLevel = 70;
                        TS->Cost = 90000000;
                    }
                    else//No old shit
                    {
                        break;
                    }
                }

                if(rval != 0)
                {
                    TS->RequiredSkillLine = skill->id;
                    TS->RequiredSkillLineValue = rval;
                }
            }
            std::map<uint32, TrainerSpellOverride>::iterator oitr = OMap.find(TS->TeachingSpellID);
            if(oitr == OMap.end()) oitr = OMap.find(TS->SpellID);
            if(oitr != OMap.end())
            {
                TrainerSpellOverride * ov = &oitr->second;
                TS->Cost = ov->Cost ? ov->Cost : TS->Cost;
                TS->RequiredClass = ov->RequiredClass ? ov->RequiredClass : TS->RequiredClass;
                TS->RequiredSpell = ov->RequiredSpell ? ov->RequiredSpell : TS->RequiredSpell;
                TS->RequiredSkillLine = ov->RequiredSkill ? ov->RequiredSkill : TS->RequiredSkillLine;
                TS->RequiredSkillLineValue = ov->RequiredSkillValue ? ov->RequiredSkillValue : TS->RequiredSkillLineValue;
                TS->DeleteSpell = ov->DeleteSpell ? ov->DeleteSpell : TS->DeleteSpell;
                TS->RequiredLevel = ov->RequiredLevel ? ov->RequiredLevel : TS->RequiredLevel;
                TS->RequiredClass = ov->RequiredClass ? ov->RequiredClass : TS->RequiredClass;
            } 

            if(skill->type == SKILL_TYPE_PROFESSION || skill->type == SKILL_TYPE_SECONDARY)
            {
                if(skill->type == SKILL_TYPE_SECONDARY)
                {
                    if(skill->id == 185 || skill->id == 129 || skill->id == 356)
                    {
                        if(sp->next == 0 && TS->RequiredSkillLineValue <= 1)
                        {
                            delete TS;
                            continue;
                        }
                    }
                }
                else
                {
                    if(TS->IsProfession != 1 && TS->RequiredSkillLineValue <= 1)
                    {
                        delete TS;
                        continue;
                    }
                }
            }

            TrainerSpellMap::iterator iter = destmap->find(sp->skilline);
            if(iter == destmap->end())
            {
                vector<TrainerSpell*> v;
                v.push_back(TS);
                destmap->insert( TrainerSpellMap::value_type( sp->skilline, v ) );
                //const char* skillname = sSkillLineStore.LookupString(skill->Name);
                /*printf("Skill line: %s created for %s.\n", skillname,
                    ( destmap == &mNormalSpells ? "NORMAL" : "PET"));*/
            } else {
                iter->second.push_back(TS);
            }
        }
        if(!(i % 50))
        {
            SetProgressBar(i, mx, "Generating Spell Lists");
        }
    }
    ClearProgressBar();

    // Cleanup
    for(it1 = SpellRankMap.begin(); it1 != SpellRankMap.end(); ++it1)
    {
        it1->second->clear();
        delete it1->second;
    }
    SpellRankMap.clear();
}

void ObjectMgr::LoadTrainers()
{
    QueryResult * result = sDatabase.Query("SELECT * FROM trainers");
    if(!result) return;

    uint32 mx = result->GetRowCount();
    uint32 c = 0;
    uint32 entry, maxlevel, class_;
    uint32 skilllines[20];
    skilllineentry *skill;
    do 
    {
        Field * fields = result->Fetch();
        entry = fields[0].GetUInt32();
        
        Trainer * tr = new Trainer;
        tr->TrainerType = 0;
        
        for(uint32 i = 0; i < 20; ++i)
        {
            skilllines[i] = fields[1+i].GetUInt32();
            skill = sSkillLineStore.LookupEntry(skilllines[i]);
            if(!skill)
            {
                skilllines[i] = 0;
                continue;
            }
            if(skill->type == SKILL_TYPE_PROFESSION ||
                skill->type == SKILL_TYPE_SECONDARY)
            {
                tr->TrainerType = 2;
            }
        }
        maxlevel = fields[21].GetUInt32();
        class_ = fields[22].GetUInt32();

        vector<TrainerSpell*> tmp;
        tmp.reserve(400);
        for(uint32 i = 0; i < 20; ++i)
        {
            if(skilllines[i] != 0)
            {
                //TODO: Check for pet trainer!
                TrainerSpellMap::iterator iter = mNormalSpells.find(skilllines[i]);
                if(iter == mNormalSpells.end())
                {
                    // Not found.
                    //printf("WARNING: No spells found under skill line %u\n", skilllines[i]);
                    continue;
                }

                for(vector<TrainerSpell*>::iterator it = iter->second.begin();
                    it != iter->second.end(); ++it)
                {
                    if((*it)->pSpell->spellLevel <= maxlevel || maxlevel == 60)    // 60 trainers have all?
                    {
                        skill = sSkillLineStore.LookupEntry(skilllines[i]);
                        if(skill->type == SKILL_TYPE_PROFESSION || skill->type == SKILL_TYPE_SECONDARY)
                        {
                            if(skill->type == SKILL_TYPE_SECONDARY)
                            {
                                if(skill->id == 185 || skill->id == 129 || skill->id == 356)
                                {
                                    bool add = AddTrainerSpell(entry, (*it)->pSpell);
                                    if(add)
                                        tmp.push_back((*it));
                                }
                                else
                                {
                                    tmp.push_back((*it));
                                }
                            }
                            else
                            {
                                bool add = AddTrainerSpell(entry, (*it)->pSpell);
                                if(add)
                                    tmp.push_back((*it));
                            }
                        }
                        else
                            tmp.push_back((*it));
                    }                        
                }
            }
        }
        tr->SpellCount = tmp.size();
        tr->SpellList = new TrainerSpell*[tmp.size()];

        uint32 j = 0;
        for(vector<TrainerSpell*>::iterator it = tmp.begin();
            it != tmp.end(); ++it, j++)
        {
            tr->SpellList[j] = (*it);
        }
        
        const char * temp = fields[23].GetString();
        tr->TalkMessage = new char[strlen(temp)+1];
        strcpy(tr->TalkMessage, temp);
        tr->TalkMessage[strlen(temp)] = 0;

        tr->RequiredClass = class_;

        mTrainers.insert( TrainerMap::value_type( entry, tr ) );
        c++;
        if(!(c % 50))
            SetProgressBar(c, mx, "Trainers");
        
    } while(result->NextRow());
    ClearProgressBar();
    delete result;
}

bool ObjectMgr::AddTrainerSpell(uint32 entry, SpellEntry *pSpell)
{
    CreatureInfo *ci = objmgr.GetCreatureName(entry);
    if(ci)
    {
        const char* RankName = sSpellStore.LookupString(pSpell->Rank);
        if(strstr(ci->SubName.c_str(),"Journeyman"))
        {
            if(!stricmp(RankName, "Journeyman"))
                return true;
            else if(!stricmp(RankName, "Expert"))
                return false;
            else if(!stricmp(RankName, "Artisan"))
                return false;
            else if(!stricmp(RankName, "Master"))
                return false;
            else
                return true;
        }
        else if(strstr(ci->SubName.c_str(),"Expert"))
        {
            if(!stricmp(RankName, "Journeyman"))
                return true;
            else if(!stricmp(RankName, "Expert"))
                return true;
            else if(!stricmp(RankName, "Artisan"))
                return false;
            else if(!stricmp(RankName, "Master"))
                return false;
            else
                return true;
        }
        else if(strstr(ci->SubName.c_str(),"Artisan"))
        {
            if(!stricmp(RankName, "Journeyman"))
                return true;
            else if(!stricmp(RankName, "Expert"))
                return true;
            else if(!stricmp(RankName, "Artisan"))
                return true;
            else if(!stricmp(RankName, "Master"))
                return false;
            else
                return true;
        }
        else
            return true;
    }
    else
        return false;
}

Trainer* ObjectMgr::GetTrainer(uint32 Entry)
{
    TrainerMap::iterator iter = mTrainers.find(Entry);
    if(iter == mTrainers.end())
        return NULL;

    return iter->second;
}

void ObjectMgr::ReloadTables()
{
    sWorld.SendWorldText("Reloading CreatureNames...");
    ReloadCreatureNames();

    sWorld.SendWorldText("Reloading Items...");
    ReloadItems();

    sWorld.SendWorldText("Reloading Quests...");
    ReloadQuests();

    sWorld.SendWorldText("Reloading CreatureSpawnTemplate...");
    ReloadCreatureSpawnTemplates();

    sWorld.SendWorldText("Reloading Gameobject Names...");
    ReloadGameObjects();
}

void ObjectMgr::ReloadCreatureNames()
{
    QueryResult * QR = sDatabase.Query("SELECT * FROM creature_names");
    if(QR)
    {
        uint32 ID;
        CreatureInfo* cn;
        CreatureNameMap::iterator it;
        do 
        {
            Field * fields = QR->Fetch();
            ID = fields[0].GetUInt32();

            it = mCreatureNames.find(ID);
            if(it == mCreatureNames.end())
                cn = new CreatureInfo;
            else
                cn = it->second;

            cn->Id = ID;
            cn->Name = fields[1].GetString();
            cn->SubName = fields[2].GetString();
            cn->Flags1 = fields[3].GetUInt32();
            cn->Type = fields[4].GetUInt32();
            cn->Family = fields[5].GetUInt32();
            cn->Rank = fields[6].GetUInt32();
            cn->Unknown1 = fields[7].GetUInt32();
            cn->SpellDataID = fields[8].GetUInt32();
            cn->DisplayID = fields[9].GetUInt32();
            cn->Civilian = fields[10].GetUInt8();
            cn->Leader = fields[11].GetUInt8();

            if(it == mCreatureNames.end())
                mCreatureNames.insert( CreatureNameMap::value_type( ID, cn ) );

        } while(QR->NextRow());
        delete QR;
    }
    sLog.outString("%u creature names reloaded.", mCreatureNames.size());
}

void ObjectMgr::ReloadItems()
{
    QueryResult * QR = sDatabase.Query("SELECT * FROM items");
    if(QR)
    {
        uint32 ID;
        uint32 i;
        ItemPrototype* pItemPrototype;
        ItemPrototypeMap::iterator it;
        do 
        {
            Field * fields = QR->Fetch();
            ID = fields[0].GetUInt32();

            it = mItemPrototypes.find(ID);
            if(it == mItemPrototypes.end())
                pItemPrototype = new ItemPrototype;
            else
                pItemPrototype = it->second;

            pItemPrototype->ItemId = fields[0].GetUInt32();
            pItemPrototype->Class = fields[1].GetUInt32();
            pItemPrototype->SubClass = fields[2].GetUInt32();
            pItemPrototype->Name1 = fields[3].GetString();
            pItemPrototype->Name2 = fields[4].GetString();
            pItemPrototype->Name3 = fields[5].GetString();
            pItemPrototype->Name4 = fields[6].GetString();
            pItemPrototype->DisplayInfoID = fields[7].GetUInt32();
            pItemPrototype->Quality = fields[8].GetUInt32();
            pItemPrototype->Flags = fields[9].GetUInt32();
            pItemPrototype->BuyPrice = fields[10].GetUInt32();
            pItemPrototype->SellPrice = fields[11].GetUInt32();
            pItemPrototype->InventoryType = fields[12].GetUInt32();
            pItemPrototype->AllowableClass = fields[13].GetUInt32();
            pItemPrototype->AllowableRace = fields[14].GetUInt32();
            pItemPrototype->ItemLevel = fields[15].GetUInt32();
            pItemPrototype->RequiredLevel = fields[16].GetUInt32();
            pItemPrototype->RequiredSkill = fields[17].GetUInt32();
            pItemPrototype->RequiredSkillRank = fields[18].GetUInt32();
            pItemPrototype->RequiredSkillSubRank = fields[19].GetUInt32();
            pItemPrototype->RequiredPlayerRank1 = fields[20].GetUInt32();
            pItemPrototype->RequiredPlayerRank2 = fields[21].GetUInt32();
            pItemPrototype->RequiredFaction = fields[22].GetUInt32();
            pItemPrototype->RequiredFactionStanding = fields[23].GetUInt32();
            pItemPrototype->Unique = fields[24].GetUInt32();
            pItemPrototype->MaxCount = fields[25].GetUInt32();
            pItemPrototype->ContainerSlots = fields[26].GetUInt32();
            for(i = 0; i < 20; i+=2)
            {
                pItemPrototype->ItemStatType[i/2] = fields[27 + i].GetUInt32();
                pItemPrototype->ItemStatValue[i/2] = fields[28 + i].GetUInt32();
            }
            for(i = 0; i < 15; i+=3)
            {
                // Stupid items.sql
                //  int *a=(int *)malloc(sizeof(int)); *a=fields[48 + i].GetUInt32();
                //int *b=(int *)malloc(sizeof(int)); *b=fields[49 + i].GetUInt32();

                pItemPrototype->DamageMin[i/3] = fields[47 + i].GetFloat();
                pItemPrototype->DamageMax[i/3] = fields[48 + i].GetFloat();
                /*
                */           
                //pItemPrototype->DamageMin[i/3] = fields[46 + +i].GetFloat();
                //pItemPrototype->DamageMax[i/3] = fields[47 +i].GetFloat();
                pItemPrototype->DamageType[i/3] = fields[49 + i].GetUInt32();
                //            free(a);free(b);
            }
            pItemPrototype->Armor = fields[62].GetUInt32();
            pItemPrototype->HolyRes = fields[63].GetUInt32();
            pItemPrototype->FireRes = fields[64].GetUInt32();
            pItemPrototype->NatureRes = fields[65].GetUInt32();
            pItemPrototype->FrostRes = fields[66].GetUInt32();
            pItemPrototype->ShadowRes = fields[67].GetUInt32();
            pItemPrototype->ArcaneRes = fields[68].GetUInt32();
            pItemPrototype->Delay = fields[69].GetUInt32();
            pItemPrototype->AmmoType = fields[70].GetUInt32();

            pItemPrototype->Range = fields[71].GetFloat();

            for(i = 0; i < 30; i+=6)
            {
                pItemPrototype->SpellId[i/6] = fields[72+i].GetUInt32();
                pItemPrototype->SpellTrigger[i/6] = fields[73+i].GetUInt32();
                pItemPrototype->SpellCharges[i/6] = fields[74+i].GetUInt32();
                pItemPrototype->SpellCooldown[i/6] = fields[75+i].GetUInt32();
                pItemPrototype->SpellCategory[i/6] = fields[76+i].GetUInt32();
                pItemPrototype->SpellCategoryCooldown[i/6] = fields[77+i].GetUInt32();
            }
            pItemPrototype->Bonding = fields[102].GetUInt32();
            pItemPrototype->Description = fields[103].GetString();
            pItemPrototype->PageId = fields[104].GetUInt32();
            pItemPrototype->PageLanguage = fields[105].GetUInt32();
            pItemPrototype->PageMaterial = fields[106].GetUInt32();
            pItemPrototype->QuestId = fields[107].GetUInt32();
            pItemPrototype->LockId = fields[108].GetUInt32();
            pItemPrototype->LockMaterial = fields[109].GetUInt32();
            pItemPrototype->Field108 = fields[110].GetUInt32();
            pItemPrototype->RandomPropId = fields[111].GetUInt32();
            pItemPrototype->Block = fields[112].GetUInt32();
            pItemPrototype->ItemSet = fields[113].GetUInt32();
            pItemPrototype->MaxDurability = fields[114].GetUInt32();
            pItemPrototype->ZoneNameID = fields[115].GetUInt32();
            pItemPrototype->Field114 = fields[116].GetUInt32();
            pItemPrototype->BagFamily = fields[117].GetUInt32();

            if(it == mItemPrototypes.end())
                mItemPrototypes.insert( ItemPrototypeMap::value_type( ID, pItemPrototype ) );

        } while(QR->NextRow());
        delete QR;
    }
    sLog.outString("%u items reloaded.", mItemPrototypes.size());
}

void ObjectMgr::ReloadCreatureSpawnTemplates()
{

    QueryResult * QR = sDatabase.Query("SELECT * FROM creaturespawntemplate");
    if(QR)
    {
        uint32 ID;
        CreatureSpawnTemplate* ct;
        CreatureSpawnTemplateMap::iterator it;
        do 
        {
            Field * fields = QR->Fetch();
            ID = fields[0].GetUInt32();

            it = mCreatureSpawnTemplates.find(ID);
            if(it == mCreatureSpawnTemplates.end())
                ct = new CreatureSpawnTemplate;
            else
                ct = it->second;

            int column = 0;
            ct->EntryID = fields[column].GetUInt32();
            column += 1;
            ct->MaxHealth = fields[column].GetUInt32();
            column += 1;
            ct->MaxMana = fields[column].GetUInt32();
            column += 1;
            ct->Armor = fields[column].GetUInt32();
            column += 1;
            ct->Level = fields[column].GetUInt32();
            column += 1;
            ct->Faction = fields[column].GetUInt32();
            column += 1;
            ct->Flag = fields[column].GetUInt32();
            column += 1;
            ct->Scale = fields[column].GetFloat();
            column += 1;
            ct->Speed = fields[column].GetFloat();
            column += 1;
            //ct->Rank = fields[column].GetUInt32();
            //column += 1;
            ct->MinDamage = fields[column].GetFloat();
            column += 1;
            ct->MaxDamage = fields[column].GetFloat();
            column += 1;
            ct->MinRangedDamage = fields[column].GetFloat();
            column += 1;
            ct->MaxRangedDamage = fields[column].GetFloat();
            column += 1;
            ct->BaseAttackTime = fields[column].GetUInt32();
            column += 1;
            ct->RangedAttackTime = fields[column].GetUInt32();
            column += 1;
            ct->BoundingRadius = fields[column].GetFloat();
            column += 1;
            ct->CombatReach = fields[column].GetFloat();
            column += 1;
            ct->Slot1Model = fields[column].GetUInt32();
            column += 1;
            ct->Slot1Info1 = fields[column].GetUInt8();
            column += 1;
            ct->Slot1Info2 = fields[column].GetUInt8();
            column += 1;
            ct->Slot1Info3 = fields[column].GetUInt8();
            column += 1;
            ct->Slot1Info4 = fields[column].GetUInt8();
            column += 1;
            ct->Slot1Info5 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Model = fields[column].GetUInt32();
            column += 1;
            ct->Slot2Info1 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Info2 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Info3 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Info4 = fields[column].GetUInt8();
            column += 1;
            ct->Slot2Info5 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Model = fields[column].GetUInt32();
            column += 1;
            ct->Slot3Info1 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Info2 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Info3 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Info4 = fields[column].GetUInt8();
            column += 1;
            ct->Slot3Info5 = fields[column].GetUInt8();
            column += 1;
            //ct->Type = fields[column].GetUInt32();
            //column += 1;
            ct->MountModelID = fields[column].GetUInt32();
            column += 1;

            if(it == mCreatureSpawnTemplates.end())
                mCreatureSpawnTemplates.insert( CreatureSpawnTemplateMap::value_type( ID, ct ) );

        } while(QR->NextRow());
        delete QR;
    }
    sLog.outString("%u creature spawntemplates reloaded.", mCreatureSpawnTemplates.size());
}

void ObjectMgr::ReloadQuests()
{

}

void ObjectMgr::ReloadGameObjects()
{
    QueryResult * QR = sDatabase.Query("SELECT * FROM gameobject_names");
    if(QR)
    {
        uint32 ID;
        GameObjectInfo* gon;
        GameObjectNameMap::iterator it;
        do 
        {
            Field * fields = QR->Fetch();
            ID = fields[0].GetUInt32();

            it = mGameObjectNames.find(ID);
            if(it == mGameObjectNames.end())
                gon = new GameObjectInfo;
            else
                gon = it->second;

            gon->ID = fields[0].GetUInt32();
            gon->Type =  fields[1].GetUInt32();
            gon->DisplayID =  fields[2].GetUInt32();
            gon->Name =  fields[3].GetString();
            gon->SpellFocus =  fields[4].GetUInt32();
            gon->sound1 =  fields[5].GetUInt32();
            gon->sound2 = fields[6].GetUInt32();
            gon->sound3 = fields[7].GetUInt32();
            gon->sound4 =  fields[8].GetUInt32();
            gon->sound5 =  fields[9].GetUInt32();
            gon->sound6 =  fields[10].GetUInt32();
            gon->sound7 =   fields[11].GetUInt32();
            gon->sound8 =  fields[12].GetUInt32();
            gon->sound9 =  fields[13].GetUInt32();
            gon->Unknown1 =  fields[14].GetUInt32();
            gon->Unknown2 =  fields[15].GetUInt32();
            gon->Unknown3 =  fields[16].GetUInt32();
            gon->Unknown4 =  fields[17].GetUInt32();
            gon->Unknown5 =  fields[18].GetUInt32();
            gon->Unknown6 =  fields[19].GetUInt32();
            gon->Unknown7 =  fields[20].GetUInt32();
            gon->Unknown8 =  fields[21].GetUInt32();
            gon->Unknown9 =  fields[22].GetUInt32();
            gon->Unknown10 =  fields[23].GetUInt32();
            gon->Unknown11 =  fields[24].GetUInt32();
            gon->Unknown12 =  fields[25].GetUInt32();
            gon->Unknown13 =  fields[26].GetUInt32();
            gon->Unknown14 =  fields[27].GetUInt32();
            gon->Size = fields[28].GetFloat();

            if(it == mGameObjectNames.end())
                mGameObjectNames.insert( GameObjectNameMap::value_type( ID, gon ) );

        } while(QR->NextRow());
        delete QR;
    }
    sLog.outString("%u gameobject names reloaded.", mGameObjectNames.size());
}

void ObjectMgr::GenerateLevelUpInfo()
{
    // Generate levelup information for each class.
    PlayerCreateInfo * PCI;
    for(uint32 Class = WARRIOR; Class <= DRUID; ++Class)
    {
        // These are empty.
        if(Class == 10 || Class == 6)
            continue;

        // Search for a playercreateinfo.
        for(uint32 Race = RACE_HUMAN; Race <= RACE_DRAENEI; ++Race )
        {
            PCI = GetPlayerCreateInfo(Race, Class);

            if(PCI == 0)
                continue;   // Class not valid for this race.

            // Generate each level's information
            uint32 MaxLevel = sWorld.LevelCap + 1;
            LevelInfo* lvl=0, lastlvl;
            lastlvl.HP = PCI->health;
            lastlvl.Mana = PCI->mana;
            lastlvl.Stat[0] = PCI->strength;
            lastlvl.Stat[1] = PCI->ability;
            lastlvl.Stat[2] = PCI->stamina;
            lastlvl.Stat[3] = PCI->intellect;
            lastlvl.Stat[4] = PCI->spirit;
            lastlvl.XPToNextLevel = 400;
            LevelMap * lMap = new LevelMap;

            // Create first level.
            lvl = new LevelInfo;
            *lvl = lastlvl;

            // Insert into map
            lMap->insert( LevelMap::value_type( 1, lvl ) );

            uint32 val;
            for(uint32 Level = 2; Level < MaxLevel; ++Level)
            {
                lvl = new LevelInfo;

                // Calculate Stats
                for(uint32 s = 0; s < 5; ++s)
                {
                    val = GainStat(Level, Class, s);
                    lvl->Stat[s] = lastlvl.Stat[s] + val;
                }

                // Calculate HP/Mana
                uint32 TotalHealthGain = 0;
                uint32 TotalManaGain = 0;

                switch(Class)
                {
                case WARRIOR:
                    if(Level<13)TotalHealthGain+=19;
                    else if(Level <36) TotalHealthGain+=Level+6;
                    else TotalHealthGain+=2*Level-30;
                    break;
                case HUNTER:
                    if(Level<13)TotalHealthGain+=17;
                    else TotalHealthGain+=Level+4;

                    if(Level<11)TotalManaGain+=29;
                    else if(Level<27)TotalManaGain+=Level+18;
                    else TotalManaGain+=45;
                    break;
                case ROGUE:
                    if(Level <15)TotalHealthGain+=17;
                    else TotalHealthGain+=Level+2;
                    break;
                case DRUID:
                    if(Level < 17)TotalHealthGain+=17;
                    else TotalHealthGain+=Level;

                    if(Level < 26)TotalManaGain+=Level+20;
                    else TotalManaGain+=45;
                    break;
                case MAGE:
                    if(Level < 23)TotalHealthGain+=15;
                    else TotalHealthGain+=Level-8;

                    if(Level <28)TotalManaGain+=Level+23;
                    else TotalManaGain+=51;
                    break;
                case SHAMAN:
                    if(Level <16)TotalHealthGain+=17;
                    else TotalHealthGain+=Level+1;

                    if(Level<22)TotalManaGain+=Level+19;
                    else TotalManaGain+=49;
                    break;
                case WARLOCK:
                    if(Level <17)TotalHealthGain+=17;
                    else TotalHealthGain+=Level-2;

                    if(Level< 30)TotalManaGain+=Level+21;
                    else TotalManaGain+=51;
                    break;
                case PALADIN:
                    if(Level < 14)TotalHealthGain+=18;
                    else TotalHealthGain+=Level+4;

                    if(Level<30)TotalManaGain+=Level+17;
                    else TotalManaGain+=42;
                    break;
                case PRIEST:
                    if(Level <21)TotalHealthGain+=15;
                    else TotalHealthGain+=Level-6;

                    if(Level <22)TotalManaGain+=Level+22;
                    else if(Level <32)TotalManaGain+=Level+37;
                    else TotalManaGain+=54;
                    break;
                }

                // Apply HP/Mana
                lvl->HP = lastlvl.HP + TotalHealthGain;
                lvl->Mana = lastlvl.Mana + TotalManaGain;

                // Calculate next level XP
                uint32 nextLvlXP = 0;
                if( Level > 0 && Level <= 30 )
                {
                    nextLvlXP = ((int)((((double)(8 * Level * ((Level * 5) + 45)))/100)+0.5))*100;
                }
                else if( Level == 31 )
                {
                    nextLvlXP = ((int)((((double)(((8 * Level) + 3) * ((Level * 5) + 45)))/100)+0.5))*100;
                }
                else if( Level == 32 )
                {
                    nextLvlXP = ((int)((((double)(((8 * Level) + 6) * ((Level * 5) + 45)))/100)+0.5))*100;
                }
                else
                {
                    nextLvlXP = ((int)((((double)(((8 * Level) + ((Level - 30) * 5)) * ((Level * 5) + 45)))/100)+0.5))*100;
                }

                lvl->XPToNextLevel = nextLvlXP;
                lastlvl = *lvl;
                lastlvl.HP = lastlvl.HP;

                // Apply to map.
                lMap->insert( LevelMap::value_type( Level, lvl ) );
            }

            // Now that our level map is full, let's create the class/race pair
            pair<uint32, uint32> p;
            p.first = Race;
            p.second = Class;

            // Insert back into the main map.
            mLevelInfo.insert( LevelInfoMap::value_type( p, lMap ) );
        }
    }
}

LevelInfo* ObjectMgr::GetLevelInfo(uint32 Race, uint32 Class, uint32 Level)
{
    // Iterate levelinfo map until we find the right class+race.
    LevelInfoMap::iterator itr = mLevelInfo.begin();
    for(; itr != mLevelInfo.end(); ++itr)
    {
        if(itr->first.first == Race &&
            itr->first.second == Class)
        {
            // We got a match.
            // Let's check that our level is valid first.
            if(Level > sWorld.LevelCap) // too far.
                Level = sWorld.LevelCap;

            // Pull the level information from the second map.
            LevelMap::iterator it2 = itr->second->find(Level);
            ASSERT(it2 != itr->second->end());

            return it2->second;
        }
    }

    return NULL;
}

void ObjectMgr::LoadDefaultPetSpells()
{
    QueryResult * result = sDatabase.Query("SELECT * FROM petdefaultspells");
    if(result)
    {
        do 
        {
            Field * f = result->Fetch();
            uint32 Entry = f[0].GetUInt32();
            uint32 spell = f[1].GetUInt32();
            SpellEntry * sp = sSpellStore.LookupEntry(spell);

            if(spell && Entry && sp)
            {
                PetDefaultSpellMap::iterator itr = mDefaultPetSpells.find(Entry);
                if(itr != mDefaultPetSpells.end())
                    itr->second.insert(sp);
                else
                {
                    set<SpellEntry*> s;
                    s.insert(sp);
                    mDefaultPetSpells[Entry] = s;
                }
            }
        } while(result->NextRow());

        delete result;
    }
}

set<SpellEntry*>* ObjectMgr::GetDefaultPetSpells(uint32 Entry)
{
    PetDefaultSpellMap::iterator itr = mDefaultPetSpells.find(Entry);
    if(itr == mDefaultPetSpells.end())
        return 0;

    return &(itr->second);
}

void ObjectMgr::LoadPetSpellCooldowns()
{
    DBCFile dbc;
    dbc.open("DBC/CreatureSpellData.dbc");
    uint32 SpellId;
    uint32 Cooldown;
    for(uint32 i = 0; i < dbc.getRecordCount(); ++i)
    {
        for(uint32 j = 0; j < 3; ++j)
        {
            SpellId = dbc.getRecord(i).getUInt(1 + j);
            Cooldown = dbc.getRecord(i).getUInt(5 + j);
            Cooldown *= 10;
            if(SpellId)
            {
                PetSpellCooldownMap::iterator itr = mPetSpellCooldowns.find(SpellId);
                if(itr == mPetSpellCooldowns.end())
                {
                    mPetSpellCooldowns.insert( make_pair(SpellId, Cooldown) );
                }
                else
                {
                    uint32 SP2 = mPetSpellCooldowns[SpellId];
                    ASSERT(Cooldown == SP2);
                }
            }
        }
    }
}

uint32 ObjectMgr::GetPetSpellCooldown(uint32 SpellId)
{
    PetSpellCooldownMap::iterator itr = mPetSpellCooldowns.find(SpellId);
    if(itr != mPetSpellCooldowns.end())
        return itr->second;
    return 0;
}

void ObjectMgr::LoadSpellFixes()
{
    // Loads data from stored 1.12 dbc to fix spells that have had spell data removed in 2.0.
    QueryResult * result = sDatabase.Query("SELECT * FROM spells112");
    if(result == 0) return;

    uint32 count = result->GetRowCount();
    uint32 counter = 0;
    uint32 fixed_count = 0;
    uint32 proc_chance;
    do 
    {
        ++counter;
        if(!(counter % 100))
            SetProgressBar(counter, count, "Repairing Spells...");

        Field * fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        SpellEntry * sp = sSpellStore.LookupEntry(entry);
        if(sp == 0) continue;

        // FIX SPELL GROUP RELATIONS
        {
            uint32 sgr[3];
            sgr[0] = fields[103].GetUInt32();
            sgr[1] = fields[104].GetUInt32();
            sgr[2] = fields[105].GetUInt32();
            proc_chance = fields[25].GetUInt32();

            for(uint32 i = 0; i < 3; ++i)
            {
                if(sgr[i] && sp->EffectSpellGroupRelation[i] == 0)
                {
                    //string name = fields[120].GetString();
                    //printf("%s[%u] %u->%u\n", name.c_str(),i, sp->EffectSpellGroupRelation[i], sgr[i]);
                    sp->EffectSpellGroupRelation[i] = sgr[i];
                    ++fixed_count;
                }
            }

            sp->procChance = min(proc_chance, sp->procChance);            
        }

        // FIX OTHER STUFF.. we'll find out..

    } while(result->NextRow());
    delete result;

    ClearProgressBar();
}

void ObjectMgr::SetVendorList(uint32 Entry, std::vector<CreatureItem>* list_)
{
    mVendors[Entry] = list_;
}

Creature * ObjectMgr::GetCreatureBySqlId(uint32 Sql_Id)
{
    Guard guard(CreatureSqlIdMapMutex);

    CreatureSqlIdMap::iterator itr = mCreatureSqlIds.find(Sql_Id);
    if(itr == mCreatureSqlIds.end()) return NULL;
    return itr->second;
}

void ObjectMgr::SetCreatureBySqlId(uint32 Sql_Id, Creature * pCreature)
{
    Guard guard(CreatureSqlIdMapMutex);

    if(pCreature != 0)
        mCreatureSqlIds[Sql_Id] = pCreature;
    else
    {
        CreatureSqlIdMap::iterator itr = mCreatureSqlIds.find(Sql_Id);
        if(itr != mCreatureSqlIds.end())
            mCreatureSqlIds.erase(itr);
    }
}



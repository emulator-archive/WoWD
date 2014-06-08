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

Player::Player ( uint32 high, uint32 low ): Unit(high, low)
{
    m_objectType           |= TYPE_PLAYER;
    m_objectTypeId          = TYPEID_PLAYER;

    m_valuesCount           = PLAYER_END;

    info                    = NULL;                 // Playercreate info
	bSafeFall               = false;
	SoulStone               = 0;
	Seal                    = 0;
	judgespell              = 0;
    m_session               = 0;
	TrackingSpell           = 0;
    m_status                = 0;
	offhand_dmg_mod         = 0.5;
    m_runSpeed              = PLAYER_NORMAL_RUN_SPEED;
    m_isMoving              = false;
	m_ShapeShifted          = 0;
    m_curTarget             = 0;
    m_curSelection          = 0;
    m_lootGuid              = 0;
    m_Summon                = NULL;
    m_PetNumberMax          = 0;
    m_lastShotTime          = 0;

    m_regenTimer            = 0;
    m_onTaxi                = false;
    
    m_taxi_pos_x            = 0;
    m_taxi_pos_y            = 0;
    m_taxi_pos_z            = 0;
    m_taxi_ride_time        = 0;
    m_fallTime              = 0;

    // Attack related variables
    m_blockfromspellPCT     = 0;
    m_blockfromspell        = 0;
    m_critfromspell         = 0;
    m_spellcritfromspell    = 0;
    m_dodgefromspell        = 0;
    m_parryfromspell        = 0;
    m_hitfromspell          = 0; 
    m_hitfrommeleespell     = 0;
    m_meleeattackspeedmod   = 0;
    m_rangedattackspeedmod  = 0;

    m_healthfromspell       = 0;
    m_manafromspell         = 0;
    m_healthfromitems       = 0;
    m_manafromitems         = 0;

    m_talentresettimes      = 0;

    m_nextSave              = getMSTime() + sWorld.getIntRate(INTRATE_SAVE);

    m_currentSpell          = NULL;
    m_resurrectHealth       = m_resurrectMana = 0;

    m_Group                 = NULL;
    m_SubGroup              = NULL;
    m_GroupInviter          = 0;
    
    Lfgcomment = "";
	
	for(int i=0;i<3;i++)
	{
		LfgType[i]=0;
		LfgDungeonId[i]=0;
	}
	
	m_Autojoin = true;
	m_AutoAddMem = false;

    m_invitersGuid          = 0;

    m_currentMovement       = MOVE_UNROOT;
    m_isGmInvisible         = false;

    //DK
    m_invitersGuid          = 0;

    //Trade
	ResetTradeVariables();
	mTradeTarget = 0;

    //Duel
    DuelingWith             = NULL;
    m_duelCountdownTimer    = 0;
    m_duelStatus            = 0;
    m_duelState             = 2;        // finished

    //WayPoint
    waypointunit            = NULL;

    //PVP
    //PvPTimeoutEnabled       = false;

    //Tutorials
    for ( int aX = 0 ; aX < 8 ; aX++ )
        m_Tutorials[ aX ] = 0x00;

//    eating = 0;
    m_lastRestUpdate        = 0;

    m_lootGuid              = 0;
    m_banned                = false;

    //Bind possition
    m_bind_pos_x            = 0;
    m_bind_pos_y            = 0;
    m_bind_pos_z            = 0;
    m_bind_mapid            = 0;
    m_bind_zoneid           = 0;

    // Rest
    m_timeLogoff            = 0;
    m_isResting             = 0;
    m_restState             = 0;
    m_restAmount            = 0;
    m_afk_reason            = "";
    m_playedtime[0]         = 0;
    m_playedtime[1]         = 0;
    m_playedtime[2]         = (uint32)time(NULL);

    m_AllowAreaTriggerPort  = true;

    // Battleground
    m_bgInBattleground      = false;
    m_bgBattlegroundID      = 0;
    m_bgLastBattlegroundID  = 0;
    m_bgEntryPointMap       = 0;
    m_bgEntryPointX         = 0;    
    m_bgEntryPointY         = 0;
    m_bgEntryPointZ         = 0;
    m_bgEntryPointO         = 0;

    m_bgTeam                = 0;
    m_bgHasFlag             = false;
    m_bgEntryPointInstance  = 0;

    // gm stuff
    //m_invincible            = false;
    bGMTagOn                = false;
    CooldownCheat           = false;
    CastTimeCheat           = false;
    PowerCheat              = false;
    GodModeCheat            = false;
    FlyCheat                = false;
    
    //FIX for professions
    weapon_proficiency      = 0x4000;//2^14
    //FIX for shit like shirt etc
    armor_proficiency       = 1;

    m_bUnlimitedBreath      = false;
    m_UnderwaterState       = 0;
    m_UnderwaterTime        = 60000;
    m_UnderwaterMaxTime     = 60000;
    m_UnderwaterLastDmg     = getMSTime();
    m_SwimmingTime          = 0;
    m_BreathDamageTimer     = 0;

    //transport shit
    m_TransporterGUID       = 0;
    m_TransporterX          = 0.0f;
    m_TransporterY          = 0.0f;
    m_TransporterZ          = 0.0f;
    m_TransporterO          = 0.0f;
    m_TransporterUnk        = 0.0f;
    m_lockTransportVariables= false;

    // Autoshot variables
    m_AutoShotStartX        = 0;
    m_AutoShotStartY        = 0;
    m_AutoShotStartZ        = 0;
    m_AutoShotTarget        = 0;

    m_AttackMsgTimer        = 0;

    timed_quest_slot        = 0;
    m_GM_SelectedGO         = NULL;

    for(uint32 x=0;x<7;x++)
    {
        FlatResistanceModifierPos[x]=0;
        FlatResistanceModifierNeg[x]=0;
        BaseResistanceModPctPos[x]=0;
        BaseResistanceModPctNeg[x]=0; 
        ResistanceModPctPos[x]=0;
        ResistanceModPctNeg[x]=0;
        SpellDmgDoneByInt[x]=0;
        SpellHealDoneByInt[x]=0;
    } 
        
    for(uint32 x=0;x<5;x++)
    {
        FlatStatModPos[x]=0;
        FlatStatModNeg[x]=0;
        StatModPctPos[x]=0;
        StatModPctNeg[x]=0;
		TotalStatModPctPos[x]=0;
		TotalStatModPctNeg[x]=0;
	}

	for(uint32 x=0;x<37;x++)
    {
        mRatingToPct[x]=0;
        mRatingToPoint[x]=0;
    } 

    m_targetIcon            = 0;
    bShouldHaveLootableOnCorpse = false;
    m_MountSpellId          = 0;
    bHasBindDialogOpen      = false;
    m_CurrentCharm          = NULL;
    m_CurrentTransporter    = NULL;
    m_SummonedObject        = NULL;
    m_currentLoot           = NULL;
    pctReputationMod        = 0;
    roll                    = 0;
    mUpdateCount            = 0;
    bUpdateBuffer.reserve(50000);//ought to be > than enough ;)
    mOutOfRangeIds.reserve(20000);
    mOutOfRangeIdCount      = 0;

    bProcessPending         = false;
    for(int i = 0; i < 25; ++i)
        m_questlog[i] = NULL;

    m_ItemInterface         = new ItemInterface(this);
    CurrentGossipMenu       = NULL;

    ResetHeartbeatCoords();
    cannibalize             = false;

    m_AreaID                = 0;
	m_actionsDirty          = false;
    m_bgScore               = 0;
    cannibalizeCount = 0;
    rageFromDamageDealt = 0;

    m_honorToday            = 0;
    m_honorYesterday        = 0;
    m_honorPoints           = 0;
    m_killsToday            = 0;
    m_killsYesterday        = 0;
    m_killsLifetime         = 0;
    m_lastSeenWeather       = 0;
    source                  = 0; // regarding mapmgr
    m_attacking             = false;
    
    myCorpse                = 0;
    bCorpseCreateable       = true;
    blinked = false;
    m_speedhackChances = 1;
    m_explorationTimer = getMSTime();
    linkTarget = 0;
    currentPVPArea = 0;
    stack_cheat = false;
    myGuild = 0;
    m_pvpTimer = 0;
    m_cooldownTimer = getMSTime() + 10000;
    GlobalCooldown = 0;
}


Player::~Player ( )
{
    if(mTradeTarget != 0)
        mTradeTarget->mTradeTarget = 0;
    mTradeTarget = 0;

    if(DuelingWith != 0)
        DuelingWith->DuelingWith = 0;
    DuelingWith = 0;

    CleanupGossipMenu();
    ASSERT(!IsInWorld());

    sEventMgr.RemoveEvents(this);

    // delete m_talenttree

    CleanupChannels();
    for(int i = 0; i < 25; ++i)
    {
        if(m_questlog[i] != NULL)
        {
            delete m_questlog[i];
        }
    }

    // clean up ITEMCOOLDOWN stuff
    if (m_itemcooldown.size())
    {
        ItemCooldownSet::iterator itr, it2;
        for (itr = m_itemcooldown.begin(); itr != m_itemcooldown.end();)
        {
            ItemCooldown * temp = (*itr);
            it2 = itr;
            ++itr;
            m_itemcooldown.erase(it2);
            delete (temp);
        }
    }
    

    if(m_Group != NULL)
        m_Group->RemovePlayer(this);

    for(SplineMap::iterator itr = _splineMap.begin(); itr != _splineMap.end(); ++itr)
        delete itr->second;
    _splineMap.clear();

    delete m_ItemInterface;
}

///====================================================================
///  Create
///  params: p_newChar
///  desc:   data from client to create a new character
///====================================================================
bool Player::Create( uint32 guidlow, WorldPacket& data )
{
    uint8 race,class_,gender,skin,face,hairStyle,hairColor,facialHair,outfitId;
    uint32 baseattacktime[2];

    Object::_Create(guidlow, HIGHGUID_PLAYER);

    //for (i = 0; i < BANK_SLOT_BAG_END; i++)
      //  m_items[i] = NULL;

    // unpack data into member variables
    data >> m_name;
    data >> race >> class_ >> gender >> skin >> face;
    data >> hairStyle >> hairColor >> facialHair >> outfitId;

    info = objmgr.GetPlayerCreateInfo(race, class_);
    if(!info)
    {
        // info not found... disconnect
        sCheatLog.writefromsession(m_session, "tried to create invalid player with race %u and class %u", race, class_);
        m_session->Disconnect();
        return false;
    }

    // check that the account CAN create TBC characters, if we're making some
    if(race >= RACE_BLOODELF && !m_session->HasFlag(ACCOUNT_FLAG_XPACK_01))
    {
        sCheatLog.writefromsession(m_session, "tried to create player with race %u and class %u but no expansion flags", race, class_);
        m_session->Disconnect();
        return false;
    }

    baseattacktime[0] = 2000;
    baseattacktime[1] = 2000;

    m_mapId = info->mapId;
    m_zoneId = info->zoneId;
    m_position.ChangeCoords(info->positionX, info->positionY, info->positionZ);
    m_bind_pos_x = info->positionX;
    m_bind_pos_y = info->positionY;
    m_bind_pos_z = info->positionZ;
    m_bind_mapid = info->mapId;
    m_bind_zoneid = info->zoneId;
    m_isResting = 0;
    m_restAmount = 0;
    m_restState = 0;

    memset(m_taximask, 0, sizeof(uint32)*8);
    
    // set race dbc
    myRace = sCharRaceStore.LookupEntry(race);
    myClass = sCharClassStore.LookupEntry(class_);
    if(!myRace || !myClass)
    {
        // information not found
        sCheatLog.writefromsession(m_session, "tried to create invalid player with race %u and class %u, dbc info not found", race, class_);
        m_session->Disconnect();
        return false;
    }

    sLog.outString("Account %s creating a %s %s %s", m_session->GetAccountName().c_str(), gender ? "Female" : "Male", 
        sCharRaceStore.LookupString(myRace->name2), sCharClassStore.LookupString(myClass->name));

    uint8 powertype = myClass->power_type;

    // Automatically add the race's taxi hub to the character's taximask at creation time ( 1 << (taxi_node_id-1) )
    switch(race)
    {
    case RACE_TAUREN:        m_taximask[0]= 1 << (22-1); break;
    case RACE_HUMAN:         m_taximask[0]= 1 << ( 2-1); break;
    case RACE_DWARF:         m_taximask[0]= 1 << ( 6-1); break;
    case RACE_NIGHTELF:      m_taximask[0]= 1 << (27-1); break;
    case RACE_GNOME:         m_taximask[0]= 1 << ( 6-1); break;
    case RACE_ORC:           m_taximask[0]= 1 << (23-1); break;
    case RACE_UNDEAD:        m_taximask[0]= 1 << (11-1); break;
    case RACE_TROLL:         m_taximask[0]= 1 << (23-1); break;
    }

    // Set Starting stats for char
    SetFloatValue(OBJECT_FIELD_SCALE_X, ((race==RACE_TAUREN)?1.3f:1.0f));
    SetUInt32Value(UNIT_FIELD_HEALTH, info->health);
    SetUInt32Value(UNIT_FIELD_POWER1, info->mana );
    SetUInt32Value(UNIT_FIELD_POWER2, 0 ); // this gets devided by 10
    SetUInt32Value(UNIT_FIELD_POWER3, info->focus );
    SetUInt32Value(UNIT_FIELD_POWER4, info->energy );
   
	SetUInt32Value(UNIT_FIELD_MAXHEALTH, info->health);
    SetUInt32Value(UNIT_FIELD_MAXPOWER1, info->mana );
    SetUInt32Value(UNIT_FIELD_MAXPOWER2, info->rage );
    SetUInt32Value(UNIT_FIELD_MAXPOWER3, info->focus );
    SetUInt32Value(UNIT_FIELD_MAXPOWER4, info->energy );
	
	//THIS IS NEEDED
	SetUInt32Value(UNIT_FIELD_BASE_HEALTH, info->health);
    SetUInt32Value(UNIT_FIELD_BASE_MANA, info->mana );
	SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, info->factiontemplate );
	SetUInt32Value(UNIT_FIELD_LEVEL, 1 );
    
    SetUInt32Value(UNIT_FIELD_BYTES_0, ( ( race ) | ( class_ << 8 ) | ( gender << 16 ) | ( powertype << 24 ) ) );
    //UNIT_FIELD_BYTES_1    (standstate) | (unk1) | (unk2) | (attackstate)
    if(class_ == WARRIOR)
        SetUInt32Value(UNIT_FIELD_BYTES_1, 0x00110000 );
    else
        SetUInt32Value(UNIT_FIELD_BYTES_1, 0x00000000 );
    SetUInt32Value(UNIT_FIELD_BYTES_2, (0x28 << 8) );
    SetFlag(UNIT_FIELD_FLAGS , U_FIELD_FLAG_PLAYER_CONTROLLED );
    SetUInt32Value(UNIT_FIELD_STAT0, info->strength );
    SetUInt32Value(UNIT_FIELD_STAT1, info->ability );
    SetUInt32Value(UNIT_FIELD_STAT2, info->stamina );
    SetUInt32Value(UNIT_FIELD_STAT3, info->intellect );
    SetUInt32Value(UNIT_FIELD_STAT4, info->spirit );
    SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, baseattacktime[0] );
    SetUInt32Value(UNIT_FIELD_BASEATTACKTIME+1, baseattacktime[1]  );
    SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.388999998569489f );
    SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f   );
    if(race != 10)
    {
        SetUInt32Value(UNIT_FIELD_DISPLAYID, info->displayId + gender );
        SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, info->displayId + gender );
    } else	{
        SetUInt32Value(UNIT_FIELD_DISPLAYID, info->displayId - gender );
        SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, info->displayId - gender );
    }
    //SetFloatValue(UNIT_FIELD_MINDAMAGE, info->mindmg );
    //SetFloatValue(UNIT_FIELD_MAXDAMAGE, info->maxdmg );
    SetUInt32Value(UNIT_FIELD_ATTACK_POWER, info->attackpower );
    SetUInt32Value(PLAYER_BYTES, ((skin) | (face << 8) | (hairStyle << 16) | (hairColor << 24)));
    //PLAYER_BYTES_2                             GM ON/OFF     BANKBAGSLOTS   RESTEDSTATE
   // SetUInt32Value(PLAYER_BYTES_2, (facialHair | (0xEE << 8) | (0x01 << 16) | (0x02 << 24)));
    SetUInt32Value(PLAYER_BYTES_2, (facialHair /*| (0xEE << 8)*/  | (0x02 << 24)));//no bank slot by default!

    //PLAYER_BYTES_3                           DRUNKENSTATE                 PVPRANK
    SetUInt32Value(PLAYER_BYTES_3, ((gender) | (0x00 << 8) | (0x00 << 16) | (GetPVPRank() << 24)));
    SetUInt32Value(PLAYER_NEXT_LEVEL_XP, 400);
    SetUInt32Value(PLAYER_FIELD_BYTES, 0x08 );
    SetUInt32Value(PLAYER_CHARACTER_POINTS2,2);
    SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);
    SetUInt32Value(PLAYER_FIELD_MAX_LEVEL, sWorld.LevelCap);
  
    for(uint32 x=0;x<7;x++)
    SetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+x, 1.00);
    //SetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG, 0);
    //SetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, 0);

    SetUInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX, 0xEEEEEEEE);

    m_StableSlotCount = 0;
    Item *item;

    

    for(std::list<uint16>::iterator sp = info->spell_list.begin();sp!=info->spell_list.end();sp++)
    {
		mSpells.insert((*sp));
    }

    m_FirstLogin = true;

    for(std::list<CreateInfo_SkillStruct>::iterator ss = info->skills.begin(); ss!=info->skills.end(); ss++)
    {
        if(ss->skillid && ss->currentval && ss->maxval)
            AddSkillLine(ss->skillid, ss->currentval, ss->maxval);        
    }
	//Chances depend on stats must be in this order!
	UpdateStats();
    UpdateChances();
    
    _InitialReputation();

    // Add actionbars
    for(std::list<CreateInfo_ActionBarStruct>::iterator itr = info->actionbars.begin();itr!=info->actionbars.end();++itr)
    {
        addAction(itr->button, itr->action, itr->type, itr->misc);
    }

    //Base Data filling
    memcpy(m_baseUint32Values, m_uint32Values,m_valuesCount*sizeof(uint32));

	for(std::list<CreateInfo_ItemStruct>::iterator is = info->items.begin(); is!=info->items.end(); is++)
    {
        if ( (*is).protoid != 0)
        {
			item=objmgr.CreateItem((*is).protoid,this);
			if(item)
			{
				item->SetUInt32Value(ITEM_FIELD_STACK_COUNT,(*is).amount);
				if((*is).slot<EQUIPMENT_SLOT_END)
					GetItemInterface()->SafeAddItem(item, INVENTORY_SLOT_NOT_SET, (*is).slot);
				else
					GetItemInterface()->AddItemToFreeSlot(item);
			}
            
        }
    }

    return true;
}

bool Player::Create( WorldPacket& data )
{
    return Create(m_GuidLow, data);
}

void Player::Update( uint32 p_time )
{
    if(!IsInWorld())
        return;

    Unit::Update( p_time );
    uint32 mstime = getMSTime();

    if(m_attacking)
    {
        // Check attack timer.
        if(mstime >= m_attackTimer)
            _EventAttack(false);

        if(m_duelWield && mstime >= m_attackTimer_1)
            _EventAttack(true);
    }
    
    // Breathing
    if(m_UnderwaterState & UNDERWATERSTATE_UNDERWATER)
    {
        // keep subtracting timer
        if(m_UnderwaterTime)
        {
            // not taking dmg yet
            if(p_time >= m_UnderwaterTime)
                m_UnderwaterTime = 0;
            else
                m_UnderwaterTime -= p_time;
        }

        if(!m_UnderwaterTime)
        {
            // check last damage dealt timestamp, and if enough time has elapsed deal damage
            if(mstime >= m_UnderwaterLastDmg)
            {
                uint32 damage = m_uint32Values[UNIT_FIELD_MAXHEALTH] / 10;
                WorldPacket data(SMSG_ENVIRONMENTALDAMAGELOG, 21);
                data << GetGUID() << uint8(DAMAGE_DROWNING) << damage << uint64(0);
                SendMessageToSet(&data, true);

                DealDamage(this, damage, 0, 0, 0);
                m_UnderwaterLastDmg = mstime + 1000;
            }
        }
    }
    else
    {
        // check if we're not on a full breath timer
        if(m_UnderwaterTime < m_UnderwaterMaxTime)
        {
            // regenning
            m_UnderwaterTime += (p_time * 10);
            if(m_UnderwaterTime >= m_UnderwaterMaxTime)
            {
                m_UnderwaterTime = m_UnderwaterMaxTime;
                StopMirrorTimer(1);
            }
        }
    }

    // Lava Damage
    if(m_UnderwaterState & UNDERWATERSTATE_LAVA)
    {
        // check last damage dealt timestamp, and if enough time has elapsed deal damage
        if(mstime >= m_UnderwaterLastDmg)
        {
            uint32 damage = m_uint32Values[UNIT_FIELD_MAXHEALTH] / 5;
            WorldPacket data(SMSG_ENVIRONMENTALDAMAGELOG, 21);
            data << GetGUID() << uint8(DAMAGE_LAVA) << damage << uint64(0);
            SendMessageToSet(&data, true);

            DealDamage(this, damage, 0, 0, 0);
            m_UnderwaterLastDmg = mstime + 1000;
        }
    }

    // Autosave
    if(mstime >= m_nextSave)
        SaveToDB(false);

    if(m_CurrentTransporter && !m_lockTransportVariables)
    {
        // Update our position, using trnasporter X/Y
        float c_tposx = m_CurrentTransporter->GetPositionX() + m_TransporterX;
        float c_tposy = m_CurrentTransporter->GetPositionY() + m_TransporterY;
        float c_tposz = m_CurrentTransporter->GetPositionZ() + m_TransporterZ;
        SetPosition(c_tposx, c_tposy, c_tposz, GetOrientation(), false);
    }

    // Exploration
    if(mstime >= m_explorationTimer)
    {
        _EventExploration();
        m_explorationTimer = mstime + 3000;
    }

    // Cooldowns
    if(mstime >= m_cooldownTimer)
    {
        UpdateCooldowns();
        m_cooldownTimer = mstime + 10000;
    }

    if(m_pvpTimer)
    {
        if(p_time >= m_pvpTimer)
        {
            RemovePvPFlag();
            m_pvpTimer = 0;
        }
        else
            m_pvpTimer -= p_time;
    }
}

void Player::EventDismount(uint32 money, float x, float y, float z)
{
    ModUInt32Value( PLAYER_FIELD_COINAGE , -(int32)money );

    SetPosition(x, y, z, true);
    SetTaxiState(false);
    SetTaxiPath(NULL);
    UnSetTaxiPos();
    m_taxi_ride_time = 0;

    SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID , 0);
    RemoveFlag( UNIT_FIELD_FLAGS, U_FIELD_FLAG_MOUNT_SIT );

    if (HasFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER ))   
        RemoveFlag( UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER );

    SetPlayerSpeed(RUN, m_runSpeed, 6, true);

    sEventMgr.RemoveEvents(this, EVENT_PLAYER_TAXI_INTERPOLATE);

    // Save to database on dismount
    SaveToDB(false);
}

void Player::_EventAttack(bool offhand)
{
    if (m_currentSpell)
    {
        setAttackTimer(500, offhand);
        return;
    }

    Unit *pVictim = 0;
    if(m_curSelection)
        pVictim = World::GetUnit(m_curSelection);
    
    //Can't find victim, stop attacking
    if (!pVictim)
    {
        Log::getSingleton( ).outDetail("Player::Update:  No valid current selection to attack, stopping attack\n");
        setRegenTimer(5000); //prevent clicking off creature for a quick heal
        EventAttackStop();
        return;
    }

    if (!canReachWithAttack(pVictim))
    {
        if(m_AttackMsgTimer != 1)
        {
            m_session->OutPacket(SMSG_ATTACKSWING_NOTINRANGE);
            m_AttackMsgTimer = 1;
        }
        setAttackTimer(300, offhand);
    }
    else if(!isInFront(pVictim))
    {
        // We still have to do this one.
        if(m_AttackMsgTimer != 2)
        {
            m_session->OutPacket(SMSG_ATTACKSWING_BADFACING);
            m_AttackMsgTimer = 2;
        }
        setAttackTimer(300, offhand);
    }
    else
    {
        m_AttackMsgTimer = 0;
        
        // Set to weapon time.
        setAttackTimer(0, offhand);

        //pvp timeout reset
        if(pVictim->IsPlayer())
        {
            if (static_cast<Player *>(pVictim)->cannibalize)
            {
                sEventMgr.RemoveEvents(pVictim, EVENT_CANNIBALIZE);
                pVictim->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                static_cast<Player *>(pVictim)->cannibalize = false;
            }
        }

        if (!GetOnMeleeSpell())
        {
            Strike(pVictim,(uint32)offhand,NULL,0,0,0);
        } 
        else 
        { 
            SpellEntry *spellInfo = sSpellStore.LookupEntry(GetOnMeleeSpell());
            SetOnMeleeSpell(0);
            Spell *spell = new Spell(this,spellInfo,true,NULL);
            SpellCastTargets targets;
            targets.m_unitTarget = GetSelection();
            spell->prepare(&targets);
        }
    }
}

void Player::_EventCharmAttack()
{
    if(!m_CurrentCharm)
        return;

    Unit *pVictim = NULL;
    if(m_curSelection == 0)
    {
        sEventMgr.RemoveEvents(this, EVENT_PLAYER_CHARM_ATTACK);
        return;
    }
    /*else if(GUID_HIPART(m_curSelection) != 0) //Creature
    {
    pVictim = sObjHolder.GetObject<Creature>(m_curSelection);
    }
    else //Player
    {
    pVictim = sObjHolder.GetObject<Player>(m_curSelection);
    }*/

    switch(UINT32_LOPART(GUID_HIPART(m_curSelection)))
    {
    case HIGHGUID_UNIT:
        pVictim = sObjHolder.GetObject<Creature>(m_curSelection);
        break;
    case HIGHGUID_PET:
        pVictim = sObjHolder.GetObject<Pet>(m_curSelection);
        break;
    case HIGHGUID_PLAYER:
        pVictim = sObjHolder.GetObject<Player>(m_curSelection);
        break;
    }

    //Can't find victim, stop attacking
    if (!pVictim)
    {
        Log::getSingleton( ).outError( "WORLD: "I64FMT" doesn't exist.",m_curSelection);
        Log::getSingleton( ).outDetail("Player::Update:  No valid current selection to attack, stopping attack\n");
        this->setRegenTimer(5000); //prevent clicking off creature for a quick heal
        clearStateFlag(UF_ATTACKING);
        EventAttackStop();
        //smsg_AttackStop(m_curSelection);
    }
    else
    {
        if (!m_CurrentCharm->canReachWithAttack(pVictim))
        {
            if(m_AttackMsgTimer == 0)
            {
                //m_session->OutPacket(SMSG_ATTACKSWING_NOTINRANGE);
                m_AttackMsgTimer = 2000;        // 2 sec till next msg.
            }
            // Shorten, so there isnt a delay when the client IS in the right position.
            sEventMgr.ModifyEventTimeLeft(this, EVENT_PLAYER_CHARM_ATTACK, 100);    
        }
        else if(!m_CurrentCharm->isInFront(pVictim))
        {
            if(m_AttackMsgTimer == 0)
            {
                m_session->OutPacket(SMSG_ATTACKSWING_BADFACING);
                m_AttackMsgTimer = 2000;        // 2 sec till next msg.
            }
            // Shorten, so there isnt a delay when the client IS in the right position.
            sEventMgr.ModifyEventTimeLeft(this, EVENT_PLAYER_CHARM_ATTACK, 100);    
        }
        else
        {
            //if(pVictim->GetTypeId() == TYPEID_UNIT)
            //    pVictim->GetAIInterface()->StopMovement(5000);

            //pvp timeout reset
            /*if(pVictim->IsPlayer())
            {
                if(((Player*)pVictim)->DuelingWith == NULL)//Dueling doesn't trigger PVP
                    ((Player*)pVictim)->PvPTimeoutUpdate(false); //update targets timer

                if(DuelingWith == NULL)//Dueling doesn't trigger PVP
                    PvPTimeoutUpdate(false); //update casters timer
            }*/

            if (!m_CurrentCharm->GetOnMeleeSpell())
            {
                m_CurrentCharm->Strike(pVictim,0,NULL,0,0,0);
            } 
            else 
            { 
                SpellEntry *spellInfo = sSpellStore.LookupEntry(m_CurrentCharm->GetOnMeleeSpell());
                m_CurrentCharm->SetOnMeleeSpell(0);
                Spell *spell = new Spell(m_CurrentCharm,spellInfo,true,NULL);
                SpellCastTargets targets;
                targets.m_unitTarget = GetSelection();
                spell->prepare(&targets);
                //delete spell;         // deleted automatically, no need to do this.
            }
        }
    }   
}

void Player::EventAttackStart()
{
    m_attacking = true;
}

void Player::EventAttackStop()
{
    //sEventMgr.RemoveEvents(this, EVENT_PLAYER_REPEAT_SHOT);

    if(m_CurrentCharm != NULL)
        sEventMgr.RemoveEvents(this, EVENT_PLAYER_CHARM_ATTACK);

    setAttackTarget(NULL);  // nice and simple!
    m_attacking = false;
}

void Player::_EventExploration()
{
    if (isDead())
        return;

    if (!IsInWorld())
        return;

    if (!m_mapMgr)
        return;

    if(GetMapMgr()->GetCellByCoords(GetPositionX(),GetPositionY()) == NULL) 
        return;

    uint16 AreaId=GetMapMgr()->GetAreaID(GetPositionX(),GetPositionY());
    
    if(!AreaId || AreaId == 0xFFFF)
        return;
    AreaTable * at = sAreaStore.LookupEntry(AreaId);
    if(at == 0)
        return;

    char areaname[200];
    if(at)
    {
        strcpy(areaname, sAreaStore.LookupString((uint32)at->name));
    }
    else
    {
        strcpy(areaname, "UNKNOWN");
    }

    /*sChatHandler.BlueSystemMessage(m_session, "Current zone: %s%u %s[%s]\n%sPlayer z: %s%f, %sadt z: %s%f", MSG_COLOR_GREEN, AreaId, MSG_COLOR_YELLOW, areaname,
        MSG_COLOR_GREEN, MSG_COLOR_YELLOW, GetPositionZ(), MSG_COLOR_GREEN, MSG_COLOR_YELLOW, m_mapMgr->GetLandHeight(GetPositionX(), GetPositionY()));
    
    printf("Player is at %f %f with areaid %u and areaname %s\n", GetPositionX(), GetPositionY(), AreaId, areaname);
    printf("Player z: %f, adt z: %f\n", GetPositionZ(), m_mapMgr->GetLandHeight(GetPositionX(), GetPositionY()));*/

    int offset = at->explorationFlag / 32;
    offset += PLAYER_EXPLORED_ZONES_1;

    uint32 val = (uint32)(1 << (at->explorationFlag % 32));
    uint32 currFields = GetBaseUInt32Value(offset);

    if(AreaId != m_AreaID)
    {
        m_AreaID = AreaId;
        UpdatePvPArea();
    }

    if(at->ZoneId != 0 && m_zoneId != at->ZoneId)
        ZoneUpdate(at->ZoneId);

    // Check for a restable area
    if(at->AreaFlags == 312)
    {
        // check faction
        if((at->factiongroup == 2 && GetTeam() == 0) ||
            (at->factiongroup == 4 && GetTeam() == 1) )
        {
            if(!m_isResting) ApplyPlayerRestState(true);
        }
        else
        {
            if(m_isResting) ApplyPlayerRestState(false);
        }
    }
    else
    {
        if(m_isResting) ApplyPlayerRestState(false);
    }

    if( !(currFields & val) && !GetTaxiState() && !m_TransporterGUID)//Unexplored Area        // bur: we dont want to explore new areas when on taxi
    {
        SetBaseUInt32Value(offset, (uint32)(currFields | val));
        SetUInt32Value(offset, (uint32)(currFields | val));

        uint32 explore_xp = at->level * 10;
        WorldPacket data(SMSG_EXPLORATION_EXPERIENCE, 8);
        data << at->AreaId << explore_xp;
        m_session->SendPacket(&data);

        if(getLevel() < sWorld.LevelCap && explore_xp)
            GiveXP(explore_xp, 0, false);
    }
}

void Player::EventDeath()
{
    if (m_state & UF_ATTACKING)
        EventAttackStop();

    if (m_onTaxi)
        sEventMgr.RemoveEvents(this, EVENT_PLAYER_TAXI_DISMOUNT);

    //Todo: respawn
}

void Player::BuildEnumData( WorldPacket * p_data )
{
    *p_data << GetGUID();
    *p_data << m_name;

    uint32 bytes = GetUInt32Value(UNIT_FIELD_BYTES_0);
    *p_data << uint8(bytes & 0xff); // race
    *p_data << uint8((bytes >> 8) & 0xff); // class
    *p_data << uint8((bytes >> 16) & 0xff); // gender

    bytes = GetUInt32Value(PLAYER_BYTES);
    *p_data << uint8(bytes & 0xff); //skin
    *p_data << uint8((bytes >> 8) & 0xff); //face
    *p_data << uint8((bytes >> 16) & 0xff); //hairstyle
    *p_data << uint8((bytes >> 24) & 0xff); //haircolor

    bytes = GetUInt32Value(PLAYER_BYTES_2);
    *p_data << uint8(bytes & 0xff); //facialhair

    *p_data << uint8(GetUInt32Value(UNIT_FIELD_LEVEL)); //level

    *p_data << m_zoneId;
    *p_data << m_mapId;
    /**p_data << m_positionX;
    *p_data << m_positionY;
    *p_data << m_positionZ;*/
    *p_data << m_position;

    *p_data << GetUInt32Value(PLAYER_GUILDID);// guild

    if(m_banned)        *p_data << (uint32)7;    // Banned (cannot login)
    else if(isDead())   *p_data << (uint32)8704; // Dead (displaying as Ghost)
    else                *p_data << (uint32)1;    // Alive

    *p_data << (uint8)m_restState;      // rest state

    // pet stuff
    char query[200];
    sprintf(query, "SELECT entryid FROM playerpets WHERE ownerguid=%u AND active=1", GetGUIDLow());
    QueryResult *result = sDatabase.Query(query);
    
    CreatureInfo *info = NULL;
    if(result)
    {
        uint32 entryid = result->Fetch()[0].GetUInt32();
        info = objmgr.GetCreatureName(entryid);
        delete result;
    }

    if(info)        // uint32 displayid, uint32 level, uint32 familyid
        *p_data << uint32(info->DisplayID) << uint32(getLevel()) << uint32(info->Family);
    else
        *p_data << uint32(0) << uint32(0) << uint32(0);
    //*p_data << (uint32)0;  // pet display id
    //*p_data << (uint32)0;      // pet level
    //*p_data << (uint32)0;   // pet family id            // BURLEXFIXME: here


    for (int8 i = 0; i < 20; i++)
    {
        if (GetItemInterface()->GetInventoryItem(i) != NULL)
        {
            *p_data << (uint32)GetItemInterface()->GetInventoryItem(i)->GetProto()->DisplayInfoID;
            *p_data << (uint8)GetItemInterface()->GetInventoryItem(i)->GetProto()->InventoryType;
        }
        else
        {
            *p_data << (uint32)0;
            *p_data << (uint8)0;
        }
    }
}

///  This function sends the message displaying the purple XP gain for the char
///  It assumes you will send out an UpdateObject packet at a later time.
void Player::GiveXP(uint32 xp, const uint64 &guid, bool allowbonus)
{
    WorldPacket data(SMSG_LOG_XPGAIN, 21);
    uint32 restxp = xp;
    if ( xp < 1 )
        return;
    if(!GetSession()->CanUseCommand('p'))
        if(getLevel() >= PLAYER_LEVEL_CAP)
            return;

    if(m_restState == RESTSTATE_RESTED && allowbonus)
    {
        restxp = SubtractRestXP(xp);
        xp += restxp;
    }
    UpdateRestState();
        
    if (guid == 0)
    {
        data << uint64(0);
        data << uint32(xp);
        data << uint8(0);
        data << uint32(restxp);
        data << uint16(0) << uint8(0x8) << uint8(0x3F);
    }
    else
    {
        data << guid;
        data << uint32(xp);                                     // Normal XP
        data << uint8(0);                                       // Unknown.. seems to always be 0        
        data << uint32(restxp);                                 // "Rest XP", is equal to XP for no rest xp message
        data << uint16(0x00) << uint8(0x80) << uint8(0x3F);     // Static data.. seems to always be 3F80
    }
    GetSession()->SendPacket(&data);

    uint32 curXP = GetUInt32Value(PLAYER_XP);
    uint32 nextLvlXP = GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
    uint32 newXP = curXP + xp;
    uint32 level = GetUInt32Value(UNIT_FIELD_LEVEL);
    bool levelup = false;

    if(m_Summon != NULL && m_Summon->GetUInt32Value(UNIT_CREATED_BY_SPELL) == 0)
        m_Summon->GiveXP(xp);

    uint32 TotalHealthGain = 0, TotalManaGain = 0;
    uint32 cl=getClass();
    // Check for level-up
    while (newXP >= nextLvlXP)
    {
        levelup = true;
        // Level-Up!
        newXP -= nextLvlXP;  // reset XP to 0, but add extra from this xp add

        level ++;    // increment the level

        //bur: this was spamming on .levelup, lets kill it for now.

        //char irctext[200];
        //sprintf(irctext, "AUTOMATED MESSAGE: %s reached level %d.", GetName(), level);
        //sWorld.SendIRCMessage(irctext);

        //nextlevel XP Formulas
        if( level > 0 && level <= 30 )
        {
            nextLvlXP = ((int)((((double)(8 * level * ((level * 5) + 45)))/100)+0.5))*100;
        }
        else if( level == 31 )
        {
            nextLvlXP = ((int)((((double)(((8 * level) + 3) * ((level * 5) + 45)))/100)+0.5))*100;
        }
        else if( level == 32 )
        {
            nextLvlXP = ((int)((((double)(((8 * level) + 6) * ((level * 5) + 45)))/100)+0.5))*100;
        }
        else
        {
            nextLvlXP = ((int)((((double)(((8 * level) + ((level - 30) * 5)) * ((level * 5) + 45)))/100)+0.5))*100;
        }

        
        if( level > 9)
        {
            //Give Talent Point
            uint32 curTalentPoints = GetUInt32Value(PLAYER_CHARACTER_POINTS1);
            SetUInt32Value(PLAYER_CHARACTER_POINTS1,curTalentPoints+1);
            SetBaseUInt32Value(PLAYER_CHARACTER_POINTS1, curTalentPoints+1); //saving
        }

		switch(cl)//added by Phantomas, this info i have from official server, no interpolation or anything this is exact data!
		{
			case WARRIOR:
				if(level<13)TotalHealthGain+=19;
				else if(level <36) TotalHealthGain+=level+6;
				else TotalHealthGain+=2*level-30;
			break;
			case HUNTER:
				if(level<13)TotalHealthGain+=17;
				else TotalHealthGain+=level+4;

				if(level<11)TotalManaGain+=29;
				else if(level<27)TotalManaGain+=level+18;
				else TotalManaGain+=45;
			break;
			case ROGUE:
				if(level <15)TotalHealthGain+=17;
				else TotalHealthGain+=level+2;
			break;
			case DRUID:
				if(level < 17)TotalHealthGain+=17;
				else TotalHealthGain+=level;

				if(level < 26)TotalManaGain+=level+20;
				else TotalManaGain+=45;
			break;
			case MAGE:
				if(level < 23)TotalHealthGain+=15;
				else TotalHealthGain+=level-8;

				if(level <28)TotalManaGain+=level+23;
				else TotalManaGain+=51;
			break;
			case SHAMAN:
				if(level <16)TotalHealthGain+=17;
				else TotalHealthGain+=level+1;

				if(level<22)TotalManaGain+=level+19;
				else TotalManaGain+=49;
			break;
			case WARLOCK:
				if(level <17)TotalHealthGain+=17;
				else TotalHealthGain+=level-2;

				if(level< 30)TotalManaGain+=level+21;
				else TotalManaGain+=51;
			break;
			case PALADIN:
				if(level < 14)TotalHealthGain+=18;
				else TotalHealthGain+=level+4;

				if(level<30)TotalManaGain+=level+17;
				else TotalManaGain+=42;
			break;
			case PRIEST:
				if(level <21)TotalHealthGain+=15;
				else TotalHealthGain+=level-6;

				if(level <22)TotalManaGain+=level+22;
				else if(level <32)TotalManaGain+=level+37;
				else TotalManaGain+=54;
			break;
		}
    }

    if(levelup)
    {
		m_playedtime[0] = 0; //Reset the "Current level played time"        
	
        SetUInt32Value(PLAYER_NEXT_LEVEL_XP, nextLvlXP);
        SetUInt32Value(UNIT_FIELD_LEVEL, level);

		SetBaseUInt32Value(PLAYER_NEXT_LEVEL_XP, nextLvlXP);
        SetBaseUInt32Value(UNIT_FIELD_LEVEL, level);

		//SetUInt32Value(UNIT_FIELD_MAXHEALTH, newHealth);
        //SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
//        SetBaseUInt32Value(UNIT_FIELD_MAXHEALTH, basenewHealth);
  //      SetBaseUInt32Value(UNIT_FIELD_HEALTH, basenewHealth);

		//SetUInt32Value(UNIT_FIELD_POWER1, newMana);
        //SetUInt32Value(UNIT_FIELD_MAXPOWER1, newMana);
		uint32 bh=GetUInt32Value(UNIT_FIELD_BASE_HEALTH)+TotalHealthGain;

		SetBaseUInt32Value(UNIT_FIELD_BASE_HEALTH,bh);
		SetUInt32Value(UNIT_FIELD_BASE_HEALTH,bh);		
		
		//SetBaseUInt32Value(UNIT_FIELD_POWER1, basenewMana);
        //SetBaseUInt32Value(UNIT_FIELD_MAXPOWER1, basenewMana);
      
        data.Initialize(SMSG_LEVELUP_INFO);
        data << uint32(level);
        data << uint32(TotalHealthGain); // health gain
        //mana gain only for those that have it
        if( (cl == WARRIOR) || (cl == ROGUE))//no mana gain
			data << uint32(0); 
        else
		{	
			uint32 bm=GetUInt32Value(UNIT_FIELD_BASE_MANA)+TotalManaGain;
			SetBaseUInt32Value(UNIT_FIELD_BASE_MANA,bm);
			SetUInt32Value(UNIT_FIELD_BASE_MANA,bm);

			data << uint32(TotalManaGain); // mana gain
		}
        data << uint32(0); 
        data << uint32(0);
        data << uint32(0);
        // 6 new fields
        data << uint32(0);
        for(uint32 x=0;x<5;x++)
        {
            uint32 val=GainStat(level, getClass(), x);
            data << val;
            BaseStats[x]+=val;
			SetBaseUInt32Value(UNIT_FIELD_STAT0+x,BaseStats[x]);
            CalcStat(x);
        }     
        GetSession()->SendPacket(&data);
    
		UpdateMaxSkills();
		UpdateStats();
		UpdateChances();	
		
		//set full hp and mana
		SetUInt32Value(UNIT_FIELD_HEALTH,GetUInt32Value(UNIT_FIELD_MAXHEALTH));
		if(cl!=WARRIOR && cl!=ROGUE)
		SetUInt32Value(UNIT_FIELD_POWER1,GetUInt32Value(UNIT_FIELD_MAXPOWER1));

        sSocialMgr.SendUpdateToFriends( this );
		RecalcAllRatings();
	}
    // Set the update bit
    SetUInt32Value(PLAYER_XP, newXP);
    SetBaseUInt32Value(PLAYER_XP, newXP);
    //CalcBaseStats();	
}

void Player::smsg_InitialSpells()
{
    uint16 spellCount = mSpells.size();
	uint16 itemCount = m_itemcooldown.size();

	WorldPacket data(SMSG_INITIAL_SPELLS, 5 + (spellCount * 4) + (itemCount * 4) );
    data << uint8(0);
    data << uint16(spellCount); // spell count

    SpellSet::iterator itr;
    for (itr = mSpells.begin(); itr != mSpells.end(); ++itr)
    {
        data << uint16(*itr);       // spell id
        data << uint16(0xeeee);     // this is not slot,values is always eeee or 0,seems to be cooldown
    }

    // burlex: this is broken like hell for 2.0... fix!
    
    /*if (itemCount)
    {
       data << uint16(itemCount); // item / spell count
       ItemCooldownSet::iterator itr;
       for (itr = m_itemcooldown.begin(); itr != m_itemcooldown.end(); ++itr)
       {
            ItemCooldown * testje = (*itr);                     // for debug

            data << uint16((*itr)->SpellID);                    // spell id
            data << uint16((*itr)->ItemEntry);                  // item entry
            data << uint16((*itr)->SpellCategory);              // spell Category
            data << uint32(0);                                  // kinda unknown maybe constant 0
            data << uint32((*itr)->CooldownTimeStamp - now());  // Current time time remaining of the cooldown in ms
        }
    }
    else
    {*/
        data << uint16(0);
    /*}*/

    GetSession()->SendPacket(&data);

    Log::getSingleton( ).outDetail( "CHARACTER: Sent Initial Spells" );
}

void Player::smsg_InitialFactions()
{
    WorldPacket data(SMSG_INITIALIZE_FACTIONS, 324);
    data << uint32(64); // 64 factions
    for(uint32 i=0;i<64;i++)
    {
        std::map<uint32, Reputation>::iterator itr = m_reputation.find(i);
        if(itr == m_reputation.end())
        {
            // huh?
            data << uint8(0) << uint32(0);
        } else {
            data << uint8(itr->second.flag) << uint32(itr->second.standing);
        }
    }

    GetSession()->SendPacket(&data);
}

void Player::modReputation(uint8 id, int32 mod, bool updateclient)
{
    std::map<uint32, Reputation>::iterator itr = m_reputation.find(id);
    if(itr == m_reputation.end())
    {
        sLog.outDebug("REPUTATION: modReputation(%d, %d) : Invalid faction not found in player map! %d", id, mod, id);
        return;
    }
    // apply pct modifier
    int32 mod_real = mod;
    if(pctReputationMod)
    {
        mod_real += (mod * (float)(pctReputationMod / 100));
    }
    m_reputation[id].standing += mod_real;
    if(itr->second.flag == 0 && updateclient)// If this Faction is not Visible yet, make it visible
    {
        WorldPacket data(SMSG_SET_FACTION_VISIBLE, 4);
        data << itr->first;
        GetSession()->SendPacket(&data);
        itr->second.flag = 1;
    }

    // Send Reputation Packet
    if(updateclient)
    {
        WorldPacket data(SMSG_SET_FACTION_STANDING, 12);
        data << uint32(m_reputation[id].flag);  // should be 1 I guess..
        data << uint32(itr->first);
        data << uint32(itr->second.standing);
        GetSession()->SendPacket(&data);
    }

    if(updateclient)    // auras we won't save.
        m_changedreputation.insert( id );
}

void Player::modPercAllReputation(int32 mod, bool updateclient)
{
    std::map<uint32, Reputation>::iterator itr;
    for(itr = m_reputation.begin();itr != m_reputation.end(); itr++)
    {
		itr->second.standing += itr->second.standing*(mod+pctReputationMod)/100;
	    if(itr->second.flag == 0 && updateclient)// If this Faction is not Visible yet, make it visible
	    {
            WorldPacket data(SMSG_SET_FACTION_VISIBLE, 4);
	        data << itr->first;
	        GetSession()->SendPacket(&data);
	        itr->second.flag = 1;
	    }
	
	    // Send Reputation Packet
	    if(updateclient)
        {
            WorldPacket data(SMSG_SET_FACTION_STANDING, 12);
	        data << uint32(itr->second.flag);  // should be 1 I guess..
	        data << uint32(itr->first);
	        data << uint32(itr->second.standing);
	        GetSession()->SendPacket(&data);
        }
        if(updateclient)
            m_changedreputation.insert( itr->first );
    }
}

void Player::SetFactionState(uint8 id, uint8 state)
{
    if(m_reputation.find(id) == m_reputation.end())
    {
        sLog.outError("REPUTATION: SetFactionState(%d, %d) : Invalid faction not found in player map!", id, state);
        return;
    }

    m_reputation[id].flag = state;
    m_changedreputation.insert( id );
}

uint8 Player::GetStandingById(uint8 id)
{
    std::map<uint32, Reputation>::iterator itr=m_reputation.find(id);
    if(itr == m_reputation.end())
    {
        sLog.outError("REPUTATION: GetStandingById(%d) : Invalid faction not found in player map!", id);
        return STANDING_NEUTRAL;
    }
 /*   uint8 state;
    uint32 standing;*/
    uint32 NumRep[8] = {36000, 3000, 3000, 3000, 6000, 12000, 21000, 1000}; // Reputation for each Reputation Level in Numbers


    // Get Faction Template
    /*Faction* factTemp = objmgr.GetFaction(getRace(), id);
    if(!factTemp){
        Log::getSingleton( ).outString( "Missing Faction Template for Race: %u, Id: %u -> return STANDING_NEUTRAL", getRace(), id );
        return STANDING_NEUTRAL;
    }*/

    // Get Reputation
    /*if(itr->second.flag == 2)// At War
        return STANDING_HATED;

    state = factTemp->state;
    standing = itr->second.standing;

    // Calculate Standing based on FactionTemplate and Player's Reputation
    if(standing + factTemp->standing < NumRep[factTemp->state])
        return state;

    standing += factTemp->standing;
    while(standing >= NumRep[state])
    {
        standing -= NumRep[state];
        state++;
    }
    return state;*/
    return STANDING_NEUTRAL;
}

uint8 Player::GetStandingByFactionTemplate(uint32 fctTmp)
{
    FactionTemplateDBC *factionInfo = sFactionTmpStore.LookupEntry( fctTmp );

    if(!factionInfo)
    {
        Log::getSingleton( ).outError("WORLD: unknown factionTemplate id %i\n", fctTmp);
        return STANDING_NEUTRAL;
    }

    return GetStandingById(factionInfo->FactionId);
}
uint32 Player::GetFactionId()
{
    switch(getRace())
    {
        case RACE_ORC:
        case RACE_UNDEAD:
        case RACE_TAUREN:
        case RACE_TROLL:
            {
                return 67;
            }break;
        case RACE_HUMAN:
        case RACE_DWARF:
        case RACE_NIGHTELF:
        case RACE_GNOME:
            {
                return 469;
            }break;
        default:
            {
                sLog.outError("Couldn't determine faction? wtfhax?");
                return 0;
            }break;
    }
    return 0;
}

void Player::_InitialReputation()
{
    FactionDBC *f = NULL;
    uint32 FactionVal;
    uint32 OpposingFactionVal;
    uint32 Force;
    uint32 ForceOpposing;

    switch(getRace())
    {
        case RACE_ORC:
        case RACE_UNDEAD:
        case RACE_TAUREN:
        case RACE_TROLL:
        case RACE_BLOODELF:
            {
                FactionVal = 67;
                OpposingFactionVal = 469;
                Force = 892;
                ForceOpposing = 891;
            }break;
        case RACE_HUMAN:
        case RACE_DWARF:
        case RACE_NIGHTELF:
        case RACE_GNOME:
        case RACE_DRAENEI:
            {
                FactionVal = 469;
                OpposingFactionVal = 67;
                Force = 891;
                ForceOpposing = 892;               
            }break;
        default:
            {
                sLog.outError("Couldn't determine FactionVal? wtfhax?");
                return;
            }break;
    }


    for(int i=0;i<1006;i++)  // max FactionVal is 921
    {
        f = sFactionStore.LookupEntry(i);
        if(f == NULL) continue; // non-existant
        if(f->unk1 >= 0)
        {
            Reputation rep;
            rep.standing = 0;
            rep.flag = 0;
            if(f->factionGroupReputationFrame != 0 && f->factionGroupReputationFrame != 169)
            {
                if( f->factionGroupReputationFrame == FactionVal || f->factionGroupReputationFrame == Force)
                {
                    rep.flag = f->unk13;
                } else if ( f->factionGroupReputationFrame == OpposingFactionVal || f->factionGroupReputationFrame == ForceOpposing)
                {
                    rep.flag = f->unk14;
                }
            } else {
                if(f->unk13&&!f->unk14)
                {
                    rep.flag = f->unk13;
                }
                else if(f->unk13&&!f->unk14)
                {
                    rep.flag = f->unk14;
                }
                else if(f->unk13 && f->unk14)
                {
                    if(f->Id == 469)
                    {
                        if(FactionVal == 469) rep.flag = f->unk13;
                        else if(FactionVal == 67) rep.flag = f->unk14;
                    }
                    else if(f->Id == 67)
                    {
                        if(FactionVal == 469) rep.flag = f->unk14;
                        else if(FactionVal == 67) rep.flag = f->unk13;
                    }
                    else
                    {    
                        if(FactionVal == 469) rep.flag = f->unk13;
                        else rep.flag = f->unk14;
                    }
                }
            }
            /*Faction* fct = objmgr.GetFaction(getRace(), f->unk1);
            if(fct != NULL)
                if(fct->standing)
                    rep.standing = fct->standing;*/
            
            //m_reputation[f->unk1] = rep;
            m_reputation.insert( std::map<uint32, Reputation>::value_type( f->unk1, rep ) );
            m_addreputation.insert( f->unk1 );
        }
    }
}

bidentry* Player::GetBid(uint32 id)
{
    std::list<bidentry*>::iterator itr;
    for (itr = m_bids.begin(); itr != m_bids.end();)
    {
        if ((*itr)->AuctionID == id)
        {
            return (*itr);
        }
        else
        {
            ++itr;
        }
    }
    return NULL;
}

void Player::AddBid(bidentry *be)
{
    std::list<bidentry*>::iterator itr;
    for (itr = m_bids.begin(); itr != m_bids.end();)
    {
        if ((*itr)->AuctionID == be->AuctionID)
        {
            m_bids.erase(itr++);
        }
        else
        {
            ++itr;
        }
    }
    m_bids.push_back(be);
}


void Player::_SaveAuctions()
{
    /*std::stringstream delinvq, del;
    delinvq << "DELETE FROM auctionhouse WHERE itemowner = " << GetGUIDLow(); // TODO: use full guids                
    sDatabase.Execute( delinvq.str().c_str( ) );
    ObjectMgr::AuctionEntryMap::iterator itr;
    for (itr = objmgr.GetAuctionsBegin();itr != objmgr.GetAuctionsEnd();itr++)
    {
        AuctionEntry *Aentry = itr->second;
        if ((Aentry) && (Aentry->owner == GetGUIDLow()))
        {
            Item *it = objmgr.GetAItem(Aentry->item);
			if(!it)continue;
			it->SetOwner(NULL);
			it->SaveToDB(0);

            std::stringstream invq;
            invq <<  "INSERT INTO auctionhouse (auctioneerguid, itemguid, itemowner,buyoutprice,time,buyguid,lastbid,Id) VALUES ( " <<
                Aentry->auctioneer << ", " << Aentry->item << ", " << Aentry->owner << ", " << Aentry->buyout << ", " << Aentry->time << ", " << Aentry->bidder << ", " << Aentry->bid << ", " << Aentry->Id << " )";
            sDatabase.Execute( invq.str().c_str( ) );
            
			
        }
    }*/
}


void Player::_SaveBids()
{
    std::stringstream delinvq;
    delinvq << "DELETE FROM bids WHERE bidder = " << GetGUID(); // TODO: use full guids
    sDatabase.Execute( delinvq.str().c_str( ) );
    std::list<bidentry*>::iterator itr;
    for (itr = m_bids.begin(); itr != m_bids.end(); itr++)
    {
        AuctionEntry *a = sAuctionMgr.GetAuctionHouse((*itr)->AHid)->GetAuction((*itr)->AuctionID);
        if (a)
        {
            std::stringstream invq;
            invq <<  "INSERT INTO bids (bidder, Id, auctionhouse, amt) VALUES ( " <<
                GetGUID() << ", " << (*itr)->AuctionID << ", " << (*itr)->AHid << ", " << (*itr)->amt << " )";

            sDatabase.Execute( invq.str().c_str( ) );
        }
    }

}

/* Saves ItemCooldowns
   checks for invalid items and deletes them from the list and don't save them */
void Player::_SaveItemCooldown()
{
    std::stringstream delinvq;
    delinvq << "DELETE FROM playercooldownitems WHERE OwnerGuid = " << GetGUID(); // TODO: use full guids
    sDatabase.Execute( delinvq.str().c_str( ) );

    ItemCooldownSet::iterator itr, it2;
    for (itr = m_itemcooldown.begin(); itr != m_itemcooldown.end();)
    {
        if (now() > (*itr)->CooldownTimeStamp)      // if item is invalid
        {
            ItemCooldown * temp = (*itr);           // get a temp pointer
	        it2 = itr;
	        ++itr;
            m_itemcooldown.erase(it2);              // remove the object of the list
            delete temp;                            // delete its mem, using the temp pointer
            continue;
        }

        std::stringstream invq;
        invq <<  "INSERT INTO playercooldownitems (OwnerGuid, ItemEntry, SpellID, SpellCategory, CooldownTimeStamp, Cooldown) VALUES ( " <<
            GetGUID() << ", " << (*itr)->ItemEntry << ", " << (*itr)->SpellID << ", " << (*itr)->SpellCategory << ", " << (*itr)->CooldownTimeStamp << ", " << (*itr)->Cooldown << " )";
        sDatabase.Execute( invq.str().c_str( ) );
        ++itr;
    }
}


void Player::_SavePet()
{
    // Remove any existing info
    sDatabase.Execute("DELETE FROM playerpets WHERE ownerguid=%u", GetGUIDLow());

    if(m_Summon)    // update PlayerPets array with current pet's info
    {
        m_Summon->UpdatePetInfo(false);

        if(!m_Summon->Summon)       // is a pet
        {
            // save pet spellz
            PetSpellMap::iterator itr = m_Summon->mSpells.begin();
            uint32 pn = m_Summon->m_PetNumber;
            sDatabase.Execute("DELETE FROM playerpetspells WHERE petnumber=%u", pn);

            for(; itr != m_Summon->mSpells.end(); ++itr)
            {
                sDatabase.Execute("INSERT INTO playerpetspells VALUES(%u, %u, %u, %u)", GetGUIDLow(), pn, itr->first->Id, itr->second);
            }
        }
    }


    std::stringstream ss;

    for(std::map<uint32, PlayerPet*>::iterator itr = m_Pets.begin(); itr != m_Pets.end(); itr++)
    {
        ss.rdbuf()->str("");
        ss << "INSERT INTO playerpets VALUES('"
            << GetGUIDLow() << "','"
            << itr->second->number << "','"
            << itr->second->name << "','"
            << itr->second->entry << "','"
            << itr->second->fields << "','"
            << itr->second->xp << "','"
            << itr->second->active << "','"
            << itr->second->level << "','"
            << itr->second->loyalty << "','"
            << itr->second->actionbar << "','"
            << itr->second->loyaltyupdate << "','"
            << itr->second->summon << "','"
            << itr->second->autocastspell << "')";
            
        sDatabase.Execute(ss.str().c_str());
    }
}

void Player::_SavePetSpells()
{    
    // Remove any existing
    sDatabase.Execute("DELETE FROM playersummonspells WHERE ownerguid=%u", GetGUIDLow());

    // Save summon spells
    map<uint32, set<uint32> >::iterator itr = SummonSpells.begin();
    for(; itr != SummonSpells.end(); ++itr)
    {
        set<uint32>::iterator it = itr->second.begin();
        for(; it != itr->second.end(); ++it)
        {
            sDatabase.Execute("INSERT INTO playersummonspells VALUES(%u, %u, %u)", GetGUIDLow(), itr->first, (*it));
        }
    }
}

void Player::AddSummonSpell(uint32 Entry, uint32 SpellID)
{
    SummonSpells[Entry].insert(SpellID);
}

void Player::RemoveSummonSpell(uint32 Entry, uint32 SpellID)
{
    map<uint32, set<uint32> >::iterator itr = SummonSpells.find(Entry);
    if(itr != SummonSpells.end())
    {
        itr->second.erase(SpellID);
        if(itr->second.size() == 0)
            SummonSpells.erase(itr);
    }
}

set<uint32>* Player::GetSummonSpells(uint32 Entry)
{
    map<uint32, set<uint32> >::iterator itr = SummonSpells.find(Entry);
    if(itr != SummonSpells.end())
    {
        return &(itr->second);
    }
    return 0;
}


void Player::_LoadBids()
{
    // Clear spell list
    m_bids.clear();

    // Spells
    std::stringstream query;
    query << "SELECT * FROM bids WHERE bidder=" << GetGUID();
    QueryResult *result = sDatabase.Query( query.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();
            bidentry *be = new bidentry;
            be->AuctionID = fields[1].GetUInt32();
			be->AHid = fields[2].GetUInt32();
            be->amt = fields[3].GetUInt32();
            m_bids.push_back(be);
            
        }
        while( result->NextRow() );

        delete result;
    }
 }

/* Loads ItemCooldowns
checks for invalid items and deletes them from the db */
void Player::_LoadItemCooldown()
{
    // Clear spell list
    m_itemcooldown.clear();

    std::stringstream query;
    query << "SELECT * FROM playercooldownitems WHERE OwnerGuid=" << GetGUID();
    QueryResult *result = sDatabase.Query( query.str().c_str() );
    if(result)
    {
        // TODO is there a better way to do this?
        do
        {
            // if the current item does not have cooldown delete it from db
            Field *fields               = result->Fetch();
            uint32 TempSpellID          = fields[2].GetUInt32();
            uint32 TempSpellCategory    = fields[3].GetUInt32();
            uint32 TempTimestamp        = fields[4].GetUInt32();
            uint32 TempCooldown         = fields[5].GetUInt32();
            uint32 DiffTimestamp        = TempTimestamp - now();

            if (now() > TempTimestamp || (now() < TempTimestamp && DiffTimestamp > TempCooldown)) //if timestamp overflow or dif time is larget than 7 days
            {
                query.rdbuf()->str("");
                query << "DELETE FROM playercooldownitems WHERE OwnerGuid = " << GetGUID() << "AND ItemEntry = " << fields[1].GetUInt32();
                sDatabase.WaitExecute( query.str( ).c_str( ) );
            }
            else // only add items to list that still have cooldown
            {
                ItemCooldown *be = new ItemCooldown;
                be->ItemEntry           = fields[1].GetUInt32();
                be->SpellID             = TempSpellID;
                be->SpellCategory       = TempSpellCategory;
                be->CooldownTimeStamp   = TempTimestamp;
                be->Cooldown            = TempCooldown;

                m_itemcooldown.insert(be);
            }
        }
        while( result->NextRow() );

        delete result;
    }
}




void Player::_LoadPet()
{
    std::stringstream query;
    query << "SELECT * FROM playerpets where ownerguid='" << GetGUIDLow() << "' ORDER BY 'petnumber'";
    QueryResult *result = sDatabase.Query( query.str().c_str() );
    uint32 iMax = 0;
    uint32 iActivePet = 0;

    if(result)
    {
        Field *fields = result->Fetch();
        fields = result->Fetch();

        PlayerPet *pet = new PlayerPet;
        pet->number  = fields[1].GetUInt32();
        pet->name    = fields[2].GetString();
        pet->entry   = fields[3].GetUInt32();
        pet->fields  = fields[4].GetString();
        pet->xp      = fields[5].GetUInt32();
        pet->active  = fields[6].GetUInt32();
        pet->level   = fields[7].GetUInt32();
        pet->loyalty = fields[8].GetUInt32();
        pet->actionbar = fields[9].GetString();
        pet->loyaltyupdate = fields[10].GetUInt32();
        pet->summon = fields[11].GetUInt32();
        pet->autocastspell = fields[12].GetUInt32();

        m_Pets[pet->number] = pet;
        if(pet->active)
        {
            if(iActivePet)  // how the hell can this happen
                printf("pet warning - >1 active pet.. weird..");
            else
                iActivePet = pet->number;
        }       
        
        iMax++;
    }

    if(iActivePet)
        SpawnPet(iActivePet);       // only spawn if >0

    m_PetNumberMax = iMax;
}

void Player::SpawnPet(uint32 pet_number)
{
    std::map<uint32, PlayerPet*>::iterator itr = m_Pets.find(pet_number);
    if(itr == m_Pets.end())
    {
        sLog.outError("PET SYSTEM: "I64FMT" Tried to load invalid pet %d", GetGUID(), pet_number);
        return;
    }
    Pet *pPet = sObjHolder.Create<Pet>();
    pPet->SetInstanceID(GetInstanceID());
    pPet->LoadFromDB(this, itr->second);
}

void Player::_LoadPetSpells()
{
    std::stringstream query;
    std::map<uint32, std::list<uint32>* >::iterator itr;
    uint32 entry = 0;
    uint32 spell = 0;

    query << "SELECT * FROM playersummonspells where ownerguid='" << GetGUIDLow() << "' ORDER BY entryid";
    QueryResult *result = sDatabase.Query( query.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();
            entry = fields[1].GetUInt32();
            spell = fields[2].GetUInt32();
            AddSummonSpell(entry, spell);
        }
        while( result->NextRow() ); 
    }
}

void Player::addSpell(uint32 spell_id)
{
	SpellSet::iterator iter = mSpells.find(spell_id);
	if(iter != mSpells.end())
		return;
    	
	mSpells.insert(spell_id);
	m_session->OutPacket(SMSG_LEARNED_SPELL, 4, &spell_id);

	// Check if we're a deleted spell
	iter = mDeletedSpells.find(spell_id);
	if(iter != mDeletedSpells.end())
		mDeletedSpells.erase(iter);

    // Add the skill line for this spell if we don't already have it.
    skilllinespell * sk = objmgr.GetSpellSkill(spell_id);
    if(sk && !HasSkillLine(sk->skilline))
    {
        skilllineentry * skill = sSkillLineStore.LookupEntry(sk->skilline);
        if(!skill)
        {
            AddSkillLine(sk->skilline, 1, 1);
            return;
        }
        uint32 max = 5 * getLevel();

        switch(skill->type)
        {
        case SKILL_TYPE_WEAPON:
        case SKILL_TYPE_SECONDARY:
        case SKILL_TYPE_LANGUAGE:
        case SKILL_TYPE_PROFESSION:
            break;

        default:
            {
                if(skill->id != SKILL_POISONS)
                    max = 1;
            }break;
        }

        if(skill->type==SKILL_TYPE_PROFESSION)
            ModUInt32Value(PLAYER_CHARACTER_POINTS2,-1);
        
        AddSkillLine(sk->skilline, 1, max);
    }
}

//===================================================================================================================
//  Set Create Player Bits -- Sets bits required for creating a player in the updateMask.
//  Note:  Doesn't set Quest or Inventory bits
//  updateMask - the updatemask to hold the set bits
//===================================================================================================================
void Player::_SetCreateBits(UpdateMask *updateMask, Player *target) const
{
    if(target == this)
    {
        Object::_SetCreateBits(updateMask, target);
    }
    else
    {
        UpdateMask mask;
        mask.SetCount(m_valuesCount);
        _SetVisibleBits(&mask, target);

        for(uint16 index = 0; index < m_valuesCount; index++)
        {
            if(GetUInt32Value(index) != 0 && mask.GetBit(index))
                updateMask->SetBit(index);
        }
    }
}


void Player::_SetUpdateBits(UpdateMask *updateMask, Player *target) const
{
    if(target == this)
    {
        Object::_SetUpdateBits(updateMask, target);
    }
    else
    {
        UpdateMask mask;
        mask.SetCount(m_valuesCount);
        _SetVisibleBits(&mask, target);

        Object::_SetUpdateBits(updateMask, target);
        *updateMask &= mask;
    }
}


void Player::_SetVisibleBits(UpdateMask *updateMask, Player *target) const
{
    updateMask->SetBit(OBJECT_FIELD_GUID);
    updateMask->SetBit(OBJECT_FIELD_TYPE);
    updateMask->SetBit(OBJECT_FIELD_SCALE_X);

    updateMask->SetBit(UNIT_FIELD_SUMMON);
    updateMask->SetBit(UNIT_FIELD_SUMMON+1);

    updateMask->SetBit(UNIT_FIELD_TARGET);
    updateMask->SetBit(UNIT_FIELD_TARGET+1);

    updateMask->SetBit(UNIT_FIELD_HEALTH);
    updateMask->SetBit(UNIT_FIELD_POWER1);
    updateMask->SetBit(UNIT_FIELD_POWER2);
    updateMask->SetBit(UNIT_FIELD_POWER3);
    updateMask->SetBit(UNIT_FIELD_POWER4);
    updateMask->SetBit(UNIT_FIELD_POWER5);

    updateMask->SetBit(UNIT_FIELD_MAXHEALTH);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER1);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER2);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER3);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER4);
    updateMask->SetBit(UNIT_FIELD_MAXPOWER5);

    updateMask->SetBit(UNIT_FIELD_LEVEL);
    updateMask->SetBit(UNIT_FIELD_FACTIONTEMPLATE);
    updateMask->SetBit(UNIT_FIELD_BYTES_0);
    updateMask->SetBit(UNIT_FIELD_FLAGS);
    for(uint16 i = UNIT_FIELD_AURA; i < UNIT_FIELD_AURASTATE; i ++)
        updateMask->SetBit(i);
    updateMask->SetBit(UNIT_FIELD_BASEATTACKTIME);
    updateMask->SetBit(UNIT_FIELD_BASEATTACKTIME+1);
    updateMask->SetBit(UNIT_FIELD_BOUNDINGRADIUS);
    updateMask->SetBit(UNIT_FIELD_COMBATREACH);
    updateMask->SetBit(UNIT_FIELD_DISPLAYID);
    updateMask->SetBit(UNIT_FIELD_NATIVEDISPLAYID);
    updateMask->SetBit(UNIT_FIELD_MOUNTDISPLAYID);
    updateMask->SetBit(UNIT_FIELD_BYTES_1);
    updateMask->SetBit(UNIT_FIELD_MOUNTDISPLAYID);
    updateMask->SetBit(UNIT_FIELD_PETNUMBER);
    updateMask->SetBit(UNIT_FIELD_PET_NAME_TIMESTAMP);
    updateMask->SetBit(UNIT_FIELD_CHANNEL_OBJECT);
    updateMask->SetBit(UNIT_FIELD_CHANNEL_OBJECT+1);
    updateMask->SetBit(UNIT_CHANNEL_SPELL);
    updateMask->SetBit(UNIT_DYNAMIC_FLAGS);

    updateMask->SetBit(PLAYER_FLAGS);
    updateMask->SetBit(PLAYER_BYTES);
    updateMask->SetBit(PLAYER_BYTES_2);
    updateMask->SetBit(PLAYER_BYTES_3);
    //updateMask->SetBit(PLAYER_GUILD_TIMESTAMP);
    updateMask->SetBit(PLAYER_DUEL_TEAM);
    updateMask->SetBit(PLAYER_DUEL_ARBITER);
    updateMask->SetBit(PLAYER_DUEL_ARBITER+1);
    updateMask->SetBit(PLAYER_GUILDID);
    updateMask->SetBit(PLAYER_GUILDRANK);

    for(uint16 i = 0; i < EQUIPMENT_SLOT_END; i++)
    {
        updateMask->SetBit((uint16)(PLAYER_VISIBLE_ITEM_1_0 + (i*16))); // visual items for other players
        updateMask->SetBit((uint16)(PLAYER_VISIBLE_ITEM_1_0+1 + (i*16))); // visual items for other players
    }
}


void Player::DestroyForPlayer( Player *target ) const
{
    Unit::DestroyForPlayer( target );
}


void Player::SaveToDB(bool bNewCharacter)
{
	uint32 start_time = getMSTime();
    //1.9.2 wowd new save system based on Base Object field system.
    //This object can be updated at anytime for specific savings :P

    _SaveAuras(); //auras saving so they are applyed later at login

    //ToDo: Optimize Sql cpu usage before commiting

    //Calc played times
    uint32 playedt = (uint32)time(NULL) - m_playedtime[2];
    if(playedt)
    {
        m_playedtime[0] += playedt;
        m_playedtime[1] += playedt;
        m_playedtime[2] += playedt;
    }


    std::stringstream ss;
    
    SetBaseUInt32Value(PLAYER_FIELD_COINAGE,GetUInt32Value(PLAYER_FIELD_COINAGE));
    SetBaseUInt32Value(PLAYER_BYTES_2, GetUInt32Value(PLAYER_BYTES_2));
	SetBaseUInt32Value(PLAYER_FLAGS, GetUInt32Value(PLAYER_FLAGS));
    SetBaseUInt32Value(PLAYER_CHARACTER_POINTS2,GetUInt32Value(PLAYER_CHARACTER_POINTS2));

    // honor fields -> save to base
/*    for(int i = PLAYER_FIELD_SESSION_KILLS; i < PLAYER_END; ++i)
        SetBaseUInt32Value(i, GetUInt32Value(i));*/

	//health update, can be 0 now.
	if(GetUInt32Value(UNIT_FIELD_HEALTH) <= 0)
        SetBaseUInt32Value(UNIT_FIELD_HEALTH, 0);
    else
        SetBaseUInt32Value(UNIT_FIELD_HEALTH,GetUInt32Value(UNIT_FIELD_HEALTH));

	SetBaseUInt32Value(UNIT_FIELD_POWER1,GetUInt32Value(UNIT_FIELD_POWER1));

    /*if(!bNewCharacter)
        sDatabase.Execute("DELETE FROM characters WHERE guid=%u", GetGUIDLow());*/

    ss << "REPLACE INTO characters VALUES ("
		//(guid,acct,name,mapId,zoneId,positionX,positionY,positionZ,orientation,data,taximask,banned,bindpositionX,bindpositionY,bindpositionZ,bindmapId,bindzoneId,isResting,restState,restTime,online,playedtime,deathstate,TalentTree1,TalentPoints1,TalentTree2,TalentPoints2,TalentTree3,TalentPoints3,TalentResetTimes,first_login,honor_rank_points,honor_contribution_points) 
		
    << GetGUIDLow() << ", " // TODO: use full guids
    << GetSession()->GetAccountId() 
	<< ",'";
	uint32 i;
	for( i = 0; i < m_valuesCount; i++ )
    ss << GetBaseUInt32Value(i) << " ";
		
	ss << "','"
	<< m_name << "', "
    << m_position.x << ", "
    << m_position.y << ", "
    << m_position.z << ", "
    << m_mapId << ", "
    << m_zoneId << ", "
    << m_position.o << ", '";
		
	for( i = 0; i < 8; i++ )
    ss << m_taximask[i] << " ";
	ss << "', "
    
	<< m_banned << ", '"
    << m_banreason << "', "
    << (uint32)time(NULL) << ","
	<< (uint32)(IsInWorld()) << ","
		
	<< m_bind_pos_x << ", "
    << m_bind_pos_y << ", "
    << m_bind_pos_z << ", "
    << m_bind_mapid << ","
    << m_bind_zoneid << ", "
        
	<< uint32(m_isResting) << ", "
    << uint32(m_restState) << ", "
    << uint32(m_restAmount) << ", '"
      
    << uint32(m_playedtime[0]) << " "
    << uint32(m_playedtime[1]) << " "
    << uint32(playedt) << " ', "
    << uint32(m_deathState) << ", ";
        
    ss  << m_talentresettimes << ", "
       << m_FirstLogin;
	
	Guild *pGuild;
	PlayerInfo *   pMember;

	if(GetGuildId() && (pGuild = objmgr.GetGuild(GetGuildId())) && (pMember = pGuild->GetGuildMember( GetGUID() )))
	{
	
		ss << ",'" << pMember->publicNote << "','";
		ss << pMember->officerNote << "'," << GetGuildId() << "," << GetGuildRank();

   	}else
	{
		ss << ",'','',0,0";
	}

    ss << "," << (uint32)m_StableSlotCount << ",";
    // instances
    ss << m_instanceId << ", ";
    ss << m_bgEntryPointMap << ", " << m_bgEntryPointX << ", " << m_bgEntryPointY << ", " << m_bgEntryPointZ << ", "
        << m_bgEntryPointO << ", ";
    ss << m_bgEntryPointInstance << ", ";

    // taxi
    if(m_onTaxi) {
        ss << m_CurrentTaxiPath->GetID() << ", ";
        ss << lastNode << ", ";
        ss << GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID);
    } else {
        ss << "0, 0, 0";
    }
    
    ss << "," << (m_CurrentTransporter ? m_CurrentTransporter->GetEntry() : (uint32)0);
    ss << ",'" << m_TransporterX << "','" << m_TransporterY << "','" << m_TransporterZ << "'";
    ss << ",'";

	// Dump spell data to stringstream
	SpellSet::iterator spellItr = mSpells.begin();
	for(; spellItr != mSpells.end(); ++spellItr)
	{
		ss << uint32(*spellItr) << ",";
	}

	ss << "','";
	// Dump deleted spell data to stringstream
	spellItr = mDeletedSpells.begin();
	for(; spellItr != mDeletedSpells.end(); ++spellItr)
	{
		ss << uint32(*spellItr) << ",";
	}

	ss << "','";
	// Dump reputation data
	std::map<uint32, Reputation>::iterator iter = m_reputation.begin();
	for(; iter != m_reputation.end(); ++iter)
	{
		ss << uint32(iter->first) << "," << uint32(iter->second.flag) << "," << uint32(iter->second.standing) << ",";
	}
	ss << "', ";
    ss << m_killsToday << ", " << m_killsYesterday << ", " << m_killsLifetime << ", ";
    ss << m_honorToday << ", " << m_honorYesterday << ", ";
    ss << m_honorPoints << ")";

    if(bNewCharacter)
        sDatabase.WaitExecute(ss.str().c_str());
    else
        sDatabase.Execute( ss.str().c_str() );

    //Save Other related player stuff

    // bids
    _SaveBids();

    // Auctions
    _SaveAuctions();
    
    // Inventory
     GetItemInterface()->mSaveItemsToDatabase(bNewCharacter);

    // save quest progress
    _SaveQuestLogEntry();

    // save finished quests
    _SaveFinishedQuests();

   
    // Action bar
    _SaveActions();

    // Tutorials
    _SaveTutorials();

    // GM Ticket
    objmgr.SaveGMTicket(GetGUID());

    // Cooldown Items
    _SaveItemCooldown();
    

    
    // Pets
    if(getClass() == HUNTER || getClass() == WARLOCK)
    {
        _SavePet();
        _SavePetSpells();
    }
	sLog.outDetail("[Player] Saved player %s ["I64FMT"], took %ums", GetName(), GetGUID(), getMSTime() - start_time);
    m_nextSave = getMSTime() + sWorld.getIntRate(INTRATE_SAVE);
}

void Player::_SaveQuestLogEntry()
{
    for(std::set<uint32>::iterator itr = m_removequests.begin(); itr != m_removequests.end(); ++itr)
        sDatabase.Execute("DELETE FROM questlog WHERE player_guid=%u AND quest_id=%u", GetGUIDLow(), (*itr));

    m_removequests.clear();

    for(int i = 0; i < 25; ++i)
    {
        if(m_questlog[i] != NULL)
            m_questlog[i]->SaveToDB();
    }
}

void Player::_SaveFinishedQuests()
{
    std::stringstream query;
    std::set<uint32>::iterator itr;

    query << "DELETE FROM finished_quests WHERE player_guid = " << GetGUIDLow();
    sDatabase.Execute( query.str().c_str( ) );

    for(itr = m_finishedQuests.begin(); itr != m_finishedQuests.end(); itr++)
    {
        query.rdbuf()->str("");
        query << "INSERT INTO finished_quests (player_guid, quest_id) VALUES (" <<
            GetGUIDLow() << ", " << (*itr) << ")";

        sDatabase.Execute( query.str().c_str( ) );
    }
}

bool Player::canCast(SpellEntry *m_spellInfo)
{
    if (m_spellInfo->EquippedItemClass != 0)
    {
        if(this->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND))
        {
		    if(this->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND)->GetProto()->Class == m_spellInfo->EquippedItemClass)
            {
                if (m_spellInfo->EquippedItemSubClass != 0)
                {
                    if (m_spellInfo->EquippedItemSubClass != 173555 && m_spellInfo->EquippedItemSubClass != 96 && m_spellInfo->EquippedItemSubClass != 262156)
                    {
						if (pow(2.0,(this->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND)->GetProto()->SubClass) != m_spellInfo->EquippedItemSubClass))
                            return false;
                    }
                }
            }
        }
        else if(m_spellInfo->EquippedItemSubClass == 173555)
            return false;

        if (this->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED))
        {
			if(this->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED)->GetProto()->Class == m_spellInfo->EquippedItemClass)
            {
                if (m_spellInfo->EquippedItemSubClass != 0)
                {
                    if (m_spellInfo->EquippedItemSubClass != 173555 && m_spellInfo->EquippedItemSubClass != 96 && m_spellInfo->EquippedItemSubClass != 262156)
                    {
                        if (pow(2.0,(this->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED)->GetProto()->SubClass) != m_spellInfo->EquippedItemSubClass))                            return false;
                    }
                }
            }
        }
        else if
            (m_spellInfo->EquippedItemSubClass == 262156)
            return false;
    }
    return true;
}

void Player::_SaveActions()
{
    std::stringstream query;
    query << "DELETE FROM char_actions WHERE charId=" << GetGUIDLow();
    sDatabase.Execute( query.str().c_str() );

    std::list<struct actions>::iterator itr;
    for (itr = m_actions.begin(); itr != m_actions.end(); ++itr)
    {
        query.rdbuf()->str("");
        query << "INSERT INTO char_actions (charId,button,action,type,misc) VALUES ( ";
        query << GetGUIDLow() << "," << int(itr->button) << "," << int(itr->action);
        query << "," << int(itr->type) << "," << int(itr->misc) << ")" << '\0';

        sDatabase.Execute( query.str().c_str() );
    }
	m_actionsDirty = false;
}


void Player::_SaveAuras()
{
    sDatabase.Execute("DELETE FROM char_auras WHERE charId=%u", GetGUIDLow());
    for(uint32 x=0;x<MAX_AURAS;x++)
	if(m_auras[x])
	{
		Aura *aur=m_auras[x];
		if(aur->m_spellProto->Effect[0] == SPELL_EFFECT_APPLY_AREA_AURA ||
			aur->m_spellProto->Effect[1] == SPELL_EFFECT_APPLY_AREA_AURA ||
            aur->m_spellProto->Effect[2] == SPELL_EFFECT_APPLY_AREA_AURA)
			continue;

  		uint32 d=m_auras[x]->GetTimeLeft();
		if(d>3000)//don's save auras with duration less than 3 seconds, anyway will loose it during login delay
	    sDatabase.Execute("INSERT INTO char_auras VALUES(%u,%u,%u)",
			GetGUIDLow(),aur->GetSpellId(),d);
    }
}


// NOTE: 32bit guids are only for compatibility with older bases.
bool Player::LoadFromDB( uint32 guid )
{
    QueryResult *result = sDatabase.Query("SELECT * FROM characters WHERE guid=%u AND banned=0",guid);
    if(!result)
        return false;

    Field *fields = result->Fetch();

    if(fields[1].GetUInt32() != m_session->GetAccountId())
    {
        sCheatLog.writefromsession(m_session, "player tried to load character not belonging to them (guid %u, on account %u)",
            fields[0].GetUInt32(), fields[1].GetUInt32());
        delete result;
        return false;
    }

    Object::_Create( guid, HIGHGUID_PLAYER );

    LoadValues( fields[2].GetString() );


    // set race dbc
    myRace = sCharRaceStore.LookupEntry(getRace());
    myClass = sCharClassStore.LookupEntry(getClass());
    if(!myClass || !myRace)
    {
        // bad character
        return false;
    }

    // TODO: check for overflow
    m_name = fields[3].GetString();

    m_position.ChangeCoords(fields[4].GetFloat(), fields[5].GetFloat(), fields[6].GetFloat(), fields[9].GetFloat());
    m_mapId = fields[7].GetUInt32();
    m_zoneId = fields[8].GetUInt32();
 
    LoadTaxiMask( fields[10].GetString() );

    m_banned = fields[11].GetUInt32(); //Character ban
    m_banreason = fields[12].GetString();

    m_bind_pos_x = fields[15].GetFloat();
    m_bind_pos_y = fields[16].GetFloat();
    m_bind_pos_z = fields[17].GetFloat();
    m_bind_mapid = fields[18].GetUInt32();
    m_bind_zoneid = fields[19].GetUInt32();

    m_isResting = fields[20].GetUInt8();
    m_restState = fields[21].GetUInt8();
    m_restAmount = fields[22].GetUInt32();
    m_timeLogoff = fields[13].GetUInt32();

    std::string tmpStr = fields[23].GetString();
    m_playedtime[0] = (uint32)atoi((const char*)strtok((char*)tmpStr.c_str()," "));
    m_playedtime[1] = (uint32)atoi((const char*)strtok(NULL," "));
   
    m_talentresettimes = fields[25].GetUInt32();
    m_FirstLogin = fields[26].GetUInt32();
	
	SetGuildId(fields[29].GetUInt32());
    SetUInt32Value(PLAYER_GUILDRANK,fields[30].GetUInt32());
    m_StableSlotCount = fields[31].GetUInt32();
    m_instanceId = fields[32].GetUInt32();
    m_bgEntryPointMap = fields[33].GetUInt32();
    m_bgEntryPointX = fields[34].GetFloat();
    m_bgEntryPointY = fields[35].GetFloat();
    m_bgEntryPointZ = fields[36].GetFloat();
    m_bgEntryPointO = fields[37].GetFloat();
    m_bgEntryPointInstance = fields[38].GetUInt32();    

    uint32 taxipath = fields[39].GetUInt32();
    TaxiPath *path = NULL;
    if(taxipath)
    {
        path = sTaxiMgr.GetTaxiPath(taxipath);
        if(path)
        {
            lastNode = fields[40].GetUInt32();
            SetTaxiPath(path);
            SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, fields[41].GetUInt32());
            m_onTaxi = true;
        }
    }

    m_TransporterGUID = fields[42].GetUInt32();
    if(m_TransporterGUID)
    {
        uint64 tguid = (uint64)((uint64)HIGHGUID_TRANSPORTER << 32 | m_TransporterGUID);
        m_TransporterGUID = tguid;
        m_TransporterX = fields[43].GetFloat();
        m_TransporterY = fields[44].GetFloat();
        m_TransporterZ = fields[45].GetFloat();
    }

    if(!info)
    {    
        info = objmgr.GetPlayerCreateInfo(this->getRace(), this->getClass());
        ASSERT(info);
    }

	UpdateStats();
    UpdateChances();

	string CSVData = fields[46].GetString();
	string numTemp;

	// Load Spells from CSV data.
	string::size_type pos; 
	string::size_type last_pos = 0;
	pos = CSVData.find(",");
	uint32 csvFieldData[3];

	while(pos != string::npos)
	{
		numTemp = CSVData.substr(last_pos, pos - last_pos);
		csvFieldData[0] = atoi(numTemp.c_str());
		mSpells.insert(csvFieldData[0]);
		last_pos = pos + 1;
		pos = CSVData.find(",", last_pos);
	}

	last_pos = 0;
	CSVData = fields[47].GetString();
	pos = CSVData.find(",");

	while(pos != string::npos)
	{		
		numTemp = CSVData.substr(last_pos, pos - last_pos);
		csvFieldData[0] = atoi(numTemp.c_str());
		mDeletedSpells.insert(csvFieldData[0]);
		last_pos = pos + 1;
		pos = CSVData.find(",", last_pos);
    }

	// Load Reputatation CSV Data
	CSVData = fields[48].GetString();
	last_pos = 0;
	uint32 Index;

	do 
	{
		for(Index = 0; Index < 3; ++Index)
		{
			pos = CSVData.find(",", last_pos);
			if( pos == string::npos )
				break;

			numTemp = CSVData.substr(last_pos, pos - last_pos);
            csvFieldData[Index] = atoi(numTemp.c_str());
			last_pos = pos + 1;
		}

		if(Index == 3)
		{
			// We did not get all fields otherwise.
			Reputation rep;
			rep.flag = csvFieldData[1];
			rep.standing = csvFieldData[2];
			m_reputation.insert( make_pair( csvFieldData[0], rep ) );
		}
	} while(pos != string::npos);
	
    m_killsToday = fields[49].GetUInt32();
    m_killsYesterday = fields[50].GetUInt32();
    m_killsLifetime = fields[51].GetUInt32();
    
    m_honorToday = fields[52].GetUInt32();
    m_honorYesterday = fields[53].GetUInt32();
    m_honorPoints = fields[54].GetUInt32();

    // fix sheath stuff
    SetUInt32Value(UNIT_FIELD_BYTES_2, (0x28 << 8));

    HonorHandler::RecalculateHonorFields(this);

    delete result;
    //Send profec.  
    
    uint32 skill;
    
    for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        if((skill=GetBaseUInt32Value(i)))
        {
            ItemProf * prof;
            //Add to proficeincy
            if(!(prof=(ItemProf *)GetProficiencyBySkill(skill)))
                continue;
            if(prof->itemclass==4)
                armor_proficiency|=prof->subclass;
            else
                weapon_proficiency|=prof->subclass;
         }
    }
   
    for(uint32 x=0;x<5;x++)
        BaseStats[x]=GetUInt32Value(UNIT_FIELD_STAT0+x);
  
	m_signedCharter=objmgr.SignedCharter(this->GetGUID());
    return true;
   
}

void Player::LoadFromDB_Light(Field *fields, uint32 guid)
{
    Object::_Create( guid, HIGHGUID_PLAYER );
    LoadValues( fields[1].GetString() );
    m_name = fields[2].GetString();

    /*m_positionX = fields[3].GetFloat();
    m_positionY = fields[4].GetFloat();
    m_positionZ = fields[5].GetFloat();*/
    m_position.ChangeCoords(fields[3].GetFloat(), fields[4].GetFloat(), fields[5].GetFloat());

    m_mapId = fields[6].GetUInt32();
    m_zoneId = fields[7].GetUInt32();

    m_banned = fields[8].GetUInt32();
    m_restState = fields[9].GetUInt8();
    m_deathState = (DeathState)fields[10].GetUInt32();

    _LoadInventoryLight();
    // set race dbc
    myRace = sCharRaceStore.LookupEntry(getRace());
    myClass = sCharClassStore.LookupEntry(getClass());
}

void Player::LoadPropertiesFromDB()
{
    _LoadTutorials();

    GetItemInterface()->mLoadItemsFromDatabase();

    _LoadActions();
  
    _LoadQuestLogEntry();

    _LoadFinishedQuests();

    _LoadBids();

    //capt
    _LoadItemCooldown();

    // init Faction
    _setFaction();
}

void Player::LoadNamesFromDB( uint32 guid )
{
    std::stringstream ss;
    ss << "SELECT * FROM characters WHERE guid=" << guid;

    QueryResult *result = sDatabase.Query( ss.str().c_str() );
    ASSERT(result);

    Field *fields = result->Fetch();

    Object::_Create( guid, HIGHGUID_PLAYER );

    LoadValues( fields[2].GetString() );

    // TODO: check for overflow
    m_name = fields[3].GetString();
}

void Player::_LoadInventoryLight()
{

    // Inventory
    std::stringstream invq;
    invq << "SELECT * FROM playeritems WHERE ownerguid=" << GetGUIDLow();
	invq << " and containerslot=-1 and slot < 19";//EQUIPMENT_SLOT_END

    QueryResult *result = sDatabase.Query( invq.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();
			Item* item;
	
			item=new Item();
			item->LoadFromDB(fields, 0, true);

			int8 slot=fields[11].GetInt8();
		
			GetItemInterface()->SafeAddItem(item, INVENTORY_SLOT_NOT_SET, slot);
			
	    }
        while( result->NextRow() );

        delete result;
    }
}

bool Player::HasSpell(uint32 spell)
{
    return mSpells.find(spell) != mSpells.end();
}

void Player::_LoadActions()
{
    //Clear Actions List
    m_actions.clear();

    //Actions
    std::stringstream query;
    query << "SELECT * FROM char_actions WHERE charId=" << GetGUIDLow() << " ORDER BY button";
    QueryResult *result = sDatabase.Query( query.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();

            addAction(fields[1].GetUInt8(), fields[2].GetUInt16(), fields[3].GetUInt8(), fields[4].GetUInt8());
        }
        while( result->NextRow() );

        delete result;
    }
	m_actionsDirty = false;
}

void Player::_LoadQuestLogEntry()
{
    QueryResult *result = sDatabase.Query("SELECT * FROM questlog WHERE player_guid=%u", GetGUIDLow());
    QuestLogEntry *entry;
    Quest *quest;
    Field *fields;
    uint32 questid;
    uint32 baseindex;
    
    // clear all fields
    for(int i = 0; i < 25; ++i)
    {
        baseindex = PLAYER_QUEST_LOG_1_1 + (i * 3);
        SetUInt32Value(baseindex + 0, 0);
        SetUInt32Value(baseindex + 1, 0);
        SetUInt32Value(baseindex + 2, 0);
    }

    int slot = 0;

    if(result)
    {
        do 
        {
            fields = result->Fetch();
            questid = fields[2].GetUInt32();
            quest = sQuestMgr.FindQuest(questid);
            slot = fields[3].GetUInt32();
            ASSERT(slot != -1);
            
            // remove on next save if bad quest
            if(!quest)
            {
                m_removequests.insert(questid);
                continue;
            }
            if(m_questlog[slot] != 0)
                continue;
            
            entry = new QuestLogEntry;
            entry->Init(quest, this, slot);
            entry->LoadFromDB(fields);
            entry->UpdatePlayerFields();

        } while(result->NextRow());
        delete result;
    }
}

QuestLogEntry* Player::GetQuestLogForEntry(uint32 quest)
{
    for(int i = 0; i < 25; ++i)
    {
        if(m_questlog[i] != NULL)
        {
            if(m_questlog[i]->GetQuest()->id == quest)
                return m_questlog[i];
        }
    }
    return NULL;
}

void Player::SetQuestLogSlot(QuestLogEntry *entry, uint32 slot)
{
    /*if(entry != NULL) {
        ASSERT(m_questlog[slot] == NULL);
    } else {
        ASSERT(m_questlog[slot] != NULL);
    }*/
    m_questlog[slot] = entry;
}

void Player::_LoadFinishedQuests()
{
    std::stringstream query;
    query << "SELECT * FROM finished_quests WHERE player_guid=" << GetGUIDLow();
    QueryResult *result = sDatabase.Query( query.str().c_str() );

    if (result)
    {
        do
        {
            Field *fields = result->Fetch();
            this->m_finishedQuests.insert(fields[1].GetUInt32());
        } while( result->NextRow() );

        delete result;
    }
}


void Player::DeleteFromDB()
{
	sDatabase.WaitExecute("DELETE FROM playeritems WHERE ownerguid=%u",GetGUIDLow());

    std::stringstream ss;

    ss << "DELETE FROM characters WHERE guid = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    /*ss.rdbuf()->str("");
    ss << "DELETE FROM auctionhouse WHERE auctioneerguid = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );*/

    ss.rdbuf()->str("");
    ss << "DELETE FROM char_actions WHERE charId = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    //FIXME:Add charter delete

    ss.rdbuf()->str("");
    ss << "DELETE FROM corpses WHERE guid = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "DELETE FROM finished_quests WHERE player_guid = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "DELETE FROM gm_tickets WHERE guid = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    //FIXME:Check Guild Leader

    //FIXME:Item instances

  

   // ss.rdbuf()->str("");
    //ss << "DELETE FROM playercharters WHERE playerId = " << GetGUIDLow();
    //sDatabase.Execute( ss.str( ).c_str( ) );

    //ss.rdbuf()->str("");
    //ss << "DELETE FROM playerguilds WHERE playerId = " << GetGUIDLow();
    //sDatabase.Execute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "DELETE FROM playerpets WHERE ownerguid = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "DELETE FROM playerstabledpets WHERE guid = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "DELETE FROM questlog WHERE player_guid = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "DELETE FROM social WHERE guid = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "DELETE FROM tutorials WHERE playerId = " << GetGUIDLow();
    sDatabase.WaitExecute( ss.str( ).c_str( ) );

    

    sSocialMgr.RemovePlayer(this);
}

void Player::AddToWorld()
{
    FlyCheat = false;
    // check transporter
    if(m_TransporterGUID && m_CurrentTransporter)
    {
        float c_tposx = m_CurrentTransporter->GetPositionX() + m_TransporterX;
        float c_tposy = m_CurrentTransporter->GetPositionY() + m_TransporterY;
        float c_tposz = m_CurrentTransporter->GetPositionZ() + m_TransporterZ;
        SetPosition(c_tposx, c_tposy, c_tposz, GetOrientation(), false);
        /*if(m_CurrentTransporter->GetMapId() != GetMapId())  // We loaded on the wrong map.
        {
            WorldPacket data(SMSG_NEW_WORLD, 20);
            data << m_CurrentTransporter->GetMapId() << c_tposx << c_tposy << c_tposz << m_orientation;
            m_session->SendPacket(&data);
        }*/
    }

    // If we join an invalid instance and get booted out, this will prevent our stats from doubling :P
    if(IsInWorld())
        return;
    Unit::AddToWorld();
    
    // Add failed.
    if(m_mapMgr == NULL)
        return;

    AddItemsToWorld();

    /*if(m_Summon)
    {
        m_Summon->SetInstanceID(GetInstanceID());
        m_Summon->SetMapId(GetMapId());
        m_Summon->SetPosition((GetPositionX() + 2), (GetPositionY() + 2), GetPositionZ(), M_PI);
        m_Summon->AddToWorld();
    }*/
    if(source != m_mapMgr)
    {
        m_session->Relocate(source, m_mapMgr);
        source = m_mapMgr;
    }

    m_lockTransportVariables = false;

    // delay the unlock movement packet
    WorldPacket * data = new WorldPacket(SMSG_MOVE_UNLOCK_MOVEMENT, 4);
    *data << uint32(0);
    delayedPackets.add(data);   
    sWorld.mInWorldPlayerCount++;

    // Update PVP Situation
    LoginPvPSetup();
}

void Player::ResetHeartbeatCoords()
{
    _lastHeartbeatX = _lastHeartbeatY = _lastHeartbeatZ = 0;
    _lastHeartbeatTime = 0;
}

void Player::RemoveFromWorld()
{
	// Cancel trade if it's active.
	if(mTradeTarget != 0)
	{
		uint32 Status = TRADE_STATUS_CANCELLED;
        if(mTradeTarget->GetSession() && mTradeTarget->GetSession()->GetSocket() && mTradeTarget->GetSession()->GetSocket()->_deleted == 0)
		    mTradeTarget->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &Status);

		mTradeTarget->ResetTradeVariables();
		ResetTradeVariables();
	}

    ResetHeartbeatCoords();
    ClearSplinePackets();
    //m_session->Relocate(m_mapMgr, NULL);
    source = m_mapMgr;
    if(IsInWorld())
    {
        RemoveItemsFromWorld();
        Unit::RemoveFromWorld();

        clearAttackers(true);
    }
    /*if(m_Summon && m_Summon->IsInWorld())
        m_Summon->RemoveFromWorld();*/
    if(m_Summon)
        m_Summon->Remove(false, true);

    if(m_SummonedObject)
    {
        if(m_SummonedObject->IsInWorld())
        {
            m_SummonedObject->RemoveFromWorld();
        }
        if(m_SummonedObject->GetTypeId() == TYPEID_UNIT)
            sObjHolder.Delete<Creature>(((Creature*)m_SummonedObject));
        else if(m_SummonedObject->GetTypeId() == TYPEID_GAMEOBJECT)
            sObjHolder.Delete<GameObject>(((GameObject*)m_SummonedObject));
        else if(m_SummonedObject->GetTypeId() == TYPEID_DYNAMICOBJECT)
            sObjHolder.Delete<DynamicObject>(((DynamicObject*)m_SummonedObject));
        m_SummonedObject = NULL;
    }
    sWorld.mInWorldPlayerCount--;
}



// TODO: perhaps item should just have a list of mods, that will simplify code
void Player::_ApplyItemMods(Item *item, int8 slot,bool apply)
{
    ASSERT(item);
    ItemPrototype *proto = item->GetProto();

    //fast check to skip mod applying if the item doesnt meat the requirements.
    if(!item->GetUInt32Value(ITEM_FIELD_DURABILITY) && item->GetUInt32Value(ITEM_FIELD_MAXDURABILITY) && apply)
    {
        return;
    }
    
	//check for rnd prop
	uint32 rndprop=item->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID);
	if(rndprop)
	{
		RandomProps *rp= sRandomPropStore.LookupEntry(rndprop);
		if(rp)
		for (int k=0;k<3;k++)
        {
			if (rp->spells[k] != 0)
			{
				if(apply)
				{
					EnchantEntry * ee = sEnchantStore.LookupEntry(rp->spells[k]);
					int32 Slot = item->HasEnchantment(ee->Id);
                    if(Slot < 0)
                        item->AddEnchantment(ee, 0, true, false, true);
                    else
                        item->ApplyEnchantmentBonus(Slot, true);
				}else
                {
                    EnchantEntry * ee = sEnchantStore.LookupEntry(rp->spells[k]);
					int32 Slot = item->HasEnchantment(ee->Id);
                    if(Slot >= 0)
                    {
                        //item->RemoveEnchantment(Slot);
                        item->ApplyEnchantmentBonus(Slot, false);
                    }
                }
			}
        }
	}

    //Items Set check
    uint32 setid=proto->ItemSet;
    
    if(setid)
    {
        ItemSetEntry *set=sItemSetStore.LookupEntry(setid);
        ASSERT(set);
        ItemSet* Set=NULL;
        std::list<ItemSet>::iterator i;
        for(i=m_itemsets.begin();i!=m_itemsets.end();i++)
        {
            if(i->setid==setid)
            {   Set = &(*i);
                break;
            }
        }
        if(apply)
        {
            if(!Set) 
            {
                Set=new ItemSet;
                memset(Set,0,sizeof(ItemSet));
                Set->itemscount=1;
                Set->setid=setid;
            }
            else
                Set->itemscount++;

            if(!set->RequiredSkillID || (GetSkillAmt(set->RequiredSkillID) >= set->RequiredSkillAmt))
            {
                for(uint32 x=0;x<8;x++)
                {
                    if(Set->itemscount==set->itemscount[x])
                    {//cast new spell
                        Log::getSingleton().outDetail("Casting spell %u from itemset", set->SpellID[x]);
                        SpellEntry *info= sSpellStore.LookupEntry(set->SpellID[x]);
                        Spell * spell=new Spell(this,info,true,NULL);
                        SpellCastTargets targets;
                        targets.m_unitTarget = this->GetGUID();
                        spell->prepare(&targets);
                    }
                }
            }
            if(i==m_itemsets.end())
            {
                m_itemsets.push_back(*Set);
                delete Set;
            }
        }
        else
        {
            if(Set)
            {
                for(uint32 x=0;x<8;x++)
                if(Set->itemscount == set->itemscount[x])
                {
                    this->RemoveAura(set->SpellID[x],GetGUID());
                }
       
                if(!(--Set->itemscount))
                m_itemsets.erase(i);
            }
        }
    }
 
    // Resistances
    if (proto->Armor)
    {
        if(apply)
        BaseResistance[0]+=proto->Armor;  
        else  BaseResistance[0]-=proto->Armor;
        CalcResistance(0);

    }  

    //FIXME: can there be negative resistances from items?
    if (proto->FireRes)
    {
        if(apply)FlatResistanceModifierPos[2]+=proto->FireRes;
        else FlatResistanceModifierPos[2]-=proto->FireRes;
        CalcResistance(2);
    }
        
    if (proto->NatureRes)
    {
        if(apply)FlatResistanceModifierPos[3]+=proto->NatureRes;
        else FlatResistanceModifierPos[3]-=proto->NatureRes;
        CalcResistance(3);
    }

    if (proto->FrostRes)
    {
        if(apply)FlatResistanceModifierPos[4]+=proto->FrostRes;
        else FlatResistanceModifierPos[4]-=proto->FrostRes;
        CalcResistance(4);    
    }

    if (proto->ShadowRes)
    {
        if(apply)FlatResistanceModifierPos[5]+=proto->ShadowRes;
        else FlatResistanceModifierPos[5]-=proto->ShadowRes;
        CalcResistance(5);    
    }
     
    if (proto->ArcaneRes)
    {
        if(apply)FlatResistanceModifierPos[6]+=proto->ArcaneRes;
        else FlatResistanceModifierPos[6]-=proto->ArcaneRes;
        CalcResistance(6);
    }
    
    // Stats
    for (int i = 0; i < 10; i++)
    {
		int32 val = proto->ItemStatValue[i];
		
        if(val == 0) continue;
		if(!apply)val=-val;
        switch (proto->ItemStatType[i]) 
        {
        case POWER:
            ModUInt32Value(UNIT_FIELD_MAXPOWER1,val);
            m_manafromitems += val;
            break;
        case HEALTH:
            ModUInt32Value(UNIT_FIELD_MAXHEALTH, val);
            m_healthfromitems += val;
            break;
        case STRENGHT: //modify strength
			FlatStatModPos[0]+=val;
			CalcStat(0);
            break;
        case AGILITY: // modify agility                
           	FlatStatModPos[1]+=val;
			CalcStat(1);
            break;
        case STAMINA: //modify stamina
			FlatStatModPos[2]+=val;
			CalcStat(2);
            break;
        case INTELLECT: //modify intellect 
   			FlatStatModPos[3]+=val;
			CalcStat(3);
            break;
		case SPIRIT: //modify spirit
        	FlatStatModPos[4]+=val;
			CalcStat(4);
            break;
		case DEFENSE_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_01, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_01) + val);
				CalcRating(proto->ItemStatType[i]);
			}break;
		case DODGE_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_02, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_02) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case PARRY_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_03, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_03) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case SHIELD_BLOCK_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_04, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_04) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case MELEE_HIT_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_05, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_05) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case RANGED_HIT_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_06, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_06) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case SPELL_HIT_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_07, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_07) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case MELEE_CRITICAL_STRIKE_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_08, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_08) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case RANGED_CRITICAL_STRIKE_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_09, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_09) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case SPELL_CRITICAL_STRIKE_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_10, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_10) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case MELEE_HIT_AVOIDANCE_RATING:
			{

			}break;
		case RANGED_HIT_AVOIDANCE_RATING:
			{

			}break;
		case SPELL_HIT_AVOIDANCE_RATING:
			{

			}break;
		case MELEE_CRITICAL_AVOIDANCE_RATING:
			{

			}break;
		case RANGED_CRITICAL_AVOIDANCE_RATING:
			{

			}break;
		case SPELL_CRITICAL_AVOIDANCE_RATING:
			{

			}break;
		case MELEE_HASTE_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_17, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_17) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case RANGED_HASTE_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_18, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_18) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case SPELL_HASTE_RATING:
			{

			}break;
		case HIT_RATING:
			{

			}break;
		case CRITICAL_STRIKE_RATING:
			{

			}break;
		case HIT_AVOIDANCE_RATING:
			{

			}break;
		case CRITICAL_AVOIDANCE_RATING:
			{

			}break;
		case RESILIENCE_RATING:
			{
				SetUInt32Value(PLAYER_FIELD_COMBAT_RATING_14, GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_14) + val);
				CalcRating(proto->ItemStatType[i]);

			}break;
		case HASTE_RATING:
			{

			}break;
        }
    }

    if(proto->DamageMin[0])
    {
        if( proto->InventoryType == INVTYPE_RANGED || proto->InventoryType == INVTYPE_RANGEDRIGHT || 
            proto->InventoryType == INVTYPE_THROWN )    
        {    
            BaseRangedDamage[0]+=apply ? proto->DamageMin[0] : -proto->DamageMin[0];
            BaseRangedDamage[1]+=apply ? proto->DamageMax[0] : -proto->DamageMax[0];
        }
        else
        {
            if(slot==EQUIPMENT_SLOT_OFFHAND)
            {
                BaseOffhandDamage[0]=apply ? proto->DamageMin[0] : 0;
                BaseOffhandDamage[1]=apply ? proto->DamageMax[0] : 0;
            }
            else
            {
                BaseDamage[0]=apply ? proto->DamageMin[0] : 1;
                BaseDamage[1]=apply ? proto->DamageMax[0] : 1;
            }
        }
	}

    // Misc
    if (apply)
    {
        // Apply all enchantment bonuses
        item->ApplyEnchantmentBonuses();

        for (int k = 0; k < 5;k++)
        {
            if (item->GetProto()->SpellTrigger[k] == 1)
            {
                SpellEntry* spells = sSpellStore.LookupEntry(item->GetProto()->SpellId[k]);
                Spell *spell = new Spell(this, spells ,true,NULL);
                SpellCastTargets targets;
                targets.m_unitTarget = this->GetGUID();
                spell->castedItemId = item->GetEntry();
                spell->prepare(&targets);                
            }
            else if(item->GetProto()->SpellTrigger[k] == 2)
			{
				ProcTriggerSpell ts;
				ts.origId=0;
				ts.spellId=item->GetProto()->SpellId[k];
				ts.procChance=5;
				ts.caster=this->GetGUID();
				ts.procFlags=PROC_ON_MELEE_ATTACK;
				this->m_procSpells.push_front(ts);			
			}
        }
    }
    else
    {
        // Remove all enchantment bonuses
        item->RemoveEnchantmentBonuses();

        for (int k = 0; k < 5;k++)
        {
            if (item->GetProto()->SpellTrigger[k] == 1)
            {
                this->RemoveAura(item->GetProto()->SpellId[k]);            
			}else if(item->GetProto()->SpellTrigger[k] == 2)
			{
				std::list<struct ProcTriggerSpell>::iterator i;
				for(i=m_procSpells.begin();i!=m_procSpells.end();i++)
				if((*i).spellId==item->GetProto()->SpellId[k])
				{
					m_procSpells.erase(i);
					break;
				}

			}
        }
    }
	
	UpdateStats();
}

void Player::_RemoveAllItemMods()
{
    for (int8 i = 0; i < INVENTORY_SLOT_BAG_END; i++)
    {
        if(GetItemInterface()->GetInventoryItem(i))
        {
            _ApplyItemMods(GetItemInterface()->GetInventoryItem(i),i, false);
        }
    }
}

void Player::_ApplyAllItemMods()
{
    for (int8 i = 0; i < INVENTORY_SLOT_BAG_END; i++)
    {
        if(GetItemInterface()->GetInventoryItem(i))
            _ApplyItemMods(GetItemInterface()->GetInventoryItem(i),i, true);
    }
}

void Player::SetMovement(uint8 pType, uint32 flag)
{
    WorldPacket data(13);

    switch(pType)
    {
    case MOVE_ROOT:
        {
            data.SetOpcode(SMSG_FORCE_MOVE_ROOT);
            data << GetNewGUID();
            data << flag;
            m_currentMovement = MOVE_ROOT;
        }break;
    case MOVE_UNROOT:
        {
            data.SetOpcode(SMSG_FORCE_MOVE_UNROOT);
            data << GetNewGUID();
            data << flag;
            m_currentMovement = MOVE_UNROOT;
        }break;
    case MOVE_WATER_WALK:
        {
            data.SetOpcode(SMSG_MOVE_WATER_WALK);
            data << GetNewGUID();
            data << flag;
        }break;
    case MOVE_LAND_WALK:
        {
            data.SetOpcode(SMSG_MOVE_LAND_WALK);
            data << GetNewGUID();
            data << flag;
        }break;
    default:break;
    }

    if(data.size() > 0)
        SendMessageToSet(&data, true);
}

void Player::SetPlayerSpeed(uint8 SpeedType, float value, uint32 flag, bool forced)
{
    WorldPacket data(18);

    switch(SpeedType)
    {
    case RUN:
        {
            if(forced) { data.SetOpcode(SMSG_FORCE_RUN_SPEED_CHANGE); }
            else { data.SetOpcode(MSG_MOVE_SET_RUN_SPEED); }
            data << GetNewGUID();
            data << flag;
            data << float(value);
            m_runSpeed = value;
        }break;
    case RUNBACK:
        {
            if(forced) { data.SetOpcode(SMSG_FORCE_RUN_BACK_SPEED_CHANGE); }
            else { data.SetOpcode(MSG_MOVE_SET_RUN_BACK_SPEED); }
            data << GetNewGUID();
            data << flag;
            data << float(value);
            m_backWalkSpeed = value;
        }break;
    case SWIM:
        {
            if(forced) { data.SetOpcode(SMSG_FORCE_SWIM_SPEED_CHANGE); }
            else { data.SetOpcode(MSG_MOVE_SET_SWIM_SPEED); }
            data << GetNewGUID();
            data << flag;
            data << float(value);
            m_swimSpeed = value;
        }break;
    case SWIMBACK:
        {
            data.SetOpcode(MSG_MOVE_SET_SWIM_BACK_SPEED);
            data << GetNewGUID();
            data << flag;
            data << float(value);
            m_backSwimSpeed = value;
        }break;
    case FLY:
        {
            data.SetOpcode(SMSG_MOVE_SET_FLY_SPEED);
            data << GetNewGUID();
            data << flag;
            data << float(value);
            m_flySpeed = value;
        }break;
    default:return;
    }
    
    SendMessageToSet(&data , true);

    // dont mess up on these
    ResetHeartbeatCoords();
}

void Player::BuildPlayerRepop()
{
    SetUInt32Value( UNIT_FIELD_HEALTH, 1 );

    /*uint32 reclaim_delay = 30000;

    if(m_bgInBattleground)
        reclaim_delay = 120000;

    m_session->OutPacket(SMSG_CORPSE_RECLAIM_DELAY, 4, &reclaim_delay);*/

    //8326 --for all races but ne,  9036 20584--ne
    
    SpellCastTargets tgt;
    tgt.m_unitTarget=this->GetGUID();
   
    if(getRace()==RACE_NIGHTELF)
    {
        SpellEntry *inf=sSpellStore.LookupEntry(20584);
        Spell*sp=new Spell(this,inf,true,NULL);
        sp->prepare(&tgt);
        inf=sSpellStore.LookupEntry(9036);
        sp=new Spell(this,inf,true,NULL);
        sp->prepare(&tgt);
    }else
    {
    
        SpellEntry *inf=sSpellStore.LookupEntry(8326);
        Spell*sp=new Spell(this,inf,true,NULL);
        sp->prepare(&tgt);
    }

    StopMirrorTimer(0);
    StopMirrorTimer(1);
    StopMirrorTimer(2);
    
    SetFlag(PLAYER_FLAGS, 0x10);

    SetMovement(MOVE_UNROOT, 1);
}

void Player::RepopRequestedPlayer()
{
	MapInfo * pMapinfo;

    // Set death state to corpse, that way players will lose visibility
    setDeathState(CORPSE);
    
    // Update visibility, that way people wont see running corpses :P
    UpdateVisibility(true);

    // If we're in battleground, remove the skinnable flag.. has bad effects heheh
    if(m_bgInBattleground && HasFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_SKINNABLE))
        RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_SKINNABLE);

	CreateCorpse();
	BuildPlayerRepop();

	pMapinfo = sWorld.GetMapInformation(GetMapId());
	if(pMapinfo)
	{
		if(pMapinfo->type == INSTANCE_NULL || pMapinfo->type == INSTANCE_PVP)
		{
			RepopAtGraveyard(GetPositionX(),GetPositionY(),GetPositionZ(),GetMapId());
		}
		else
		{
			RepopAtGraveyard(pMapinfo->repopx, pMapinfo->repopy, pMapinfo->repopz, pMapinfo->repopmapid);
		}
	}
	else
	{
		RepopAtGraveyard(GetPositionX(),GetPositionY(),GetPositionZ(),GetMapId());
	}
	
	SpawnCorpseBody();
}

void Player::ResurrectPlayer()
{
    if(m_resurrectHealth)
        SetUInt32Value(UNIT_FIELD_HEALTH, min(m_resurrectHealth, m_uint32Values[UNIT_FIELD_MAXHEALTH]));
    if(m_resurrectMana)
        SetUInt32Value(UNIT_FIELD_POWER1, min(m_resurrectMana, m_uint32Values[UNIT_FIELD_MAXPOWER1]));

    m_resurrectHealth = m_resurrectMana = 0;

    SpawnCorpseBones();
    
    if(getRace()==RACE_NIGHTELF)
    {
        RemoveAura(20584);
        RemoveAura(9036);
    }else
        RemoveAura(8326);

    RemoveFlag(PLAYER_FLAGS, 0x10);
    setDeathState(ALIVE);
    UpdateVisibility(true);
}

void Player::KillPlayer()
{
    setDeathState(JUST_DIED);

    // Battleground stuff
    if(m_bgInBattleground && GetCurrentBattleground() != NULL)
    {
    	if(GetCurrentBattleground()->GetMapId() == 489)
        	GetCurrentBattleground()->HandleBattlegroundEvent(this, NULL, BGEVENT_WSG_PLAYER_DEATH);
        else if(GetCurrentBattleground()->GetMapId() == 529)
        	GetCurrentBattleground()->HandleBattlegroundEvent(this, NULL, BGEVENT_AB_PLAYER_DEATH);
    }

    //if(m_bgHasFlag) GetCurrentBattleground()->HandleBattlegroundEvent(this, NULL, BGEVENT_WSG_PLAYER_DIED_WITH_FLAG);

    EventDeath();
	
    m_session->OutPacket(SMSG_CANCEL_AUTO_REPEAT);

    SetMovement(MOVE_ROOT, 0);

    StopMirrorTimer(0);
    StopMirrorTimer(1);
    StopMirrorTimer(2);

    SetFlag( UNIT_FIELD_FLAGS, 0x08 ); //player death animation, also can be used with DYNAMIC_FLAGS
    SetUInt32Value( UNIT_DYNAMIC_FLAGS, 0x00 );
    if(this->getClass() == WARRIOR) //rage resets on death
        SetUInt32Value(UNIT_FIELD_POWER2, 0);
}

void Player::CreateCorpse()
{
	Corpse *pCorpse;
	uint32 _uf, _pb, _pb2, _cfb1, _cfb2;

	objmgr.DelinkPlayerCorpses(this);
    if(!this->bCorpseCreateable)
    {
        bCorpseCreateable = true;   // for next time
        return; // no corpse allowed!
    }

	pCorpse = sObjHolder.Create<Corpse>(objmgr.GenerateLowGuid(HIGHGUID_CORPSE));
    pCorpse->SetInstanceID(GetInstanceID());
	pCorpse->Create(this, GetMapId(), GetPositionX(),
		GetPositionY(), GetPositionZ(), GetOrientation());

	_uf = GetUInt32Value(UNIT_FIELD_BYTES_0);
	_pb = GetUInt32Value(PLAYER_BYTES);
	_pb2 = GetUInt32Value(PLAYER_BYTES_2);

	uint8 race       = (uint8)(_uf);
	uint8 skin       = (uint8)(_pb);
	uint8 face       = (uint8)(_pb >> 8);
	uint8 hairstyle  = (uint8)(_pb >> 16);
	uint8 haircolor  = (uint8)(_pb >> 24);
	uint8 facialhair = (uint8)(_pb2);

	_cfb1 = ((0x00) | (race << 8) | (0x00 << 16) | (skin << 24));
	_cfb2 = ((face) | (hairstyle << 8) | (haircolor << 16) | (facialhair << 24));

	pCorpse->SetZoneId( GetZoneId() );
	pCorpse->SetUInt32Value( CORPSE_FIELD_BYTES_1, _cfb1 );
	pCorpse->SetUInt32Value( CORPSE_FIELD_BYTES_2, _cfb2 );
	pCorpse->SetUInt32Value( CORPSE_FIELD_FLAGS, 4 );
	pCorpse->SetUInt32Value( CORPSE_FIELD_DISPLAY_ID, GetUInt32Value(UNIT_FIELD_DISPLAYID) );

	if(m_bgInBattleground)
	{
        // remove our lootable flags
        if(HasFlag(UNIT_DYNAMIC_FLAGS, U_DYN_FLAG_LOOTABLE))
            RemoveFlag(UNIT_DYNAMIC_FLAGS, U_DYN_FLAG_LOOTABLE);

        if(HasFlag(UNIT_FIELD_FLAGS,U_FIELD_FLAG_SKINNABLE))
            RemoveFlag(UNIT_FIELD_FLAGS,U_FIELD_FLAG_SKINNABLE);
        
        loot.gold = 0;

        pCorpse->generateLoot();
		if(bShouldHaveLootableOnCorpse)
        {
		    pCorpse->SetUInt32Value(CORPSE_FIELD_DYNAMIC_FLAGS, 1); // sets it so you can loot the plyr
        }
        else
        {
            // hope this works
            pCorpse->SetUInt32Value(CORPSE_FIELD_FLAGS, 60);
        }

        // now that our corpse is created, don't do it again
        bShouldHaveLootableOnCorpse = false;
	}
    else
    {
        pCorpse->loot.gold = 0;
    }

	uint32 iDisplayID = 0;
	uint16 iIventoryType = 0;
	uint32 _cfi = 0;
	for (int8 i = 0; i < EQUIPMENT_SLOT_END; i++)
	{
		if(GetItemInterface()->GetInventoryItem(i))
		{
			iDisplayID = GetItemInterface()->GetInventoryItem(i)->GetProto()->DisplayInfoID;
			iIventoryType = (uint16)GetItemInterface()->GetInventoryItem(i)->GetProto()->InventoryType;

			_cfi =  (uint16(iDisplayID)) | (iIventoryType)<< 24;
			pCorpse->SetUInt32Value(CORPSE_FIELD_ITEM + i,_cfi);
		}
	}
	//save corpse in db for future use
	pCorpse->SaveToDB();
}

void Player::SpawnCorpseBody()
{
    Corpse *pCorpse;

    pCorpse = objmgr.GetCorpseByOwner(this);
    if(pCorpse && !pCorpse->IsInWorld())
    {
        if(bShouldHaveLootableOnCorpse && pCorpse->GetUInt32Value(CORPSE_FIELD_DYNAMIC_FLAGS) != 1)
            pCorpse->SetUInt32Value(CORPSE_FIELD_DYNAMIC_FLAGS, 1); // sets it so you can loot the plyr

        pCorpse->AddToWorld();
    }
    myCorpse = pCorpse;
}

void Player::SpawnCorpseBones()
{
	Corpse *pCorpse;
	pCorpse = objmgr.GetCorpseByOwner(this);
    myCorpse = 0;
	if(pCorpse)
	{
		if (pCorpse->IsInWorld() && pCorpse->GetCorpseState() == CORPSE_STATE_BODY)
		{
			pCorpse->SetUInt32Value(CORPSE_FIELD_FLAGS, 5);
			pCorpse->SetUInt64Value(CORPSE_FIELD_OWNER, 0); // remove corpse owner association
			//remove item association
			for (int i = 0; i < EQUIPMENT_SLOT_END; i++)
			{
				if(pCorpse->GetUInt32Value(CORPSE_FIELD_ITEM + i))
					pCorpse->SetUInt32Value(CORPSE_FIELD_ITEM + i, 0);
			}
			pCorpse->DeleteFromDB();
			objmgr.CorpseAddEventDespawn(pCorpse);
			pCorpse->SetCorpseState(CORPSE_STATE_BONES);
		}
		else
		{
			//Cheater!
		}
	}
}

void Player::DeathDurabilityLoss(double percent)
{
    m_session->OutPacket(SMSG_DURABILITY_DAMAGE_DEATH);
    uint32 pDurability;
    uint32 pMaxDurability;
    int32 pNewDurability;

    for (int8 i = 0; i < EQUIPMENT_SLOT_END; i++)
    {
        if(GetItemInterface()->GetInventoryItem(i))
        {
            pMaxDurability =  GetItemInterface()->GetInventoryItem(i)->GetUInt32Value(ITEM_FIELD_MAXDURABILITY);
            pDurability =  GetItemInterface()->GetInventoryItem(i)->GetUInt32Value(ITEM_FIELD_DURABILITY);
            if(pDurability)
            {
                pNewDurability = (uint32)(pMaxDurability*percent);
                pNewDurability = (pDurability - pNewDurability);
                if(pNewDurability < 0) 
                    pNewDurability = 0;

                if(pNewDurability <= 0) 
                { 
                    ApplyItemMods(GetItemInterface()->GetInventoryItem(i), i, false);
                }

                GetItemInterface()->GetInventoryItem(i)->SetUInt32Value(ITEM_FIELD_DURABILITY,(uint32)pNewDurability);
            }
        }
    }
}

void Player::RepopAtGraveyard(float ox, float oy, float oz, uint32 mapid)
{   
    float curX, curY, curZ;
    bool first = true;
    float closestX = 0, closestY = 0, closestZ = 0, closestO = 0;
    ObjectMgr::GraveyardMap::const_iterator itr;

	uint32 areaid = sWorldCreator.GetMap(mapid)->GetTerrainMgr()->GetAreaID(ox,oy);
	AreaTable * at = sAreaStore.LookupEntry(areaid);
	if(!at) return;

	uint32 mzone = ( at->ZoneId ? at->ZoneId : at->AreaId);

    for (itr = objmgr.GetGraveyardListBegin(); itr != objmgr.GetGraveyardListEnd(); itr++)
    {
        GraveyardTeleport *pGrave = itr->second;
        if(pGrave->MapId == mapid && pGrave->ZoneId == mzone && pGrave->FactionID == GetTeam() || pGrave->MapId == mapid && pGrave->ZoneId == mzone && pGrave->FactionID == 3
		    || pGrave->MapId == mapid && pGrave->AdjacentZoneId == mzone && pGrave->FactionID == GetTeam() || pGrave->MapId == mapid && pGrave->AdjacentZoneId == mzone && pGrave->FactionID == 3)
        {
            curX = pGrave->X;
            curY = pGrave->Y;
            curZ = pGrave->Z;
            if( first || pow(ox-curX,2) + pow(oy-curY,2) < pow(ox-closestX,2) + pow(oy-closestY,2) )
            {
                first = false;

                closestX = curX;
                closestY = curY;
                closestZ = curZ;
                closestO = pGrave->O;
            }
        }
    }
    if(closestX != 0 && closestY != 0 && closestZ != 0)
	{
        SafeTeleport(mapid, 0, LocationVector(closestX, closestY, closestZ, 0));
    }


//    //correct method as it works on official server, and does not require any damn sql
//    //no factions! no zones! no sqls! 1word: blizz-like
//    float closestX , closestY , closestZ ;
//    uint32 entries=sWorldSafeLocsStore.GetNumRows();
//    GraveyardEntry*g;
//    uint32 mymapid=mapid
//    float mx=ox,my=oy;
//    float last_distance=9e10;
//
//    for(uint32 x=0;x<entries;x++)
//    {
//        g=sWorldSafeLocsStore.LookupEntry(x);
//        if(g->mapid!=mymapid)continue;
//        float distance=(mx-g->x)*(mx-g->x)+(my-g->y)*(my-g->y);
//        if(distance<last_distance)
//        {
//            closestX=g->x;
//            closestY=g->y;
//            closestZ=g->z;
//            last_distance=distance;
//        }
//    
//    
//    }
//    if(last_distance<1e10)
//#endif

    
}

void Player::JoinedChannel(Channel *c)
{
    m_channels.push_back(c);
}

void Player::LeftChannel(Channel *c)
{
    m_channels.remove(c);
}

void Player::CleanupChannels()
{
    list<Channel *>::iterator i;
    for(i = m_channels.begin(); i != m_channels.end(); i++)
        (*i)->Leave(this,false);
}

// skilllines
bool Player::HasSkillLine(uint32 id)
{
    for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        if((uint16)(GetUInt32Value(i))==id)
            return true;
    }
    return false;
}

float Player::GetSkillUpChance(uint32 id)
{
    for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        if((uint16)(GetUInt32Value(i))==id)
        {
            uint32 val=GetUInt32Value(i+1);
            uint32 max=val>>16;
            float diff=max-val%0x10000;//Max-cur
         
            return (diff*100/max) / 3;
        
            //TODO: add difficulty level
            //we should not get increse for too high and 
            //too low level mobs etc
        }
    }
    
    return 0.0;
}


uint32 Player::GetSkillMax(uint32 id)
{
   for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        if((uint16)(GetUInt32Value(i))==id)
            return GetUInt32Value(i+1)>>16;
    }
    return 0;
}

uint32 Player::GetBaseSkillAmt(uint32 id)//return skill amount without bonus
{
    for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        if((uint16)(GetUInt32Value(i))==id)
        {
            return GetUInt32Value(i+1)%0x10000;
        }
    }
    return 0;
}

uint32 Player::GetSkillAmt(uint32 id)
{
    for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*128;i+=3)
    {
        if((uint16)(GetUInt32Value(i))==id)
        {
            return GetUInt32Value(i+1)%0x10000+GetUInt32Value(i+2) ;
        }
    }
    return 0;
}

void Player::AdvanceSkillLine(uint32 id)
{
  
   for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*128;i+=3)
   {
        if((uint16)(GetUInt32Value(i))==id && 
        	(GetUInt32Value(i+1)%0x10000+GetUInt32Value(i+2)) < (GetUInt32Value(i+1)>>16) ) // check if skill is not at the max
        {
            ModBaseUInt32Value(i+1,1);
            ModUInt32Value(i+1,1);
            return;        
        }           
    }
}

void Player::ModSkillMax(uint32 id, uint32 amt, uint32 setcur /* = 0 */)
{
    for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*128;i+=3)
    {
        if((uint16)(GetUInt32Value(i))==id)
        {
            uint32 val=GetUInt32Value(i+1)%0x10000+amt*0x10000;
            if(setcur)
                val = setcur | (val & 0xFFFF0000);

            SetBaseUInt32Value(i+1,val);
            SetUInt32Value(i+1,val);
            return;
        }           
    }
}



void Player::AddSkillLine(uint32 id, uint32 currVal, uint32 maxVal)
{
    ItemProf * prof;
	uint32 skillid=id;
	skilllineentry *en=sSkillLineStore.LookupEntry(id);
	if(en && en->type==SKILL_TYPE_PROFESSION)
	skillid|=0x10000;


	for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        if(!GetUInt32Value(i))
        {
            uint32 val=currVal|(maxVal<<16);
            SetBaseUInt32Value(i,skillid);
            SetUInt32Value(i,skillid);

            SetBaseUInt32Value(i+1,val);
            SetUInt32Value(i+1,val);
       
            if(IsInWorld())
            {
                //Add to proficeincy
                if(prof=(ItemProf *)GetProficiencyBySkill(id))
                {
                    WorldPacket data(SMSG_SET_PROFICIENCY, 8);
                    data << prof->itemclass;
                    if(prof->itemclass==4)
                    {
                        armor_proficiency|=prof->subclass;
                        data << armor_proficiency;
                    }else
                    {
                        weapon_proficiency|=prof->subclass;
                        data << weapon_proficiency;
                    }
                    GetSession()->SendPacket(&data);
                }
            }
            return;
        }
    }
}
void Player::RemoveSkillLine(uint32 id)
{
	for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        if((uint16)(GetUInt32Value(i))==id)
        {
			SetBaseUInt64Value(i,0);
            SetUInt64Value(i,0);
			SetBaseUInt32Value(i+2,0);
            SetUInt32Value(i+2,0);

			skilllineentry *en=sSkillLineStore.LookupEntry(id);
			if(en && en->type==SKILL_TYPE_PROFESSION)
            {
                uint32 points = GetUInt32Value(PLAYER_CHARACTER_POINTS2);
                if(points >= 2)
                    SetUInt32Value(PLAYER_CHARACTER_POINTS2, 2);
                else
                    SetUInt32Value(PLAYER_CHARACTER_POINTS2, points + 1);
            }

            return;
        }
    }
}

void Player::ModSkillBonus(uint32 id,int32 bonus)
{
    for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        if(GetUInt32Value(i)==id)
        {
            ModUInt32Value(i+2,bonus);
            return;
        }
    }
}

void Player::UpdateMaxSkills()
{
    uint32 skillid;
    for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        if(skillid=GetUInt32Value(i))
        {
            skilllineentry * sp= sSkillLineStore.LookupEntry(skillid);
            if(!sp) continue;
            if (sp->type != SKILL_TYPE_WEAPON && sp->id != 40)continue; // raise max to weapons and poison
            uint32 val=GetUInt32Value(i+1)%0x10000+getLevel()*5*0x10000;
            SetBaseUInt32Value(i+1,val);
            SetUInt32Value(i+1,val);
        }           
    }
}

void Player::smsg_InitialActions()
{
    //Log::getSingleton( ).outString( "Initializing Action Buttons for '%u'", GetGUID() );
    WorldPacket data(SMSG_ACTION_BUTTONS, 480);
    uint16 button=0;

    std::list<struct actions>::iterator itr;
    for (itr = m_actions.begin(); itr != m_actions.end();)
    {
        if (itr->button == button)
        {
            data << uint16(itr->action);
            data << uint8(itr->type);
            data << uint8(itr->misc);
            ++itr;
        }
        else
        {
            data << uint32(0);
        }
        button++;
    }

    if (button < 120 )
    {
        for (int temp_counter=(120-button); temp_counter>0; temp_counter--)
        {
            data << uint32(0);
        }
    }
    GetSession()->SendPacket( &data );
    //Log::getSingleton( ).outString( "Action Buttons for '%u' Initialized", GetGUID() );
}

void Player::addAction(const uint8 button, const uint16 action, const uint8 type, const uint8 misc)
{
    bool ButtonExists = false;
    std::list<struct actions>::iterator itr;
    for (itr = m_actions.begin(); itr != m_actions.end(); ++itr)
    {
        if (itr->button == button)
        {
            itr->button=button;
            itr->action=action;
            itr->type=type;
            itr->misc=misc;
            ButtonExists = true;
            break;
        }
    }
    if (!ButtonExists)
    {
        struct actions newaction;
        newaction.button=button;
        newaction.action=action;
        newaction.type=type;
        newaction.misc=misc;
        m_actions.push_back(newaction);
    }
	m_actionsDirty = true;
    Log::getSingleton( ).outDebug( "Player '%u' Added Action '%u' to Button '%u'", GetGUID(), action, button );
}

void Player::removeAction(uint8 button)
{
    std::list<struct actions>::iterator itr;
    for (itr = m_actions.begin(); itr != m_actions.end(); ++itr)
    {
        if (itr->button == button)
        {
            m_actions.erase(itr);
            break;
        }
    }
	m_actionsDirty = true;
    //Log::getSingleton( ).outString( "Action Button '%u' Removed from Player '%u'", button, GetGUID() );
}

//Groupcheck
bool Player::IsGroupMember(Player *plyr)
{
    if(m_Group != NULL) return m_Group->HasMember(plyr);
    else return false;
}


int32 Player::GetOpenQuestSlot()
{
    for (uint32 i = 0; i < 25; ++i)
        if (m_questlog[i] == NULL)
            return i;

    return -1;
}

void Player::AddToFinishedQuests(uint32 quest_id)
{
    //maybe that shouldn't be an assert, but i'll leave it for now
    //ASSERT(m_finishedQuests.find(quest_id) == m_finishedQuests.end());
    //Removed due to crash
    //If it failed though, then he's probably cheating. 
    if (m_finishedQuests.find(quest_id) != m_finishedQuests.end())
        return;

    m_finishedQuests.insert(quest_id);
}

bool Player::HasFinishedQuest(uint32 quest_id)
{
    return (m_finishedQuests.find(quest_id) != m_finishedQuests.end());
}

//From Mangos Project
void Player::_LoadTutorials()
{    
    std::stringstream query;
    query << "SELECT * FROM tutorials WHERE playerId=" << GetGUIDLow();
    QueryResult *result = sDatabase.Query( query.str().c_str() );

    if(result)
    {
        do
        {
            Field *fields = result->Fetch();

            for (int iI=0; iI<8; iI++) 
                m_Tutorials[iI] = fields[iI + 1].GetUInt32();

        }
        while( result->NextRow() );

        delete result;
    }
}

void Player::_SaveTutorials()
{        
    std::stringstream query;
    std::stringstream query2;
    query << "DELETE FROM tutorials WHERE playerId = " << GetGUIDLow();
    sDatabase.Execute( query.str().c_str() );

    query2 << "INSERT INTO tutorials (playerId,tut0,tut1,tut2,tut3,tut4,tut5,tut6,tut7) VALUES ('" << GetGUIDLow() << "','" << m_Tutorials[0] << "','" << m_Tutorials[1] << "','" << m_Tutorials[2] <<"','" << m_Tutorials[3] << "','" << m_Tutorials[4] << "','" << m_Tutorials[5] << "','" << m_Tutorials[6] << "','" << m_Tutorials[7] << "');";
    sDatabase.Execute( query2.str().c_str() );

    //sDatabase.Execute("REPLACE INTO tutorials VALUES('%u','%u','%u','%u','%u','%u','%u','%u','%u')", GetGUIDLow(), m_Tutorials[0], m_Tutorials[1], m_Tutorials[2], m_Tutorials[3], m_Tutorials[4], m_Tutorials[5], m_Tutorials[6], m_Tutorials[7]);
}

uint32 Player::GetTutorialInt(uint32 intId )
{
    ASSERT( (intId < 8) );
    return m_Tutorials[intId];
}

void Player::SetTutorialInt(uint32 intId, uint32 value)
{
    ASSERT( (intId < 8) );
    m_Tutorials[intId] = value;
}

//Player stats calculation for saving at lvl up, etc
/*void Player::CalcBaseStats()
{//((Player*)this)->getClass() == HUNTER ||
    //TODO take into account base stats at create
    uint32 AP, RAP;
    //Save AttAck power
    if(getClass() == ROGUE || getClass() == HUNTER)
    {
        AP = GetBaseUInt32Value(UNIT_FIELD_STAT0) + GetBaseUInt32Value(UNIT_FIELD_STAT1);
        RAP = (GetBaseUInt32Value(UNIT_FIELD_STAT1) * 2);
        SetBaseUInt32Value(UNIT_FIELD_ATTACK_POWER, AP);
        SetBaseUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER, RAP);
    }
    else
    {
        AP = (GetBaseUInt32Value(UNIT_FIELD_STAT0) * 2);
        RAP = (GetBaseUInt32Value(UNIT_FIELD_STAT1) * 2);
        SetBaseUInt32Value(UNIT_FIELD_ATTACK_POWER, AP);
        SetBaseUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER, RAP);
    }

}*/

void Player::UpdateHit(int32 hit)
{
   /*std::list<Affect*>::iterator i;
    Affect::ModList::const_iterator j;
    Affect *aff;
    uint32 in = hit;
    for (i = GetAffectBegin(); i != GetAffectEnd(); i++)
    {
        aff = *i;
        for (j = aff->GetModList().begin();j != aff->GetModList().end(); j++)
        {
            Modifier mod = (*j);
            if ((mod.GetType() == SPELL_AURA_MOD_HIT_CHANCE))
            {
                SpellEntry *spellInfo = sSpellStore.LookupEntry(aff->GetSpellId());
                if (this->canCast(spellInfo))
                    in += mod.GetAmount();
            }
        }
    }
    SetHitFromSpell(in);*/
}

void Player::UpdateChances()
{
    int clss = (int)getClass();
    const float baseDodge[12] = { 0, 0, 0.75, 0.64, 0, 3, 0, 1.75, 3.25, 2, 0, 0.75 };
    const float dodgeRatio[12] = { 0, 20, 20, 26.5, 14.5, 20, 0, 20, 20, 20, 0, 20 };
    //why do we save base values?
    float tmp = baseDodge[clss] + (GetUInt32Value( UNIT_FIELD_STAT1) / dodgeRatio[clss]) + (GetSkillAmt(SKILL_DEFENSE)*.04) + this->GetDodgeFromSpell();
    SetFloatValue(PLAYER_DODGE_PERCENTAGE,min(tmp,95.0));
    //  SetBaseFloatValue(PLAYER_DODGE_PERCENTAGE,(GetBaseUInt32Value( UNIT_FIELD_STAT1) / 14.5) + (GetSkillAmt(SKILL_DEFENSE)*.04)  + this->GetDodgeFromSpell());   

    tmp = GetUInt32Value(UNIT_FIELD_STAT0) / 22.0 + this->GetBlockFromSpellPCT();
    SetFloatValue(PLAYER_BLOCK_PERCENTAGE,min(tmp,95.0));
    //SetBaseFloatValue(PLAYER_BLOCK_PERCENTAGE,tmp);    

    tmp = 5.0f + GetSkillAmt(SKILL_DEFENSE)*.04  + this->GetParryFromSpell();
    SetFloatValue(PLAYER_PARRY_PERCENTAGE,min(tmp,95.0));
    //SetBaseFloatValue(PLAYER_PARRY_PERCENTAGE,(GetSkillAmt(SKILL_DEFENSE)*.04) + this->GetParryFromSpell());

    switch(clss)
    {
    case ROGUE: 
        tmp = 5.0f + (GetUInt32Value(UNIT_FIELD_STAT1) / 29.00);
        break;
    case HUNTER: 
        tmp = 5.0f + (GetUInt32Value(UNIT_FIELD_STAT1) / 29.00);
        break;
    default: 
        tmp = 5.0f + (GetUInt32Value(UNIT_FIELD_STAT1) / 20.00);
        break;
    }
    //std::list<WeaponModifier>::iterator i = tocritchance.begin();
    map<uint32, WeaponModifier>::iterator i = tocritchance.begin();
    Item*it = GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);

    for(;i!=tocritchance.end();i++)
    {
        if( 
            (i->second.wclass==-1) || //any weapon
            (it && (1 << it->GetProto()->SubClass & i->second.subclass)  )
            )
        {
            tmp+=i->second.value;
        }
    }

    SetFloatValue(PLAYER_CRIT_PERCENTAGE,min(tmp,95.0));
    SetFloatValue(PLAYER_RANGED_CRIT_PERCENTAGE,min(tmp,95.0));

    spellcritperc = (GetUInt32Value(UNIT_FIELD_STAT3) / 28.0f) + this->GetSpellCritFromSpell();
    UpdateChanceFields();
}

void Player::UpdateChanceFields()
{
    if(!IsInWorld())
        return;

    // Update spell crit values in fields
    //SetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1, spellcritperc);

    //uint32 healdone = 0;
    for(uint32 i = 0; i < 7; ++i)
    {
        SetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1 + i, SpellCritChanceSchool[i]);
        //healdone += HealDoneMod[i];
    }
    //SetFloatValue(PLAYER_FIELD_MOD_HEALING_DONE_POS, healdone);
}

void Player::UpdateStats()
{   
    // Attack Speed
    uint32 speed=2000;
    Item *weap = NULL;
    if(!disarmed)
    {
	    weap=GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
	    if(weap)
	        speed=weap->GetProto()->Delay;
	
	    SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, speed*(1.0f + (float)m_meleeattackspeedmod/100.0f) );
    }
    
    weap=GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
    if(weap && weap->GetProto()->Class==2)//weapon
    {
        speed=weap->GetProto()->Delay;
        SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, speed*(1.0f + (float)m_meleeattackspeedmod/100.0f) );
    }
    
    weap=GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
    if(weap)
    {
        speed=weap->GetProto()->Delay;
        SetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME, speed*(1.0f + (float)m_rangedattackspeedmod/100.0f) );
    }
    //formulas from wowwiki
    int32 AP = 0, RAP = 0;

    uint32 str,agi,lev;
    str=GetUInt32Value(UNIT_FIELD_STAT0);
    agi=GetUInt32Value(UNIT_FIELD_STAT1);
    lev=getLevel();
	uint32 sh;
    // Attack power
	uint32 cl=getClass();   
	switch (cl)
    {
        case DRUID:
		//get shapeshift
		sh=(GetUInt32Value(UNIT_FIELD_BYTES_1)<<8)>>24;
		AP=str*2-20;
        
		if(sh==FORM_CAT)
		AP+=lev*2+agi;
		else if(sh==FORM_BEAR||sh==FORM_DIREBEAR)
			AP+=lev*3;

        /*(Strength * 2) - 20
        + (Character Level * 3) (in Bear Form)
        + (Character Level * 2) + Agility (in Cat Form)*/
        break;
        
        case ROGUE:
        AP = lev * 2 + str + agi - 20;
        RAP = lev + agi * 2 - 20;
        break;
        
        case HUNTER:
        AP = lev* 2 + str + agi - 20;
        RAP = (lev + agi)*2 - 20;
        break;

        case SHAMAN:
        AP= (lev+str)*2 - 20;
        break;
    
        case PALADIN:
        AP = lev * 3 + str * 2 - 20;
        break;

        case WARRIOR:
        AP = lev * 3 + str * 2 - 20;
        RAP = (lev+agi)*2 - 20;
        break;
        default://mage,priest,warlock
        AP = str-10;
    }

    if(RAP <0) RAP=0;
    if(AP < 0) AP=0;
    SetUInt32Value(UNIT_FIELD_ATTACK_POWER, AP);
    SetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER, RAP);
    SetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER, 1.0f);     
   
	
	int32 hp=GetUInt32Value(UNIT_FIELD_BASE_HEALTH);
	int32 bonus=(GetUInt32Value(UNIT_FIELD_POSSTAT2)-GetUInt32Value(UNIT_FIELD_NEGSTAT2))*10+m_healthfromspell+m_healthfromitems;
	int32 res=hp+bonus;
    if(res<hp)res=hp;
	SetUInt32Value(UNIT_FIELD_MAXHEALTH, res  );
	
	if(GetUInt32Value(UNIT_FIELD_HEALTH)>res)
		SetUInt32Value(UNIT_FIELD_HEALTH,res);
	
		
	if(cl!=WARRIOR&&cl!=ROGUE)
	{
    // MP
	    int32 mana = GetUInt32Value(UNIT_FIELD_BASE_MANA);
		bonus=(GetUInt32Value(UNIT_FIELD_POSSTAT3)-GetUInt32Value(UNIT_FIELD_NEGSTAT3))*15+m_manafromspell +m_manafromitems ;
		res=mana+bonus;
		if(res<mana)res=mana;	
		SetUInt32Value(UNIT_FIELD_MAXPOWER1, res);

		if(GetUInt32Value(UNIT_FIELD_POWER1)>res)
			SetUInt32Value(UNIT_FIELD_POWER1,res);
	}
	UpdateChances();
	CalcDamage();
}

void Player::AddRestXP(uint32 amount)
{
    if(GetUInt32Value(UNIT_FIELD_LEVEL) >= 60)        // Save CPU, don't waste time on this if you're >= 60
        return;
    m_restAmount += amount;
    SetUInt32Value(PLAYER_REST_STATE_EXPERIENCE, (m_restAmount*0.5));
    UpdateRestState();
}

uint32 Player::SubtractRestXP(uint32 amount)
{
    if(GetUInt32Value(UNIT_FIELD_LEVEL) >= 60)        // Save CPU, don't waste time on this if you're >= 60
        return 0;
    uint32 amt = amount;
    int32 tmp = m_restAmount - amount;
    int32 pos = m_restAmount - (amount*2);
    if(pos < 0) pos = 0;

    if(tmp < 0)
    {
        amt = m_restAmount;
        m_restAmount = 0;
    }
    else
        m_restAmount -= amount;

    
    SetUInt32Value(PLAYER_REST_STATE_EXPERIENCE, pos);
    UpdateRestState();
    return amt;
}

uint32 Player::CalculateRestXP(uint32 seconds)
{
    float rate = sWorld.getRate(RATE_RESTXP);
    float xp = 0;
    if(seconds < 60)
    {
        xp = 1 * rate;
    }
    else
    {
        xp = ((seconds / 60) * rate);
    }
    return uint32(xp);
}

void Player::EventPlayerRest()
{
    if(GetUInt32Value(UNIT_FIELD_LEVEL) >= 60)        // Save CPU, don't waste time on this if you're >= 60
    {
        EventMgr::getSingleton().RemoveEvents(this, EVENT_PLAYER_REST);
        return;
    }
    // Rest timer
    float diff = difftime(time(NULL),m_lastRestUpdate);
    m_lastRestUpdate = (uint32)time(NULL);
    uint32 RestXP = CalculateRestXP((uint32)diff);
    Log::getSingleton( ).outDebug("REST: Adding %d rest XP for %.0f seconds of rest time", RestXP, diff);
    AddRestXP(RestXP);
}

void Player::UpdateRestState()
{
    if(m_restAmount && GetUInt32Value(UNIT_FIELD_LEVEL) < 60)
        m_restState = RESTSTATE_RESTED;
    else
        m_restState = RESTSTATE_NORMAL;

    // Update needle position
    SetUInt32Value(PLAYER_BYTES_2, ((GetUInt32Value(PLAYER_BYTES_2) & 0x00FFFFFF) | (m_restState << 24)));
}

void Player::ApplyPlayerRestState(bool apply)
{
    if(apply)
    {
        m_restState = RESTSTATE_RESTED;
        m_isResting = true;

        if(!HasFlag(PLAYER_FLAGS, PLAYER_FLAG_RESTING)) this->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_RESTING);    //put zzz icon

        UpdateRestState();
        m_lastRestUpdate = (uint32)time(NULL);

        if(GetUInt32Value(UNIT_FIELD_LEVEL) >= 60)        // Save CPU, don't waste time on this if you're >= 60
            return;
        sEventMgr.AddEvent(this, &Player::EventPlayerRest, EVENT_PLAYER_REST, (uint32)60000, 0);
    } else {
        m_isResting = false;
        if(HasFlag(PLAYER_FLAGS, PLAYER_FLAG_RESTING)) RemoveFlag(PLAYER_FLAGS,PLAYER_FLAG_RESTING);    //remove zzz icon
        sEventMgr.RemoveEvents(this, EVENT_PLAYER_REST);
        UpdateRestState();
    }
}

void Player::UpdateCooldowns()
{
    if(CooldownCheat) return;

    uint32 mstime = getMSTime();
	map<uint32,uint32>::iterator itr;
    map<uint32,uint32>::iterator next;
	for(itr = SpellCooldownMap.begin();itr!=SpellCooldownMap.end();)
	{
        next = itr++;
		if(mstime >= next->second)
			SpellCooldownMap.erase(next);
	}
	for(itr = SpellCooldownCategoryMap.begin();itr!=SpellCooldownCategoryMap.end();)
	{
		next = itr++;
		if(mstime >= next->second)
			SpellCooldownCategoryMap.erase(next);
	}
	if(mstime > GlobalCooldown)
		GlobalCooldown = 0;
}

void Player::AddCategoryCooldown(uint32 cat, uint32 tm)
{
    if(CooldownCheat) return;

    map<uint32, uint32>::iterator itr = SpellCooldownCategoryMap.find(cat);
    uint32 mstime = getMSTime();
    if(itr != SpellCooldownCategoryMap.end())
    {
        itr->second = mstime + tm;
    }
    else
    {
        SpellCooldownCategoryMap.insert( make_pair( cat, mstime + tm ) );
    }
}

void Player::AddCooldown(uint32 cat, uint32 tm)
{
    if(CooldownCheat) return;
    map<uint32, uint32>::iterator itr = SpellCooldownMap.find(cat);
    uint32 mstime = getMSTime();
    if(itr != SpellCooldownMap.end())
    {
        itr->second = mstime + tm;
    }
    else
    {
        SpellCooldownMap.insert( make_pair( cat, mstime + tm ) );
    }
}
void Player::AddGlobalCooldown(uint32 tm)
{
    if(CooldownCheat) return;
    GlobalCooldown = getMSTime() + tm;
}

#define CORPSE_VIEW_DISTANCE 900 // 30*30

bool Player::CanSee(Object* obj)
{
    if (obj == this)
       return true;

    uint32 object_type = obj->GetTypeId();

    if (!this->isAlive())
    {
        if(myCorpse && IsInRangeSet(myCorpse))  // We're close enough to our corpse
        {
            if(myCorpse->GetDistanceSq(obj) <= CORPSE_VIEW_DISTANCE)
            {
                // We can see any object within a certain distance from our corpse. ;)
                return true;
            }
        }

        // If we got here, we're further away from our corpse than we can see.
        switch(object_type)
	    {
		    case TYPEID_DYNAMICOBJECT:
		            return false;
            //------------------------------------------------------------------
		    case TYPEID_UNIT:
                {
                    if ( ((Unit*)obj)->IsSpiritHealer() )
                        return true;
                    else
                        return false;
                }
            //------------------------------------------------------------------
		    case TYPEID_PLAYER:
                {
    	            // if we're in corpse state, we can't see anything but other dead players.
                    // otherwise, we can still see everything
                    if(getDeathState() != CORPSE)
                        return true;
                    else
                        return false;
                }
            //------------------------------------------------------------------
		    default: 
			    return true;
	    }
    }
    else//alive
    {
        switch(object_type)
	    {    		
	        case TYPEID_PLAYER:
                {
                    Player *pObj = static_cast<Player *>(obj);

                    // gm invisible only applies if we're not a gm!
	                if (pObj->m_isGmInvisible && !m_session->GetPermissionCount())
		                return false;

                    // if the player is dead, but not in corpse state yet it means that
                    // they're lying on the ground dead, we need to be able to see them
                    // so that we can resurrect, etc. however, if the player is in corpse
                    // phase it means they just repopped or are running in deathworld
                    // and we have to destroy/can't see them.

                    if(pObj->getDeathState() == CORPSE)
                        return false;
            		
		            if(pObj->m_invisible)
			            return false; 
            		
		            if(!pObj->m_stealth)
			            return true;
                    if(pObj->trackStealth)
                        return true;
		            else
		            {
			            //stealth
			            if(this->GetSubGroup())
			            if(this->GetSubGroup() == ((Player*)(obj))->GetSubGroup())
					            //is in 1 party -> can see
				            return true;
			            if(this->GetGUID()==((Player*)(obj))->stalkedby)
				            return true;

			            if((pObj)->isInFront(this))
                            return false;
            			
			            float visibility = (float)GetStealthDetect()/(float)((Player*)(obj))->GetStealthLevel()  ;            			

			            float invisRange = visibility * 3 + GetFloatValue (UNIT_FIELD_BOUNDINGRADIUS) +obj->GetFloatValue (UNIT_FIELD_BOUNDINGRADIUS);
			            if (bGMTagOn || GetDistance2dSq (obj) <= invisRange * invisRange) 
				            return true;
			            else
				            return false;
		            }
                }
                //------------------------------------------------------------------
		    case TYPEID_UNIT:
                {	
		            if(static_cast<Unit*>(obj)->m_invisible)
			            return false;
            		
		            if ( ((Unit*)obj)->IsSpiritHealer() )
                        return false;
                    uint32 val = obj->GetUInt32Value(UNIT_FIELD_PADDING);
                    if(val > INVISIBILTY_FLAG_NONE)
                    {
                        if (val < INVISIBILTY_FLAG_TOTAL)
                        {
                            float r = GetInvisibiltyDetection(static_cast<INVISIBILTY_FLAG>(val))/PLAYER_LEVEL_CAP;
				            if (GetDistance2dSq (obj) < r * r) 
				                return true;
				            else
				                return false;
                        }
                    }
                    else
                        return true;
                }
            //------------------------------------------------------------------
		    case TYPEID_GAMEOBJECT://some go's are stealthed
                {
                    if(static_cast<GameObject*>(obj)->invisible)
		            {
			            uint64 owner = obj->GetUInt64Value(OBJECT_FIELD_CREATED_BY);
			            if(this->GetGUID() == owner)
				            return true;
            			
			            if(this->GetSubGroup())
				            return this->GetSubGroup()->HasMember(owner);		
			            else
			            {
                            float r = GetInvisibiltyDetection(static_cast<GameObject*>(obj)->invisibilityFlag)/PLAYER_LEVEL_CAP;
				            if (GetDistance2dSq (obj) < r * r) 
				                return true;
				            else
				                return false;
			            }            			
		            }
		            else 
                        return true;
                }        			
            //------------------------------------------------------------------
		    default:
			        return true;
        }
    }
}

void Player::AddInRangeObject(Object* pObj)
{
    //Visibility
    /*if (CanSee(pObj) && !IsVisible(pObj))
    {
        AddVisibleObject(pObj);
    }*/

    //Send taxi move if we're on a taxi
    if ((GetTaxiState()) && (pObj->GetTypeId() == TYPEID_PLAYER))
    {
        uint32 ntime = getMSTime();

        if (ntime > m_taxi_ride_time)
            m_CurrentTaxiPath->SendMoveForTime(this, (Player*)pObj, ntime - m_taxi_ride_time);
        else
            m_CurrentTaxiPath->SendMoveForTime(this, (Player*)pObj, m_taxi_ride_time - ntime);
    }

    Unit::AddInRangeObject(pObj);

    //if the object is a unit send a move packet if they have a destination
    if(pObj->GetTypeId() == TYPEID_UNIT)
    {
        //add an event to send move update have to send guid as pointer was causing a crash :(
        //sEventMgr.AddEvent(((Creature*)pObj)->GetAIInterface(), &AIInterface::SendCurrentMove, this->GetGUID(), EVENT_UNIT_SENDMOVE, 200, 1);
        ((Creature*)pObj)->GetAIInterface()->SendCurrentMove(this);
    }

    if(InGroup() && pObj->GetTypeId() == TYPEID_PLAYER)
    {
        if(static_cast<Player*>(pObj)->GetGroup() == GetGroup())                // we're in the same group
            if(!KnowsGroupMember(static_cast<Player*>(pObj)))                   // if we don't know him, add him.
                m_KnownGroupMembers.insert(((Player*)pObj));
    }
}

void Player::RemoveInRangeObject(Object* pObj)
{
    if (/*!CanSee(pObj) && */IsVisible(pObj))
    {
        RemoveVisibleObject(pObj);
    }

    Unit::RemoveInRangeObject(pObj);

    if(InGroup() && pObj->GetTypeId() == TYPEID_PLAYER)
    {
        if(static_cast<Player*>(pObj)->GetGroup() == GetGroup())    // we're in the same group
            if(KnowsGroupMember(static_cast<Player*>(pObj)))                   // if we don't know him, add him.
            {
                std::set<Player*>::iterator itr = m_KnownGroupMembers.find(((Player*)pObj));
                if(itr != m_KnownGroupMembers.end())
                    m_KnownGroupMembers.erase(itr);
            }
    }

    if( pObj == m_CurrentCharm )
        if(m_currentSpell)
            m_currentSpell->cancel();       // cancel the spell

    if(pObj == m_Summon)
    {
        if(m_Summon->IsSummon())
            m_Summon->Dismiss(true);
        else
            m_Summon->Remove(true, true);
        m_Summon = 0;
    }

    /*if(pObj == critterpet && !critterpet->m_BeingRemoved)
    {
        // removing critter pet
        if(critterpet->IsInWorld())
            critterpet->RemoveFromWorld();
        sObjHolder.Delete<Creature>(critterpet);
        critterpet = NULL;
    }*/ // crashy
}

void Player::ClearInRangeSet()
{
    m_visibleObjects.clear();
    Unit::ClearInRangeSet();
}

void Player::UpdateVisibility(bool destroy)
{
    ByteBuffer buf(2500);
    uint32 count;
    Player *pl;
    Object * pObj;

    for (Object::InRangeSet::iterator itr = GetInRangeSetBegin(); 
        itr != GetInRangeSetEnd();)
    {
        pObj = (*itr);
        ++itr;

        if (CanSee(pObj) && !IsVisible(pObj))
        {
            count = pObj->BuildCreateUpdateBlockForPlayer( &buf, this );
            PushUpdateData(&buf, count);
            buf.clear();

            AddVisibleObject(pObj);
        }
        else if (!CanSee(pObj) && IsVisible(pObj))
        {
            if(destroy)
                pObj->DestroyForPlayer(this);
            else                
                PushOutOfRange(pObj->GetNewGUID());

            RemoveVisibleObject(pObj);
        }

        if (pObj->GetTypeId() == TYPEID_PLAYER)
        {
            pl = ((Player*)pObj);
            if (pl->CanSee(this) && !pl->IsVisible(this))
            {
                count = BuildCreateUpdateBlockForPlayer(&buf, pl);
                pl->PushUpdateData(&buf, count);
                buf.clear();
                pl->AddVisibleObject(this);
            }
            else if (!pl->CanSee(this) && pl->IsVisible(this))
            {
                if(destroy)
                    DestroyForPlayer(pl);
                else
                    pl->PushOutOfRange(m_wowGuid);       

                pl->RemoveVisibleObject(this);
            }
        }
    }
}

void Player::EventCannibalize(uint32 amount)
{
    uint32 amt = (GetUInt32Value(UNIT_FIELD_MAXHEALTH)*amount)/100;
	
    uint32 newHealth = GetUInt32Value(UNIT_FIELD_HEALTH) + amt;
	
	if(newHealth <= GetUInt32Value(UNIT_FIELD_MAXHEALTH))
		SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
	else
		SetUInt32Value(UNIT_FIELD_HEALTH, GetUInt32Value(UNIT_FIELD_MAXHEALTH));

    cannibalizeCount++;
    if(cannibalizeCount == 5)
        SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);

    WorldPacket data(SMSG_PERIODICAURALOG, 38);
    data << GetNewGUID();                   // caster guid
    data << GetNewGUID();                   // target guid
    data << (uint32)(20577);                // spellid
	data << (uint32)1;                      // unknown?? need resource?
    data << (uint32)FLAG_PERIODIC_HEAL;	    // aura school
	data << amt;                            // amount of done to target / heal / damage
    data << (uint32)0;                      // unknown in some sniff this was 0x0F
    SendMessageToSet(&data, true);
}

void Player::EventReduceDrunk(bool full)
{
    uint8 drunk = ((GetUInt32Value(PLAYER_BYTES_3) >> 24) & 0xFF);
    if(full) drunk = 0;
    else drunk -= 10;
    SetUInt32Value(PLAYER_BYTES_3, ((GetUInt32Value(PLAYER_BYTES_3) & 0xFFFF00FF) | (drunk << 8)));
    if(drunk == 0) sEventMgr.RemoveEvents(this, EVENT_PLAYER_REDUCEDRUNK);
}

void Player::LoadTaxiMask(const char* data)
{
    vector<string> tokens = StrSplit(data, " ");

    int index;
    vector<string>::iterator iter;

    for (iter = tokens.begin(), index = 0;
        (index < 8) && (iter != tokens.end()); ++iter, ++index)
    {
        m_taximask[index] = atol((*iter).c_str());
    }
}

bool Player::HasQuestForItem(uint32 itemid)
{
    Quest *qst;
    for(int i = 0; i < 20; ++i)
    {
        if(m_questlog[i] != NULL)
        {
            qst = m_questlog[i]->GetQuest();
            if(!qst->count_required_item)
                continue;

            for(int j = 0; j < qst->count_required_item && j < 4; ++j)
                if(qst->required_item[j] == itemid && (GetItemInterface()->GetItemCount(itemid) < qst->required_itemcount[j]))
                    return true;
        }
    }
    return false;
}

/*Loot type MUST be
1-corpse, go
2-skinning/herbalism/minning
3-Fishing
*/
void Player::SendLoot(uint64 guid,uint8 loot_type)
{    
    Loot * pLoot = NULL;
    
    if(UINT32_LOPART(GUID_HIPART(guid)) == HIGHGUID_UNIT)
    {
        Creature* pCreature = sObjHolder.GetObject<Creature>(guid);
        if(!pCreature)return;
        pLoot=&pCreature->loot;
        m_currentLoot = pCreature;
    }else if(UINT32_LOPART(GUID_HIPART(guid)) == HIGHGUID_GAMEOBJECT)
    {
        GameObject* pGO = sObjHolder.GetObject<GameObject>(guid);
        if(!pGO)return;
        pGO->SetUInt32Value(GAMEOBJECT_STATE,0);
        pLoot=&pGO->loot;
        m_currentLoot = pGO;
    }
    else if((UINT32_LOPART(GUID_HIPART(guid)) == HIGHGUID_PLAYER) )
	{
		Player *p=sObjHolder.GetObject<Player>(guid);
		if(!p)return;
		pLoot=&p->loot;
        m_currentLoot = p;	
	}
    else if( (UINT32_LOPART(GUID_HIPART(guid)) == HIGHGUID_CORPSE))
    {
		Corpse *pCorpse = sObjHolder.GetObject<Corpse>(guid);
        if(!pCorpse)return;
		pLoot=&pCorpse->loot;
        m_currentLoot = pCorpse;
    }
    else if( (UINT32_LOPART(GUID_HIPART(guid)) == HIGHGUID_ITEM) )
    {
        Item *pItem = GetItemInterface()->GetItemByGUID(guid);
        if(!pItem)
            return;
        pLoot = pItem->loot;
        m_currentLoot = pItem;
    }

    if(!pLoot)
    {
        // something whack happened.. damn cheaters..
        return;
    }

    // add to looter set
    pLoot->looters.insert(this);
        
	WorldPacket data, data2(28);
    data.SetOpcode (SMSG_LOOT_RESPONSE);
   
   
    m_lootGuid = guid;

            
    data << guid;
    data << loot_type;//loot_type;
    data << pLoot->gold;
    data << (uint8) 0;//loot size reserve

      
    std::vector<__LootItem>::iterator iter=pLoot->items.begin();
    uint32 count=0;
    uint8 slottype = 0;
   
    for(uint32 x=0;iter!=pLoot->items.end();iter++,x++)
    { 
        if (iter->iItemsCount == 0)
            continue;

        ItemPrototype* itemProto =objmgr.GetItemPrototype(iter->item.itemid);
        if ((itemProto->Class == ITEM_CLASS_QUEST) && !HasQuestForItem(iter->item.itemid))           
            continue;

        slottype = 0;
        if(m_Group != NULL)
        {
            switch(m_Group->GetMethod())
            {
            case PARTY_LOOT_MASTER:
                slottype = 2;
                break;
            case PARTY_LOOT_GROUP:
            case PARTY_LOOT_RR:
            case PARTY_LOOT_NBG:
                slottype = 1;
                break;
            default:
                slottype = 0;
                break;
            }
            // only quality items are distributed
            if(itemProto->Quality < m_Group->GetThreshold())
            {
                slottype = 0;
            }
        }

        data << uint8(x); 
        data << uint32(iter->item.itemid);
        data << uint32(iter->iItemsCount);//nr of items of this type
        data << uint32(iter->item.displayid); 
        data << uint32(0) ;
        data << uint32(iter->iRandomProperty);
        data << slottype;   // "still being rolled for" flag
        
        if(slottype == 1)
        {
            if(iter->roll == NULL)
            {
                iter->roll = new LootRoll(60000, (m_Group != NULL ? m_Group->MemberCount() : 1),  guid, x, iter->item.itemid, 0, iter->iRandomProperty);
                data2.Initialize(SMSG_LOOT_START_ROLL);
                data2 << guid;
                data2 << x;
                data2 << uint32(iter->item.itemid);
                data2 << uint32(0);
                data2 << uint32(iter->iRandomProperty);
                data2 << uint32(60000); // countdown

                if(m_Group)
                {
                    for(int i = 0; i < m_Group->GetSubGroupCount(); ++i)
                    {
                        for(GroupMembersSet::iterator itr = m_Group->GetSubGroup(i)->GetGroupMembersBegin(); itr != m_Group->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
                        {
							if((*itr)->GetItemInterface()->CanReceiveItem(itemProto, iter->iItemsCount))
							{
								if((*itr)->m_rolls.count(iter->roll) == 0)
                                (*itr)->m_rolls.insert(iter->roll);

								(*itr)->GetSession()->SendPacket(&data2);
							}
                        }
                    }
                }
                else
                {
                    m_rolls.insert(iter->roll);
                    GetSession()->SendPacket(&data2);
                }
            }            
        }
        count++;      
    }
    data.wpos (13) ;
    data << (uint8)count;

    GetSession ()->SendPacket(&data);
    SetFlag(UNIT_FIELD_FLAGS,U_FIELD_ANIMATION_LOOTING);
}

void Player::EventAllowTiggerPort(bool enable)
{
    m_AllowAreaTriggerPort = enable;
}

Battleground* Player::GetCurrentBattleground()
{
    Battleground* bg = sBattlegroundMgr.GetBattleground(uint8(m_bgBattlegroundID));
    if(bg)
        return bg;
    return NULL;
}

uint32 Player::CalcTalentResetCost(uint32 resetnum)
{
	
    if(resetnum ==0 ) 
        return  10000;
    else
	{
		if(resetnum>10)
        return  500000;
		else return resetnum*50000;
	}
}

void Player::SendTalentResetConfirm()
{
    WorldPacket data(MSG_TALENT_WIPE_CONFIRM, 12);
    data << GetGUID();
    data << CalcTalentResetCost(GetTalentResetTimes());
    GetSession()->SendPacket(&data);
}

bool Player::CanShootRangedWeapon(uint32 spellid, Unit *target, bool autoshot)
{
    SpellEntry *spellinfo = sSpellStore.LookupEntry(spellid);
    if(!spellinfo)
        return false;
    
    uint8 fail = 0;
    Item *itm = GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
    if(!itm)
    {
        fail = SPELL_FAILED_NO_AMMO;
        return false;
    }

    if(m_curSelection != m_AutoShotTarget)
    {
        // Player has clicked off target. Fail spell.
        fail = SPELL_FAILED_INTERRUPTED;
    }

    if(this->m_AutoShotStartX != GetPositionX() ||
        this->m_AutoShotStartY != GetPositionY() ||
        this->m_AutoShotStartZ != GetPositionZ())
    {
        // We've moved
        //printf("Autoshot: Detected player movement. Cancelling.\n");
        fail = SPELL_FAILED_INTERRUPTED;
    }
   
    if(!target || target->isDead())
        return false;    

    SpellRange * range = sSpellRange.LookupEntry(spellinfo->rangeIndex);
    float minrange = GetMinRange(range);
    float dist = CalcDistance(this, target);
	float maxr = GetMaxRange(range);
    // Check for close
    if(spellid != SPELL_RANGED_WAND)//no min limit for wands
	    if(minrange > dist)
            fail = SPELL_FAILED_TOO_CLOSE;

	if(maxr < dist)
        fail = SPELL_FAILED_OUT_OF_RANGE;

    if(spellid == SPELL_RANGED_THROW)
    {
        if(itm) // no need for this
            if(itm->GetProto())
                if(GetItemInterface()->GetItemCount(itm->GetProto()->ItemId) == 0)
                    fail = SPELL_FAILED_NO_AMMO;
    } 
    else
	{
		if(GetUInt32Value(PLAYER_AMMO_ID))//for wand
            if(this->GetItemInterface()->GetItemCount(GetUInt32Value(PLAYER_AMMO_ID)) == 0)
                fail = SPELL_FAILED_NO_AMMO;
    }

    if(fail)// && fail != SPELL_FAILED_OUT_OF_RANGE)
    {
        WorldPacket data(SMSG_CAST_RESULT, 12);
        if(autoshot)
            data << uint32(75);
        else 
            data << uint32(spellid);
        data << uint8(2) << fail;        // we want to send a cast result for AUTOSHOT, as thats what the client sees as being cast.
        GetSession()->SendPacket(&data);
        return false;
    }
    return true;
}

void Player::EventRepeatSpell(SpellEntry* spellinfo, uint32 duration, uint64 guid)
{
    if(!m_curSelection)
        return;
    
    Unit *target = World::GetUnit(guid);

    if(!target || !this->CanShootRangedWeapon(spellinfo->Id, target, true))
    {
        sEventMgr.RemoveEvents(this, EVENT_PLAYER_REPEAT_SHOT);
        return;
    }
    else
    {        
        uint32 duration2 = this->GetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME);
        
        //Ranged Ammo Haste
        duration2 -= (duration2*GetAmmoAttackSpeed())/100;

        if(duration != duration2)
            sEventMgr.ModifyEventTime(this, EVENT_PLAYER_REPEAT_SHOT, duration2);
    
		Spell *sp=new Spell(this,spellinfo,true,NULL);
		SpellCastTargets tgt;
		tgt.m_unitTarget = m_curSelection;
		tgt.m_targetMask = TARGET_FLAG_UNIT;
		sp->prepare(&tgt);
	}
}

bool Player::removeSpell(uint32 SpellID, bool MoveToDeleted, bool SupercededSpell, uint32 SupercededSpellID)
{
    SpellSet::iterator iter = mSpells.find(SpellID);
    if(iter != mSpells.end())
    {
        mSpells.erase(iter);
        RemoveAura(SpellID,GetGUID());
    }
	else
	{
		return false;
	}

	if(MoveToDeleted)
		mDeletedSpells.insert(SpellID);

	if(!m_inWorld)
		return true;

	if(SupercededSpell)
	{
		WorldPacket data(SMSG_SUPERCEDED_SPELL, 8);
		data << SupercededSpellID << SpellID;
		m_session->SendPacket(&data);
	}
	else
	{
		m_session->OutPacket(SMSG_REMOVED_SPELL, 4, &SpellID);		
	}

    return true;
}

void Player::EventActivateGameObject(GameObject* obj)
{
	obj->BuildFieldUpdatePacket(this, GAMEOBJECT_DYN_FLAGS, 1);
}

void Player::EventDeActivateGameObject(GameObject* obj)
{
    obj->BuildFieldUpdatePacket(this, GAMEOBJECT_DYN_FLAGS, 0);
}

void Player::EventTimedQuestExpire(Quest *qst, QuestLogEntry *qle, uint32 log_slot)
{
    WorldPacket fail;
    sQuestMgr.BuildQuestFailed(&fail, qst->id);
    GetSession()->SendPacket(&fail);
    qle->Finish();
}

void Player::SendInitialLogonPackets()
{
    // Initial Packets... they seem to be re-sent on port.
    m_session->OutPacket(SMSG_SET_REST_START, 4, &m_timeLogoff);

    WorldPacket data(SMSG_BINDPOINTUPDATE, 32);
    data << m_bind_pos_x;
    data << m_bind_pos_y;
    data << m_bind_pos_z;
    data << m_bind_mapid;
    data << m_bind_zoneid;
    GetSession()->SendPacket( &data );

    //Proficiences
    data.Initialize(SMSG_SET_PROFICIENCY);   
    data << (uint8)4;
    data << armor_proficiency ; 
    GetSession()->SendPacket(&data);

    data.clear();
    data << (uint8)2;
    data << weapon_proficiency; 
    GetSession()->SendPacket(&data);
  
    //Tutorial Flags
    data.Initialize( SMSG_TUTORIAL_FLAGS );
    for (int i = 0; i < 8; i++)
        data << uint32( m_Tutorials[i] );
    GetSession()->SendPacket(&data);

    //Initial Spells
    smsg_InitialSpells();

    //Initial Actions
    smsg_InitialActions();

    //Factions
    smsg_InitialFactions();

    data.Initialize(SMSG_LOGIN_SETTIMESPEED);
    time_t minutes = sWorld.GetGameTime( ) / 60;
    time_t hours = minutes / 60; minutes %= 60;
    time_t gameTime = minutes + ( hours << 6 );
    data << (uint32)gameTime;
    data << (float)0.017f;  // Normal Game Speed
    GetSession()->SendPacket( &data );

    sLog.outDetail("WORLD: Sent initial logon packets for %s.", GetName());
}

void Player::Reset_Spells()
{
    PlayerCreateInfo *info = objmgr.GetPlayerCreateInfo(getRace(), getClass());
    ASSERT(info);

    std::list<uint32> spelllist;
    
    for(SpellSet::iterator itr = mSpells.begin(); itr!=mSpells.end(); itr++)
    {
        spelllist.push_back((*itr));
    }

    for(std::list<uint32>::iterator itr = spelllist.begin(); itr!=spelllist.end(); itr++)
    {
        removeSpell((*itr), false, false, 0);
    }

    for(std::list<uint16>::iterator sp = info->spell_list.begin();sp!=info->spell_list.end();sp++)
    {
        if(*sp)
        {
            addSpell(*sp);
        }
    }
}

void Player::Reset_Talents()
{
    unsigned int numRows = sTalentStore.GetNumRows();
   
    for (unsigned int i = 0; i < numRows; i++)          // Loop through all talents.
    {
		TalentEntry *tmpTalent = sTalentStore.LookupEntry(i);
        for (int j = 0; j < 5; j++)
        {
			if (tmpTalent->RankID[j] != 0)
	           removeSpell(tmpTalent->RankID[j], false, 0, 0);
			else
				break;
        }
    }
    uint32 l=getLevel();
	if(l>9)
	{
		SetUInt32Value(PLAYER_CHARACTER_POINTS1, l - 9); 
		SetBaseUInt32Value(PLAYER_CHARACTER_POINTS1, l - 9);

	}
	else
	{
		SetUInt32Value(PLAYER_CHARACTER_POINTS1, 0); 
		SetBaseUInt32Value(PLAYER_CHARACTER_POINTS1,0);
	}

}

void Player::Reset_ToLevel1()
{
    RemoveAllAuras();
    // clear aura fields
    for(int i=UNIT_FIELD_AURA;i<UNIT_FIELD_AURASTATE;++i)
    {
        SetUInt32Value(i, 0);
        SetBaseUInt32Value(i, 0);
    }
    SetUInt32Value(UNIT_FIELD_LEVEL, 1);
    PlayerCreateInfo *info = objmgr.GetPlayerCreateInfo(getRace(), getClass());
    ASSERT(info);

    SetUInt32Value(UNIT_FIELD_HEALTH, info->health);
    SetUInt32Value(UNIT_FIELD_POWER1, info->mana );
    SetUInt32Value(UNIT_FIELD_POWER2, 0 ); // this gets devided by 10
    SetUInt32Value(UNIT_FIELD_POWER3, info->focus );
    SetUInt32Value(UNIT_FIELD_POWER4, info->energy );
    SetUInt32Value(UNIT_FIELD_MAXHEALTH, info->health);
	SetUInt32Value(UNIT_FIELD_BASE_HEALTH, info->health);
	SetUInt32Value(UNIT_FIELD_BASE_MANA, info->mana);
    SetUInt32Value(UNIT_FIELD_MAXPOWER1, info->mana );
    SetUInt32Value(UNIT_FIELD_MAXPOWER2, info->rage );
    SetUInt32Value(UNIT_FIELD_MAXPOWER3, info->focus );
    SetUInt32Value(UNIT_FIELD_MAXPOWER4, info->energy );
    SetUInt32Value(UNIT_FIELD_STAT0, info->strength );
    SetUInt32Value(UNIT_FIELD_STAT1, info->ability );
    SetUInt32Value(UNIT_FIELD_STAT2, info->stamina );
    SetUInt32Value(UNIT_FIELD_STAT3, info->intellect );
    SetUInt32Value(UNIT_FIELD_STAT4, info->spirit );
    SetUInt32Value(UNIT_FIELD_ATTACK_POWER, info->attackpower );
    SetUInt32Value(PLAYER_CHARACTER_POINTS1,0);
    SetUInt32Value(PLAYER_CHARACTER_POINTS2,2);
    for(uint32 x=0;x<7;x++)
        SetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+x, 1.00);

	// reset base
	memcpy(m_baseUint32Values, m_uint32Values,m_valuesCount*sizeof(uint32));

}

void Player::CalcResistance(uint32 type)
{
    int32 res;
    uint32 pos;
    uint32 neg;
    ASSERT(type < 7);
    pos=(BaseResistance[type]*BaseResistanceModPctPos[type])/100;
    neg=(BaseResistance[type]*BaseResistanceModPctNeg[type])/100;

	pos+=FlatResistanceModifierPos[type];
    neg+=FlatResistanceModifierNeg[type];
    res=BaseResistance[type]+pos-neg;
    if(type==0)res+=GetUInt32Value(UNIT_FIELD_STAT1)*2;//fix armor from agi
	if(res<0)res=0;
    pos+=(res*ResistanceModPctPos[type])/100;
    neg+=(res*ResistanceModPctNeg[type])/100;
    res=pos-neg+BaseResistance[type];
	if(type==0)res+=GetUInt32Value(UNIT_FIELD_STAT1)*2;//fix armor from agi
	SetUInt32Value(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+type,pos);
    SetUInt32Value(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+type,neg);
    SetUInt32Value(UNIT_FIELD_RESISTANCES+type,res>0?res:0);

}


void Player::UpdateNearbyGameObjects()
{
    for (Object::InRangeSet::iterator itr = GetInRangeSetBegin(); itr != GetInRangeSetEnd(); ++itr)
    {
        if((*itr)->GetTypeId() == TYPEID_GAMEOBJECT)
        {
            bool activate_quest_object = false;
            GameObject *go = ((GameObject*)*itr);
            QuestLogEntry *qle;
            GameObjectInfo *info;

            info = go->GetInfo();
            bool deactivate = false;
            if(info &&
                (info->goMap.size() || info->itemMap.size()) )
            {
                for(GameObjectGOMap::iterator itr = go->GetInfo()->goMap.begin();
                    itr != go->GetInfo()->goMap.end();
                    ++itr)
                {
                    if(qle = GetQuestLogForEntry(itr->first->id))
                    {
                        for(int i = 0; i < qle->GetQuest()->count_required_mob; ++i)
                        {
                            if(qle->GetQuest()->required_mob[i] == go->GetEntry() &&
                                qle->GetMobCount(i) < qle->GetQuest()->required_mobcount[i])
                            {
                                activate_quest_object = true;
                                break;
                            }
                        }
                        if(activate_quest_object)
                            break;
                    }
                }

                if(!activate_quest_object)
                {
                    for(GameObjectItemMap::iterator itr = go->GetInfo()->itemMap.begin();
                        itr != go->GetInfo()->itemMap.end();
                        ++itr)
                    {
                        for(std::map<uint32, uint32>::iterator it2 = itr->second.begin();
                            it2 != itr->second.end(); 
                            ++it2)
                        {
                            if(GetItemInterface()->GetItemCount(it2->first) < it2->second)
                            {
                                activate_quest_object = true;
                                break;
                            }
                        }
                        if(activate_quest_object)
                            break;
                    }
                }

                if(!activate_quest_object)
                {
                    deactivate = true;
                }
            }
            bool bPassed = !deactivate;
            if((*itr)->GetUInt32Value(GAMEOBJECT_TYPE_ID) == GAMEOBJECT_TYPE_QUESTGIVER)
            {
                if(((GameObject*)(*itr))->m_quests)
                {
                    if(((GameObject*)(*itr))->m_quests->size() > 0)
                    {
                        std::list<QuestRelation*>::iterator itr2 = ((GameObject*)(*itr))->m_quests->begin();
                        for(;itr2!=((GameObject*)(*itr))->m_quests->end();++itr2)
                        {
                            uint32 status = sQuestMgr.CalcQuestStatus(NULL, this, (*itr2)->qst, (*itr2)->type);
                            if(status == QMGR_QUEST_AVAILABLE || status == QMGR_QUEST_REPEATABLE || status == QMGR_QUEST_FINISHED)
                            {
                                // Activate gameobject
                                EventActivateGameObject((GameObject*)(*itr));
                                bPassed = true;
                                break;
                            }
                        }
                    }
                }
            }
            if(!bPassed) 
                EventDeActivateGameObject((GameObject*)(*itr));
        }
    }
}


void Player::EventTaxiInterpolate()
{
    float x,y,z;
    uint32 ntime = getMSTime();

    if (ntime > m_taxi_ride_time)
        m_CurrentTaxiPath->SetPosForTime(x, y, z, ntime - m_taxi_ride_time, &lastNode);
    else
        m_CurrentTaxiPath->SetPosForTime(x, y, z, m_taxi_ride_time - ntime, &lastNode);

    if(x < _minX || x > _maxX || y < _minY || y > _maxX)
        return;

    SetPosition(x,y,z,0);
}

void Player::TaxiStart(TaxiPath *path, uint32 modelid, uint32 start_node)
{
	if(this->m_MountSpellId)
		RemoveAura(m_MountSpellId);
    
    SetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID, modelid );
    
    SetFlag( UNIT_FIELD_FLAGS ,U_FIELD_FLAG_LOCK_PLAYER );
    SetFlag( UNIT_FIELD_FLAGS, U_FIELD_FLAG_MOUNT_SIT );

    SetTaxiPath(path);
    SetTaxiPos();
    SetTaxiState(true);
    m_taxi_ride_time = getMSTime();

    //uint32 traveltime = uint32(path->getLength() * TAXI_TRAVEL_SPEED); // 36.7407
    float traveldist = 0;

    float lastx = 0, lasty = 0, lastz = 0;
    TaxiPathNode *firstNode = path->GetPathNode(start_node);
    uint32 add_time = 0;
    if(start_node)
    {
        TaxiPathNode *pn = path->GetPathNode(0);
        float dist = 0;
        lastx = pn->x;
        lasty = pn->y;
        lastz = pn->z;
        for(int i = 1; i <= start_node; ++i)
        {
            pn = path->GetPathNode(i);
            if(!pn)
            {
                JumpToEndTaxiNode(path);
                return;
            }

            dist += CalcDistance(lastx, lasty, lastz, pn->x, pn->y, pn->z);
            lastx = pn->x;
            lasty = pn->y;
            lastz = pn->z;
        }
        add_time = uint32( dist * TAXI_TRAVEL_SPEED );
        lastx = lasty = lastz = 0;
    }
    for(int i = start_node; i < path->GetNodeCount(); ++i)
    {
        TaxiPathNode *pn = path->GetPathNode(i);
        if(!pn)
        {
            JumpToEndTaxiNode(path);
            return;
        }

        if(!lastx || !lasty || !lastz)
        {
            lastx = pn->x;
            lasty = pn->y;
            lastz = pn->z;
        } else {            
            float dist = CalcDistance(lastx,lasty,lastz,
                pn->x,pn->y,pn->z);
            traveldist += dist;
            lastx = pn->x;
            lasty = pn->y;
            lastz = pn->z;
        }
    }

    uint32 traveltime = uint32(traveldist * TAXI_TRAVEL_SPEED);

    WorldPacket data(SMSG_MONSTER_MOVE, 38 + ( (path->GetNodeCount() - start_node) * 12 ) );
    data << GetNewGUID();
    data << firstNode->x << firstNode->y << firstNode->z;
    data << m_taxi_ride_time;
    data << uint8( 0 );
    data << uint32( 0x00000300 );
    data << uint32( traveltime );

    m_taxi_ride_time -= add_time;
    
    data << uint32( path->GetNodeCount() - start_node );
    uint32 timer = 0, nodecount = 0;
    TaxiPathNode *lastnode = NULL;

    for(uint32 i = start_node; i < path->GetNodeCount(); i++)
    {
        TaxiPathNode *pn = path->GetPathNode(i);
        if(!pn)
        {
            JumpToEndTaxiNode(path);
            return;
        }

        data << pn->x << pn->y << pn->z;
    }

    SendMessageToSet(&data, true);
  
    sEventMgr.AddEvent(this, &Player::EventTaxiInterpolate, 
        EVENT_PLAYER_TAXI_INTERPOLATE, 900, 0);

    TaxiPathNode *pn = path->GetPathNode(path->GetNodeCount() - 1);

    sEventMgr.AddEvent(this, &Player::EventDismount, path->GetPrice(), 
        pn->x, pn->y, pn->z, EVENT_PLAYER_TAXI_DISMOUNT, traveltime, 1); 
}

void Player::JumpToEndTaxiNode(TaxiPath * path)
{
    // this should *always* be safe in case it cant build your position on the path!
    TaxiPathNode * pathnode = path->GetPathNode(path->GetNodeCount()-1);
    if(!pathnode) return;

    SetTaxiState(false);
    SetTaxiPath(NULL);
    UnSetTaxiPos();
    m_taxi_ride_time = 0;

    SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID , 0);
    RemoveFlag( UNIT_FIELD_FLAGS, U_FIELD_FLAG_MOUNT_SIT );

    if (HasFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER ))   
        RemoveFlag( UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER );

    SetPlayerSpeed(RUN, m_runSpeed, 6, true);

    SafeTeleport(pathnode->mapid, 0, LocationVector(pathnode->x, pathnode->y, pathnode->z));
    return;
}

void Player::RemoveSpellsFromLine(uint32 skill_line)
{
    uint32 cnt = sSkillStore.GetNumRows();
    for(uint32 i=0; i < cnt; i++)
    {
        skilllinespell * sp = sSkillStore.LookupEntry(i);
        if(sp)
        {
            if(sp->skilline == skill_line)
            {
                // Check ourselves for this spell, and remove it..
              //  if(HasSpell(sp->spell))//what for? 
					removeSpell(sp->spell, 0, 0, 0);
            }
        }
    }
}

void Player::CalcStat(uint32 type)
{
	int32 res;
    ASSERT(type < 5);
	int32 pos=(BaseStats[type]*StatModPctPos[type])/100+FlatStatModPos[type];
	int32 neg=(BaseStats[type]*StatModPctNeg[type])/100+FlatStatModNeg[type];
	res=pos+BaseStats[type]-neg;
	pos+=(res*((Player*)this)->TotalStatModPctPos[type])/100;
	neg+=(res*((Player*)this)->TotalStatModPctNeg[type])/100;
	res=pos+BaseStats[type]-neg;
	SetUInt32Value(UNIT_FIELD_POSSTAT0+type,pos);
	SetUInt32Value(UNIT_FIELD_NEGSTAT0+type,neg);
	SetUInt32Value(UNIT_FIELD_STAT0+type,res>0?res:0);
	if(type==1)
       CalcResistance(0);
}

void Player::RegenerateMana()
{
    const static float ClassMultiplier[12]={
        0,0,0.2,0.25,0,0.25,0,0.2,0.25,0.2,0,0.2};
        const static float ClassFlatMod[12]={
            0,0,15,15,0,12.5,0,17,12.5,15,0,15};
            float amt;
            if (m_interruptRegen)
                return;

            uint32 cur = GetUInt32Value(UNIT_FIELD_POWER1);
            uint32 mm = GetUInt32Value(UNIT_FIELD_MAXPOWER1);
            if(cur >= mm)return;
            uint32 cl = getClass();
            uint32 Spirit = GetUInt32Value(UNIT_FIELD_STAT4);
            amt = (Spirit*ClassMultiplier[cl]+ClassFlatMod[cl])*PctPowerRegenModifier;

            //Apply shit from conf file
            amt *= sWorld.getRate(RATE_POWER1);

            //Near values from official
			// wowwiki says no faster mp while resting, anyways this is wrong it reduces instead of increasing.
            /*if(m_isResting)
                amt = amt * 0.7f;
            else
                amt = amt * 0.3f;*/

            if(amt<=1.0)//this fixes regen like 0.98
                cur++;
            else
                cur += amt;    

            SetUInt32Value(UNIT_FIELD_POWER1,(cur >= mm) ? mm : cur);
}

void Player::RegenerateHealth()
{
    const static float ClassMultiplier[12]={
        0,0.8,0.25,0.25,0.5,0.1,0,0.11,0.1,0.11,0,0.09};

	const static float ClassFlatMod[12]={
            0,6,6,6,2,4,0,6,4,6,0,6.5};

        float amt;
        uint32 cur = GetUInt32Value(UNIT_FIELD_HEALTH);
        uint32 mh = GetUInt32Value(UNIT_FIELD_MAXHEALTH);
        if(cur >= mh)
            return;
        uint32 cl = getClass();
        uint32 Spirit = GetUInt32Value(UNIT_FIELD_STAT4);
        amt = (Spirit*ClassMultiplier[cl]+ClassFlatMod[cl])*PctRegenModifier;

        //Apply shit from conf file
        amt *= sWorld.getRate(RATE_HEALTH);
        //Near values from official
		// wowwiki: Health Regeneration is increased by 33% while sitting.
        if(m_isResting)
            amt = amt * 1.33f;

        if(amt <= 1.0)//this fixes regen like 0.98
            cur++;
        else
            cur += amt;
        SetUInt32Value(UNIT_FIELD_HEALTH,(cur>=mh) ? mh : cur);
}

void Player::LooseRage()
{
    //Rage is lost at a rate of 3 rage every 3 seconds. 
    //The Anger Management talent changes this to 2 rage every 3 seconds.
    uint32 cur = GetUInt32Value(UNIT_FIELD_POWER2 );
    if(cur)
        SetUInt32Value(UNIT_FIELD_POWER2,(cur<=20) ? 0 : (cur-20));
}

void Player::RegenerateEnergy()
{
    uint32 cur = GetUInt32Value(UNIT_FIELD_POWER4);
    uint32 mh = GetUInt32Value(UNIT_FIELD_MAXPOWER4);
    if(cur >= mh)
        return;
    float amt = 20.0 * PctPowerRegenModifier;

    cur += amt;
    SetUInt32Value(UNIT_FIELD_POWER4,(cur>=mh) ? mh : cur);
}

uint32 Player::GeneratePetNumber()
{
    uint32 val = m_PetNumberMax + 1;
    for (uint32 i = 1; i < m_PetNumberMax; i++)
        if(m_Pets.find(i) == m_Pets.end())
            return i;                       // found a free one

    return val;
}

void Player::RemovePlayerPet(uint32 pet_number)
{
    std::map<uint32, PlayerPet*>::iterator itr = m_Pets.find(pet_number);
    if(itr != m_Pets.end())
    {
        delete itr->second;
        m_Pets.erase(itr);
    }
}

void Player::_Relocate(uint32 mapid, const LocationVector & v, bool sendpending, bool force_new_world)
{
    //this func must only be called when switching between maps!
	WorldPacket data(41);
	if(sendpending && mapid != m_mapId && force_new_world)
    {
        data.SetOpcode(SMSG_TRANSFER_PENDING);
	    data << mapid;
	    GetSession()->SendPacket(&data);
    }

	SetPlayerStatus(TRANSFER_PENDING);
    
    if(m_mapId != mapid || force_new_world)
    {
	    if(IsInWorld())
        {
		    RemoveFromWorld();
	    }

	    data.Initialize(SMSG_NEW_WORLD);
        data << (uint32)mapid << v << v.o;
        GetSession()->SendPacket( &data );
    }
    else
    {
        // via teleport ack msg
        WorldPacket * data = BuildTeleportAckMsg(v);
        m_session->SendPacket(data);
        delete data;
    }
    
	SetMapId(mapid);
	SetPosition(v);
    ResetHeartbeatCoords();
}

// Player::AddItemsToWorld
// Adds all items to world, applies any modifiers for them.

void Player::AddItemsToWorld()
{
    for(int8 i = 0; i < INVENTORY_KEYRING_END; i++)
    {
        if(GetItemInterface()->GetInventoryItem(i))
        {
            GetItemInterface()->GetInventoryItem(i)->AddToWorld();
            
            if(i < INVENTORY_SLOT_BAG_END)      // only equipment slots get mods.
			{
                _ApplyItemMods(GetItemInterface()->GetInventoryItem(i), i, true);
			}

            if(GetItemInterface()->GetInventoryItem(i)->IsContainer() && GetItemInterface()->IsBagSlot(i))
            {
                for(int8 e=0; e < GetItemInterface()->GetInventoryItem(i)->GetProto()->ContainerSlots; e++)
                {
                    Item *item = ((Container*)GetItemInterface()->GetInventoryItem(i))->GetItem(e);
                    if(item)
                    {
                        item->AddToWorld();
                    }
                }
            }
        }
    }
}

// Player::RemoveItemsFromWorld
// Removes all items from world, reverses any modifiers.

void Player::RemoveItemsFromWorld()
{
	for(int8 i = 0; i < INVENTORY_KEYRING_END; i++)
	{
		if(GetItemInterface()->GetInventoryItem(i))
		{
			if(i < INVENTORY_SLOT_BAG_END)      // only equipment slots get mods.
			{
				_ApplyItemMods(GetItemInterface()->GetInventoryItem(i), i, false);
			}

			GetItemInterface()->GetInventoryItem(i)->RemoveFromWorld();
		
			if(GetItemInterface()->GetInventoryItem(i)->IsContainer() && GetItemInterface()->IsBagSlot(i))
			{
				for(int8 e=0; e < GetItemInterface()->GetInventoryItem(i)->GetProto()->ContainerSlots; e++)

				{
					Item *item = ((Container*)GetItemInterface()->GetInventoryItem(i))->GetItem(e);
					if(item)
					{
						item->RemoveFromWorld();
					}
				}
			}
		}
	}
}

uint32 Player::BuildCreateUpdateBlockForPlayer(ByteBuffer *data, Player *target )
{
    int count = 0;
    if(target == this)
    {
        // we need to send create objects for all items.
        count += GetItemInterface()->m_CreateForPlayer(data);
    }
    count += Unit::BuildCreateUpdateBlockForPlayer(data, target);
    return count;
}

void Player::Kick(uint32 delay /* = 0 */)
{
    if(!delay)
    {
        m_KickDelay = 0;
        _Kick();
    } else {
        m_KickDelay = delay;
        sEventMgr.AddEvent(this, &Player::_Kick, EVENT_PLAYER_KICK, 1000, 0);
    }
}

void Player::_Kick()
{
    if(!m_KickDelay)
    {
        sEventMgr.RemoveEvents(this, EVENT_PLAYER_KICK);
        // remove now
        GetSession()->LogoutPlayer(true);
    } else {
        if((m_KickDelay - 1000) < 500)
        {
            m_KickDelay = 0;
        } else {
            m_KickDelay -= 1000;
        }
        sChatHandler.BlueSystemMessageToPlr(this, "You will be removed from the server in %u seconds.", (uint32)(m_KickDelay/1000));
    }
}

bool Player::HasDeletedSpell(uint32 spell)
{
    return (mDeletedSpells.count(spell) > 0) ? true : false;
}

void Player::ClearCooldownForSpell(uint32 spell_id)
{
    WorldPacket data(12);
    data.SetOpcode(SMSG_CLEAR_COOLDOWN);
    data << spell_id << GetGUID();
    GetSession()->SendPacket(&data);
}

void Player::ClearCooldownsOnLine(uint32 skill_line, uint32 called_from)
{
    // found an easier way.. loop spells, check skill line
    SpellSet::const_iterator itr = mSpells.begin();
    skilllinespell *sk;
    for(; itr != mSpells.end(); ++itr)
    {
        if((*itr) == called_from)       // skip calling spell.. otherwise spammies! :D
            continue;

        sk = objmgr.GetSpellSkill((*itr));
        if(sk && sk->skilline == skill_line)
            ClearCooldownForSpell((*itr));
    }

    /*
    // this _probably_ isn't the most efficient way of doing this.. looping
    // ~5000 times.. but oh well how else can we get skillline from a spell

    int count = sSkillStore.GetNumRows();
    skilllinespell *sp;
    for(int i = 0; i < count; ++i)
    {
        sp = sSkillStore.LookupEntry(i);
        if(sp->skilline == skill_line && HasSpell(sp->spell))
            ClearCooldownForSpell(sp->spell);
    }
    */
}

void Player::EventFieldUpdateExpire()
{
    //gift of life only for now
    //SpellId = 23725
    //ModPFloatValue(UNIT_FIELD_MAXHEALTH,0.15,false);    
}

void Player::PushUpdateData(ByteBuffer *data, uint32 updatecount)
{
    // imagine the bytebuffer getting appended from 2 threads at once! :D
    _bufferS.Acquire();

    mUpdateCount += updatecount;
    bUpdateBuffer.append(*data);

    // add to process queue
    if(m_mapMgr && !bProcessPending)
    {
        bProcessPending = true;
        m_mapMgr->PushToProcessed(this);
    }
    
    _bufferS.Release();
}

void Player::PushOutOfRange(const WoWGuid & guid)
{
    _bufferS.Acquire();
    mOutOfRangeIds << guid;
    ++mOutOfRangeIdCount;

    // add to process queue
    if(m_mapMgr && !bProcessPending)
    {
        bProcessPending = true;
        m_mapMgr->PushToProcessed(this);
    }
    _bufferS.Release();
}

void Player::ProcessPendingUpdates()
{
    _bufferS.Acquire();

    uint32 buffer_size = bUpdateBuffer.size() + 10 + (mOutOfRangeIds.size() * 9);
    uint8 * update_buffer = new uint8[buffer_size];
    uint32 c = 0;

    *(uint32*)&update_buffer[c] = ((mOutOfRangeIds.size() > 0) ? (mUpdateCount + 1) : mUpdateCount);    c += 4;
    update_buffer[c] = 1;                                                                               ++c;

    // append any out of range updates
    if(mOutOfRangeIdCount)
    {
        update_buffer[c]                = UPDATETYPE_OUT_OF_RANGE_OBJECTS;             ++c;
        *(uint32*)&update_buffer[c]     = mOutOfRangeIdCount;                          c += 4;
        memcpy(&update_buffer[c], mOutOfRangeIds.contents(), mOutOfRangeIds.size());   c += mOutOfRangeIds.size();
        mOutOfRangeIds.clear();
        mOutOfRangeIdCount = 0;
    }

	if(bUpdateBuffer.size())
		memcpy(&update_buffer[c], bUpdateBuffer.contents(), bUpdateBuffer.size());  c += bUpdateBuffer.size();

    // clear our update buffer
    bUpdateBuffer.clear();
    bProcessPending = false;
    mUpdateCount = 0;

    _bufferS.Release();

    // compress update packet
    // while we said 350 before, i'm gonna make it 500 :D
    if(c < 1000 || !CompressAndSendUpdateBuffer(c, update_buffer))
    {
        // send uncompressed packet -> because we failed
        m_session->OutPacket(SMSG_UPDATE_OBJECT, c, update_buffer);
    }
    
    delete [] update_buffer;

    // send any delayed packets
    WorldPacket * pck;
    while(delayedPackets.size())
    {
        pck = delayedPackets.next();
        m_session->SendPacket(pck);
        delete pck;
    }
}

bool Player::CompressAndSendUpdateBuffer(uint32 size, const uint8* update_buffer)
{
    uint32 destsize = size + size/10 + 16;

    // set up stream
    z_stream stream;
    stream.zalloc = 0;
    stream.zfree  = 0;
    stream.opaque = 0;
    
    if(deflateInit(&stream, sWorld.getIntRate(INTRATE_COMPRESSION)) != Z_OK)
    {
        sLog.outError("deflateInit failed.");
        return false;
    }

    uint8 *buffer = new uint8[destsize];
    
    // set up stream pointers
    stream.next_out  = (Bytef*)buffer+4;
    stream.avail_out = destsize;
    stream.next_in   = (Bytef*)update_buffer;
    stream.avail_in  = size;

    // call the actual process
    if(deflate(&stream, Z_NO_FLUSH) != Z_OK ||
        stream.avail_in != 0)
    {
        sLog.outError("deflate failed.");
        delete [] buffer;
        return false;
    }

    // finish the deflate
    if(deflate(&stream, Z_FINISH) != Z_STREAM_END)
    {
        sLog.outError("deflate failed: did not end stream");
        delete [] buffer;
        return false;
    }

    // finish up
    if(deflateEnd(&stream) != Z_OK)
    {
        sLog.outError("deflateEnd failed.");
        delete [] buffer;
        return false;
    }

    // fill in the full size of the compressed stream
    *(uint32*)&buffer[0] = size;

    // send it
    m_session->OutPacket(SMSG_COMPRESSED_UPDATE_OBJECT, stream.total_out + 4, buffer);

    // cleanup memory
    delete [] buffer;
    
    return true;
}

void Player::ClearAllPendingUpdates()
{
    _bufferS.Acquire();
    bProcessPending = false;
    mUpdateCount = 0;
    bUpdateBuffer.clear();
    _bufferS.Release();
}

void Player::AddSplinePacket(uint64 guid, ByteBuffer* packet)
{
    //ASSERT(_splineMap.find(guid) == _splineMap.end());
    SplineMap::iterator itr = _splineMap.find(guid);
    if(itr != _splineMap.end())
    {
        delete itr->second;
        _splineMap.erase(itr);
    }
    _splineMap.insert( SplineMap::value_type( guid, packet ) );
}

ByteBuffer* Player::GetAndRemoveSplinePacket(uint64 guid)
{
    SplineMap::iterator itr = _splineMap.find(guid);
    if(itr != _splineMap.end())
    {
        ByteBuffer *buf = itr->second;
        _splineMap.erase(itr);
        return buf;
    }
    return NULL;
}

void Player::ClearSplinePackets()
{
    SplineMap::iterator it2;
    for(SplineMap::iterator itr = _splineMap.begin(); itr != _splineMap.end();)
    {
        it2 = itr;
        ++itr;
        delete it2->second;
        _splineMap.erase(it2);
    }
    _splineMap.clear();
}

uint32 Player::GetAmmoAttackSpeed()
{
    std::list<AmmoAttackSpeed*>::iterator itr;
    uint32 ammoId = GetUInt32Value(PLAYER_AMMO_ID);
    for(itr = m_ammoAttackSpeed.begin(); itr != m_ammoAttackSpeed.end(); itr++)
    {
        if((*itr)->itemId == ammoId)
        {
            return (*itr)->rangedAmmoAttackSpeedMod;
        }
    }
    return 0;
}

bool Player::ExitInstance()
{
    if(!m_bgEntryPointX)
        return false;

    RemoveFromWorld();

    SafeTeleport(m_bgEntryPointMap, m_bgEntryPointInstance, LocationVector(
        m_bgEntryPointX, m_bgEntryPointY, m_bgEntryPointZ, m_bgEntryPointO));

    return true;
}

void Player::SaveEntryPoint()
{
    if(GetInstanceID() > 2 || GetMapId() > 1)
        return; // dont save if we're not on the main continent.
    //otherwise we could end up in an endless loop :P

    m_bgEntryPointX = GetPositionX();
    m_bgEntryPointY = GetPositionY();
    m_bgEntryPointZ = GetPositionZ();
    m_bgEntryPointO = GetOrientation();
    m_bgEntryPointMap = GetMapId();
    m_bgEntryPointInstance = GetInstanceID();
}

void Player::CleanupGossipMenu()
{
    if(CurrentGossipMenu)
    {
        delete CurrentGossipMenu;
        CurrentGossipMenu = NULL;
    }
}

void Player::Gossip_Complete()
{
    GetSession()->OutPacket(SMSG_GOSSIP_COMPLETE, 0, NULL);
    CleanupGossipMenu();
}

void Player::Gossip_SendPOI(float X, float Y, uint32 Icon, uint32 Flags, uint32 Data, const char* Name)
{
    WorldPacket data(SMSG_GOSSIP_POI, 21+strlen(Name));
    data << Flags << X << Y << Icon << Data << Name;
    GetSession()->SendPacket(&data);
}

void Player::ZoneUpdate(uint32 ZoneId)
{
    m_zoneId = ZoneId;

    /*std::map<uint32, AreaTable*>::iterator iter = sWorld.mZoneIDToTable.find(ZoneId);
    if(iter == sWorld.mZoneIDToTable.end())
        return;

    AreaTable *p = iter->second;
    if(p->AreaId != m_AreaID)
    {
        m_AreaID = p->AreaId;
        UpdatePVPStatus(m_AreaID);
    }

    sLog.outDetail("ZONE_UPDATE: Player %s entered zone %s", GetName(), sAreaStore.LookupString((int)p->name));*/
}

void Player::SendTradeUpdate()
{
	ASSERT(mTradeTarget != 0);

	WorldPacket data(SMSG_TRADE_STATUS_EXTENDED, 500);
    data << uint8(1);
    data << uint32(2) << uint32(2);
    data << mTradeGold << uint32(0);
    	
	// Items
	for(uint8 Index = 0; Index < 7; ++Index)
	{
		Item * pItem = mTradeItems[Index];
		if(pItem != 0)
		{
			ItemPrototype * pProto = pItem->GetProto();
			ASSERT(pProto != 0);

            data << uint8(Index);

			data << pProto->ItemId;
			data << pProto->DisplayInfoID;

			data << uint32(0);
			
			data << uint32(0);
			data << uint32(0);
			data << uint32(0);
			data << uint32(0);
			
			data << uint64(0);
			data << uint64(0);

			data << uint32(0);
			data << uint32(0);

			data << pItem->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID);
			data << uint32(0);
            data << pItem->GetUInt32Value(ITEM_FIELD_FLAGS);
            data << pItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY);
            data << pItem->GetUInt32Value(ITEM_FIELD_DURABILITY);
            
		}
	}

	mTradeTarget->GetSession()->SendPacket(&data);
}

void Player::RequestDuel(Player *pTarget)
{
    if (DuelingWith != NULL) return; // We Already Dueling or have Requested a Duel

    SetDuelState(DUEL_STATE_REQUESTED);

    //Setup Duel
    pTarget->DuelingWith = this;
    DuelingWith = pTarget;

    //Get Flags position
    float dist = CalcDistance(pTarget);
    dist = dist/2; //half way
    float x = (GetPositionX() + pTarget->GetPositionX()*dist)/(1+dist) + cos(GetOrientation()+(M_PI/2))*2;
    float y = (GetPositionY() + pTarget->GetPositionY()*dist)/(1+dist) + sin(GetOrientation()+(M_PI/2))*2;
    float z = (GetPositionZ() + pTarget->GetPositionZ()*dist)/(1+dist);

    //Create flag/arbiter
    GameObject* pGameObj = sObjHolder.Create<GameObject>();
    pGameObj->Create(GetMapId(), x, y, z, GetOrientation());
    pGameObj->SetInstanceID(GetInstanceID());

    //Spawn the Flag
    pGameObj->SetUInt32Value(OBJECT_FIELD_TYPE,33);
    pGameObj->SetUInt32Value(OBJECT_FIELD_ENTRY,21680);
    pGameObj->SetFloatValue(OBJECT_FIELD_SCALE_X,1);
    pGameObj->SetUInt64Value(OBJECT_FIELD_CREATED_BY, GetGUID());
    pGameObj->SetUInt32Value(GAMEOBJECT_DISPLAYID,787);
    pGameObj->SetFloatValue(GAMEOBJECT_POS_X, x);
    pGameObj->SetFloatValue(GAMEOBJECT_POS_Y, y);
    pGameObj->SetFloatValue(GAMEOBJECT_POS_Z, z);
    pGameObj->SetFloatValue(GAMEOBJECT_FACING, GetOrientation());
    pGameObj->SetUInt32Value(GAMEOBJECT_STATE,1);
    pGameObj->SetUInt32Value(GAMEOBJECT_FACTION, GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
    pGameObj->SetUInt32Value(GAMEOBJECT_TYPE_ID, 16);
    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, GetUInt32Value(UNIT_FIELD_LEVEL));

    //Assign the Flag 
    SetUInt64Value(PLAYER_DUEL_ARBITER,pGameObj->GetGUID());
    pTarget->SetUInt64Value(PLAYER_DUEL_ARBITER,pGameObj->GetGUID());

    pGameObj->AddToWorld();

    WorldPacket data(SMSG_DUEL_REQUESTED, 16);
    data << pGameObj->GetGUID();
    data << GetGUID();
    pTarget->GetSession()->SendPacket(&data);

    m_duelCountdownTimer = 3000;
}

void Player::DuelCountdown()
{
    m_duelCountdownTimer -= 1000;
    if(DuelingWith == 0)
        return;

    if(m_duelCountdownTimer == 0)
    {
        // Start Duel.
        SetUInt32Value(UNIT_FIELD_POWER2, 0);
        DuelingWith->SetUInt32Value(UNIT_FIELD_POWER2, 0);

        //Give the players a Team
        DuelingWith->SetUInt32Value(PLAYER_DUEL_TEAM, 1); // Duel Requester
        SetUInt32Value(PLAYER_DUEL_TEAM, 2);

        SetDuelState(DUEL_STATE_STARTED);
        DuelingWith->SetDuelState(DUEL_STATE_STARTED);
        
        sEventMgr.AddEvent(this, &Player::DuelBoundaryTest, EVENT_PLAYER_DUEL_BOUNDARY_CHECK, 500, 0);
        sEventMgr.AddEvent(DuelingWith, &Player::DuelBoundaryTest, EVENT_PLAYER_DUEL_BOUNDARY_CHECK, 500, 0);
    }
}

void Player::DuelBoundaryTest()
{
    //check if in bounds
    GameObject * pGameObject = sObjHolder.GetObject<GameObject>(GetUInt64Value(PLAYER_DUEL_ARBITER));
    if(!pGameObject)
    {
        EndDuel(DUEL_WINNER_RETREAT);
        return;
    }

    float Dist = CalcDistance((Object*)pGameObject);

    if(Dist > 75.0f)
    {
        // Out of bounds
        if(m_duelStatus == DUEL_STATUS_OUTOFBOUNDS)
        {
            // we already know, decrease timer by 500
            m_duelCountdownTimer -= 500;
            if(m_duelCountdownTimer == 0)
            {
                // Times up :p
                DuelingWith->EndDuel(DUEL_WINNER_RETREAT);
            }
        }
        else
        {
            // we just went out of bounds
            // set timer
            m_duelCountdownTimer = 10000;
            
            // let us know
            m_session->OutPacket(SMSG_DUEL_OUTOFBOUNDS, 4, &m_duelCountdownTimer);
            m_duelStatus = DUEL_STATUS_OUTOFBOUNDS;
        }
    }
    else
    {
        // we're in range
        if(m_duelStatus == DUEL_STATUS_OUTOFBOUNDS)
        {
            // just came back in range
            m_session->OutPacket(SMSG_DUEL_INBOUNDS);
            m_duelStatus = DUEL_STATUS_INBOUNDS;
        }
    }
}

void Player::EndDuel(uint8 WinCondition)
{
    if(m_duelState == DUEL_STATE_FINISHED)
        return;

    // Remove the events
    sEventMgr.RemoveEvents(this, EVENT_PLAYER_DUEL_COUNTDOWN);
    sEventMgr.RemoveEvents(this, EVENT_PLAYER_DUEL_BOUNDARY_CHECK);

    m_duelState = DUEL_STATE_FINISHED;
    if(DuelingWith == 0) return;

    sEventMgr.RemoveEvents(DuelingWith, EVENT_PLAYER_DUEL_BOUNDARY_CHECK);
    sEventMgr.RemoveEvents(DuelingWith, EVENT_PLAYER_DUEL_COUNTDOWN);
    DuelingWith->m_duelState = DUEL_STATE_FINISHED;

    //Announce Winner
    WorldPacket data(SMSG_DUEL_WINNER, 500);
    data << uint8(WinCondition);
    data << GetName() << DuelingWith->GetName();

    if(WinCondition == DUEL_WINNER_KNOCKOUT)
    {
        DuelingWith->Emote(EMOTE_ONESHOT_BEG);
        DuelingWith->Root();
        sEventMgr.AddEvent<Player>(DuelingWith, &Unit::Unroot, EVENT_UNIT_UNROOT, 3000, 1); 
    }

    SendMessageToSet(&data, true);

    //get Arbiter
    GameObject *arbiter = World::GetGameObject(GetUInt64Value(PLAYER_DUEL_ARBITER));
    
    //Clear Duel Related Stuff
    SetUInt64Value(PLAYER_DUEL_ARBITER, 0);
    DuelingWith->SetUInt64Value(PLAYER_DUEL_ARBITER, 0);
    SetUInt32Value(PLAYER_DUEL_TEAM, 0);
    DuelingWith->SetUInt32Value(PLAYER_DUEL_TEAM, 0);
    
    if(arbiter)
    {
        //Despawn Arbiter
        arbiter->Despawn(10000);
        sObjHolder.Delete(arbiter);
    }

    EventAttackStop();
    DuelingWith->EventAttackStop();

    // removing auras that kills players after if low HP
    RemoveNegativeAuras();
    DuelingWith->RemoveNegativeAuras();

    //Stop Players attacking so they don't kill the other player
    m_session->OutPacket(SMSG_CANCEL_COMBAT);
    DuelingWith->m_session->OutPacket(SMSG_CANCEL_COMBAT);
    
    if(getAttackTarget() == DuelingWith->GetGUID())
        setAttackTarget(NULL);

    if(DuelingWith->getAttackTarget() == GetGUID())
        DuelingWith->setAttackTarget(NULL);

    smsg_AttackStop(DuelingWith);
    DuelingWith->smsg_AttackStop(this);

    DuelingWith->DuelingWith = NULL;
    DuelingWith = NULL;
}

void Player::StopMirrorTimer(uint32 Type)
{
    m_session->OutPacket(SMSG_STOP_MIRROR_TIMER, 4, &Type);
}

void Player::EventTeleport(uint32 mapid, float x, float y, float z)
{
    //Relocate(mapid, x, y, z, m_orientation, true, false);
    SafeTeleport(mapid, 0, LocationVector(x, y, z));
}

void Player::ApplyLevelInfo(LevelInfo* Info, uint32 Level)
{
    // Apply level
    SetUInt32Value(UNIT_FIELD_LEVEL, Level);
    SetBaseUInt32Value(UNIT_FIELD_LEVEL, Level);

    // Set next level conditions
    SetUInt32Value(PLAYER_NEXT_LEVEL_XP, Info->XPToNextLevel);
    SetBaseUInt32Value(PLAYER_NEXT_LEVEL_XP, Info->XPToNextLevel);

    // Set stats
    for(uint32 i = 0; i < 5; ++i)
    {
        SetBaseUInt32Value(UNIT_FIELD_STAT0+i, Info->Stat[i]);
        BaseStats[i] = Info->Stat[i];
        CalcStat(i);
    }

    // Set health / mana
    SetUInt32Value(UNIT_FIELD_HEALTH, Info->HP);
    SetBaseUInt32Value(UNIT_FIELD_HEALTH, Info->HP);
    
    SetUInt32Value(UNIT_FIELD_MAXHEALTH, Info->HP);
    SetBaseUInt32Value(UNIT_FIELD_MAXHEALTH, Info->HP);
    
    SetUInt32Value(UNIT_FIELD_POWER1, Info->HP);
    SetBaseUInt32Value(UNIT_FIELD_POWER1, Info->HP);
    
    SetUInt32Value(UNIT_FIELD_MAXPOWER1, Info->Mana);
    SetBaseUInt32Value(UNIT_FIELD_MAXPOWER1, Info->Mana);

    // Calculate talentpoints
    uint32 TalentPoints = 0;
    if(Level >= 10)
        TalentPoints = Level - 9;

    SetUInt32Value(PLAYER_CHARACTER_POINTS1, TalentPoints);
    SetBaseUInt32Value(PLAYER_CHARACTER_POINTS1, TalentPoints);

    // Set base fields
    SetBaseUInt32Value(UNIT_FIELD_BASE_HEALTH, Info->HP);
    SetUInt32Value(UNIT_FIELD_BASE_HEALTH, Info->HP);

    SetBaseUInt32Value(UNIT_FIELD_BASE_MANA, Info->Mana);
    SetUInt32Value(UNIT_FIELD_BASE_MANA, Info->Mana);

    UpdateMaxSkills();
    UpdateStats();
    UpdateChances();	

    sLog.outDetail("Player %s set parameters to level %u", GetName(), Level);
}

void Player::BroadcastMessage(const char* Format, ...)
{
    va_list l;
    va_start(l, Format);
    char Message[1024];
    vsprintf(Message, Format, l);
    va_end(l);

    WorldPacket data(SMSG_BROADCAST_MSG, strlen(Message) + 5);
    data << uint32(3) << Message;
    m_session->SendPacket(&data);
}

//-------------------------------------------
//Combat single rating calculation
//-------------------------------------------
void Player::CalcRating(uint32 t)
{
	uint32 uPct = 0;
	float fPct = 0.0f;

	switch(t)
	{
	case DEFENSE_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    uPct = GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_01)/PCT_DEFENSE_RATING*((-3.0f/82.0f)*getLevel()+(131.0f/41.0f));
			}
			else if(getLevel() >= 60)
			{
                uPct = GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_01)/PCT_DEFENSE_RATING/((1.0f/52.0f)*getLevel()-(8.0f/52.0f));
			}
			mRatingToPoint[DEFENSE_RATING] = uPct;

		}break;
	case DODGE_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_02))/float(PCT_DODGE_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_02))/float(PCT_DODGE_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[DODGE_RATING] = fPct;
			
		}break;
	case PARRY_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_03))/float(PCT_PARRY_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_03))/float(PCT_PARRY_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[PARRY_RATING] = fPct;

		}break;
	case SHIELD_BLOCK_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_04))/float(PCT_SHIELD_BLOCK_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_04))/float(PCT_SHIELD_BLOCK_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[SHIELD_BLOCK_RATING] = fPct;

		}break;
	case MELEE_HIT_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_05))/float(PCT_MELEE_HIT_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_05))/float(PCT_MELEE_HIT_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[MELEE_HIT_RATING] = fPct;
		
		}break;
	case RANGED_HIT_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_06))/float(PCT_RANGED_HIT_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_06))/float(PCT_RANGED_HIT_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[RANGED_HIT_RATING] = fPct;

		}break;
	case SPELL_HIT_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_07))/float(PCT_SPELL_HIT_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_07))/float(PCT_SPELL_HIT_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[SPELL_HIT_RATING] = fPct;

		}break;
	case MELEE_CRITICAL_STRIKE_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_08))/float(PCT_MELEE_CRITICAL_STRIKE_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_08))/float(PCT_MELEE_CRITICAL_STRIKE_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[MELEE_CRITICAL_STRIKE_RATING] = fPct;

		}break;
	case RANGED_CRITICAL_STRIKE_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_09))/float(PCT_RANGED_CRITICAL_STRIKE_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_09))/float(PCT_RANGED_CRITICAL_STRIKE_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}

			mRatingToPct[RANGED_CRITICAL_STRIKE_RATING] = fPct;

		}break;
	case SPELL_CRITICAL_STRIKE_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_10))/float(PCT_SPELL_CRITICAL_STRIKE_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_10))/float(PCT_SPELL_CRITICAL_STRIKE_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[SPELL_CRITICAL_STRIKE_RATING] = fPct;

		}break;
	case MELEE_HIT_AVOIDANCE_RATING:
		{
			mRatingToPct[MELEE_HIT_AVOIDANCE_RATING] = fPct;

		}break;
	case RANGED_HIT_AVOIDANCE_RATING:
		{
			mRatingToPct[RANGED_HIT_AVOIDANCE_RATING] = fPct;
		}break;
	case SPELL_HIT_AVOIDANCE_RATING:
		{
			mRatingToPct[SPELL_HIT_AVOIDANCE_RATING] = fPct;
		}break;
	case MELEE_CRITICAL_AVOIDANCE_RATING:
		{
			mRatingToPct[MELEE_CRITICAL_AVOIDANCE_RATING] = fPct;
		}break;
	case RANGED_CRITICAL_AVOIDANCE_RATING:
		{
			mRatingToPct[RANGED_CRITICAL_AVOIDANCE_RATING] = fPct;
		}break;
	case SPELL_CRITICAL_AVOIDANCE_RATING:
		{
			mRatingToPct[SPELL_CRITICAL_AVOIDANCE_RATING] = fPct;
		}break;
	case MELEE_HASTE_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_17))/float(PCT_MELEE_HASTE_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_17))/float(PCT_MELEE_HASTE_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[MELEE_HASTE_RATING] = fPct;

		}break;
	case RANGED_HASTE_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_18))/float(PCT_RANGED_HASTE_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_18))/float(PCT_RANGED_HASTE_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[RANGED_HASTE_RATING] = fPct;

		}break;
	case SPELL_HASTE_RATING:
		{

		}break;
	case HIT_RATING:
		{

		}break;
	case CRITICAL_STRIKE_RATING:
		{

		}break;
	case HIT_AVOIDANCE_RATING:
		{

		}break;
	case CRITICAL_AVOIDANCE_RATING:
		{

		}break;
	case RESILIENCE_RATING:
		{
			if(getLevel() >= 1 && getLevel() < 60)
			{
			    fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_14))/float(PCT_RESILIENCE_RATING)*((-3.0f/82.0f)*float(getLevel())+(131.0f/41.0f)));
			}
			else if(getLevel() >= 60)
			{
                fPct = ceil(float(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_14))/float(PCT_RESILIENCE_RATING)/((1.0f/52.0f)*float(getLevel())-(8.0f/52.0f)));
			}
			mRatingToPct[RESILIENCE_RATING] = fPct;

		}break;
	case HASTE_RATING:
		{

		}break;
	default:
		{
		}break;
	}
}

void Player::RecalcAllRatings()
{
	for(uint32 i=12; i < 37; i++)
	{
		CalcRating(i);
	}
}

void Player::RemoveLanguages()
{
    for(uint32 i=PLAYER_SKILL_INFO_1_1;i<PLAYER_SKILL_INFO_1_1+3*127;i+=3)
    {
        uint16 sk = GetUInt32Value(i);
        switch(sk)
        {
            case SKILL_LANG_COMMON:
            case SKILL_LANG_ORCISH:
            case SKILL_LANG_DWARVEN:
            case SKILL_LANG_DARNASSIAN:
            case SKILL_LANG_TAURAHE:
            case SKILL_LANG_THALASSIAN:
            case SKILL_LANG_DRACONIC:
            case SKILL_LANG_DEMON_TONGUE:
            case SKILL_LANG_TITAN:
            case SKILL_LANG_OLD_TONGUE:
            case SKILL_LANG_GNOMISH:
            case SKILL_LANG_TROLL:
            case SKILL_LANG_GUTTERSPEAK:
            case SKILL_LANG_DRAENEI:
            {
                SetBaseUInt64Value(i,0);
                SetUInt64Value(i,0);
                SetBaseUInt32Value(i+2,0);
                SetUInt32Value(i+2,0);
            }break;
        }
    }
}

void Player::AddLanguages()
{
    addSpell(668);
    addSpell(669);
    addSpell(670);
    addSpell(671);
    addSpell(672);
    addSpell(813);
    addSpell(814);
    addSpell(815);
    addSpell(816);
    addSpell(817);
    addSpell(7340);
    addSpell(7341);
    addSpell(17737);
    addSpell(29932);

    AddSkillLine(SKILL_LANG_COMMON, 300, 300);
    AddSkillLine(SKILL_LANG_ORCISH, 300, 300);
    AddSkillLine(SKILL_LANG_DWARVEN, 300, 300);
    AddSkillLine(SKILL_LANG_DARNASSIAN, 300, 300);
    AddSkillLine(SKILL_LANG_TAURAHE, 300, 300);
    AddSkillLine(SKILL_LANG_THALASSIAN, 300, 300);
    AddSkillLine(SKILL_LANG_DRACONIC, 300, 300);
    AddSkillLine(SKILL_LANG_DEMON_TONGUE, 300, 300);
    AddSkillLine(SKILL_LANG_TITAN, 300, 300);
    AddSkillLine(SKILL_LANG_OLD_TONGUE, 300, 300);
    AddSkillLine(SKILL_LANG_GNOMISH, 300, 300);
    AddSkillLine(SKILL_LANG_TROLL, 300, 300);
    AddSkillLine(SKILL_LANG_GUTTERSPEAK, 300, 300);
    AddSkillLine(SKILL_LANG_DRAENEI, 300, 300);
}

bool Player::SafeTeleport(uint32 MapID, uint32 InstanceID, float X, float Y, float Z, float O)
{
    return SafeTeleport(MapID, InstanceID, LocationVector(X, Y, Z, O));
}

bool Player::SafeTeleport(uint32 MapID, uint32 InstanceID, const LocationVector & vec)
{
    bool instance = false;
    if(InstanceID && m_instanceId != InstanceID)
        instance = true;
    else if(m_mapId != MapID)
        instance = true;

    // Lookup map info
    MapInfo * mi = sWorld.GetMapInformation(MapID);
    if(mi && mi->flags & WMI_INSTANCE_XPACK_01 && !m_session->HasFlag(ACCOUNT_FLAG_XPACK_01))
    {
        WorldPacket msg(SMSG_BROADCAST_MSG, 50);
        msg << uint32(3) << "You must have The Burning Crusade Expansion to access this content." << uint8(0);
        m_session->SendPacket(&msg);
        return false;
    }

    _Relocate(MapID, vec, true, instance);
    return true;
}

void Player::SetGuildId(uint32 guildId)
{
    myGuild = guildId ? objmgr.GetGuild(guildId) : 0;
    SetUInt32Value(PLAYER_GUILDID,guildId);
}

void Player::UpdatePvPArea()
{
    PvPArea * oldArea = currentPVPArea;

    currentPVPArea = ObjectMgr::getSingleton().GetPvPArea(m_AreaID);
    if(currentPVPArea == oldArea ||
        currentPVPArea == 0 || !currentPVPArea->AreaId)
    {
        // Someone is too lazy to extract adt, or they haven't filled in the info in the table.
        return;
    }

    // This is where all the magic happens :P
    if(currentPVPArea->HomeFor(this))
    {
        if(IsPvPFlagged())
        {
            // I'm flagged and I just walked into a zone of my type. Start the 5min counter.
            ResetPvPTimer();
        }
    }
    else
    {
        // I just walked into either an enemies town, or a contested zone.
        // Force flag me if i'm not already.
        if(!IsPvPFlagged()) SetPvPFlag();

        // Check for FFA PvP
        if(currentPVPArea->PvPType == AREA_PVPARENA)
        {
            if(!HasFlag(PLAYER_FLAGS, PLAYER_FLAG_FREE_FOR_ALL_PVP))
                SetFlag(PLAYER_FLAGS, PLAYER_FLAG_FREE_FOR_ALL_PVP);
        }
        else
        {
            if(HasFlag(PLAYER_FLAGS, PLAYER_FLAG_FREE_FOR_ALL_PVP))
                RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_FREE_FOR_ALL_PVP);
        }
    }
}

void Player::LoginPvPSetup()
{
    // Make sure we know our area ID.
    _EventExploration();

    // Lazy bastards..
    if(currentPVPArea == 0)
        return;

    // If we're not in the home area for our faction, add a honorless target
    // buff to avoid ganking.
    if(!currentPVPArea->HomeFor(this))
        CastSpell(this, PLAYER_HONORLESS_TARGET_SPELL, true);
}

void Player::PvPToggle()
{
    if(m_pvpTimer > 0)
    {
        // Means that we typed /pvp while we were "cooling down". Stop the timer.
        StopPvPTimer();

        if(!HasFlag(PLAYER_FLAGS, PLAYER_FLAG_PVP_TOGGLE))
            SetFlag(PLAYER_FLAGS, PLAYER_FLAG_PVP_TOGGLE);
    }
    else
    {
        uint32 AreaType = AREA_CONTESTED;
        if(currentPVPArea)
            AreaType = currentPVPArea->PvPType;

        // Can't modify flags in a contested area.
        if(AreaType == AREA_CONTESTED)
            return;

        if(IsPvPFlagged())
        {
            // Start the "cooldown" timer.
            ResetPvPTimer();

            if(HasFlag(PLAYER_FLAGS, PLAYER_FLAG_PVP_TOGGLE))
                RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_PVP_TOGGLE);
        }
        else
        {
            // Move into PvP state.
            if(!HasFlag(PLAYER_FLAGS, PLAYER_FLAG_PVP_TOGGLE))
                SetFlag(PLAYER_FLAGS, PLAYER_FLAG_PVP_TOGGLE);

            StopPvPTimer();
            SetPvPFlag();
        }        
    }
}

bool PvPArea::HomeFor(Player* plr)
{
    if(plr->GetTeam() == 0 && PvPType == AREA_ALLIANCE ||
        plr->GetTeam() == 1 && PvPType == AREA_HORDE)
    {
        return true;
    }
    return false;
}

bool Player::CanCastDueToCooldown(SpellEntry * spellid)
{
    map<uint32, uint32>::iterator itr;
    uint32 mstime = getMSTime();
    
    // no point checking single spells unless they have
    // a cooldown
    if(spellid->RecoveryTime)
    {
        itr = SpellCooldownMap.find(spellid->Id);
        if(itr != SpellCooldownMap.end())
        {
            if(mstime < itr->second)
                return false;
        }
    }

    if(spellid->Category)
    {
        itr = SpellCooldownCategoryMap.find(spellid->Category);
        if(itr != SpellCooldownCategoryMap.end())
        {
            if(mstime < itr->second)
                return false;
        }
    }

    if(mstime < GlobalCooldown)
        return false;

    return true;
}

void Player::ResetPvPTimer()
{
     m_pvpTimer = sWorld.getIntRate(INTRATE_PVPTIMER);
}



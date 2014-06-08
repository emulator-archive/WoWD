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

#ifndef __EVENTMGR_H
#define __EVENTMGR_H

enum EventTypes
{
    EVENT_UNK = 0,
    EVENT_MAPMGR_UPDATEOBJECTS,
    EVENT_MAPCELL_UNLOAD,
    EVENT_WORLD_UPDATEAUCTIONS,
    EVENT_WORLD_UPDATESESSIONS,
    EVENT_CREATURE_UPDATE,
    EVENT_PLAYER_UPDATE,
    EVENT_PLAYER_UPDATEEXPLORATION,
    EVENT_GAMEOBJECT_UPDATE,
    EVENT_DYNAMICOBJECT_UPDATE,
    EVENT_GAMEOBJECT_ENV_DAMAGE,
    EVENT_PLAYER_STOPPVP,
    EVENT_CREATURE_JUSTDIED,
    EVENT_CREATURE_REMOVE_CORPSE,
    EVENT_CREATURE_RESPAWN,
    EVENT_MAPMGR_UNLOAD_CELL,
    EVENT_PLAYER_REST,
    EVENT_PLAYER_SEND_PACKET,
    EVENT_UNIT_EMOTE,
    EVENT_WORLD_UPDATEQUEUES,
    EVENT_PET_UPDATE,
    EVENT_PLAYER_REDUCEDRUNK,
    EVENT_WEATHER_UPDATE,
    EVENT_TOTEM_EXPIRE,
    EVENT_TOTEM_SEEK_TARGET,
    EVENT_TOTEM_SEEK_TARGET_AREA_AURA,
    EVENT_ENSLAVE_EXPIRE,
    EVENT_PLAYER_TAXI_DISMOUNT,
    EVENT_GAMEOBJECT_DOOR_CLOSE,
    EVENT_PLAYER_ALLOW_TRIGGERPORT,
    EVENT_SPELL_DAMAGE_HIT,
    EVENT_PET_SPELL_COOLDOWN,
    EVENT_GAMEOBJECT_UNHOOK,
    EVENT_GAMEOBJECT_FISH_HOOKED,
    EVENT_GAMEOBJECT_FISH_NOT_HOOKED,
    EVENT_GAMEOBJECT_END_FISHING,
    EVENT_BATTLEGROUND_REMOVE,
    EVENT_BATTLEGROUND_ALMOST_START,
    EVENT_BATTLEGROUND_START,
    EVENT_BATTLEGROUND_REVIVE_PLAYERS,
    EVENT_BATTLEGROUND_STABLE_RCONTEST,
    EVENT_BATTLEGROUND_FARM_RCONTEST,
    EVENT_BATTLEGROUND_BlACKSMITH_RCONTEST,
    EVENT_BATTLEGROUND_MINE_RCONTEST,
    EVENT_BATTLEGROUND_LUMBERMILL_RCONTEST,
    EVENT_BATTLEGROUND_UPDATE_RESOURCES_ALLIANCE,
    EVENT_BATTLEGROUND_UPDATE_RESOURCES_HORDE,
    EVENT_BATTLEGROUND_SPAWN_BUFF1,
    EVENT_BATTLEGROUND_SPAWN_BUFF2,
    EVENT_BATTLEGROUND_SPAWN_BUFF3,
    EVENT_BATTLEGROUND_SPAWN_BUFF4,
    EVENT_BATTLEGROUND_SPAWN_BUFF5,
    EVENT_BATTLEGROUND_SPAWN_BUFF6,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE1,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE2,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE3,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE4,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE5,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE6,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE7,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE8,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE9,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE10,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE11,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE12,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE13,
    EVENT_BATTLEGROUND_AV_CAPTURE_BASE14,
    EVENT_PLAYER_REPEAT_SHOT,
    EVENT_PLAYER_ACTIVATE_GAMEOBJECT,
    EVENT_UNIT_REPEAT_MSG,
    EVENT_UNIT_CHAT_MSG,
    EVENT_PLAYER_MOTD,
    EVENT_TIMED_QUEST_EXPIRE,
    EVENT_UNIT_MANUAL_REPEAT_MSG,
    EVENT_PLAYER_TAXI_POSITION_UPDATE,
    EVENT_PLAYER_TAXI_INTERPOLATE,
    EVENT_PLAYER_CHECKFORCHEATS,
    EVENT_AREAAURA_UPDATE,
    EVENT_AURA_REMOVE,
    EVENT_AURA_PERIODIC_DAMAGE,
    EVENT_AURA_PERIODIC_DAMAGE_PERCENT,
    EVENT_AURA_PERIODIC_HEAL,
    EVENT_AURA_PERIODIC_TRIGGERSPELL,
    EVENT_AURA_PERIODIC_ENERGIZE,
    EVENT_AURA_PERIODIC_LEECH,
    EVENT_AURA_PERIOCIC_MANA,
    EVENT_AURA_PERIODIC_HEALTH_FUNNEL,
    EVENT_AURA_MANA_FUNNEL,
    EVENT_CANNIBALIZE,
    EVENT_GROUP_POSITION_UPDATE,
    EVNET_RAID_POSITION_UPDATE,
    EVENT_DELETE_TIMER,
    EVENT_UNIT_SENDMOVE,
    EVENT_GAMEOBJECT_ITEM_SPAWN,
    EVENT_GAMEOBJECT_ITEM_DESPAWN,
    EVENT_GAMEOBJECT_EXPIRE,
    EVENT_CREATURE_SAFE_DELETE,
    EVENT_PET_DELAYED_REMOVE,
    EVENT_REMOVE_ENCHANTMENT,
    EVENT_REMOVE_ENCHANTMENT1,
    EVENT_REMOVE_ENCHANTMENT2,
    EVENT_REMOVE_ENCHANTMENT3,
    EVENT_REMOVE_ENCHANTMENT4,
    EVENT_REMOVE_ENCHANTMENT5,
    EVENT_REMOVE_ENCHANTMENT6,
    EVENT_REMOVE_ENCHANTMENT7,
    EVENT_REMOVE_ENCHANTMENT8,
    EVENT_REMOVE_ENCHANTMENT9,
    EVENT_REMOVE_ENCHANTMENT10,
    EVENT_REMOVE_ENCHANTMENT11,
    EVENT_REMOVE_ENCHANTMENT12,
    EVENT_REMOVE_ENCHANTMENT13,
    EVENT_REMOVE_ENCHANTMENT14,
    EVENT_REMOVE_ENCHANTMENT15,
    EVENT_REMOVE_ENCHANTMENT16,
    EVENT_REMOVE_ENCHANTMENT17,
    EVENT_REMOVE_ENCHANTMENT18,
    EVENT_REMOVE_ENCHANTMENT19,
    EVENT_REMOVE_ENCHANTMENT20,
    EVENT_REMOVE_ENCHANTMENT21,
    EVENT_PLAYER_CHARM_ATTACK,
    EVENT_PLAYER_KICK,
    EVENT_TRANSPORTER_NEXT_WAYPOINT,
    EVENT_LOOT_ROLL_FINALIZE,
    EVENT_FIELD_UPDATE_EXPIRE,
    EVENT_SUMMON_PET_EXPIRE,
    EVENT_CORPSE_DESPAWN,
    EVENT_SCRIPT_UPDATE_EVENT,
    EVENT_INSTANCE_SOFT_RESET,
    EVENT_INSTANCE_HARD_RESET,
    EVENT_INSTANCE_LIMIT_COUNTER,
    EVENT_MAP_SPAWN_EVENTS,
    EVENT_PLAYER_DUEL_COUNTDOWN,
    EVENT_PLAYER_DUEL_BOUNDARY_CHECK,
    EVENT_GAMEOBJECT_TRAP_SEARCH_TARGET,
    EVENT_PLAYER_TELEPORT,
    EVENT_UNIT_DIMINISHING_RETURN,
    EVENT_UNIT_UNROOT,
    EVENT_MAILSYSTEM_RELOAD,
    EVENT_CREATURE_FORMATION_LINKUP,
};

struct TimedEvent
{
    TimedEvent(void* object, CallbackBase* callback, uint32 flags, time_t time, uint32 repeat) : 
        obj(object), cb(callback), eventFlags(flags), msTime(time), currTime(time), repeats(repeat), deleted(false) {}
        
    void *obj;
    CallbackBase *cb;
    uint32 eventFlags;
    time_t msTime;
    time_t currTime;
    uint32 repeats;
    bool deleted;
};

class EventMgr;
class EventableObjectHolder;
typedef map<int32, EventableObjectHolder*> HolderMap;

class WOWD_SERVER_DECL EventMgr : public Singleton < EventMgr >
{
    friend class MiniEventMgr;
public:
    template <class Class>
        void AddEvent(Class *obj, void (Class::*method)(), uint32 flags, uint32 time, uint32 repeats)
    {
        // create a timed event
        TimedEvent * event = new TimedEvent(obj, new CallbackP0<Class>(obj, method), flags, time, repeats);

        // add this to the object's list, updating will all be done later on...
        obj->event_AddEvent(event);
    }

    template <class Class, typename P1>
        void AddEvent(Class *obj, void (Class::*method)(P1), P1 p1, uint32 flags, uint32 time, uint32 repeats)
    {
        // create a timed event
        TimedEvent * event = new TimedEvent(obj, new CallbackP1<Class, P1>(obj, method, p1), flags, time, repeats);

        // add this to the object's list, updating will all be done later on...
        obj->event_AddEvent(event);
    }

    template <class Class, typename P1, typename P2>
        void AddEvent(Class *obj, void (Class::*method)(P1,P2), P1 p1, P2 p2, uint32 flags, uint32 time, uint32 repeats)
    {
        // create a timed event
        TimedEvent * event = new TimedEvent(obj, new CallbackP2<Class, P1, P2>(obj, method, p1, p2), flags, time, repeats);

        // add this to the object's list, updating will all be done later on...
        obj->event_AddEvent(event);
    }

    template <class Class, typename P1, typename P2, typename P3>
        void AddEvent(Class *obj,void (Class::*method)(P1,P2,P3), P1 p1, P2 p2, P3 p3, uint32 flags, uint32 time, uint32 repeats)
    {
        // create a timed event
        TimedEvent * event = new TimedEvent(obj, new CallbackP3<Class, P1, P2, P3>(obj, method, p1, p2, p3), flags, time, repeats);

        // add this to the object's list, updating will all be done later on...
        obj->event_AddEvent(event);
    }

    template <class Class, typename P1, typename P2, typename P3, typename P4>
        void AddEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4), P1 p1, P2 p2, P3 p3, P4 p4, uint32 flags, uint32 time, uint32 repeats)
    {
        // create a timed event
        TimedEvent * event = new TimedEvent(obj, new CallbackP4<Class, P1, P2, P3, P4>(obj, method, p1, p2, p3, p4), flags, time, repeats);

        // add this to the object's list, updating will all be done later on...
        obj->event_AddEvent(event);
    }

    template <class Class> inline void RemoveEvents(Class *obj) { RemoveEvents(obj, -1); }
    template <class Class> void RemoveEvents(Class *obj, int32 type)
    {
        obj->event_RemoveEvents(type);
    }

    template <class Class> void ModifyEventTimeLeft(Class *obj, uint32 type, uint32 time)
    {
        obj->event_ModifyTimeLeft(type, time);
    }

    template <class Class> void ModifyEventTime(Class *obj, uint32 type, uint32 time)
    {
        obj->event_ModifyTime(type, time);
    }

    template <class Class> bool HasEvent(Class *obj, uint32 type)
    {
        return obj->event_HasEvent(type);
    }

    EventableObjectHolder * GetEventHolder(int32 InstanceId)
    {
        HolderMap::iterator itr = mHolders.find(InstanceId);
        if(itr == mHolders.end()) return 0;
        return itr->second;
    }

    inline void AddEventHolder(EventableObjectHolder * holder, int32 InstanceId)
    {
        holderLock.Acquire();
        mHolders.insert( HolderMap::value_type( InstanceId, holder) );
        holderLock.Release();
    }

    inline void RemoveEventHolder(int32 InstanceId)
    {
        holderLock.Acquire();
        mHolders.erase(InstanceId);
        holderLock.Release();
    }

    inline void RemoveEventHolder(EventableObjectHolder * holder)
    {
        holderLock.Acquire();
        HolderMap::iterator itr = mHolders.begin();
        for(; itr != mHolders.end(); ++itr)
        {
            if(itr->second == holder)
            {
                mHolders.erase(itr);
                holderLock.Release();
                return;
            }
        }
        holderLock.Release();
    }

protected:
    HolderMap mHolders;
    Mutex holderLock;
};

#define sEventMgr EventMgr::getSingleton()

#endif

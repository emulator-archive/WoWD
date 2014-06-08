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

#ifndef WOWSERVER_AIINTERFACE_H
#define WOWSERVER_AIINTERFACE_H

#define M_PI       3.14159265358979323846
#define UNIT_MOVEMENT_INTERPOLATE_INTERVAL 400/*750*/ // ms smoother server/client side moving vs less cpu/ less b/w
#define TARGET_UPDATE_INTERVAL 500 // ms
#define oocr 50.0f // out of combat range

class Object;
class Creature;
class Unit;
class Player;
class WorldSession;
class SpellCastTargets;


enum AIType
{
    AITYPE_LONER,
    AITYPE_AGRO,
    AITYPE_SOCIAL,
    AITYPE_PET,
    AITYPE_TOTEM,
};

enum MovementType
{
    MOVEMENTTYPE_NONE,
    MOVEMENTTYPE_RANDOMWP,
    MOVEMENTTYPE_CIRCLEWP,
    MOVEMENTTYPE_WANTEDWP,
    MOVEMENTTYPE_DONTMOVEWP,
    MOVEMENTTYPE_QUEST = 10,
};


/*struct AI_Target
{
    Unit* target;
    int32 threat;
};*/


enum AI_Agent
{
    AGENT_NULL,
    AGENT_MELEE,
    AGENT_RANGED,
    AGENT_FLEE,
    AGENT_SPELL,
    AGENT_CALLFORHELP
};

enum AI_SpellType
{
    STYPE_NULL,
    STYPE_ROOT,
    STYPE_HEAL,
    STYPE_STUN,
    STYPE_FEAR,
    STYPE_SILENCE,
    STYPE_CURSE,
    STYPE_AOEDAMAGE,
    STYPE_DAMAGE,
    STYPE_SUMMON,
    STYPE_BUFF,
    STYPE_DEBUFF
};

enum AI_SpellTargetType
{
    TTYPE_NULL,
    TTYPE_SINGLETARGET,
    TTYPE_DESTINATION,
    TTYPE_SOURCE
};

enum AI_State
{
    STATE_IDLE,
    STATE_ATTACKING,
    STATE_CASTING,
    STATE_FLEEING,
    STATE_FOLLOWING,
    STATE_EVADE,
    STATE_MOVEWP,
    STATE_FEAR,
    STATE_WANDER,
    STATE_STOPPED,
    STATE_SCRIPTMOVE,
    STATE_SCRIPTIDLE
};

enum MovementState
{
    MOVEMENTSTATE_MOVE,
    MOVEMENTSTATE_FOLLOW,
    MOVEMENTSTATE_STOP,
    MOVEMENTSTATE_FOLLOW_OWNER
};

enum CreatureState
{
    STOPPED,
    MOVING,
    ATTACKING
};

enum AiEvents
{
    EVENT_ENTERCOMBAT,
    EVENT_LEAVECOMBAT,
    EVENT_DAMAGETAKEN,
    EVENT_TARGETCASTSPELL,
    EVENT_TARGETPARRYED,
    EVENT_TARGETDODGED,
    EVENT_TARGETBLOCKED,
    EVENT_TARGETCRITHIT,
    EVENT_TARGETDIED,
    EVENT_UNITPARRYED,
    EVENT_UNITDODGED,
    EVENT_UNITBLOCKED,
    EVENT_UNITCRITHIT,
    EVENT_UNITDIED,
    EVENT_ASSISTTARGETDIED,
    EVENT_FEAR,
    EVENT_UNFEAR,
    EVENT_FOLLOWOWNER,
    EVENT_PET_ATTACK,
    EVENT_WANDER,
    EVENT_UNWANDER,
};

struct SpellEntry;
//enum MOD_TYPES;

struct AI_Spell
{
    uint32 entryId;
    uint16 agent;
    uint32 procEvent;
    uint32 procChance;
    uint32 procCount;
    uint32 spellId;
    uint8 spellType;
    uint8 spelltargetType;
    uint32 spellCooldown;
    float floatMisc1;
    uint32 Misc2;
    float minrange;
    float maxrange;
    uint32 procCounter;
    uint32 spellCooldownTimer;
};

struct WayPoint
{
    WayPoint()
    {
        o = 0.0f;
    }
    uint32 id;
    float x;
    float y;
    float z;
    float o;
    uint32 waittime; //ms
    uint32 flags;
    bool forwardemoteoneshot;
    uint32 forwardemoteid;
    bool backwardemoteoneshot;
    uint32 backwardemoteid;
    uint32 forwardskinid;
    uint32 backwardskinid;

};

typedef HM_NAMESPACE::hash_map<Unit*, int32> TargetMap;
typedef HM_NAMESPACE::hash_set<Unit*> AssistTargetSet;
typedef HM_NAMESPACE::hash_map<uint32, AI_Spell*> SpellMap;

class WOWD_SERVER_DECL AIInterface
{
public:

    AIInterface();
    ~AIInterface();

    // Misc
    void Init(Unit *un, AIType at, MovementType mt);
    void Init(Unit *un, AIType at, MovementType mt, Unit *owner); // used for pets
    Unit *GetUnit() { return m_Unit; }
    Unit *GetPetOwner() { return m_PetOwner; }
    void DismissPet();
    void SetUnitToFollow(Unit* un) { UnitToFollow = un; };
    void SetUnitToFear(Unit* un)  { UnitToFear = un; };
    void SetFollowDistance(float dist) { FollowDistance = dist; };
    void SetUnitToFollowAngle(float angle) { m_fallowAngle = angle; }
    bool setInFront(Unit* target);
    inline Unit* getUnitToFollow() { return UnitToFollow; }
    void setCreatureState(CreatureState state){ m_creatureState = state; }
    inline uint8 getAIState() { return m_AIState; }
    inline uint8 getAIType() { return m_AIType; }
    inline uint8 getCurrentAgent() { return m_aiCurrentAgent; }
    void setCurrentAgent(AI_Agent agent) { m_aiCurrentAgent = agent; }
    uint32 getThreatByGUID(uint64 guid);
    uint32 getThreatByPtr(Unit* obj);
    bool modThreatByGUID(uint64 guid, int32 mod);
    bool modThreatByPtr(Unit* obj, int32 mod);
    inline AssistTargetSet GetAssistTargets() { return m_assistTargets; }
    inline TargetMap *GetAITargets() { return &m_aiTargets; }
    void addAssistTargets(Unit* Friends);
    void WipeHateList();
    void WipeTargetList();
    bool taunt(Unit* caster, bool apply = true);
    Unit* getTauntedBy();
    bool GetIsTaunted();
    inline uint32 getAITargetsCount() { return m_aiTargets.size(); }
    inline uint32 getOutOfCombatRange() { return m_outOfCombatRange; }
    void setOutOfCombatRange(uint32 val) { m_outOfCombatRange = val; }

    // Spell
    void CastSpell(Unit* caster, SpellEntry *spellInfo, SpellCastTargets targets);
    SpellEntry *getSpellEntry(uint32 spellId);
    SpellCastTargets setSpellTargets(SpellEntry *spellInfo, Unit* target);
    AI_Spell *getSpellByEvent(uint32 event);
    void resetSpellCounter();
    void increaseProcCounter(uint32 event, AI_Spell *sp);
    void addSpellToList(AI_Spell *sp);

    // Event Handler
    void HandleEvent(uint32 event, Unit* pUnit, uint32 misc1);
    void OnDeath(Object* pKiller);
    void AttackReaction(Unit *pUnit, uint32 damage_dealt, uint32 state, uint32 spellId = 0);
    bool HealReaction(Unit* caster, Unit* victim, uint32 amount);

    // Update
    void Update(uint32 p_time);

    // Movement
    void SendMoveToPacket(float toX, float toY, float toZ, float toO, uint32 time, uint32 MoveFlags);
    //void SendMoveToSplinesPacket(std::list<Waypoint> wp, bool run);
    void MoveTo(float x, float y, float z, float o);
    uint32 getMoveFlags();
    void UpdateMove();
    void SendCurrentMove(Player* plyr/*uint64 guid*/);
    bool StopMovement(uint32 time);
    uint32 getCurrentWaypoint() { return m_currentWaypoint; }
    void changeWayPointID(uint32 oldwpid, uint32 newwpid);
    bool addWayPoint(WayPoint* wp);
    void setWaypointMap(HM_NAMESPACE::hash_map<uint32, WayPoint*> wpm);
    bool saveWayPoints(uint32 wpid);
    bool showWayPoints(uint32 wpid, Player* pPlayer, bool Backwards);
    bool hideWayPoints(uint32 wpid, Player* pPlayer);
    WayPoint* getWayPoint(uint32 wpid);
    void deleteWayPoint(uint32 wpid, bool save = true);
    inline bool hasWaypoints() { return m_nWaypoints > 0; }
    inline void setMoveType(uint32 movetype) { m_moveType = movetype; }
    inline uint32 getMoveType() { return m_moveType; }
    inline void setMoveRunFlag(bool f) { m_moveRun = f; }
    inline bool getMoveRunFlag() { return m_moveRun; }
    void setWaypointToMove(uint32 id) { m_currentWaypoint = id; }
    bool IsFlying();

    // Calculation
    float _CalcAggroRange(Unit* target);
    void _CalcDestinationAndMove(Unit *target, float dist);
    float _CalcCombatRange(Unit* target, bool ranged);
    float _CalcDistanceFromHome();
    uint32 _CalcThreat(uint32 damage, uint32 spellId, Unit* Attacker);
    
    void SetAllowedToEnterCombat(bool val) { m_AllowedToEnterCombat = val; }
    inline bool GetAllowedToEnterCombat(void) { return m_AllowedToEnterCombat; }

    void CheckTarget(Unit* target);
    inline void SetAIState(AI_State newstate) { m_AIState = newstate; }

    // Movement
    bool m_canMove;
    bool m_WayPointsShowing;
    bool m_WayPointsShowBackwards;
    uint32 m_currentWaypoint;
    bool m_moveBackward;
    uint32 m_moveType;
    bool m_moveRun;
    bool m_moveFly;
    bool m_moveSprint;
    CreatureState m_creatureState;
    uint32 m_nWaypoints;

    bool m_canFlee;
    bool m_canCallForHelp;
    bool m_canRangedAttack;
    float m_FleeHealth;
    uint32 m_FleeDuration;
    float m_CallForHelpHealth;
    uint32 m_totemspelltimer;
    uint32 m_totemspelltime;
    SpellEntry * totemspell;

    inline AI_Spell* GetDefaultSpell(void) { return m_DefaultSpell; }
    inline void SetDefaultSpell(AI_Spell* sp)
    {
        if(!sp)
            m_DefaultSpell = m_DefaultMeleeSpell;
        else
            m_DefaultSpell = sp;
    }

    inline void CleanupSpellMap(SpellMap* sm)
    {
        for(SpellMap::iterator itr = sm->begin(); itr != sm->end(); ++itr)
        {
			if(itr->second) // dunno why it happens, but it does :P
                delete itr->second;
        }
		sm->clear();
    }
    float m_sourceX, m_sourceY, m_sourceZ;
    uint32 m_totalMoveTime;
    inline void AddStopTime(uint32 Time) { m_moveTimer += Time; }
    inline void SetNextSpell(AI_Spell * sp) { m_nextSpell = sp; }
    inline Unit* GetNextTarget() { return m_nextTarget; }
    inline void SetNextTarget (Unit *nextTarget) { m_nextTarget = nextTarget; }

    Creature * m_formationLinkTarget;
    float m_formationFollowDistance;
    float m_formationFollowAngle;
    uint32 m_formationLinkSqlId;

    void WipeReferences();

private:
    bool m_AllowedToEnterCombat;

    // Update
    void _UpdateTargets();
    void _UpdateMovement(uint32 p_time);
    void _UpdateCombat(uint32 p_time);
    void _UpdateTimer(uint32 p_time);
    void _UpdateCooldownTimers(uint32 p_time);
    inline void _UpdateCooldownTimer(bool* b_val, SpellMap* spellmap, uint32 p_time)
    {
        bool changed = false;
        if((*b_val) == true)
        {
            for(SpellMap::iterator i = spellmap->begin();i != spellmap->end(); ++i)
            {
                if(i->second->spellCooldownTimer > p_time)
                {
                    changed = true;
                    i->second->spellCooldownTimer -= p_time;
                }else
                {
                    i->second->spellCooldownTimer = 0;
                }
            }
            *b_val = changed;
        }
    }
    int m_updateAssist;
    int m_updateTargets;
    uint32 m_updateAssistTimer;
    uint32 m_updateTargetsTimer;

    // Misc
    bool firstLeaveCombat;
    Unit* FindTarget();
    Unit* FindTargetForSpell(AI_Spell *sp);
    bool FindFriends(float dist);
    AI_Spell *m_nextSpell;
    Unit* m_nextTarget;
    uint32 m_fleeTimer;
    bool m_hasFleed;
    bool m_hasCalledForHelp;
    uint32 m_outOfCombatRange;

    // Spell lists
    bool m_hasOnEnterCombatSpells;
    bool m_hasOnLeaveCombatSpells;
    bool m_hasOnDamageTakenSpells;
    bool m_hasOnTargetCastSpellSpells;
    bool m_hasOnTargetParryedSpells;
    bool m_hasOnTargetDodgedSpells;
    bool m_hasOnTargetBlockedSpells;
    bool m_hasOnTargetCritHitSpells;
    bool m_hasOnTargetDiedSpells;
    bool m_hasOnUnitParryedSpells;
    bool m_hasOnUnitDodgedSpells;
    bool m_hasOnUnitBlockedSpells;
    bool m_hasOnUnitCritHitSpells;
    bool m_hasOnUnitDiedSpells;
    bool m_hasOnAssistTargetDiedSpells;
    bool m_hasOnFollowOwnerSpells;
    SpellMap m_OnEnterCombatSpells;
    SpellMap m_OnLeaveCombatSpells;
    SpellMap m_OnDamageTakenSpells;
    SpellMap m_OnTargetCastSpellSpells;
    SpellMap m_OnTargetParryedSpells;
    SpellMap m_OnTargetDodgedSpells;
    SpellMap m_OnTargetBlockedSpells;
    SpellMap m_OnTargetCritHitSpells;
    SpellMap m_OnTargetDiedSpells;
    SpellMap m_OnUnitParryedSpells;
    SpellMap m_OnUnitDodgedSpells;
    SpellMap m_OnUnitBlockedSpells;
    SpellMap m_OnUnitCritHitSpells;
    SpellMap m_OnUnitDiedSpells;
    SpellMap m_OnAssistTargetDiedSpells;
    SpellMap m_OnFollowOwnerSpells;
    // Spell Cooldown Lists
    bool m_hasCooldownOnEnterCombatSpells;
    bool m_hasCooldownOnLeaveCombatSpells;
    bool m_hasCooldownOnDamageTakenSpells;
    bool m_hasCooldownOnTargetCastSpellSpells;
    bool m_hasCooldownOnTargetParryedSpells;
    bool m_hasCooldownOnTargetDodgedSpells;
    bool m_hasCooldownOnTargetBlockedSpells;
    bool m_hasCooldownOnTargetCritHitSpells;
    bool m_hasCooldownOnTargetDiedSpells;
    bool m_hasCooldownOnUnitParryedSpells;
    bool m_hasCooldownOnUnitDodgedSpells;
    bool m_hasCooldownOnUnitBlockedSpells;
    bool m_hasCooldownOnUnitCritHitSpells;
    bool m_hasCooldownOnUnitDiedSpells;
    bool m_hasCooldownOnAssistTargetDiedSpells;
    bool m_hasCooldownOnFollowOwnerSpells;


    Unit *m_Unit;
    Unit *m_PetOwner;
    float FollowDistance;
    float m_fallowAngle;

    //std::set<AI_Target> m_aiTargets;
    TargetMap m_aiTargets;
    AssistTargetSet m_assistTargets;
    AIType m_AIType;
    AI_State m_AIState;
    AI_Agent m_aiCurrentAgent;

    Unit* tauntedBy;
    bool isTaunted;


    // Movement
    float m_moveSpeed;
    float m_destinationX;
    float m_destinationY;
    float m_destinationZ;
    
    float m_nextPosX;
    float m_nextPosY;
    float m_nextPosZ;

    //Return position after attacking a mob
    float m_returnX;
    float m_returnY;
    float m_returnZ;

    float m_lastFollowX;
    float m_lastFollowY;

    //typedef std::map<uint32, WayPoint*> WayPointMap;
    typedef HM_NAMESPACE::hash_map<uint32, WayPoint*> WayPointMap;
    WayPointMap m_waypoints;
    Unit *UnitToFollow;
    Unit *UnitToFear;
    uint32 m_timeToMove;
    uint32 m_timeMoved;
    uint32 m_moveTimer;
    uint32 m_FearTimer;
    uint32 m_WanderTimer;

    MovementType m_MovementType;
    MovementState m_MovementState;
    AI_Spell* m_DefaultSpell;
    AI_Spell* m_DefaultMeleeSpell;
    uint32 m_guardTimer;

};
#endif

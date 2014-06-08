#ifndef SCRIPTMGR_H
#define SCRIPTMGR_H

//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Class ScriptMgr - Handles storage, loading
// unloading, compilation, and calling of
// external binary scripts that are used by
// the server.

#ifdef WIN32
#define WOWD_MODULE HMODULE
#else
#define WOWD_MODULE void*
#endif

#define SCRIPTLIB_VERSION_MAJOR 2
#define SCRIPTLIB_VERSION_MINOR 1

class Spell;
class Aura;
class Object;
class Unit;
class GameObject;
class MapMgr;
struct ItemPrototype;

class DummySpellScript;
class CreatureAIScript;
class GameObjectAIScript;
class GossipScript;
class EventScript;
class ServerOutputPlugin;
class PluginModule;
class InstanceScript;

struct ScriptModule;

enum ScriptTypes
{
    SCRIPT_TYPE_CREATURE_AI     = 0x01,
    SCRIPT_TYPE_SPELL           = 0x02,
    SCRIPT_TYPE_AURA            = 0x04,
    SCRIPT_TYPE_GOSSIP          = 0x08,
    SCRIPT_TYPE_GAMEOBJECT_AI   = 0x10,
    SCRIPT_TYPE_MISC            = 0x12,
    SCRIPT_TYPE_INSTANCE        = 0x14,
};

#define MAKE_SCRIPT_VERSION(major, minor) (uint32)(((uint16)major << 16) | ((uint16)minor))

#ifdef WOWD_SCRIPTLIB

// Scriptlib defines
#define BUILD_VERSIONINFO_DATA(major, minor) extern "C" WOWD_SCRIPT_DECL void GetModuleVersion(uint32 *pVersionInfo); \
    void GetModuleVersion(uint32 *pVersionInfo) { *pVersionInfo = MAKE_SCRIPT_VERSION(major, minor); }

#endif

#ifndef WOWD_SCRIPTLIB
typedef CreatureAIScript*(*CreateCreatureAIClass)(uint32 uEntryId, Creature* creature);
typedef GameObjectAIScript*(*CreateGameObjectAIClass)(uint32 uEntryId, GameObject* gameobject);
typedef InstanceScript*(*CreateInstanceClass)(uint32 uMapId, MapMgr *instance);
typedef EventScript*(*CreateWoWEvent)(MapMgr *instance);
typedef PluginModule*(*CreatePluginModule)();

typedef GossipScript*(*CreateGossipScript)(uint32 uEntryId);

typedef bool(*HandleDummySpell)(uint32 uSpellId, uint32 i, Spell* pSpell);
typedef bool(*HandleDummyAura)(uint32 uSpellId, uint32 i, Aura* pAura, bool apply);
typedef void(*GetModuleVersion)(uint32 *pVersionInfo);
typedef void(*ScriptInitialize)(ScriptModule *mod);

struct ScriptModule
{
    WOWD_MODULE _handle;
    uint32 _version;
    std::string filename;

    // creation functions
    CreateCreatureAIClass _createCreatureAI;
    CreateGameObjectAIClass _createGameObjectAI;
    CreateInstanceClass _createInstance;

    HandleDummySpell _dummySpellHandler;
    HandleDummyAura _dummyAuraHandler;

    // gossip
    CreateGossipScript _createGossipScript;
    CreatePluginModule _createPluginModule;
    CreateWoWEvent    _createEvent;
};

#else

struct ScriptModule;

#endif

typedef std::map<uint32, ScriptModule*> ScriptMap;
typedef std::map<ScriptModule*, uint32> EventScriptMap;
typedef std::set<ScriptModule*> ScriptSet;
typedef std::map<uint32, GossipScript*> GossipScriptMap;
typedef map<Creature*, CreatureAIScript*> InstancedAIMap;
typedef std::vector<PluginModule*> PluginInstanceSet;

struct PluginMessage;
enum LoadMsgTypes
{
    LOAD_MSG_NONE = 0,
    LOAD_MSG_SPELL = 1,
    LOAD_MSG_AURA = 2,
    LOAD_MSG_CREATURE = 3,
};

class WOWD_SERVER_DECL ScriptMgr : public Singleton<ScriptMgr>
{
public:
    ScriptMgr();
    ~ScriptMgr();
    void Init();

    // Loading and unloading of scripts
    void UnloadScripts();
    void LoadScripts();

    // Compiling state, dirty/clean
    bool RecompileScripts();
    bool CheckScriptStatus();

    bool Creature_HasAIScript(uint32 uEntryId);
    bool Spell_HasDummyHandler(uint32 uSpellId);
    bool Aura_HasDummyHandler(uint32 uSpellId);

    CreatureAIScript * CreateAIScriptClassForEntry(uint32 uEntryId, Creature* pCreature);
    GameObjectAIScript * CreateAIScriptClassForGameObject(uint32 uEntryId, GameObject* pGameObject);
    InstanceScript * CreateInstanceScriptForMap(uint32 uMapId, MapMgr *pInstance);

    bool CallScriptedDummySpell(uint32 uSpellId, uint32 i, Spell* pSpell);
    bool CallScriptedDummyAura( uint32 uSpellId, uint32 i, Aura* pAura, bool apply);

    GossipScript* GetGossipScript(uint32 uEntryId);

    // Addition from script funcs.
    void ScriptAddCreatureModule(uint32 uEntryId, ScriptModule *mod);
    void ScriptAddGameObjectModule(uint32 uEntryId, ScriptModule *mod);
    void ScriptAddInstanceModule(uint32 uMapId, ScriptModule *mod);
    void ScriptAddBattlegroundModule(uint32 uMapId, ScriptModule *mod);
    void ScriptAddSpellModule(uint32 uSpellId, ScriptModule *mod);
    void ScriptAddAuraModule(uint32 uSpellId, ScriptModule *mod);
    void ScriptAddGossipModule(uint32 uEntryId, ScriptModule *mod);
    void ScriptAddEventModule(uint32 uMapId, ScriptModule *mod);
    void ScriptAddServerModule(ScriptModule *mod);

    void ExecuteEventScriptsOnMap(uint32 uMapId, MapMgr* instance);
    void PostMessageToPlugins(PluginMessage * msg);
    void QueueLoadedMessage(uint32 Type, const char * Format, ...);
    void FlushLoadedMessages();

    void PostMessageToServer(PluginMessage * msg);
    void SpawnThread(WowdThread * thread);
    bool HasPlugins();

protected:
    bool _loaded;
    bool _initialized;

    void _PrintLoadName(const char* filename);
    void _PrintLoadStatus(int pass);
    void _PrintLoadError(bool warning, const char* str, ...);

    ScriptMap _creatureModules;
    ScriptMap _gameobjectModules;
    ScriptMap _instanceModules;

    ScriptMap _battlegroundModules;

    ScriptMap _spellModules;
    ScriptMap _auraModules;
    ScriptMap _gossipModules;

    ScriptSet _scripts;

    ScriptSet _serverPlugins;
    PluginInstanceSet _pluginInstances;
    EventScriptMap _eventModules;

    InstancedAIMap _aiInstances;
    GossipScriptMap _GossipScripts;
    vector<pair<uint32, string> > loadmsg;
};

class WOWD_SERVER_DECL CreatureAIScript
{
public:
    CreatureAIScript(Creature* creature);
    virtual ~CreatureAIScript() {};

    virtual void OnCombatStart(Unit* mTarget) {}
    virtual void OnCombatStop(Unit* mTarget) {}
    virtual void OnDamageTaken(Unit* mAttacker, float fAmount) {}
    virtual void OnCastSpell(uint32 iSpellId) {}
    virtual void OnTargetParried(Unit* mTarget) {}
    virtual void OnTargetDodged(Unit* mTarget) {}
    virtual void OnTargetBlocked(Unit* mTarget, int32 iAmount) {}
    virtual void OnTargetCritHit(Unit* mTarget, float fAmount) {}
    virtual void OnTargetDied(Unit* mTarget) {}
    virtual void OnParried(Unit* mTarget) {}
    virtual void OnDodged(Unit* mTarget) {}
    virtual void OnBlocked(Unit* mTarget, int32 iAmount) {}
    virtual void OnCritHit(Unit* mTarget, float fAmount) {}
    virtual void OnHit(Unit* mTarget, float fAmount) {}
    virtual void OnDied(Unit *mKiller) {}
    virtual void OnAssistTargetDied(Unit* mAssistTarget) {}
    virtual void OnFear(Unit* mFeared, uint32 iSpellId) {}
    virtual void OnFlee(Unit* mFlee) {}
    virtual void OnCallForHelp() {}
    virtual void OnLoad() {}
    virtual void OnReachWP(uint32 iWaypointId, bool bForwards) {}
    virtual void OnLootTaken(Player* pPlayer, ItemPrototype *pItemPrototype) {}
    virtual void AIUpdate() {}

    void RegisterAIUpdateEvent(uint32 frequency);
    void RemoveAIUpdateEvent();

    virtual void Destroy() {}
    Creature* GetUnit() { return _unit; }

protected:
    Creature* _unit;
};

class WOWD_SERVER_DECL GameObjectAIScript
{
public:
    GameObjectAIScript(GameObject* goinstance);
    virtual ~GameObjectAIScript() {}

    virtual void OnCreate() {}
    virtual void OnSpawn() {}
    virtual void OnDespawn() {}
    virtual void OnLootTaken(Player * pLooter, ItemPrototype *pItemInfo) {}
    virtual void OnActivate(Player * pPlayer) {}
    virtual void AIUpdate() {}
    virtual void Destroy() {}

    void RegisterAIUpdateEvent(uint32 frequency);

protected:

    GameObject* _gameobject;
};

class WOWD_SERVER_DECL GossipScript
{
public:
    GossipScript();
    virtual ~GossipScript() {} 

    virtual void GossipHello(Creature* pCreature, Player* Plr, bool AutoSend);
    virtual void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId);
    virtual void GossipEnd(Creature* pCreature, Player* Plr);
    virtual void Destroy();

    bool AutoCreated;
};

class WOWD_SERVER_DECL EventScript
{
public:
    EventScript(MapMgr *instance);
    virtual ~EventScript() {}

    virtual void Remove() {}
    virtual void Execute() {}
    virtual void Delete() {}

protected:
    MapMgr *_map;
};

class WOWD_SERVER_DECL PluginModule
{
public:
    PluginModule() {}
    virtual ~PluginModule() {}
    virtual void HandleMessage(PluginMessage * msg) {}
};

/*class SpellScript
{
public:
};*/
#define sScriptMgr ScriptMgr::getSingleton()

#ifdef WOWD_SCRIPTLIB
#define ADD_CREATURE(x) sScriptMgr.ScriptAddCreatureModule(x, mod)
#define ADD_SPELL(x) sScriptMgr.ScriptAddSpellModule(x, mod)
#define ADD_AURA(x) sScriptMgr.ScriptAddAuraModule(x, mod)
#endif

#endif



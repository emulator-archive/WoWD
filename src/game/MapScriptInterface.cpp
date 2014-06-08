/************************************************************************/
/* Copyright (C) 2006 Burlex for WoWd Project                           */
/************************************************************************/

/* * Class InstanceScript
   * Instanced class created for each instance of the map, holds all 
   * scriptable exports
*/

/* * Class MapScriptInterface
   * Provides an interface to mapmgr for scripts, to obtain objects,
   * get players, etc.
*/

#include "StdAfx.h"
createFileSingleton(StructFactory);

MapScriptInterface::MapScriptInterface(MapMgr & mgr) : mapMgr(mgr)
{
    //sLog.outDetail("   Map script interface created for instance %u", mgr.GetInstanceID());
}

MapScriptInterface::~MapScriptInterface()
{
    //sLog.outDetail("   Map script interface destroyed for instance %u", mapMgr.GetInstanceID());
    mapMgr.ScriptInterface = 0;
}

uint32 MapScriptInterface::GetPlayerCountInRadius(float x, float y, float z /* = 0.0f */, float radius /* = 5.0f */)
{
    // use a cell radius of 2
    uint32 PlayerCount = 0;
    uint32 cellX = mapMgr.GetPosX(x);
    uint32 cellY = mapMgr.GetPosY(y);

    uint32 endX = cellX < _sizeX ? cellX + 1 : _sizeX;
    uint32 endY = cellY < _sizeY ? cellY + 1 : _sizeY;
    uint32 startX = cellX > 0 ? cellX - 1 : 0;
    uint32 startY = cellY > 0 ? cellY - 1 : 0;
    MapCell * pCell;
    ObjectSet::iterator iter, iter_end;

    for(uint32 cx = startX; cx < endX; ++cx)
    {
        for(uint32 cy = startY; cy < startY; ++cy)
        {
            pCell = mapMgr.GetCell(cx, cy);
            if(pCell == 0 || pCell->GetPlayerCount() == 0)
                continue;

            iter = pCell->Begin();
            iter_end = pCell->End();

            for(; iter != iter_end; ++iter)
            {
                if((*iter)->GetTypeId() == TYPEID_PLAYER &&
                    (*iter)->CalcDistance(x, y, (z == 0.0f ? (*iter)->GetPositionZ() : z)) < radius)
                {
                    ++PlayerCount;
                }
            }
        }
    }

    return PlayerCount;
}

GameObject* MapScriptInterface::SpawnGameObject(uint32 Entry, float cX, float cY, float cZ, float cO, bool AddToWorld, uint32 Misc1, uint32 Misc2)
{
    GameObjectInfo *pInfo = objmgr.GetGameObjectNameNULL(Entry);
    if(pInfo == 0)
        return 0;

    GameObject *pGameObject = sObjHolder.Create<GameObject>();
    pGameObject->CreateFromProto(Entry, mapMgr.GetMapId(), cX, cY, cZ, cO);
    pGameObject->SetInstanceID(mapMgr.GetInstanceID());

    if(AddToWorld)
        pGameObject->AddToWorld();

    return pGameObject;
}

Creature* MapScriptInterface::SpawnCreature(uint32 Entry, float cX, float cY, float cZ, float cO, bool AddToWorld, bool tmplate, uint32 Misc1, uint32 Misc2)
{
    Creature* pCreature = NULL;
    if(tmplate)
    {
        CreatureSpawnTemplate *pTemplate = objmgr.GetCreatureSpawnTemplate(Entry);
        if(pTemplate == 0)
            return 0;

        Creature *pCreature = sObjHolder.Create<Creature>();
        pCreature->Create(pTemplate, mapMgr.GetMapId(), cX, cY, cZ, cO);
        pCreature->SetInstanceID(mapMgr.GetInstanceID());
    }
    else
    {
        CreatureInfo *ci = objmgr.GetCreatureName(Entry);
        if(!ci)
            return NULL;
        Creature* pCreature = sObjHolder.Create<Creature>();

        pCreature->SetInstanceID(mapMgr.GetInstanceID());
        pCreature->Create(ci->Name.c_str(), mapMgr.GetMapId(), cX, cY, cZ, cO);
        pCreature->SetZoneId(0);
        pCreature->SetUInt32Value(OBJECT_FIELD_ENTRY, ci->Id);
        pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
        pCreature->SetUInt32Value(UNIT_FIELD_DISPLAYID, ci->DisplayID);
        pCreature->SetUInt32Value(UNIT_NPC_FLAGS, 0);
        pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE , Misc1);
        pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 28 + 30*Misc2);
        pCreature->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 28 + 30*Misc2);
        pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, Misc2);
        pCreature->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);
        pCreature->SetFloatValue(UNIT_FIELD_MAXDAMAGE, 150.0f);
        pCreature->SetFloatValue(UNIT_FIELD_MINDAMAGE, 135.0f);
        pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 1900);
        pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME+1, 2000);
        pCreature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 2.0f);

        pCreature->_setFaction();

        pCreature->_LoadHealth();

        pCreature->AddToWorld();

        pCreature->UpdateOppFactionSet();

        pCreature->SetCreatureName(ci);

        pCreature->GetAIInterface()->Init(static_cast<Unit *>(pCreature), AITYPE_AGRO, MOVEMENTTYPE_NONE);
  /*      CreatureTemplate *t = new CreatureTemplate;
        t->Create(pCreature);
        pCreature->GetMapMgr()->GetBaseMap()->GetTemplate()->AddIndex<Creature>(pCreature, t);        */
    }
    if(!pCreature)
        return 0;

    if(AddToWorld)
        pCreature->AddToWorld();

    return pCreature;
}

void MapScriptInterface::DeleteCreature(Creature* ptr)
{
    sObjHolder.Delete<Creature>(ptr);
}

void MapScriptInterface::DeleteGameObject(GameObject *ptr)
{
    sObjHolder.Delete<GameObject>(ptr);
}

WayPoint * StructFactory::CreateWaypoint()
{
    return new WayPoint;
}

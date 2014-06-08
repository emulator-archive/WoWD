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

#ifndef __OBJECTTEMPLATE_H
#define __OBJECTTEMPLATE_H

struct WayPoint;
struct CreatureItem;
struct QuestRelation;

class Object;
class Creature;


typedef struct
{
    float mindmg, maxdmg;
    uint32 maxhealth;
    uint32 maxmana;
    uint32 attack_times[2];
}temphealth;

typedef struct
{
    uint32 follow_id;
    float follow_dist;
    float follow_angle;
}tempformation;


class ObjectTemplate
{
    friend class TemplateMgr;
    friend class ObjectMgr;
public:
    ObjectTemplate() { }
    virtual ~ObjectTemplate()
    {
        delete [] _fields;
    }

    virtual void Load(Field *field);
    virtual void Create(Object *obj);
protected:
    uint32 _guid;
    float _x, _y, _z, _o;
    uint32 _zone;
    uint32 _map;
    char *_fields;
};

class CorpseTemplate : public ObjectTemplate
{
    friend class ObjectMgr;
public:
    virtual void Load(Field *field);
    virtual void Create(Object *obj);
protected:
    bool _loadedfromdb;
};

class GameObjectTemplate : public ObjectTemplate
{
    friend class GameObject;
public:
    virtual void Load(Field *field);

protected:
    uint32 _entry;

    std::list<QuestRelation *>* _quests;
};

class CreatureTemplate : public ObjectTemplate
{
    friend class Creature;
    friend class CreatureDataCache;
public:
    virtual void Load(Field *field);

    ~CreatureTemplate() { }

protected:

    // Formations
    uint32 formation_sqlid;
    float formation_dist;
    float formation_angle;

    uint32 _entry;
    uint32 _moverandom;
    bool _running;

    // Temp health
    temphealth healthdata;

    // Movement data
    HM_NAMESPACE::hash_map<uint32, WayPoint*> _movement;
    vector<AI_Spell*> _aiSpells;

    // Vendor data
    std::vector<CreatureItem>* _items;

    /// Quest data
    std::list<QuestRelation *>* _quests;
};

class CreatureDataCache : public Singleton<CreatureDataCache>
{
public:

    //! Loading
    CreatureDataCache();
    ~CreatureDataCache();

    void ApplyMovement(CreatureTemplate * t);
    void ApplyHealth(CreatureTemplate * t);
    void ApplyFormation(CreatureTemplate * t);
    void ApplySpells(CreatureTemplate * t);

private:

    map<uint32, HM_NAMESPACE::hash_map<uint32, WayPoint*> > waypointData;
    map<uint32, temphealth> healthData;
    map<uint32, tempformation> formationData;
    map<uint32, vector<AI_Spell*> > spellData;

};

#endif

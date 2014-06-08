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

#ifndef _OBJECTHOLDER_H
#define _OBJECTHOLDER_H

#define CONTAINER_SIZE 1024
#define HIGH_MAX 5

class Player;
class GameObject;
class Creature;
class DynamicObject;
class Corpse;
class Object;
class Pet;
class Transporter;

enum HIGHGUID {
    HIGHGUID_PLAYER        = 0x00000000,
    HIGHGUID_UNIT          = 0x00000001,
    HIGHGUID_GAMEOBJECT    = 0x00000002,
    HIGHGUID_CORPSE        = 0x00000003,
    HIGHGUID_DYNAMICOBJECT = 0x00000004,
    HIGHGUID_ITEM          = 0x00000005,
    HIGHGUID_CONTAINER     = 0x00000006,
    HIGHGUID_WAYPOINT      = 0x00000007,
    HIGHGUID_TRANSPORTER   = 0x00000008,
    HIGHGUID_PET           = 0x00000009,
};

/*
enum HIGHGUID {
    HIGHGUID_ITEM          = 0x40000000,
    HIGHGUID_CONTAINER     = 0x40000000,
    HIGHGUID_UNIT          = 0xF0001000,
    HIGHGUID_PLAYER        = 0x00000000,
    HIGHGUID_WAYPOINT      = 0x0000F000,
    HIGHGUID_GAMEOBJECT    = 0xF0007000,
    HIGHGUID_DYNAMICOBJECT = 0xF000A000,
    HIGHGUID_CORPSE        = 0xF0007000,
    HIGHGUID_TRANSPORTER   = 0x00000008
};
*/

/************************************************************************/
/* Static Object Container (1024 elements)                              */
/************************************************************************/

template < class T , uint32 ContainerSize, class ThreadingPolicy >
class StaticObjectMap
{
public:
    StaticObjectMap(uint32 HiGuid) : m_HiGuid(HiGuid), m_LowGuid(1)
    {
        // Null out all of the array.
        memset(m_Objects, 0, sizeof(T*) * ContainerSize);
    }

    T * Create()    // Automatically assigns guid.
    {
        //sLog.outString("Creating object with highguid 0x%08X lowguid 0x%08X", m_HiGuid, m_LowGuid);

        T * obj = new T(m_HiGuid, m_LowGuid);
        m_Objects[m_LowGuid] = obj;
        
        ++m_LowGuid;
        return obj;
    }

    bool Delete(uint32 guid)
    {
        if(guid > ContainerSize)
            return false;

        T * obj = m_Objects[guid];
        if(obj == 0)
            return false;

        //sLog.outString("Deleting object with highguid 0x%08X lowguid 0x%08X", m_HiGuid, m_LowGuid);

        delete obj;
        m_Objects[guid] = 0;    // possible re-use later on.
        
        // Succeeded.
        return true;
    }

    T * Find(uint32 guid)
    {
        if(guid > ContainerSize)
            return 0;

        return m_Objects[guid];
    }

    void Cleanup()
    {
        for(uint32 Index = 0; Index < ContainerSize; ++Index)
        {
            if(m_Objects[Index] != 0)
            {
                delete m_Objects[Index];
                m_Objects[Index] = 0;
            }
        }
    }

    inline bool Full()
    {
        return (m_LowGuid == ContainerSize) ? true : false;
    }

private:
    uint32 m_HiGuid;
    uint32 m_LowGuid;
    T* m_Objects[ContainerSize];
};

template < class T , class ThreadingPolicy > 
class ObjectSubMap
{
public:
    ObjectSubMap(uint32 guid) : m_HiGuid(guid) { }

    T* Create(uint32 guid);
    
    bool Delete(uint32 guid);
    
    T* Find(uint32 guid);

    inline uint32 Size() { return m_Objects.size(); }
    
    typename HM_NAMESPACE::hash_map<uint32, T*>::iterator Begin();
    
    typename HM_NAMESPACE::hash_map<uint32, T*>::iterator End();

private:
    uint32 m_HiGuid;
    HM_NAMESPACE::hash_map<uint32, T*> m_Objects;
};

// ----- Class templates defenitions ------

template < class T , class ThreadingPolicy > 
T* ObjectSubMap<T, ThreadingPolicy>::Create(uint32 guid)
{
    //sLog.outDebug("Creating object with GUID %8X %8X", m_HiGuid, guid);
    ASSERT(m_Objects.find(guid) == m_Objects.end());
    T* obj = new T(m_HiGuid, guid);
    //m_Objects[guid] = obj;
    typename HM_NAMESPACE::hash_map<uint32, T*>::value_type inserted( guid, obj );
    m_Objects.insert(inserted);
    
    return obj;
}

template < class T , class ThreadingPolicy >    
bool ObjectSubMap<T, ThreadingPolicy>::Delete(uint32 guid)
{
    //sLog.outDebug("Deleting object with GUID %8X %8X", m_HiGuid, guid);
    typename HM_NAMESPACE::hash_map<uint32, T*>::iterator it;
    it = m_Objects.find(guid);
    if (it == m_Objects.end())
        return false;
    delete it->second;
    m_Objects.erase(it);
    return true;
}
    
template < class T , class ThreadingPolicy >   
T* ObjectSubMap<T, ThreadingPolicy>::Find(uint32 guid)
{
    typename HM_NAMESPACE::hash_map<uint32, T*>::iterator it;
    it = m_Objects.find(guid);
    if (it == m_Objects.end())
        return NULL;
    else
        return it->second;
}

template < class T , class ThreadingPolicy >    
typename HM_NAMESPACE::hash_map<uint32, T*>::iterator ObjectSubMap<T, ThreadingPolicy>::Begin() { return m_Objects.begin(); }

template < class T , class ThreadingPolicy >    
typename HM_NAMESPACE::hash_map<uint32, T*>::iterator ObjectSubMap<T, ThreadingPolicy>::End() { return m_Objects.end(); }

// -----------------------------

class ObjectTypeContainerBase
{
public:
    ObjectTypeContainerBase(uint32 guidhigh) : m_GuidMax(1), m_GuidHigh(guidhigh) { }
    ~ObjectTypeContainerBase() { }

protected:
    uint32 m_GuidMax;
    uint32 m_GuidHigh;
};

template < class T , class ThreadingPolicy >
class ObjectTypeContainerSingle : public ObjectTypeContainerBase
{
public:
    ObjectTypeContainerSingle(uint32 guidhigh) : ObjectTypeContainerBase(guidhigh)
    {
        m_Objects = new ObjectSubMap<T, Mutex>(m_GuidHigh);
    }
    ~ObjectTypeContainerSingle()
    {
        delete m_Objects;
    }
    
    T* Create();
    T* Create(uint32 guid);
    bool Delete(uint64 guid);
    T* Find(uint64 guid);

    typename HM_NAMESPACE::hash_map<uint32, T*>::iterator Begin();
    typename HM_NAMESPACE::hash_map<uint32, T*>::iterator End();

    inline uint32 Size()
    {
        return m_Objects.size();
    }

private:
    ObjectSubMap<T, Mutex>* m_Objects;
};

// ----- Class templates defenitions ------

template < class T , class ThreadingPolicy >
T* ObjectTypeContainerSingle<T, ThreadingPolicy>::Create()
{
    return m_Objects->Create(m_GuidMax++);
}

template < class T , class ThreadingPolicy >
T* ObjectTypeContainerSingle<T, ThreadingPolicy>::Create(uint32 guid)
{
    if (guid > m_GuidMax)
        m_GuidMax = guid + 1;

    return m_Objects->Create(guid);
}

template < class T , class ThreadingPolicy >
bool ObjectTypeContainerSingle<T, ThreadingPolicy>::Delete(uint64 guid)
{
    if (UINT32_LOPART(GUID_HIPART(guid)) != UINT32_LOPART(m_GuidHigh))
        return false;

    return m_Objects->Delete(GUID_LOPART(guid));
}

template < class T , class ThreadingPolicy >
T* ObjectTypeContainerSingle<T, ThreadingPolicy>::Find(uint64 guid)
{
    if (UINT32_LOPART(GUID_HIPART(guid)) != UINT32_LOPART(m_GuidHigh))
        return NULL;

    return m_Objects->Find(GUID_LOPART(guid));
}

template < class T , class ThreadingPolicy >
typename HM_NAMESPACE::hash_map<uint32, T*>::iterator ObjectTypeContainerSingle<T, ThreadingPolicy>::Begin() { return m_Objects->Begin(); }
    
template < class T , class ThreadingPolicy >
typename HM_NAMESPACE::hash_map<uint32, T*>::iterator ObjectTypeContainerSingle<T, ThreadingPolicy>::End() { return m_Objects->End(); }

// -----------------------------

template < class T , class ThreadingPolicy >
class ObjectTypeContainerMulti : public ObjectTypeContainerBase
{
    uint16 m_ContainerMax;
    vector<StaticObjectMap<T, CONTAINER_SIZE, ThreadingPolicy>* > m_subContainers;
    uint16 m_Container;
    ThreadingPolicy MUTEX;

public:
    ObjectTypeContainerMulti(uint32 guidhigh) : m_ContainerMax(0), 
                                                ObjectTypeContainerBase(guidhigh)
    {
        // Create initial subcontainer.
        CreateSubContainer();
    }

    ~ObjectTypeContainerMulti()
    {
        Cleanup();
    }

    void CreateSubContainer()
    {
        m_Container = m_subContainers.size();
        ASSERT(m_Container == m_ContainerMax);

        uint32 guidhi = (((uint32)(m_Container)) << 16) | m_GuidHigh;
        StaticObjectMap<T, CONTAINER_SIZE, ThreadingPolicy>* container =
            new StaticObjectMap<T, CONTAINER_SIZE, ThreadingPolicy>(guidhi);
        //sLog.outString("Creating new subcontainer with higuid 0x%08X", guidhi);
        m_subContainers.push_back(container);
        ++m_ContainerMax;
    }

    T* Create();
    T* Create(uint32 guid);
    bool Delete(uint64 guid);
    T* Find(uint64 guid);
    void Cleanup();
};

// ----- Class templates defenitions ------

template < class T , class ThreadingPolicy >
T* ObjectTypeContainerMulti<T, ThreadingPolicy>::Create()
{
    MUTEX.Acquire();
    StaticObjectMap<T, CONTAINER_SIZE, ThreadingPolicy> * Container = m_subContainers[m_Container];
    if(Container->Full())
    {
        // Spawn new container.
        CreateSubContainer();
        Container = m_subContainers[m_Container];
    }

    T* obj = Container->Create();
    MUTEX.Release();
    return obj;
}

template < class T , class ThreadingPolicy >
bool ObjectTypeContainerMulti<T, ThreadingPolicy>::Delete(uint64 guid)
{
    if (UINT32_LOPART(GUID_HIPART(guid)) != UINT32_LOPART(m_GuidHigh))
        return false;

    uint16 serial = UINT32_HIPART(GUID_HIPART(guid));       
    uint32 id = GUID_LOPART(guid);

    if(serial > m_Container || id > CONTAINER_SIZE)
        return false;

    return m_subContainers[serial]->Delete(id);
}

template < class T , class ThreadingPolicy >
T* ObjectTypeContainerMulti<T, ThreadingPolicy>::Find(uint64 guid)
{
    if (UINT32_LOPART(GUID_HIPART(guid)) != UINT32_LOPART(m_GuidHigh))
        return NULL;

    uint16 serial = UINT32_HIPART(GUID_HIPART(guid));       
    uint32 id = GUID_LOPART(guid);

    if(serial > m_Container || id > CONTAINER_SIZE)
        return 0;

    return m_subContainers[serial]->Find(id);
}

template <class T, class ThreadingPolicy>
void ObjectTypeContainerMulti<T, ThreadingPolicy>::Cleanup()
{
    typename vector<StaticObjectMap<T, CONTAINER_SIZE, ThreadingPolicy>*>::iterator iter = m_subContainers.begin();
    for(; iter != m_subContainers.end(); ++iter)
    {
        (*iter)->Cleanup();
        delete (*iter);
    }
    m_subContainers.clear();
}

// -----------------------------

class WOWD_SERVER_DECL ObjectHolder : public Singleton < ObjectHolder >
{
public:
    ObjectHolder()
    {
        m_PlayerContainer = new ObjectTypeContainerSingle<Player, 
                                                Mutex>(HIGHGUID_PLAYER);
        m_CreatureContainer = new ObjectTypeContainerMulti<Creature,
                                                Mutex>(HIGHGUID_UNIT);
        m_GameObjectContainer = new ObjectTypeContainerMulti<GameObject,
                                                Mutex>(HIGHGUID_GAMEOBJECT);
        m_CorpseContainer = new ObjectTypeContainerSingle<Corpse,
                                                Mutex>(HIGHGUID_CORPSE);
        m_DynamicObjectContainer = new ObjectTypeContainerMulti<DynamicObject,
                                                Mutex>(HIGHGUID_DYNAMICOBJECT);
        m_PetContainer = new ObjectTypeContainerSingle<Pet,
                                                Mutex>(HIGHGUID_PET);
        m_TransporterContainer = new ObjectTypeContainerSingle<Transporter,
                                                Mutex>(HIGHGUID_TRANSPORTER);
    }
    ~ObjectHolder()
    {
        delete m_PlayerContainer;
        delete m_CreatureContainer;
        delete m_GameObjectContainer;
        delete m_CorpseContainer;
        delete m_DynamicObjectContainer;
        delete m_PetContainer;
        delete m_TransporterContainer;
    }

    template < class T >
    T* Create()
    {
        return (T*)((typename Select<T>::Result)(_GetContainer<T>()))->Create();
    }

    template < class T >
    T* Create(uint32 guid)
    {
        return (T*)((typename Select<T>::Result)(_GetContainer<T>()))->Create(guid);
    }

    template < class T >
    bool Delete(uint64 guid)
    {
        return ((typename Select<T>::Result)(_GetContainer<T>()))->Delete(guid);
    }
    
    template < class T >
    bool Delete(T* obj)
    {
        return ((typename Select<T>::Result)(_GetContainer<T>()))->Delete(obj->GetGUID());
    }

    template < class T >
    T* GetObject(uint64 guid)
    {
        return (T*)((typename Select<T>::Result)(_GetContainer<T>()))->Find(guid);
    }

    template < class T >
    typename HM_NAMESPACE::hash_map<uint32, T*>::iterator Begin() 
    {
        return ((typename Select<T>::Result)(_GetContainer<T>()))->Begin();
    }

    template < class T >
    typename HM_NAMESPACE::hash_map<uint32, T*>::iterator End() 
    { 
        return ((typename Select<T>::Result)(_GetContainer<T>()))->End();
    }

    template < class T >
    uint32 Size()
    {
        return ((typename Select<T>::Result)(_GetContainer<T>()))->Size();
    }

    //burlex: cleanup functions
    template<class T>
    void CleanupContainer()
    {
        return ((typename Select<T>::Result)(_GetContainer<T>()))->Cleanup();
    }

private:

    template<class T> HIGHGUID _GetHighGuid() const;

    template <class T>
    struct Select
    {
        typedef ObjectTypeContainerBase Result;
    };

    template<class T> 
    ObjectTypeContainerBase* _GetContainer() const;

    ObjectTypeContainerSingle<Player, Mutex>* m_PlayerContainer;
    ObjectTypeContainerMulti<Creature, Mutex>* m_CreatureContainer;
    ObjectTypeContainerMulti<GameObject, Mutex>* m_GameObjectContainer;
    ObjectTypeContainerSingle<Corpse, Mutex>* m_CorpseContainer;
    ObjectTypeContainerMulti<DynamicObject, Mutex>* m_DynamicObjectContainer;
    ObjectTypeContainerSingle<Pet, Mutex>* m_PetContainer;
    ObjectTypeContainerSingle<Transporter, Mutex>* m_TransporterContainer;
};

template<> inline HIGHGUID ObjectHolder::_GetHighGuid<DynamicObject>() const
{ return HIGHGUID_DYNAMICOBJECT; }
template<> inline HIGHGUID ObjectHolder::_GetHighGuid<GameObject>() const
{ return HIGHGUID_GAMEOBJECT; }
template<> inline HIGHGUID ObjectHolder::_GetHighGuid<Creature>() const
{ return HIGHGUID_UNIT; }
template<> inline HIGHGUID ObjectHolder::_GetHighGuid<Player>() const
{ return HIGHGUID_PLAYER; }
template<> inline HIGHGUID ObjectHolder::_GetHighGuid<Corpse>() const
{ return HIGHGUID_CORPSE; }
template<> inline HIGHGUID ObjectHolder::_GetHighGuid<Pet>() const
{ return HIGHGUID_PET; }
template<> inline HIGHGUID ObjectHolder::_GetHighGuid<Transporter>() const
{ return HIGHGUID_TRANSPORTER; }

template <>
struct ObjectHolder::Select<Player>
{
    typedef ObjectTypeContainerSingle<Player, Mutex>* Result;
};
template <>
struct ObjectHolder::Select<Creature>
{
    typedef ObjectTypeContainerMulti<Creature, Mutex>* Result;
};
template <>
struct ObjectHolder::Select<GameObject>
{
    typedef ObjectTypeContainerMulti<GameObject, Mutex>* Result;
};
template <>
struct ObjectHolder::Select<Corpse>
{
    typedef ObjectTypeContainerSingle<Corpse, Mutex>* Result;
};
template <>
struct ObjectHolder::Select<DynamicObject>
 {
    typedef ObjectTypeContainerMulti<DynamicObject, Mutex>* Result;
};
template <>
struct ObjectHolder::Select<Pet>
{
    typedef ObjectTypeContainerSingle<Pet, Mutex>* Result;
};
template <>
struct ObjectHolder::Select<Transporter>
{
    typedef ObjectTypeContainerSingle<Transporter, Mutex>* Result;
};

template<> inline ObjectTypeContainerBase* ObjectHolder::_GetContainer<DynamicObject>() const
{ return m_DynamicObjectContainer; }
template<> inline ObjectTypeContainerBase* ObjectHolder::_GetContainer<GameObject>() const
{ return m_GameObjectContainer; }
template<> inline ObjectTypeContainerBase* ObjectHolder::_GetContainer<Creature>() const
{ return m_CreatureContainer; }
template<> inline ObjectTypeContainerBase* ObjectHolder::_GetContainer<Player>() const
{ return m_PlayerContainer; }
template<> inline ObjectTypeContainerBase* ObjectHolder::_GetContainer<Corpse>() const
{ return m_CorpseContainer; }
template<> inline ObjectTypeContainerBase* ObjectHolder::_GetContainer<Pet>() const
{ return m_PetContainer; }
template<> inline ObjectTypeContainerBase* ObjectHolder::_GetContainer<Transporter>() const
{ return m_TransporterContainer; }

#define sObjHolder ObjectHolder::getSingleton()

#endif

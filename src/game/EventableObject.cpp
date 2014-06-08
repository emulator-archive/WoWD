#include "StdAfx.h"
#include "EventableObject.h"

EventableObject::~EventableObject()
{
    if(m_lastHolder && m_active)    // update currently in progress
        m_lastHolder->deletedObject = true;

    if(m_active < 2)
        event_SetActive(false);
    
    m_eventHolder = 0;
    m_active = 0xFFFFFFFF;
    for(EventList::iterator itr = m_events.begin(); itr != m_events.end(); ++itr)
    {
        delete (*itr)->cb;
        delete (*itr);
    }
    m_events.clear();
}

EventableObject::EventableObject()
{
    m_active = false;
    m_eventHolder = 0;
    m_lastHolder = 0;
}

void EventableObject::event_AddEvent(TimedEvent * ptr)
{
    eventListLock.Acquire();
    m_events.insert(ptr);
    eventListLock.Release();

    event_UpdateActiveStatus();
}

void EventableObject::event_RemoveEvents(uint32 EventType)
{
    eventListLock.Acquire();

    // loop and destroy any events with this type
    TimedEvent * event;
    for(EventList::iterator itr = m_events.begin(); itr != m_events.end(); ++itr)
    {
        event = *itr;
        if(EventType == EVENT_REMOVAL_FLAG_ALL || event->eventFlags == EventType)
        {
            event->deleted = true;
        }
    }

    eventListLock.Release();
}

void EventableObject::event_RemoveEvents()
{
    event_RemoveEvents(EVENT_REMOVAL_FLAG_ALL);
}

bool EventableObject::event_UpdateEvents(uint32 diff)
{
    // Update our events.
    EventList::iterator itr = m_events.begin();
    EventList::iterator it2;
    TimedEvent * event;
    EventableObjectHolder * h = m_eventHolder;
    m_lastHolder = h;
    EventList::iterator itr_end = m_events.end();

    for(; itr != itr_end; )
    {
        event = (*itr);
        it2 = itr;
        ++itr;

        // dunno how this happened, hackfix
        if(it2 == itr_end)
            break;

        if(!event->deleted)
        {
            // Event update procedure
            event->currTime -= diff;

            if (event->currTime <= 0)
            {
                // Reset timer.
                event->currTime = event->msTime;

                // Check if this event has expired.
                if(event->repeats == 1)
                    event->deleted = true;

                /*if(m_lastHolder != h)
                    return false;*/

                m_lastHolder = h;

                // Execute the callback.
                event->cb->execute();

                // If we hit this, it means the event deleted us.
                // I know it's a hack.. but I honestly can't think of any other way to approach this.
                if(h->deletedObject)
                {
                    h->deletedObject = false;
                    return false;
                }

                m_lastHolder = 0;

                if(m_eventHolder != h)
                {
                    // an event moved us to a different instance. abort to prevent heap corruption on
                    // double deletes and executing from the wrong thread.
                    return false;
                }

                if(event->repeats > 0 && !event->deleted)
                    --event->repeats;
            }
        }

        if(m_eventHolder != h)
        {
            // an event moved us to a different instance. abort to prevent heap corruption on
            // double deletes and executing from the wrong thread.
            return false;
        }

        if(event->deleted)
        {
            // Remove us from the list.
            m_events.erase(it2);
            
            // Free the memory.
            delete event->cb;
            delete event;
        }
    }

    if(!event_HasEvents())
    {
        event_UpdateActiveStatus();
        return false;
    }

    return true;
}

void EventableObject::event_UpdateActiveStatus()
{
    if(!m_eventHolder && m_active < 2)
        event_Relocate();

    if(m_active && !event_HasEvents())
        event_SetActive(false);
    else if(!m_active && event_HasEvents())
        event_SetActive(true);
}

void EventableObject::event_SetActive(bool value)
{
    m_active = value ? 1 : 0;
    if(m_eventHolder == 0) return;

    if(value)
        m_eventHolder->AddObject(this);
    else
        m_eventHolder->RemoveObject(this);
}

void EventableObject::event_ModifyTimeLeft(uint32 EventType, uint32 TimeLeft)
{
    EventList::iterator itr = m_events.begin();
    TimedEvent * e;
    for(; itr != m_events.end(); ++itr)
    {
        e = *itr;
        if(e->eventFlags == EventType)
            e->currTime = (TimeLeft > e->msTime) ? e->msTime : TimeLeft;
    }
}

void EventableObject::event_ModifyTime(uint32 EventType, uint32 Time)
{
    EventList::iterator itr = m_events.begin();
    TimedEvent * e;
    for(; itr != m_events.end(); ++itr)
    {
        e = *itr;
        if(e->eventFlags == EventType)
            e->msTime = Time;
    }
}

void EventableObject::event_ModifyTimeAndTimeLeft(uint32 EventType, uint32 Time)
{
    EventList::iterator itr = m_events.begin();
    TimedEvent * e;
    for(; itr != m_events.end(); ++itr)
    {
        e = *itr;
        if(e->eventFlags == EventType)
        {
            e->currTime = Time;
            e->msTime = Time;
        }
    }
}


bool EventableObject::event_HasEvent(uint32 EventType)
{
    EventList::iterator itr = m_events.begin();
    for(; itr != m_events.end(); ++itr)
    {
        if((*itr)->eventFlags == EventType)
            return true;
    }

    return false;
}

EventableObjectHolder::EventableObjectHolder(int32 instance_id) : mInstanceId(instance_id)
{
    sEventMgr.AddEventHolder(this, instance_id);
    deletedObject = false;
}

EventableObjectHolder::~EventableObjectHolder()
{
    sEventMgr.RemoveEventHolder(this);
    set<EventableObject*>::iterator itr;
/*    for(itr = removalList.begin(); itr != removalList.end(); ++itr)
    {
        myObjects.erase((*itr));
    }
    removalList.clear();*/

    // clear objects references to us
    for(itr = myObjects.begin(); itr != myObjects.end(); ++itr)
        (*itr)->m_eventHolder = 0;
}

void EventableObjectHolder::Update(uint32 time_difference)
{
    iteratorLock.Acquire();

    set<EventableObject*>::iterator itr_end = myObjects.end();;
    current = myObjects.begin();
    EventableObject * obj;

    while(current != itr_end)
    {
        obj = (*current);
        ++current;

        if(obj->m_eventHolder == this)
            obj->event_UpdateEvents(time_difference);
    }

    iteratorLock.Release();
}

void EventableObjectHolder::AddObject(EventableObject * obj)
{
    Guard guard(setLock);
    myObjects.insert(obj);
}

void EventableObjectHolder::RemoveObject(EventableObject * obj)
{
    Guard guard(setLock);

    EventableObjectSet::iterator itr = myObjects.find(obj);
    if(itr == myObjects.end()) return;

    if(iteratorLock.AttemptAcquire())
    {
        // Set isn't updating, we're safe to remove.
        myObjects.erase(itr);
        
        // Release it
        iteratorLock.Release();
    }
    else
    {
        // We're updating. We need to check the iterator to make sure
        // our next target isn't this object, otherwise we'll crash.
        if(current == itr)
            ++current;          // Increment it before removing.

        // Now it's safe to remove.
        myObjects.erase(itr);
    }
}

void EventableObject::event_Relocate()
{
    int32 instance = event_GetInstanceID();
    EventableObjectHolder * h = sEventMgr.GetEventHolder(instance);
    assert(h);
    if(h != m_eventHolder && m_eventHolder && m_active == 1)
        m_eventHolder->RemoveObject(this);        

    m_eventHolder = h;
    if(m_active)
        m_eventHolder->AddObject(this);
}
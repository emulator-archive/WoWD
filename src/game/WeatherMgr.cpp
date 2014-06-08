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

initialiseSingleton( WeatherMgr );

WeatherMgr::WeatherMgr()
{
}

WeatherMgr::~WeatherMgr()
{
    std::map<uint32, WeatherInfo*>::iterator itr;
    for(itr = m_zoneWeathers.begin(); itr != m_zoneWeathers.end(); itr++)
    {
        delete itr->second;
    }

    m_zoneWeathers.clear();
}

void WeatherMgr::LoadFromDB()
{
    QueryResult *result = sDatabase.Query( "SELECT * FROM weather" );

    if( !result )
    {
        sLog.outString("  Query failed.");
        return;
    }
    uint32 total =(uint32) result->GetRowCount();
    int num = 0;

    do
    {
        Field *fields = result->Fetch();
        WeatherInfo *wi = new WeatherInfo;
        wi->m_zoneId = fields[0].GetUInt32();
        wi->m_effectValues[0] = fields[1].GetUInt32();
        wi->m_effectValues[1] = fields[2].GetUInt32();
        wi->m_effectValues[2] = fields[3].GetUInt32();
        wi->m_effectValues[3] = fields[4].GetUInt32();
        wi->m_effectValues[4] = fields[5].GetUInt32();

        m_zoneWeathers[wi->m_zoneId] = wi;

        wi->_GenerateWeather();
        ++num;
        if(!(num % 15)) SetProgressBar(num, total, "Zone Weather");
    } while( result->NextRow() );

    delete result;
    ClearProgressBar();
}

void WeatherMgr::SendWeather(Player *plr)
{
    std::map<uint32, WeatherInfo*>::iterator itr;
    itr = m_zoneWeathers.find(plr->GetZoneId());

    if (itr == m_zoneWeathers.end())
    {
        WorldPacket data(SMSG_WEATHER, 13);
        data << (uint32)0;
        data << (float)0;
        data << (uint32)0;
        data << (uint8)0;
        plr->GetSession()->SendPacket( &data );
        plr->m_lastSeenWeather = 0;

        return;
    }
    else
    {
        itr->second->SendUpdate(plr);
    }
}

uint32 WeatherMgr::GetSound(uint32 effect, float density)
{
    //Maybe i should find a better way to do this, but atm can't think of any
    // - Doron

    if (effect == 0)
    {
        return 0;
    }
    else if ((effect == 1) || (effect == 3))
    {
        if (density < 0.15)
            return 0;
        else if (density < 0.25)
            return 8533;
        else if (density < 0.35)
            return 8534;
        else
            return 8535;
    }
    else if ((effect == 2) || (effect == 4))
    {
        if (density < 0.15)
            return 0;
        else if (density < 0.25)
            return 8536;
        else if (density < 0.35)
            return 8537;
        else
            return 8538;
    }
    else
    {
        return 0;
    }
}

WeatherInfo::WeatherInfo()
{
    m_currentDensity = 0;
    m_currentEffect = 0;
    m_currentTime = 0;
    m_maxDensity = 0;
    m_totalTime = 0;
    m_zoneId = 0;
    m_increase = true;
}

WeatherInfo::~WeatherInfo()
{

}

void WeatherInfo::_GenerateWeather()
{
    m_currentTime = 0;
    m_currentDensity = 0;
    m_currentEffect = 0;

    uint32 rv = sRand.randInt(100);

    std::map<uint32, uint32>::iterator itr;

    for(itr = m_effectValues.begin(); itr != m_effectValues.end(); itr++)
    {
        if (rv <= itr->second)
        {
            m_currentEffect = itr->first;
            break;
        }
        else
        {
            rv -= itr->second;
        }
    }

    m_maxDensity = sRand.rand(2); //0 - 2
    m_totalTime = (sRand.randInt(11) + 5)*1000*60;
    m_increase = true;

    SendUpdate();

    sEventMgr.AddEvent(this, &WeatherInfo::Update, EVENT_WEATHER_UPDATE, 
        (uint32)(m_totalTime/ceil(m_maxDensity/WEATHER_DENSITY_UPDATE)*2), 0);
}

void WeatherInfo::Update()
{
    if (m_increase)
    {
        m_currentDensity += WEATHER_DENSITY_UPDATE;
        if (m_currentDensity >= m_maxDensity)
        {
            m_currentDensity = m_maxDensity;
            m_increase = false;
        }
    }
    else
    {
        m_currentDensity -= WEATHER_DENSITY_UPDATE;
        if (m_currentDensity <= 0)
        {
            m_currentDensity = 0;
            sEventMgr.RemoveEvents(this, EVENT_WEATHER_UPDATE);
            _GenerateWeather();
            return;
        }
    }

    SendUpdate();
}

void WeatherInfo::SendUpdate()
{
    WorldPacket data(SMSG_WEATHER, 13);
    data << (uint32)this->m_currentEffect;
    if (this->m_currentEffect == 0)
        data << (float)0;
    else
        data << (float)this->m_currentDensity;
    data << (uint32)sWeatherMgr.GetSound(this->m_currentEffect, this->m_currentDensity);
    data << (uint8)0;

    sWorld.SendZoneMessage(&data, m_zoneId, 0);
}

void WeatherInfo::SendUpdate(Player *plr)
{
    if(plr->m_lastSeenWeather == m_currentEffect)
        return;
    plr->m_lastSeenWeather = m_currentEffect;

    WorldPacket data(SMSG_WEATHER, 13);
    data << (uint32)this->m_currentEffect;
    if (this->m_currentEffect == 0)
        data << (float)0;
    else
        data << (float)this->m_currentDensity;
    data << (uint32)sWeatherMgr.GetSound(this->m_currentEffect, this->m_currentDensity);
    data << (uint8)0;

    plr->GetSession()->SendPacket( &data );
}

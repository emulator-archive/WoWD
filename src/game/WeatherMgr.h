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

#ifndef __WEATHERMGR_H
#define __WEATHERMGR_H

#define WEATHER_DENSITY_UPDATE 0.01f

class WeatherInfo;
class WeatherMgr;

class WeatherMgr :  public Singleton < WeatherMgr >
{
public:
    WeatherMgr();
    ~WeatherMgr();

    void LoadFromDB();
    uint32 GetSound(uint32 effect, float density);
    void SendWeather(Player *plr);

private:
    std::map<uint32, WeatherInfo*> m_zoneWeathers;
};

class WeatherInfo : public EventableObject
{
    friend class WeatherMgr;
public:
    WeatherInfo();
    ~WeatherInfo();

    void Update();    
    void SendUpdate();
    void SendUpdate(Player *plr);

protected:
    void _GenerateWeather();

    uint32 m_zoneId;

    uint32 m_totalTime;
    uint32 m_currentTime;

    float m_maxDensity;
    float m_currentDensity;

    uint32 m_currentEffect;
    bool m_increase;
    std::map<uint32, uint32> m_effectValues;
};

#define sWeatherMgr WeatherMgr::getSingleton()

#endif

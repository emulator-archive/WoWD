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

class ChannelMgr :  public Singleton < ChannelMgr >
{
 
public:
    ChannelMgr()
    {
        seperatechannels = Config.MainConfig.GetBoolDefault("SeperateChatChannels", false);
    }

    ~ChannelMgr();

    Channel *GetJoinChannel(const char *name, Player *p)
    { 
        ChannelList* cl=&Channels[0];
        if(seperatechannels)
            cl = &Channels[p->GetTeam()];

        ChannelList::iterator i=cl->find(string(name));
        if(i!=cl->end())
            return i->second;
        else 
        {
            Channel *nchan = new Channel(name,p);
            (*cl)[name]=nchan;
            return nchan;
        }

    }
    Channel *GetChannel(const char *name, Player *p)
    { 
        ChannelList* cl=&Channels[0];
        if(seperatechannels)
            cl = &Channels[p->GetTeam()];

        ChannelList::iterator i=cl->find(string(name));
        if(i!=cl->end())
            return i->second;
        else 
        {
            WorldPacket data(100);
            data.SetOpcode (SMSG_CHANNEL_NOTIFY);
            data << (uint8)0x05 << string(name);
            p->GetSession()->SendPacket(&data);
            return NULL;
        }
    }
    bool LeftChannel(const char *name,Player*p)
    { 
        ChannelList* cl=&Channels[0];
        if(seperatechannels)
            cl = &Channels[p->GetTeam()];

        ChannelList::iterator i=cl->find(string(name));
        if(i!=cl->end())
        {
            Channel *ch=i->second;

            ch->Leave(p);
            if(!ch->IsConstant()  && ch->GetNumPlayers() == 0 )
            {
                cl->erase(i);
                delete ch;
            }
            return true;
        }
        return false;
    }
private:
    //team 0: aliance, team 1 horde
    bool seperatechannels;
    typedef map<string,Channel *> ChannelList;
    ChannelList Channels[2];

};

#define channelmgr ChannelMgr::getSingleton()

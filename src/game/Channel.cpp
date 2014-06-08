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

bool Channel::Join(Player *p, const char *pass)
{    
    WorldPacket data(100);
    if(IsOn(p))
    {
        MakeAlreadyOn(&data,p);
        SendToOne(&data,p);
    }
    else if(IsBanned(p->GetGUID()))
    {
        MakeYouAreBanned(&data);
        SendToOne(&data,p);
    }
    else if((password.length() > 0 && strcmp(pass,password.c_str())) || (strcmp(name.c_str(), sWorld.getGmClientChannel().c_str()) == 0 && !p->GetSession()->CanUseCommand('c')))
    {
        MakeWrongPass(&data);
        SendToOne(&data,p);
    }
    else
    {
        PlayerInfo pinfo; 
        pinfo.player = p;
        pinfo.muted = false;
        pinfo.owner = false;
        pinfo.moderator = false;

        MakeJoined(&data,p);
        p->JoinedChannel(this);
        if(announce && !m_DefaultChannel)
            SendToAll(&data);

        data.clear();
        players[p] = pinfo;

        /*MakeYouJoined(&data,p);
        SendToOne(&data,p);*/

        if(owner == NULL)
        {
            if(!constant)
            {
                SetOwner(p);
                players[p].moderator = true;
            }
            else if(p->GetSession()->CanUseCommand('c'))
            {
                SetOwner(p);
                players[p].moderator = true;
            }
        }
        return true;
    }
    return false;
}

void Channel::Leave(Player *p, bool send)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        if(send) SendToOne(&data,p);
    }
    else
    {
        bool changeowner = (owner == p);

        //MakeYouLeft(&data);
        if(send)
        {
            //SendToOne(&data,p);
            p->LeftChannel(this);
        }
        //data.clear();

        players.erase(p);
        MakeLeft(&data,p);
        if(announce && !m_DefaultChannel)
            SendToAll(&data);

        if(changeowner)
        {
            Player *newowner = players.size() > 0 ? players.begin()->second.player : NULL;
            SetOwner(newowner);
        }
    }
}

void Channel::KickOrBan(Player *good, const char *badname, bool ban)
{
    WorldPacket data(100);
    if(!IsOn(good))
    {
        MakeNotOn(&data);
        SendToOne(&data,good);
    }
    else if(!players[good].moderator && !good->GetSession()->CanUseCommand('c'))
    {
        MakeNotModerator(&data);
        SendToOne(&data,good);
    }
    else
    {
        Player *bad = objmgr.GetPlayer(badname);
        if(bad == NULL || !IsOn(bad))
        {
            MakeNotOn(&data,badname);
            SendToOne(&data,good);
        }
        else if(!good->GetSession()->CanUseCommand('c') && bad == owner && good != owner)
        {
            MakeNotOwner(&data);
            SendToOne(&data,good);
        }
        else
        {
            bool changeowner = (owner == bad);

            if(ban && !IsBanned(bad->GetGUID()))
            {
                banned.push_back(bad->GetGUID());
                MakeBanned(&data,good,bad);
            }
            else
                MakeKicked(&data,good,bad);    

            SendToAll(&data);
            players.erase(bad);

            if(changeowner)
            {
                Player *newowner = players.size() > 0 ? good : NULL;
                SetOwner(newowner);
            }
        }
    }
}

void Channel::UnBan(Player *good, const char *badname)
{
    WorldPacket data(100);
    if(!IsOn(good))
    {
        MakeNotOn(&data);
        SendToOne(&data,good);
    }
    else if(!players[good].moderator && !good->GetSession()->CanUseCommand('c'))
    {
        MakeNotModerator(&data);
        SendToOne(&data,good);
    }
    else
    {
        Player *bad = objmgr.GetPlayer(badname);
        if(bad == NULL || !IsBanned(bad->GetGUID()))
        {
            MakeNotOn(&data,badname); // Change to <Not Banned> message. Not sure what it is.
            SendToOne(&data,good);
        }
        else
        {
            banned.remove(bad->GetGUID());
            MakeUnbanned(&data,good,bad);
            SendToAll(&data);
        }
    }
}

void Channel::Password(Player *p, const char *pass)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        SendToOne(&data,p);
    }
    else if(!players[p].moderator && !p->GetSession()->CanUseCommand('c'))
    {
        MakeNotModerator(&data);
        SendToOne(&data,p);
    }
    else
    {
        password = pass;
        MakeSetPassword(&data,p);
        SendToAll(&data);
    }
}

void Channel::SetMode(Player *p, const char *p2n, bool mod, bool set)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        SendToOne(&data,p);
    }
    else if(!players[p].moderator && !p->GetSession()->CanUseCommand('c'))
    {
        MakeNotModerator(&data);
        SendToOne(&data,p);
    }
    else 
    {
        Player *newp = objmgr.GetPlayer(p2n);
//        PlayerInfo inf = players[newp];
        if(p == owner && newp == owner && mod)
            return;
        if(newp == NULL || !IsOn(newp))
        {
            MakeNotOn(&data,p2n);
            SendToOne(&data,p);
        }
        else if(owner == newp && owner != p)
        {
            MakeNotOwner(&data);
            SendToOne(&data,p);
        }
        else
        {
            if(mod)
                SetModerator(newp,set);
            else
                SetMute(newp,set);
        }
    }
}

void Channel::SetOwner(Player *p, const char *newname)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        SendToOne(&data,p);
    }
    else if(!p->GetSession()->CanUseCommand('c') && p != owner)
    {
        MakeNotOwner(&data);
        SendToOne(&data,p);
    }
    else 
    {
        Player *newp = objmgr.GetPlayer(newname);
        if(newp == NULL || !IsOn(newp))
        {
            MakeNotOn(&data,newname);
            SendToOne(&data,p);
        }
        else
        {
            MakeChangeOwner(&data,newp);
            if(!m_DefaultChannel) SendToAll(&data);

            SetModerator(newp,true);
            owner = newp;
        }
    }
}

void Channel::GetOwner(Player *p)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        SendToOne(&data,p);
    }
    else
    {
        MakeWhoOwner(&data);
        SendToOne(&data,p);
    }
}

void Channel::List(Player *p)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        SendToOne(&data,p);
    }
    else
    {
        data.Initialize(SMSG_CHANNEL_LIST);
        data << (uint8)3 << (uint32)players.size();

        PlayerList::iterator i;
        for(i = players.begin(); i!=players.end(); i++)
        {
            data << i->first->GetGUID();
            uint8 mode = 0x00;
            if(i->second.muted) 
                mode |= 0x04;
            if(i->second.moderator)
                mode |= 0x02;
            data << mode;
        }
        SendToOne(&data,p);
    }
}

void Channel::Announce(Player *p)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        SendToOne(&data,p);
    }
    else if(!players[p].moderator && !p->GetSession()->CanUseCommand('c'))
    {
        MakeNotModerator(&data);
        SendToOne(&data,p);
    }
    else
    {
        announce = !announce;
        MakeAnnounce(&data,p,announce);
        SendToAll(&data);
    }
}

void Channel::Moderate(Player *p)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        SendToOne(&data,p);
    }
    else if(!players[p].moderator && !p->GetSession()->CanUseCommand('c'))
    {
        MakeNotModerator(&data);
        SendToOne(&data,p);
    }
    else
    {
        moderate = !moderate;
        MakeModerate(&data,p,moderate);
        SendToAll(&data);
    }
}

void Channel::Say(Player *p, const char *what, Player *t)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        SendToOne(&data,p);
    }
    else if(players[p].muted)
    {
        MakeYouCantSpeak(&data);
        SendToOne(&data,p);
    }
    else if(moderate && !players[p].moderator && !p->GetSession()->CanUseCommand('c'))
    {
        MakeNotModerator(&data);
        SendToOne(&data,p);
    }
    else
    {
        //Packet structure
        //uint8      type;
        //uint32     language;
        //uint32     PVP rank
        //uint64     guid;
        //uint32      len_of_text;
        //char       text[];
        //uint8      afk_state;

        uint32 messageLength = strlen((char*)what) + 1;
//        uint8 afk = 0;

        data.Initialize(SMSG_MESSAGECHAT);
        data << (uint8)14; // CHAT_MESSAGE_CHANNEL
        data << (uint32)0; // Universal lang
        data << name.c_str();
        //data << (uint32)0; // pvp ranks
        data << p->GetGUID();
        data << messageLength;
        data << what;
        data << (uint8)(p->bGMTagOn ? 3 : 0);

        if(t == NULL)
            SendToAll(&data);
        else
            SendToOne(&data, t);
        // Send the actual talking stuff.
    }
}

void Channel::SayFromIRC(const char* msg)
{
    WorldPacket data;
    //Packet structure
    //uint8      type;
    //uint32     language;
    //uint32     PVP rank
    //uint64     guid;
    //uint32      len_of_text;
    //char       text[];
    //uint8      afk_state;

    uint32 messageLength = strlen((char*)msg) + 1;
//    uint8 afk = 0;

    data.Initialize(SMSG_MESSAGECHAT);
    data << (uint8)14; // CHAT_MESSAGE_CHANNEL
    data << (uint32)0; // Universal lang
    data << name.c_str();
    data << (uint32)0; // pvp ranks
    data << (uint64)0;  // was.. guid..
    data << messageLength;
    data << msg;
    data << (uint8)0;

    SendToAll(&data);
}

void Channel::Invite(Player *p, const char *newname)
{
    WorldPacket data(100);
    if(!IsOn(p))
    {
        MakeNotOn(&data);
        SendToOne(&data,p);
    }
    else 
    {
        Player *newp = objmgr.GetPlayer(newname);
        if(newp == NULL)
        {
            MakeNotOn(&data,newname);
            SendToOne(&data,p);
        }
        else if(IsOn(newp))
        {
            MakeAlreadyOn(&data,newp);
            SendToOne(&data,p);
        }
        else
        {
            MakeInvited(&data,p);
            SendToOne(&data,newp);
            data.clear();
            MakeYouInvited(&data,newp);
            SendToOne(&data,p);
        }
    }
}

bool Channel::IsDefaultChannel()
{
    const char* cName = name.c_str();

    if( !strncmp(cName, "General", 7)           ||
        !strncmp(cName, "Général", 9)         // Fr
        )
        return true;
    if( !strncmp(cName, "LookingForGroup", 15)  ||
        !strncmp(cName, "RechercheGroupe", 15)   // Fr
        )
        return true;
    if( !strncmp(cName, "LocalDefense", 12)     ||
        !strncmp(cName, "DéfenseLocale", 14)   // Fr
        )
        return true;
    if( !strncmp(cName, "Trade", 5)             ||
        !strncmp(cName, "Marchander", 10)       // Fr
        )
        return true;

    return false;
}

ChannelMgr::~ChannelMgr()
{
    for(int i = 0; i < 2; ++i)
    {
        ChannelList::iterator itr = this->Channels[i].begin();
        for(; itr != this->Channels[i].end(); ++itr)
        {
            delete itr->second;
        }
        Channels[i].clear();
    }
}


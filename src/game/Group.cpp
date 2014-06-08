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

// New group system written by Burlex, if you need help with anything shoot a pm.

#include "StdAfx.h"

Group::Group()
{
    m_GroupType = GROUP_TYPE_PARTY;     // Always init as party

    // Create initial subgroup
    m_SubGroups[0] = new SubGroup(this, 0);

    m_Leader = NULL;
    m_Looter = NULL;
    m_LootMethod = 0;
    m_LootThreshold = 2;
    m_SubGroupCount = 1;
    m_MemberCount = 0;

    //EventMgr::getSingleton().AddEvent(this, &Group::UpdatePositions, EVENT_GROUP_POSITION_UPDATE, 5000, 0);
    ObjectMgr::getSingleton().AddGroup(this);
}

Group::~Group()
{
    // SAFE
    //EventMgr::getSingleton().RemoveEvents(this);
    ObjectMgr::getSingleton().RemoveGroup(this);
}

SubGroup::~SubGroup()
{
    GroupMembersSet::iterator itr = m_GroupMembers.begin();
    for(; itr != m_GroupMembers.end(); ++itr)
    {
        (*itr)->SetSubGroup(NULL);
    }

    m_GroupMembers.clear();
}

bool SubGroup::HasMember(uint64 guid)
{
    for(GroupMembersSet::iterator itr = m_GroupMembers.begin(); itr != m_GroupMembers.end(); ++itr)
       if((*itr)->GetGUID()==guid) return true;

    return false;
}

void SubGroup::RemovePlayer(Player *pPlayer)
{
    GroupMembersSet::iterator itr = m_GroupMembers.find(pPlayer);
    if(itr != m_GroupMembers.end())
    {
        sLog.outDebug("GROUP: Removing player %s from subgroup %d", pPlayer->GetName(), m_Id);
        m_GroupMembers.erase(itr);
    }
    else
    {
        sLog.outDebug("GROUP: Tried to remove player %s from subgroup %d but he does not exist!", pPlayer->GetName(), m_Id);
    }
    pPlayer->SetSubGroup(NULL);
    m_Parent->m_MemberCount--;

    if(m_SubGroupLeader == pPlayer)
    {
        // Assign new subgroup leader
        if(m_GroupMembers.size() > 0)
            m_SubGroupLeader = (*m_GroupMembers.begin());
        else
            m_SubGroupLeader = NULL;
    }
}

void SubGroup::AddPlayer(Player* pPlayer)
{
    sLog.outDebug("GROUP: Adding player %s to subgroup %d", pPlayer->GetName(), m_Id);
    if(m_GroupMembers.size() == 0)
        m_SubGroupLeader = pPlayer;

    m_GroupMembers.insert(pPlayer);
    pPlayer->SetSubGroup(this);
    m_Parent->m_MemberCount++;
}

SubGroup * Group::FindFreeSubGroup()
{
    for(uint32 i = 0; i < m_SubGroupCount; i++)
        if(!m_SubGroups[i]->IsFull())
            return m_SubGroups[i];

    return NULL;
}

bool Group::AddMember(Player* pPlayer)
{
    if(!IsFull())
    {
        SubGroup* subgroup = FindFreeSubGroup();
        if(subgroup == NULL)
        {
            // weird shit..
            sLog.outDebug("GROUP: Tried to add member %s but FindFreeSubGroup returned NULL!", pPlayer->GetName());
            return false;
        }

        subgroup->AddPlayer(pPlayer);
        pPlayer->SetGroup(this);

        if(m_MemberCount == 1)
        {
            // We're the only member? Set us to the leader.
            SetLeader(pPlayer);
            // assign id
            m_Id = objmgr.GenerateGroupId();
        }        

        UpdatePositions();   // Send create object for any players that don't know who we are.
        //EventMgr::getSingleton().ModifyEventTimeLeft(this, EVENT_GROUP_POSITION_UPDATE, 5000);

        Update();    // Send group update
        
        return true;

    }
    else
    {
        return false;
    }
}

void Group::SetLeader(Player* pPlayer)
{
    m_Leader = pPlayer;
    WorldPacket *data = new WorldPacket;
    data->Initialize(SMSG_GROUP_SET_LEADER);
    *data << pPlayer->GetName();

    SendPacketToAll(data);
    delete data;

    Update();
}

void Group::Update()
{
    WorldPacket data;
    GroupMembersSet::iterator itr1, itr2;

    uint32 i=0,j=0;
    SubGroup *sg1=NULL;
    SubGroup *sg2=NULL;

    for(i=0;i<m_SubGroupCount;i++)
    {
        sg1 = m_SubGroups[i];
        for(itr1 = sg1->GetGroupMembersBegin(); itr1!=sg1->GetGroupMembersEnd(); ++itr1)
        {
            data.Initialize(SMSG_GROUP_LIST);
            data << uint8(m_GroupType);    //0=party,1=raid
            data << uint8(0);   // unk
            data << uint8(sg1->GetID());
            data << uint32(m_MemberCount-1);    // we don't include self
            for(j=0;j<m_SubGroupCount;j++)
            {
                sg2 = m_SubGroups[j];
                for(itr2 = sg2->GetGroupMembersBegin(); itr2 != sg2->GetGroupMembersEnd(); ++itr2)
                {
                    if((*itr1) != (*itr2))
                    {
                        data << (*itr2)->GetName() << (*itr2)->GetGUID();
                        data << uint8(1);                                  // online/offline flag maybe?
                        /*if(m_GroupType == GROUP_TYPE_RAID && (*itr2) == sg2->m_SubGroupLeader)
                            data << uint8(80 + sg2->GetID());
                        else*/      // how do we do this? 80 obviously doesn't work.
                            data << uint8(sg2->GetID());
                    }
                }
            }

            if(m_Leader != NULL) data << m_Leader->GetGUID();
            else data << uint64(0);

            data << uint8(m_LootMethod);

            if(m_Looter != NULL) data << m_Looter->GetGUID();
            else data << uint64(0);

            data << uint16(m_LootThreshold);

            data << uint64(0);      // new in 2.0.3, dunno what it is
            data << uint64(0);      // new in 2.0.3, dunno what it is
            
            (*itr1)->GetSession()->SendPacket(&data);
        }        
    }
}

void Group::Disband()
{
    uint32 i = 0;
    for(i = 0; i < m_SubGroupCount; i++)
    {
        SubGroup *sg = m_SubGroups[i];
        sg->Disband(true);
    }

    delete this;    // destroy ourselves, the destructor removes from eventmgr and objectmgr.
}

void SubGroup::Disband(bool bRemoveGroup)
{
    WorldPacket data;
    data.SetOpcode(SMSG_GROUP_DESTROYED);

    WorldPacket data2;
    data2.SetOpcode(SMSG_PARTY_COMMAND_RESULT);
    data2 << uint32(2) << uint8(0) << uint32(0);    // you leave the group

    GroupMembersSet::iterator itr = m_GroupMembers.begin();
    for(; itr != m_GroupMembers.end(); ++itr)
    {
        (*itr)->GetSession()->SendPacket(&data);
        (*itr)->GetSession()->SendPacket(&data2);
        (*itr)->SetSubGroup(NULL);
        m_Parent->m_MemberCount--;

        if(bRemoveGroup)
        {
            (*itr)->SetGroup(NULL);
            m_Parent->SendNullUpdate((*itr));
        }
    }

    m_Parent->m_SubGroups[m_Id] = NULL;
    delete this;
}

Player* Group::FindFirstPlayer()
{
//    SubGroup *sg = NULL;
    GroupMembersSet::iterator itr;

    uint32 i = 0;
    for(; i < m_SubGroupCount; i++)
        for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr)
            return (*itr);   // return first

    return NULL;
}

void Group::RemovePlayer(Player* pPlayer)
{
    WorldPacket data;
    
    SubGroup *sg = pPlayer->GetSubGroup();
    ASSERT(sg); // something wrong here if that isn't right

    sg->RemovePlayer(pPlayer);
    pPlayer->SetGroup(NULL);
    if(pPlayer->GetSession() != NULL)
    {
        SendNullUpdate(pPlayer);

        //pPlayer->RemoveAllAreaAuras();

        data.SetOpcode(SMSG_GROUP_DESTROYED);
        pPlayer->GetSession()->SendPacket(&data);

        data.Initialize(SMSG_PARTY_COMMAND_RESULT);
        data << uint32(2) << uint8(0) << uint32(0);  // you leave the group
        pPlayer->GetSession()->SendPacket(&data);
    }

    if(m_MemberCount < 2)
    {
        Disband();
        return;
    }

    Player *newPlayer = FindFirstPlayer();

    if(m_Looter == pPlayer)
        m_Looter = newPlayer;

    if(m_Leader == pPlayer)
        SetLeader(newPlayer);
    else
        Update();
}

void Group::SendPacketToAll(WorldPacket *packet)
{
    SendPacketToAllButOne(packet, NULL);
}

void Group::ExpandToRaid()
{
    // Very simple ;)

    uint32 i = 1;
    m_SubGroupCount = 8;

    for(; i < m_SubGroupCount; i++)
        m_SubGroups[i] = new SubGroup(this, i);

    m_GroupType = GROUP_TYPE_RAID;
    Update();
}

void Group::SetLooter(Player *pPlayer, uint8 method, uint16 threshold)
{ 
    m_LootMethod = method;
    m_Looter = pPlayer;
    m_LootThreshold  = threshold;
    Update();
}

void Group::UpdatePositions()
{
    uint8 i=0,j=0;

    WorldPacket data;
    Player *plr1 = NULL;
    Player *plr2 = NULL;

    GroupMembersSet::iterator itr1, itr2;

    for(i = 0; i < m_SubGroupCount; i++)
    {
        for(itr1 = m_SubGroups[i]->GetGroupMembersBegin(); itr1 != m_SubGroups[i]->GetGroupMembersEnd(); ++itr1)
        {
            plr1 = (*itr1);
            for(j = 0; j < m_SubGroupCount; j++)
            {
                for(itr2 = m_SubGroups[j]->GetGroupMembersBegin(); itr2 != m_SubGroups[j]->GetGroupMembersEnd(); ++itr2)
                {
                    plr2 = (*itr2);
                    if(plr1 == plr2) continue;

                    // burlex fix: check mapids. if they are different, ignore.
                    if(plr1->m_CurrentTransporter && plr1->m_CurrentTransporter == plr2->m_CurrentTransporter && 
                        (plr1->GetMapMgr() == NULL || plr2->GetMapMgr() == NULL) ||
                        (plr1->GetMapMgr() != plr2->GetMapMgr()) || 
                        !plr2->IsInWorld())
                        continue;

                    bool bInRange = plr1->IsInRangeSet(plr2);
                    bool bKnowsHim = plr1->KnowsGroupMember(plr2); 
                    if(bInRange && !bKnowsHim)
                    {
                        bKnowsHim = true;
                        plr1->m_KnownGroupMembers.insert(plr2);
                        continue;   // we can skip the rest, as mapmgr handles updates here.
                    }

                    if(!bInRange && bKnowsHim)   // player 1 cannot see player 2
                    {
                        // this _should_ work.. but VERY hacky

                        data.Initialize(SMSG_MONSTER_MOVE);
                        data << plr2->GetNewGUID();
                        data << plr2->GetPositionX();
                        data << plr2->GetPositionY();
                        data << plr2->GetPositionZ();
                        data << getMSTime();
                        data << uint8(0x00);
                        data << uint32(0x00000100);

                        data << uint32(100);
                        data << uint32(1);
                        data << plr2->GetPositionX() << plr2->GetPositionY() << plr2->GetPositionZ();

                        plr1->GetSession()->SendPacket(&data);

                    }
                    else if(!bInRange && !bKnowsHim)
                    {
                        // Send A9 Create
                        //udata.Clear();
                        /*plr2->BuildCreateUpdateBlockForPlayer(&udata, plr1);
                        data.clear();
                        udata.BuildPacket(&data);
                        plr1->GetSession()->SendPacket(&data);*/
                        ByteBuffer buf(2500);
                        uint32 count;
                        count = plr2->BuildCreateUpdateBlockForPlayer(&buf, plr1);
                        plr1->PushUpdateData(&buf, count);

                        plr1->m_KnownGroupMembers.insert(plr2);
                    }
                }
            }
        }
    }
}

void Group::SendPacketToAllButOne(WorldPacket *packet, Player *pSkipTarget)
{
    GroupMembersSet::iterator itr;
    uint32 i = 0;
    for(; i < m_SubGroupCount; i++)
        for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr)
            if((*itr) != pSkipTarget) (*itr)->GetSession()->SendPacket(packet);
}

bool Group::HasMember(Player * pPlayer)
{
    GroupMembersSet::iterator itr;
    uint32 i = 0;
    for(; i < m_SubGroupCount; i++)
        for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr)
            if((*itr) == pPlayer) return true;

    return false;
}

void Group::MovePlayer(Player *pPlayer, uint8 subgroup)
{
    ASSERT(subgroup < m_SubGroupCount);
    SubGroup *sgr = pPlayer->GetSubGroup();
    
    sgr->RemovePlayer(pPlayer);

    // Grab the new group, and insert
    sgr = m_SubGroups[subgroup];
    sgr->AddPlayer(pPlayer);

    Update();
}

void Group::SetSubGroupLeader(Player *pPlayer, uint8 subgroup)
{
    sLog.outString("Set subgroup %d leader to %s", (uint32)subgroup, pPlayer->GetName());
    ASSERT(subgroup < m_SubGroupCount);
    SubGroup *group = m_SubGroups[subgroup];
    if(group != pPlayer->GetSubGroup())
    {
        sLog.outString("GROUP: Tried to set leader of subgroup %d and player %s is not in that group", group->GetID(), pPlayer->GetName());
        return;
    }

    group->m_SubGroupLeader = pPlayer;
    Update();
}

void Group::SendNullUpdate(Player *pPlayer)
{
    WorldPacket data;

    data.Initialize(SMSG_GROUP_LIST);
    data << uint8(0);
    data << uint8(0);
    data << uint32(0);
    data << uint64(0);
    pPlayer->GetSession()->SendPacket(&data);
}

// player is object class becouse its called from unit class
void Group::SendPartyKillLog(Object * player, Object * Unit)
{
    if (!player || !Unit || !HasMember(((Player*)player)))
        return;

    WorldPacket data(16);
    data.Initialize(SMSG_PARTYKILLLOG);
    data << player->GetGUID();
    data << Unit->GetGUID();
    SendPacketToAll(&data);
}

void Group::LoadFromDB(Field *fields)
{
    m_Id = fields[0].GetUInt32();
    uint64 leader = fields[1].GetUInt64();
    uint64 looter = fields[2].GetUInt64();
    m_LootMethod = fields[3].GetUInt32();
    m_LootThreshold = fields[4].GetUInt32();
    m_GroupType = fields[5].GetUInt32();
    m_SubGroupCount = fields[6].GetUInt32();
    // create groups
    for(int i = 1; i < m_SubGroupCount; ++i)
        m_SubGroups[i] = new SubGroup(this, i);

    // assign players into groups
    for(int i = 0; i < m_SubGroupCount; ++i)
    {
        std::string memberguids = fields[7+i].GetString();
    }
}


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

void WorldSession::HandleTaxiNodeStatusQueryOpcode( WorldPacket & recv_data )
{
    sLog.outDebug( "WORLD: Recieved CMSG_TAXINODE_STATUS_QUERY" );

    uint64 guid;
    uint32 curloc;
    uint8 field;
    uint32 submask;

    recv_data >> guid;

    curloc = sTaxiMgr.GetNearestTaxiNode( GetPlayer( )->GetPositionX( ), GetPlayer( )->GetPositionY( ), GetPlayer( )->GetPositionZ( ), GetPlayer( )->GetMapId( ) );

    field = (uint8)((curloc - 1) / 32);
    submask = 1<<((curloc-1)%32);

    WorldPacket data;
    data.Initialize( SMSG_TAXINODE_STATUS );
    data << guid;

    // Check for known nodes
    if ( (GetPlayer( )->GetTaximask(field) & submask) != submask )
    {   
        data << uint8( 0 );    
    }
    else
    {
        data << uint8( 1 );
    }    

    SendPacket( &data );
    sLog.outDebug( "WORLD: Sent SMSG_TAXINODE_STATUS" );
}


void WorldSession::HandleTaxiQueryAvaibleNodesOpcode( WorldPacket & recv_data )
{
    sLog.outDebug( "WORLD: Recieved CMSG_TAXIQUERYAVAILABLENODES" );
    uint64 guid;
    recv_data >> guid;
    Creature *pCreature = World::GetCreature(guid);
    if(!pCreature) return;

    SendTaxiList(pCreature);
    
    
}

void WorldSession::SendTaxiList(Creature* pCreature)
{
    uint32 curloc;
    uint8 field;
    uint32 TaxiMask[8];
    uint32 submask;
    uint64 guid = pCreature->GetGUID();

    curloc = pCreature->m_TaxiNode;
    if (!curloc)
        return;

    field = (uint8)((curloc - 1) / 32);
    submask = 1<<((curloc-1)%32);

    // Check for known nodes
    if (!(GetPlayer( )->GetTaximask(field) & submask))
    {
        GetPlayer()->SetTaximask(field, (submask | GetPlayer( )->GetTaximask(field)) );

        OutPacket(SMSG_NEW_TAXI_PATH);

        //Send packet
        WorldPacket update(SMSG_TAXINODE_STATUS, 9);
        update << guid << uint8( 1 );
        SendPacket( &update );
    }

    //Set Mask
    memset(TaxiMask, 0, sizeof(uint32)*8);
    sTaxiMgr.GetGlobalTaxiNodeMask(curloc, TaxiMask);
    TaxiMask[field] |= 1 << ((curloc-1)%32);

    //Remove nodes unknown to player
    for(int i = 0; i < 8; i++)
    {
        TaxiMask[i] &= GetPlayer( )->GetTaximask(i);
    }

    WorldPacket data;
    data.Initialize( SMSG_SHOWTAXINODES );
    data << uint32( 1 ) << guid;
    data << uint32( curloc );
    for(int i = 0; i < 8; i++)
    {
        data << TaxiMask[i];
    }
    SendPacket( &data );

    sLog.outDebug( "WORLD: Sent SMSG_SHOWTAXINODES" );
}

void WorldSession::HandleActivateTaxiOpcode( WorldPacket & recv_data )
{
    sLog.outDebug( "WORLD: Recieved CMSG_ACTIVATETAXI" );

    uint64 guid;
    uint32 sourcenode, destinationnode;
    int32 newmoney;
    uint32 curloc;
    uint8 field;
    uint32 submask;
    WorldPacket data(SMSG_ACTIVATETAXIREPLY, 4);

    recv_data >> guid >> sourcenode >> destinationnode;

    if( GetPlayer( )->GetUInt32Value( UNIT_FIELD_FLAGS ) & U_FIELD_FLAG_LOCK_PLAYER )
        return;

    TaxiPath* taxipath = sTaxiMgr.GetTaxiPath(sourcenode, destinationnode);
    TaxiNode* taxinode = sTaxiMgr.GetTaxiNode(sourcenode);

    curloc = taxinode->id;
    field = (uint8)((curloc - 1) / 32);
    submask = 1<<((curloc-1)%32);

    // Check for known nodes
    if ( (GetPlayer( )->GetTaximask(field) & submask) != submask )
    {   
        data << uint32( 1 );
        SendPacket( &data );
        return;
    }

    // Check for valid node
    if (!taxinode)
    {
        data << uint32( 1 );
        SendPacket( &data );
        return;
    }

    if (!taxipath || !taxipath->GetNodeCount())
    {
        data << uint32( 2 );
        SendPacket( &data );
        return;
    }

    // Check for gold
    newmoney = ((GetPlayer()->GetUInt32Value(PLAYER_FIELD_COINAGE)) - taxipath->GetPrice());
    if(newmoney < 0 )
    {
        data << uint32( 3 );
        SendPacket( &data );
        return;
    }

    // MOUNTDISPLAYID
    // bat: 1566
    // gryph: 1147
    // wyvern: 295
    // hippogryph: 479

    uint32 modelid =0;
    if( _player->GetTeam() )
    {
        if( taxinode->horde_mount == 2224 )
            modelid =295; // In case it's a wyvern
        else
            modelid =1566; // In case it's a bat or a bad id
    }
    else
    {
        if( taxinode->alliance_mount == 3837 )
            modelid =479; // In case it's an hippogryph
        else
            modelid =1147; // In case it's a gryphon or a bad id
    }

    //GetPlayer( )->setDismountCost( newmoney );

    data << uint32( 0 );
    // 0 Ok
    // 1 Unspecified Server Taxi Error
    // 2.There is no direct path to that direction
    // 3 Not enough Money
    SendPacket( &data );
    sLog.outDebug( "WORLD: Sent SMSG_ACTIVATETAXIREPLY" );

    // 0x001000 seems to make a mount visible
    // 0x002000 seems to make you sit on the mount, and the mount move with you
    // 0x000004 locks you so you can't move, no msg_move updates are sent to the server
    // 0x000008 seems to enable detailed collision checking

    // check for a summon -> if we do, remove.
    if(_player->GetSummon() != NULL)
    {
        if(_player->GetSummon()->GetUInt32Value(UNIT_CREATED_BY_SPELL) > 0)
            _player->GetSummon()->Dismiss(false);                           // warlock summon -> dismiss
        else
            _player->GetSummon()->Remove(false, true);                      // hunter pet -> just remove for later re-call
    }

    GetPlayer()->TaxiStart(taxipath, modelid, 0);
    
    //sLog.outString("TAXI: Starting taxi trip. Next update in %d msec.", first_node_time);
}

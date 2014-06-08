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

//////////////////////////////////////////////////////////////
/// This function handles CMSG_SKILL_LEVELUP
//////////////////////////////////////////////////////////////

/*void WorldSession::HandleSkillLevelUpOpcode( WorldPacket & recv_data )
{
WorldPacket data;
uint32 slot, skill_id, amount, current_points, current_skill, points;
recv_data >> slot >> skill_id >> amount;
current_points = GetPlayer( )->GetUInt32Value( PLAYER_SKILL_INFO_1_1+slot+1 );
current_skill = GetPlayer( )->GetUInt32Value( PLAYER_SKILL_INFO_1_1+slot );
points = GetPlayer( )->GetUInt32Value( PLAYER_CHARACTER_POINTS2 );
GetPlayer( )->SetUInt32Value( PLAYER_SKILL_INFO_1_1+slot , ( 0x000001a1 ));
GetPlayer( )->SetUInt32Value( PLAYER_SKILL_INFO_1_1+slot+1 , ( (current_points & 0xffff) + (amount << 16) ) );
GetPlayer( )->SetUInt32Value( PLAYER_CHARACTER_POINTS2, points-amount );
GetPlayer( )->UpdateObject( );
}*/
/*
struct TalentEntry
{
    uint32 Id;
    uint32 treeId;
    uint32 deeplvl;
    uint32 reqId;
    uint32 reqflags;
    uint32 reqpoints;
    uint32 rankId[5];
    uint32 notcast;
};

TalentEntry *getFirstTalentinfoFromQuery(std::stringstream *ss)
{
    QueryResult *result = sDatabase.Query( ss->str().c_str() );
    if(!result) return NULL;

    Field *fields = result->Fetch();

    TalentEntry *tt = new TalentEntry;
    tt->Id = fields[0].GetUInt32();
    tt->treeId = fields[1].GetUInt32();
    tt->deeplvl = fields[2].GetUInt32();
    tt->reqId = fields[3].GetUInt32();
        
    for(int i=0;i<5;i++)
        tt->rankId[i] = fields[i+4].GetUInt32();

    tt->reqflags = fields[9].GetUInt32(); 
    tt->reqpoints = fields[10].GetUInt32();
    tt->notcast = fields[11].GetUInt32();
    
    return tt;
}*/

void WorldSession::HandleLearnTalentOpcode( WorldPacket & recv_data )
{
   /* WorldPacket data;
    uint32 talent_id, requested_rank;

    recv_data >> talent_id >> requested_rank; 

    std::stringstream ss;
    ss << "SELECT * FROM talents WHERE Id=" << talent_id;

    TalentEntry *talentInfo = getFirstTalentinfoFromQuery(&ss);
    if(!talentInfo) return;

    uint32 CurTalentPoints =  GetPlayer()->GetUInt32Value(PLAYER_CHARACTER_POINTS1); 
    if(CurTalentPoints == 0) return; 


    uint32 spellid = talentInfo->rankId[requested_rank];
    if( spellid == 0 || requested_rank > 4) 
    { 
        sLog.outDebug("Talent: %d Rank: %d = 0", talent_id, requested_rank); 
    }
    else if(GetPlayer()->getTalentpointsByTreeId(talentInfo->treeId) < talentInfo->deeplvl*5)
    {
        sLog.outDebug("Not enought talent points in tree: %d to learn that talent",talentInfo->treeId); 
    }
    else 
    { 
        if(talentInfo->reqflags)
        {
            std::stringstream ss1;
            ss1 << "SELECT * FROM talents where Id != " << talentInfo->Id << " AND treeId = " << talentInfo->treeId << " AND deeplvl < " << talentInfo->deeplvl << " AND reqId = " << talentInfo->reqId << " ORDER BY deeplvl DESC LIMIT 1 ";
            TalentEntry *tmptinfo = getFirstTalentinfoFromQuery(&ss1);
            if(tmptinfo)
            {
                uint32 rankspell = tmptinfo->rankId[talentInfo->reqpoints];
                if(!(GetPlayer( )->HasSpell(rankspell)))
                {
                    sLog.outDebug("Player requires talent: %d to learn this talent",rankspell);
                    return;
                }     
            }
        }
       

        if(!(GetPlayer( )->HasSpell(spellid)))
        {
            //Unlearn old Ranks
            if(requested_rank > 0)
            {
                uint32 oldrankspell = talentInfo->rankId[requested_rank-1];
                if(GetPlayer()->HasSpell(oldrankspell))
                {
                    GetPlayer()->removeSpell(oldrankspell);
                    GetPlayer()->RemoveTalent(oldrankspell);
        
                    // Already handled in removespell.
                    //data.Initialize(SMSG_REMOVED_SPELL);
                    //data << oldrankspell; 
                    //SendPacket( &data );
                    sLog.outDetail("Removed Old Rank TalentID: %d", oldrankspell);
                }
            }
            
            //Send data if all OK 
            sLog.outDetail("TalentID: %d Rank: %d Spell: %d", talent_id, requested_rank, spellid); 
        
            GetPlayer()->addSpell(spellid);
            GetPlayer()->AddTalent(spellid);
            
            if(!talentInfo->notcast)
            {
                GetPlayer()->GetSession()->SendPacket(&data);
                SpellEntry *spellInfo = sSpellStore.LookupEntry(spellid);
                if(!spellInfo) return;
                Spell *spell = new Spell(GetPlayer( ), spellInfo,false,NULL);
                WPAssert(spell);

                SpellCastTargets targets;
                targets.m_unitTarget = GetPlayer( )->GetGUID();
                spell->prepare(&targets);
            }
             //Update Talent Points 
            GetPlayer()->SetUInt32Value(PLAYER_CHARACTER_POINTS1, CurTalentPoints - 1); 
            GetPlayer()->SetBaseUInt32Value(PLAYER_CHARACTER_POINTS1, CurTalentPoints - 1); //saving
            GetPlayer()->IncrTalentpointsByTreeId(talentInfo->treeId);
        }
    } */
     
    WorldPacket data;
    uint32 talent_id, requested_rank;
    recv_data >> talent_id >> requested_rank;

    uint32 CurTalentPoints =  GetPlayer()->GetUInt32Value(PLAYER_CHARACTER_POINTS1);
    if(CurTalentPoints == 0)
        return;

    if (requested_rank > 4)
        return;

    unsigned int numRows = sTalentStore.GetNumRows();
    TalentEntry *talentInfo=NULL ;
    for (unsigned int i = 0; i < numRows; i++)          // Loop through all talents.
    {
        TalentEntry *t= sTalentStore.LookupEntry( i );
        if(t->TalentID==talent_id)
        {
            talentInfo=t;
            break;
        }
    }
    if(!talentInfo)return;
  
    Player * player = GetPlayer();

    // Check if it requires another talent
    if (talentInfo->DependsOn > 0)
    {
        TalentEntry *depTalentInfo ;
        for (unsigned int i = 0; i < numRows; i++)          // Loop through all talents.
        {
            TalentEntry *t= sTalentStore.LookupEntry( i );
            if(t->TalentID==talentInfo->DependsOn)
            {
                depTalentInfo=t;
                break;
            }
        }
        bool hasEnoughRank = false;
        for (int i = talentInfo->DependsOnRank; i < 5; i++)
        {
            if (depTalentInfo->RankID[i] != 0)
            {
                if (player->HasSpell(depTalentInfo->RankID[i]))
                {
                    hasEnoughRank = true;
                    break;
                }
            }
        }
        if (!hasEnoughRank)
            return;
    }

    // Find out how many points we have in this field
    uint32 spentPoints = 0;

    uint32 tTree = talentInfo->TalentTree;
    if (talentInfo->Row > 0)
    {
        for (unsigned int i = 0; i < numRows; i++)          // Loop through all talents.
        {
            // Someday, someone needs to revamp
            TalentEntry *tmpTalent = sTalentStore.LookupEntry(i);
            if (tmpTalent)                                  // the way talents are tracked
            {
                if (tmpTalent->TalentTree == tTree)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        if (tmpTalent->RankID[j] != 0)
                        {
                            if (player->HasSpell(tmpTalent->RankID[j]))
                            {
                                spentPoints += j + 1;
                            //    break;
                            }
                        }
                        else 
                            break;
                    }
                }
            }
        }
    }

    uint32 spellid = talentInfo->RankID[requested_rank];
    if( spellid == 0 )
    {
        sLog.outDetail("Talent: %u Rank: %u = 0", talent_id, requested_rank);
    }
    else
    {
        if(spentPoints < (talentInfo->Row * 5))             // Min points spent
        {
            return;
        }

        if(!(GetPlayer( )->HasSpell(spellid)))
        {
            GetPlayer( )->addSpell(spellid);            
    
            SpellEntry *spellInfo = sSpellStore.LookupEntry( spellid );     
            
            if(requested_rank > 0 )
            {
                uint32 respellid = talentInfo->RankID[requested_rank-1];
                if(respellid)
                {
                    _player->removeSpell(respellid, false, false, 0);
                }

            }
            if(spellInfo->Attributes & ATTRIBUTES_PASSIVE || (spellInfo->Effect[0] == SPELL_EFFECT_LEARN_SPELL ||
                                                              spellInfo->Effect[1] == SPELL_EFFECT_LEARN_SPELL ||
                                                              spellInfo->Effect[2] == SPELL_EFFECT_LEARN_SPELL))
            {
                Spell*sp=new Spell(_player,spellInfo,true,NULL);
                SpellCastTargets tgt;
                tgt.m_unitTarget=_player->GetGUID();
                sp->prepare(&tgt);
            }

            _player->SetUInt32Value(PLAYER_CHARACTER_POINTS1, CurTalentPoints-1);
            _player->SetBaseUInt32Value(PLAYER_CHARACTER_POINTS1, CurTalentPoints-1);

        }
    }
}

void WorldSession::HandleUnlearnTalents( WorldPacket & recv_data )
{
    uint32 playerGold = GetPlayer()->GetUInt32Value( PLAYER_FIELD_COINAGE );
    uint32 price = GetPlayer()->CalcTalentResetCost(GetPlayer()->GetTalentResetTimes());

    if( playerGold < price ) return;

    GetPlayer()->SetTalentResetTimes(GetPlayer()->GetTalentResetTimes() + 1);
    GetPlayer()->SetUInt32Value( PLAYER_FIELD_COINAGE, playerGold - price );
    GetPlayer()->Reset_Talents();

/*    // Unlearn all talent spells
    WorldPacket data;
    std::list<uint32> *talentproto = GetPlayer()->getTalentproto();

    std::list<uint32>::iterator itr;
    for (itr = talentproto->begin(); itr != talentproto->end(); ++itr)
    {
        GetPlayer()->removeSpell((*itr));

        // Handled in removespell.
        //data.Initialize(SMSG_REMOVED_SPELL);
        //data << (*itr); 
        //SendPacket( &data );
    }
    talentproto->clear();
*/    
}

void WorldSession::HandleUnlearnSkillOpcode(WorldPacket& recv_data)
{
    uint32 skill_line;
    recv_data >> skill_line;

    // Cheater detection
    // if(!_player->HasSkillLine(skill_line)) return;

    // Remove any spells within that line that the player has
    _player->RemoveSpellsFromLine(skill_line);
    
    // Finally, remove the skill line.
    _player->RemoveSkillLine(skill_line);
}

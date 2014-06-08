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

void WorldSession::HandleUseItemOpcode(WorldPacket& recvPacket)
{
    typedef std::list<Aura*> AuraList;
    WorldPacket data;
    Player* p_User = GetPlayer();
    sLog.outDetail("WORLD: got use Item packet, data length = %i",recvPacket.size());
    int8 tmp1,slot,tmp3;
    uint32 spellId = 0;

    recvPacket >> tmp1 >> slot >> tmp3;
    Item* tmpItem = NULL;
    tmpItem = p_User->GetItemInterface()->GetInventoryItem(tmp1,slot);
    if (!tmpItem)
        tmpItem = p_User->GetItemInterface()->GetInventoryItem(slot);
    if (!tmpItem)
        return;
    ItemPrototype *itemProto = tmpItem->GetProto();
    if(!itemProto)
        return;

    if(itemProto->Bonding == ITEM_BIND_ON_USE)
        tmpItem->SoulBind();

    if(itemProto->QuestId)
    {
        // Item Starter
        Quest *qst = sQuestMgr.FindQuest(itemProto->QuestId);
        if(!qst) 
            return;
        sQuestMgr.BuildQuestDetails(&data, qst, tmpItem, 0);
        SendPacket(&data);
    }
    
    SpellCastTargets targets(recvPacket, _player->GetGUID());
    for(int x = 0; x < 5; x++)
    {
        if(itemProto->SpellTrigger[x] == USE)
        {
            if(itemProto->SpellId[x])
            {
                spellId = itemProto->SpellId[x];
                break;
            }
        }
    }
    if(!spellId)
        return;
    // check for spell id
    SpellEntry *spellInfo = sSpellStore.LookupEntry( spellId );

    if(!spellInfo)
    {
        sLog.outError("WORLD: unknown spell id %i\n", spellId);
        return;
    }

    if (spellInfo->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP)
    {
        if (p_User->isInCombat())
        {
            _player->GetItemInterface()->BuildInventoryChangeError(tmpItem,NULL,INV_ERR_CANT_DO_IN_COMBAT);
            return;
        }
        if(p_User->GetStandState()!=1)
        p_User->SetStandState(STANDSTATE_SIT);
        // loop through the auras and removing existing eating spells
    }
    
    if (spellInfo->Category > 1) // 0 and -1 a.k.a. infinite not included
    {
        ItemCooldown * item = new ItemCooldown;
       
        for(uint32 i = 0; i < 5; i++) 
        {
            if(itemProto->SpellId[i])
            {
                if(itemProto->SpellTrigger[i] == USE)
                {
                    item->ItemEntry = itemProto->ItemId;
                    item->SpellID = spellId;
                    item->SpellCategory = spellInfo->Category;
                    if(itemProto->SpellCategory[i])
                    {
                        item->Cooldown = itemProto->SpellCategoryCooldown[i];
                        item->CooldownTimeStamp = now() + itemProto->SpellCategoryCooldown[i];
                    }
                    else
                    {
                        item->Cooldown = itemProto->SpellCooldown[i];
                        item->CooldownTimeStamp = now() + itemProto->SpellCooldown[i];
                    }
                }
            }
        }

        _player->m_itemcooldown.insert(item);
    }

    Spell *spell = new Spell(_player, spellInfo, false, NULL);
    spell->i_caster = tmpItem;
    //GetPlayer()->setCurrentSpell(spell);
    spell->prepare(&targets);
}

void WorldSession::HandleCastSpellOpcode(WorldPacket& recvPacket)
{
    uint32 spellId;

    recvPacket >> spellId;

    sLog.outDetail("WORLD: got cast spell packet, spellId - %i, data length = %i",
        spellId, recvPacket.size());
	
    // Cheat Detection only if player and not from an item
    // this could fuck up things but meh it's needed ALOT of the newbs are using WPE now
    // WPE allows them to mod the outgoing packet and basicly choose what ever spell they want :(

    if( !GetPlayer()->HasSpell(spellId) )
    {
        sLog.outDetail("WORLD: Spell isn't casted because player \"%s\" is cheating", GetPlayer()->GetName());
        return;
    }

    if (GetPlayer()->GetOnMeleeSpell() != spellId)
    {
        // check for spell id
        SpellEntry *spellInfo = sSpellStore.LookupEntry(spellId );

        if(!spellInfo)
        {
            sLog.outError("WORLD: unknown spell id %i\n", spellId);
            return;
        }
        //autoshot 75
        if((spellInfo->Flags3 & FLAGS3_ACTIVATE_AUTO_SHOT) /*spellInfo->Attributes == 327698*/)    // auto shot..
        {
            Item *weapon = GetPlayer()->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
            if(!weapon) 
                return;
            uint32 spellid;
            switch(weapon->GetProto()->SubClass)
            {
            case 2:             // bows
                spellid = SPELL_RANGED_BOW;
                break;
            case 3:             // guns
                spellid = SPELL_RANGED_GUN;
                break;
            case 16:            // thrown
                spellid = SPELL_RANGED_THROW;
                break;
            case 18:            // crossbow
                spellid = SPELL_RANGED_CROSSBOW;
                break;
            case 19:            // wands
                spellid = SPELL_RANGED_WAND;
                break;
            default:
                spellid = 0;
                break;
            }
           
            if(!spellid) 
                spellid = spellInfo->Id;
             
            sEventMgr.RemoveEvents(_player, EVENT_PLAYER_REPEAT_SHOT);
            _player->m_AutoShotStartX = _player->GetPositionX();
            _player->m_AutoShotStartY = _player->GetPositionY();
            _player->m_AutoShotStartZ = _player->GetPositionZ();
            _player->m_AutoShotTarget = _player->GetSelection();
            //_player->EventRepeatSpell(spellid);
            uint32 duration = _player->GetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME);
            SpellCastTargets targets(recvPacket,GetPlayer()->GetGUID());
            if(!targets.m_unitTarget)
                return;
            SpellEntry *sp = sSpellStore.LookupEntry(spellid);
        
            sEventMgr.AddEvent(_player, &Player::EventRepeatSpell, sp, duration, targets.m_unitTarget, EVENT_PLAYER_REPEAT_SHOT, duration, 0);

            // modify the event time -> first shot doesnt have a delay
            sEventMgr.ModifyEventTimeLeft(_player, EVENT_PLAYER_REPEAT_SHOT, 500);

            return;
        }
        /*const char * name = sSpellStore.LookupString(spellInfo->Name);
        if(name)
            sChatHandler.SystemMessageToPlr(_player, "%sSpell Cast:%s %s %s[Group %u, family %u]", MSG_COLOR_LIGHTBLUE,
            MSG_COLOR_SUBWHITE, name, MSG_COLOR_YELLOW, spellInfo->SpellGroupType, spellInfo->SpellFamilyName);*/

        Spell *spell = new Spell(GetPlayer(), spellInfo, false, NULL);
    
        SpellCastTargets targets(recvPacket,GetPlayer()->GetGUID());       
        spell->prepare(&targets);
    }
}

void WorldSession::HandleCancelCastOpcode(WorldPacket& recvPacket)
{
    uint32 spellId;
    recvPacket >> spellId;

    if(GetPlayer()->m_currentSpell)
        GetPlayer()->m_currentSpell->cancel();
}

void WorldSession::HandleCancelAuraOpcode( WorldPacket& recvPacket)
{
    uint32 spellId;
    //uint64 guid;
    recvPacket >> spellId;
    Player *plyr = GetPlayer();
    
    if(plyr && plyr->GetCurrentBattleground() != NULL && spellId == (23333+(plyr->m_bgTeam*2)))
        plyr->GetCurrentBattleground()->HandleBattlegroundEvent(plyr, NULL, BGEVENT_WSG_PLAYER_DIED_WITH_FLAG);
    else
    {
        for(uint32 x = 0; x < MAX_AURAS+MAX_POSITIVE_AURAS; ++x)
        {
            if(_player->m_auras[x] && _player->m_auras[x]->IsPositive() && _player->m_auras[x]->GetSpellId() == spellId)
                _player->m_auras[x]->Remove();
        }
    }
    sLog.outDebug("removing aura %u",spellId);
}

void WorldSession::HandleCancelChannellingOpcode( WorldPacket& recvPacket)
{
    uint32 spellId;
    //uint32 guid ;
    recvPacket >> spellId;

    Player *plyr = GetPlayer();
    if(!plyr)
        return;
    if(plyr->m_currentSpell)
    {        
        plyr->m_currentSpell->cancel();
    }
}

void WorldSession::HandleCancelAutoRepeatSpellOpcode(WorldPacket& recv_data)
{
    //sLog.outString("Recieved CMSG_CANCEL_AUTO_REPEAT_SPELL message.");

    sEventMgr.RemoveEvents(GetPlayer(), EVENT_PLAYER_REPEAT_SHOT);
}
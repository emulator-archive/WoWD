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

#ifndef __STDAFX_H
#define __STDAFX_H

#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <string>
//#include <fstream>


#include "../shared/Common.h"
#include "../shared/MersenneTwister.h"
#include "../shared/WorldPacket.h"
#include "../shared/Log.h"
#include "../shared/ByteBuffer.h"
#include "../shared/Config/ConfigEnv.h"
#include "../shared/crc32.h"
#include "../shared/LocationVector.h"

#include <zlib/zlib.h>

#include "../shared/Database/DatabaseEnv.h"
#include "../shared/Database/DBCStores.h"
#include "../shared/Database/dbcfile.h"

#include <NetworkBase/BaseSocket.h>
#include <NetworkBase/SocketHolder.h>

#include "../shared/Auth/md5.h"
#include "../shared/Auth/BigNumber.h"
#include "../shared/Auth/Sha1.h"
#include "../shared/Auth/WowCrypt.h"
#include "../shared/CrashHandler.h"

#include "UpdateFields.h"
#include "UpdateMask.h"
#include "Opcodes.h"
#include "../shared/CallBack.h"
#include "EventMgr.h"
#include "EventableObject.h"
#include "Object.h"
#include "LootMgr.h"
#include "Unit.h"
#include "ObjectHolder.h"

#include "AddonMgr.h"
#include "AIInterface.h"
#include "AreaTrigger.h"
#include "BattlegroundMgr.h"
#include "AlteracValley.h"
#include "ArathiBasin.h"
#include "Arenas.h"
#include "CellHandler.h"
#include "Chat.h"
#include "Corpse.h"
#include "Quest.h"
#include "QuestMgr.h"
#include "Creature.h"
#include "DynamicObject.h"
#include "GameObject.h"
#include "Group.h"
#include "Guild.h"
#include "HonorHandler.h"
#include "ItemPrototype.h"
#include "Item.h"
#include "Container.h"
#include "AuctionHouse.h"
#include "AuctionMgr.h"
#include "LfgMgr.h"
#include "MailSystem.h"
#include "Map.h"
#include "MapCell.h"
#include "TerrainMgr.h"
#include "MapMgr.h"
#include "MapScriptInterface.h"
#include "MiscHandler.h"
#include "NameTables.h"
#include "NPCHandler.h"
#include "ObjectTemplate.h"
#include "Pet.h"
#include "WorldSocket.h"
#include "WorldSession.h"
#include "Player.h"
#include "faction.h"
#include "Professions.h"
#include "Skill.h"
#include "SocialMgr.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "StdAfx.h"
#include "TaxiMgr.h"
#include "TemplateCell.h"
#include "TemplateMgr.h"
#include "TransporterHandler.h"
#include "WarsongGulch.h"
#include "WeatherMgr.h"
#include "World.h"
#include "ItemInterface.h"
#include "Stats.h"
#include "WorldCreator.h"

#include "ObjectMgr.h"

#include "WowdThreads.h"
#include "ThreadMgr.h"
#include "ScriptMgr.h"

#include "PluginOperations.h"

#include "Channel.h"
#include "ChannelMgr.h"

#include "LogonCommClient.h"
#include "LogonCommHandler.h"
#include "../wowd/MainServerDefines.h"
#include "WorldRunnable.h"

#endif

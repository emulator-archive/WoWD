// Copyright (C) 2004 WoW Daemon
// Copyright (C) 2006 Burlex
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

#ifndef __ADDONMGR_H
#define __ADDONMGR_H

struct AddonEntry
{
    std::string name;
    uint64 crc;
    bool banned;
    bool isNew;
    bool showinlist;
};

class AddonMgr :  public Singleton < AddonMgr >
{
public:
    AddonMgr();
    ~AddonMgr();

    void LoadFromDB();
    void SaveToDB();

    void SendAddonInfoPacket(WorldPacket *source, uint32 pos, WorldSession *m_session);
    bool AppendPublicKey(WorldPacket& data, string AddonName, uint32 CRC);

private:
    std::map<std::string, AddonEntry*> KnownAddons;
    map<string, ByteBuffer> AddonData;

    bool IsAddonBanned(uint64 crc, std::string name = "");
    bool IsAddonBanned(std::string name, uint64 crc = 0);
    bool ShouldShowInList(std::string name);
};

#define sAddonMgr AddonMgr::getSingleton()

#endif

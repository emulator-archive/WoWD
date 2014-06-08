#include "StdAfx.h"

void ScriptMgr::SpawnThread(WowdThread * thread)
{
    launch_thread(thread);
}

NameTableEntry g_pluginOpcodeNames[] = {
    { PSMSG_CHAT,   "PSMSG_CHAT" },
    { PCMSG_ANNOUNCE, "PCMSG_ANNOUNCE" },
    /*{ PCMSG_BAN, "PCMSG_BAN" },
    { PCMSG_KICK, "PCMSG_KICK" },
    { PCMSG_WHISPER, "PCMSG_WHISPER" },
    { PSMSG_ERRMESSAGE, "PCMSG_ERRMESSAGE" },*/
};

void PostPluginMessage(WorldPacket & data)
{
    // we need to move this into a struct
    PluginMessage * msg = new PluginMessage;
    msg->DataLen = data.size();
    msg->Data = new uint8[data.size()];
    data.read((uint8*)msg->Data, data.size());    

    //msg->Data = new uint8[data.size()];
    //memcpy((uint8*)msg->Data, data.contents(), data.size());
    msg->Opcode = data.GetOpcode();

    // tell scriptmgr to distribute it to all plugins
    // (it will clean up memory)
    sScriptMgr.PostMessageToPlugins(msg);
}

/*void ScriptMgr::PostMessageToServer(PluginMessage * msg)
{
    // convert to a worldpacket
    WorldPacket data(msg->Opcode, msg->DataLen);
    data.resize(msg->DataLen);
    memcpy(const_cast<uint8*>(data.contents()), msg->Data, msg->DataLen);
    HandlePluginCommand(data);
}

typedef void(*CmdHandler)(WorldPacket& data);
CmdHandler Handlers[PMSG_NUM_TYPES];

void InitPluginHandlerTable()
{    
    memset(Handlers, 0, sizeof(Handlers));

    Handlers[PCMSG_ANNOUNCE] = &HandleAnnounceCommand;
    Handlers[PCMSG_BAN] = &HandleBanCommand;
    Handlers[PCMSG_KICK] = &HandleKickCommand;
    Handlers[PCMSG_WHISPER] = &HandleWhisperCommand;
}

void HandlePluginCommand(WorldPacket & data)
{
    uint32 op = data.GetOpcode();
    if(op >= NUM_MSG_TYPES || Handlers[op] == 0)
    {
        script_errorlog("Receieved bad/unhandled script opcode: 0x%03X", op);
        return;
    }
    
    Handlers[op](data);
}

void HandleAnnounceCommand(WorldPacket & data)
{

}

void HandleBanCommand(WorldPacket & data)
{

}

void HandleWhisperCommand(WorldPacket & data)
{

}

void HandleKickCommand(WorldPacket & data)
{

}*/


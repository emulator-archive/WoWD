#ifndef __PLUGINOPERATIONS_H
#define __PLUGINOPERATIONS_H

#pragma pack(push, 1)
struct PluginMessage
{
    uint32 Opcode;
    uint32 DataLen;
    const uint8 * Data;
};
#pragma pack(pop)

enum PluginMessageTypes
{
    PSMSG_CHAT                  = 1,
    PCMSG_ANNOUNCE              = 2,
    PSMSG_MESSAGE               = 3,
    PMSG_NUM_TYPES              = 4,
};

#ifndef WOWD_SCRIPTLIB

void PostPluginMessage(WorldPacket & data);

#endif

#endif

#ifndef __LOGONOPCODES_H
#define __LOGONOPCODES_H

enum RealmListOpcodes
{
    // Initialization of server/client connection...
    RCMSG_REGISTER_REALM            = 1,
    RSMSG_REALM_REGISTERED          = 2,

    // Upon client connect (for WS)
    RCMSG_REQUEST_SESSION           = 3,
    RSMSG_SESSION_RESULT            = 4,

    // Ping/Pong
    RCMSG_PING                      = 5,
    RSMSG_PONG                      = 6,

    // SQL Query Execute
    RCMSG_SQL_EXECUTE                = 7,
};

#endif

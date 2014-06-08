#pragma once

#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <string>
//#include <fstream>

#include "../shared/Common.h"
#include <NetworkBase/BaseSocket.h>
#include <NetworkBase/ListenSocket.h>

#include "../shared/Log.h"
#include "../shared/Util.h"
#include "../shared/ByteBuffer.h"
#include "../shared/Config/ConfigEnv.h"
#include <zlib/zlib.h>

#include "../shared/Database/DatabaseEnv.h"
#include "../shared/Database/DBCStores.h"
#include "../shared/Database/dbcfile.h"

#include "../shared/Auth/md5.h"
#include "../shared/Auth/BigNumber.h"
#include "../shared/Auth/Sha1.h"
#include "../shared/Auth/WowCrypt.h"
#include "../shared/CrashHandler.h"

#include "LogonOpcodes.h"
#include "../logonserver/Main.h"
#include "../game/NameTables.h"
#include "../game/ThreadMgr.h"
#include "AccountCache.h"
#include "PeriodicFunctionCall_Thread.h"
#include "../logonserver/AuthSocket.h"
#include "../logonserver/AuthStructs.h"
#include "../logonserver/LogonOpcodes.h"
#include "../logonserver/LogonCommServer.h"
#include "../logonserver/LogonConsole.h"
#include "../shared/WorldPacket.h"

// database decl
extern Database* sLogonSQL;

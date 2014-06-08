# Copyright (C) 2004 WoW Daemon
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include

noinst_LIBRARIES = libshared.a libauth.a libconfig.a libdatabase.a libnetworkbase.a

libshared_a_SOURCES = \
    Log.cpp \
    MemoryLeaks.cpp \
    MersenneTwister.cpp \
    MersenneTwister.h \
    PreallocatedQueue.h \
    ByteBuffer.h \
    Common.h \
    Errors.h \
    Log.h \
    MemoryLeaks.h \
    Singleton.h \
    Threading.h \
    Timer.h \
    UnixThreading.h \
    Util.h \
    Util.cpp \
    WinThreading.h \
    WorldPacket.h \
    crc32.cpp \
    crc32.h

libauth_a_SOURCES = \
    Auth/BigNumber.cpp \
    Auth/BigNumber.h \
    Auth/md5.c \
    Auth/md5.h \
    Auth/Sha1.cpp \
    Auth/Sha1.h \
    Auth/WowCrypt.cpp \
    Auth/WowCrypt.h


libconfig_a_SOURCES = \
    Config/Config.cpp \
    Config/Config.h \
    Config/ConfigEnv.h \
    Config/dotconfpp/dotconfpp.cpp \
    Config/dotconfpp/dotconfpp.h \
    Config/dotconfpp/mempool.cpp \
    Config/dotconfpp/mempool.h \
    Config/dotconfpp

libdatabase_a_SOURCES = \
    Database/DBC.cpp \
    Database/Database.cpp \
    Database/DBC.h \
    Database/Database.h \
    Database/DBCStores.cpp \
    Database/DatabaseEnv.h \
    Database/DBCStores.h \
    Database/Field.h \
    Database/DataStore.h \
    Database/dbcfile.cpp \
    Database/dbcfile.h \
    Database/impl/MySQLDatabase.h \
    Database/impl/MySQLDatabase.cpp \
    Database/impl/PostgreDatabase.h \
    Database/impl/PostgreDatabase.cpp

libnetworkbase_a_SOURCES = \
    NetworkBase/BaseSocket.h \
    NetworkBase/BaseSocket.cpp \
    NetworkBase/ListenSocket.h \
    NetworkBase/ListenSocket.cpp \
    NetworkBase/SocketDefines.h \
    NetworkBase/SocketHolder.cpp \
    NetworkBase/SocketHolder.h

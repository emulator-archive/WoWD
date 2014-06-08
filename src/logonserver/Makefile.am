# Copyright (C) 2007 WoW Daemon
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include -I$(srcdir)/../shared
INCLUDES += -I$(srcdir)/../game

bin_PROGRAMS = logonserver

logonserver_SOURCES = \
	AccountCache.cpp \
	AccountCache.h \
	AuthSocket.cpp \
	AuthSocket.h \
	AuthStructs.h \
	LogonCommServer.cpp \
	LogonCommServer.h \
	LogonConsole.cpp \
	LogonConsole.h \
	LogonOpcodes.cpp \
	LogonOpcodes.h \
	LogonStdAfx.cpp \
	LogonStdAfx.h \
	Main.cpp \
	Main.h \
	PeriodicFunctionCall_Thread.h \
	RealmlistThreadMgr.cpp

logonserver_LDADD = -lgame -ldatabase -lconfig -lauth -lshared -lzlib -lnetworkbase


logonserver_LDFLAGS = -L$(srcdir)/../shared \
               -L$(srcdir)/../../dep/src/zlib \
               -L$(srcdir)/../game


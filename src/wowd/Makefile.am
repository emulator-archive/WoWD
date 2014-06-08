# Copyright (C) 2004 WoW Daemon
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

bin_PROGRAMS = wowd

wowd_SOURCES = \
    Main.cpp \
    Master.cpp \
    Master.h \
    WowdConsole.cpp \
    WowdConsole.h

wowd_LDADD = -lgame -ldatabase -lconfig -lauth -lshared -lzlib -lnetworkbase

wowd_LDFLAGS = -L$(srcdir)/../shared \
               -L$(srcdir)/../game \
               -L$(srcdir)/../../dep/src/zlib \
               -Wl,--export-dynamic

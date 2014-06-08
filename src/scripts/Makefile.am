# Copyright (C) 2006 WoW Daemon
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES += -I$(srcdir) -I$(srcdir)/../../dep/include -I$(srcdir)/../shared
INCLUDES += -I$(srcdir)/../script -I$(srcdir)/../../src -I$(srcdir)/../game
INCLUDES += -I$(srcdir)/../logonserver

AM_CXXFLAGS = -DWOWD_SCRIPTLIB

lib_LTLIBRARIES = libGossip_Innkeepers.la libGossip_Battlemaster.la libGuardGossip.la \
libInstance_Deadmines.la\
libWarlockSpells.la libWarriorSpells.la libPaladinSpells.la libShamanSpells.la\
libMageSpells.la libItemSpells_1.la libRogueSpells.la libHunterSpells.la \
libSimpleCreatureAI.la libOnyxiaAI.la

libGossip_Innkeepers_la_SOURCES = Gossip_Innkeepers.cpp
libGossip_Battlemaster_la_SOURCES = Gossip_Battlemaster.cpp
libGuardGossip_la_SOURCES = GuardGossip.cpp
libPaladinSpells_la_SOURCES = PaladinSpells.cpp
libMageSpells_la_SOURCES = MageSpells.cpp
libRogueSpells_la_SOURCES = RogueSpells.cpp
libHunterSpells_la_SOURCES = HunterSpells.cpp
libWarlockSpells_la_SOURCES = WarlockSpells.cpp
libWarriorSpells_la_SOURCES = WarriorSpells.cpp
libShamanSpells_la_SOURCES = ShamanSpells.cpp
libItemSpells_1_la_SOURCES = ItemSpells_1.cpp
libInstance_Deadmines_la_SOURCES = Instance_Deadmines.cpp
libSimpleCreatureAI_la_SOURCES = SimpleCreatureAI.cpp
libOnyxiaAI_la_SOURCES = OnyxiaAI.cpp

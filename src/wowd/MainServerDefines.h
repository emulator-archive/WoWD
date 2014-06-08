#ifndef _MAINSERVER_DEFINES_H
#define _MAINSERVER_DEFINES_H

class Database;

extern Database* Database_Main;

#define sDatabase (*Database_Main)

#endif

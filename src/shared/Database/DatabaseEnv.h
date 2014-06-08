#if !defined(DATABASEENV_H)
#define DATABASEENV_H

/************************************************************************/
/* Database Support Setup                                               */
/************************************************************************/
// Define the databases that you would like the server to be compiled with here.

#define DATABASE_SUPPORT_MYSQL
//#define DATABASE_SUPPORT_PGSQL
//#define DATABASE_SUPPORT_ORACLE10

//! Other libs we depend on.
#include "Common.h"
#include "Singleton.h"
#include "Log.h"
#include "../game/WowdThreads.h"

//! Our own includes.
#include <mysql/mysql.h>
#include "Database/DBCStores.h"
#include "Database/Field.h"
#include "Database/Database.h"

// Implementations

/************************************************************************/
/* MySQL                                                                */
/************************************************************************/
#ifdef DATABASE_SUPPORT_MYSQL
#include "Database/impl/MySQLDatabase.h"
#endif

/************************************************************************/
/* PostgreSQL                                                           */
/************************************************************************/
#ifdef DATABASE_SUPPORT_PGSQL
#include "Database/impl/PostgreDatabase.h"
#endif

/************************************************************************/
/* Oracle 10g                                                           */
/************************************************************************/
#ifdef DATABASE_SUPPORT_ORACLE10
//#include "Database/impl/Oracle10.h"
#endif

#endif

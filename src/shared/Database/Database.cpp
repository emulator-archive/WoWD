#include "DatabaseEnv.h"

Database* CreateDatabaseInterface(DatabaseType type)
{
    switch(type)
    {

#ifdef DATABASE_SUPPORT_MYSQL

    case DATABASE_TYPE_MYSQL:
        return new MySQLDatabase;
        break;

#endif

#ifdef DATABASE_SUPPORT_PGSQL

    case DATABASE_TYPE_PGSQL:
        return new PostgreDatabase;
        break;

#endif

    }
    sLog.outError("Invalid database type specified. It has to be in the range of 1 to 3.");
    return 0;
}

void DestroyDatabaseInterface(Database * ptr)
{
    switch(ptr->GetType())
    {

#ifdef DATABASE_SUPPORT_MYSQL

    case DATABASE_TYPE_MYSQL:
        delete ((MySQLDatabase*)ptr);
        break;

#endif

#ifdef DATABASE_SUPPORT_PGSQL

    case DATABASE_TYPE_PGSQL:
        delete ((PostgreDatabase*)ptr);
        break;

#endif

    }
    sLog.outError("Invalid database type specified. It has to be in the range of 1 to 3.");
}


QueryResult::QueryResult(uint32 FieldCount, uint32 RowCount, uint32 Type)
{
    mCurrentRow = new Field[FieldCount];
    mRowCount = RowCount;
    mFieldCount = FieldCount;
    mType = Type;
}

QueryResult::~QueryResult()
{
    delete [] mCurrentRow;
    switch(mType)
    {

#ifdef DATABASE_SUPPORT_MYSQL

    case DATABASE_TYPE_MYSQL:
        ((MySQLQueryResult*)this)->Destroy();
        break;

#endif

#ifdef DATABASE_SUPPORT_PGSQL

    case DATABASE_TYPE_PGSQL:
        ((PostgreQueryResult*)this)->Destroy();
        break;

#endif

    default:
        assert(false);
        break;
    }
}

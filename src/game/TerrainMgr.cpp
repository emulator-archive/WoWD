/************************************************************************/
/* Copyright (C) 2007 Burlex for WoWd Project                           */
/************************************************************************/

#include "StdAfx.h"
#include <fcntl.h>
#include "TerrainMgr.h"

TerrainMgr::TerrainMgr(string MapPath, uint32 MapId, bool Instanced) : mapPath(MapPath), mapId(MapId), Instance(Instanced)
{
    // Allocate both storage arrays.
    CellInformation = new CellTerrainInformation**[_sizeX];
    for(uint32 x = 0; x < _sizeX; ++x)
    {
        CellInformation[x] = new CellTerrainInformation*[_sizeY];
        for(uint32 y = 0; y < _sizeY; ++y)
        {
            // Clear the pointer.
            CellInformation[x][y] = 0;
            CellOffsets[x][y] = 0;
        }
    }

    // Load the file.
    LoadTerrainHeader();
}

TerrainMgr::~TerrainMgr()
{
    // Free up our file pointer.
    mutex.Acquire();
#ifdef WIN32
    _close(FileDescriptor);
#else
    fclose(FileDescriptor);
#endif
    mutex.Release();

    // Big memory cleanup, whee.
    for(uint32 x = 0; x < _sizeX; ++x)
    {
        for(uint32 y = 0; y < _sizeY; ++y)
        {
            if(CellInformation[x][y] != 0)
                delete CellInformation[x][y];
        }
        delete [] CellInformation[x];
    }
    delete CellInformation;
}

bool TerrainMgr::LoadTerrainHeader()
{
    // Create the path
    char File[200];

    sprintf(File, "%s/Map_%u.bin", mapPath.c_str(), mapId);

    if(mapId < 2 || mapId == 530)
        sLog.outDetail("  TerrainMgr created for map %u. Initializing file `%s`.", mapId, File);

#ifdef WIN32
    FileDescriptor = _open(File, O_RDONLY | O_BINARY | O_RANDOM);
#else
    FileDescriptor = fopen(File, "rb");
#endif

    if((uint32)FileDescriptor == -1 || FileDescriptor == 0)
    {
        sLog.outError("  TerrainMgr could not open `%s`. Please make sure this file exists or adt capabilities will be disabled.", File);
        return false;
    }

    // Read in the header.
#ifdef WIN32
    if(_read(FileDescriptor, CellOffsets, TERRAIN_HEADER_SIZE) != TERRAIN_HEADER_SIZE)
#else
    if(fread(CellOffsets, TERRAIN_HEADER_SIZE, 1, FileDescriptor) < 1)
#endif
        return false;

    return true;
}

bool TerrainMgr::LoadCellInformation(uint32 x, uint32 y)
{
    uint32 Start = getMSTime();
    // Make sure that we're not already loaded.
    assert(CellInformation[x][y] == 0);

    // Find our offset in our cached header.
    uint32 Offset = CellOffsets[x][y];

    // If our offset = 0, it means we don't have cell information for 
    // these coords.
    if(Offset == 0)
        return false;

    // Lock the mutex to prevent double reading.
    mutex.Acquire();

    // Check that we haven't been loaded by another thread.
    if(CellInformation[x][y] != 0)
    {
        mutex.Release();
        return true;
    }
    
    // Seek to our specified offset.
#ifdef WIN32
    if(_lseek(FileDescriptor, Offset, SEEK_SET) == Offset)
#else
    if(fseek(FileDescriptor, Offset, SEEK_SET) == 0)
#endif
    {
        // Allocate the cell information.
        CellInformation[x][y] = new CellTerrainInformation;

        // Read from our file into this newly created struct.
#ifdef WIN32
        _read(FileDescriptor, CellInformation[x][y], sizeof(CellTerrainInformation));
#else
        fread(CellInformation[x][y], sizeof(CellTerrainInformation), 1, FileDescriptor);
#endif
    }

    // Release the mutex.
    mutex.Release();
    sLog.outDetail("Loaded cell information for cell [%u][%u] in %ums.", x, y, getMSTime() - Start);

    // If we don't equal 0, it means the load was successful.
    if(CellInformation[x][y] != 0)
        return true;
    else
        return false;
}

bool TerrainMgr::UnloadCellInformation(uint32 x, uint32 y)
{
    uint32 Start = getMSTime();

    assert(!Instance);
    // Find our information pointer.
    CellTerrainInformation * ptr = CellInformation[x][y];
    assert(ptr != 0);

    // Set the spot to unloaded (null).
    CellInformation[x][y] = 0;

    // Free the memory.
    delete ptr;

    sLog.outDetail("Unloaded cell information for cell [%u][%u] in %ums.", x, y, getMSTime() - Start);
    // Success
    return true;
}

uint8 TerrainMgr::GetWaterType(float x, float y)
{
    if(!AreCoordinatesValid(x, y))
        return 0;

    // Convert the co-ordinates to cells.
    uint32 CellX = ConvertGlobalXCoordinate(x);
    uint32 CellY = ConvertGlobalYCoordinate(y);

    if(!CellInformationLoaded(CellX, CellY))
        return 0;

    // Convert the co-ordinates to cell's internal
    // system.
    float IntX = ConvertInternalXCoordinate(x, CellX);
    float IntY = ConvertInternalYCoordinate(y, CellY);

    // Find the offset in the 2d array.
    return GetCellInformation(CellX, CellY)->LiquidType[ConvertTo2dArray(IntX)][ConvertTo2dArray(IntY)];
}

float TerrainMgr::GetWaterHeight(float x, float y)
{
    if(!AreCoordinatesValid(x, y))
        return 0.0f;

    // Convert the co-ordinates to cells.
    uint32 CellX = ConvertGlobalXCoordinate(x);
    uint32 CellY = ConvertGlobalYCoordinate(y);

    if(!CellInformationLoaded(CellX, CellY))
        return 0.0f;

    // Convert the co-ordinates to cell's internal
    // system.
    float IntX = ConvertInternalXCoordinate(x, CellX);
    float IntY = ConvertInternalYCoordinate(y, CellY);

    // Find the offset in the 2d array.
    return GetCellInformation(CellX, CellY)->LiquidLevel[ConvertTo2dArray(IntX)][ConvertTo2dArray(IntY)];
}

uint8 TerrainMgr::GetWalkableState(float x, float y)
{
    // This has to be implemented.
    return 1;
}

uint16 TerrainMgr::GetAreaID(float x, float y)
{
    if(!AreCoordinatesValid(x, y))
        return 0;

    // Convert the co-ordinates to cells.
    uint32 CellX = ConvertGlobalXCoordinate(x);
    uint32 CellY = ConvertGlobalYCoordinate(y);

    if(!CellInformationLoaded(CellX, CellY) && !LoadCellInformation(CellX, CellY))
        return 0;

    // Convert the co-ordinates to cell's internal
    // system.
    float IntX = ConvertInternalXCoordinate(x, CellX);
    float IntY = ConvertInternalYCoordinate(y, CellY);

    // Find the offset in the 2d array.
    return GetCellInformation(CellX, CellY)->AreaID[ConvertTo2dArray(IntX)][ConvertTo2dArray(IntY)];
}

float TerrainMgr::GetLandHeight(float x, float y)
{
    if(!AreCoordinatesValid(x, y))
        return 0.0f;

    // Convert the co-ordinates to cells.
    uint32 CellX = ConvertGlobalXCoordinate(x);
    uint32 CellY = ConvertGlobalYCoordinate(y);

    if(!CellInformationLoaded(CellX, CellY) && !LoadCellInformation(CellX, CellY))
        return 0.0f;

    // Convert the co-ordinates to cell's internal
    // system.
    float IntX = ConvertInternalXCoordinate(x, CellX);
    float IntY = ConvertInternalYCoordinate(y, CellY);

    // Calculate x index.
    float TempFloat = IntX * (MAP_RESOLUTION / CellsPerTile / _cellSize);
    uint32 XOffset = FL2UINT(TempFloat);
    if((TempFloat - (XOffset * _cellSize)) >= 0.5f)
        ++XOffset;

    // Calculate y index.
    TempFloat = IntY * (MAP_RESOLUTION / CellsPerTile / _cellSize);
    uint32 YOffset = FL2UINT(TempFloat);
    if((TempFloat - (YOffset * _cellSize)) >= 0.5f)
        ++YOffset;

    // Return our cached information.
    return GetCellInformation(CellX, CellY)->Z[XOffset][YOffset];
}

void TerrainMgr::CellGoneActive(uint32 x, uint32 y)
{
    // Load cell information if it's not already loaded.
    if(!CellInformationLoaded(x, y))
        LoadCellInformation(x, y);
}

void TerrainMgr::CellGoneIdle(uint32 x, uint32 y)
{
    // If we're not an instance, unload our cell info.
    if(!Instance && CellInformationLoaded(x, y) && sWorld.UnloadMapFiles)
        UnloadCellInformation(x, y);
}

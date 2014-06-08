//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Class ScriptMgr - Handles storage, loading
// unloading, compilation, and calling of
// external binary scripts that are used by
// the server.

#include "StdAfx.h"
#ifdef WIN32
#include <direct.h>
#else
#define LoadLibrary dlopen
#define GetProcAddress dlsym
#define FreeLibrary dlclose
#define HMODULE void*
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#endif
initialiseSingleton(ScriptMgr);


ScriptMgr::ScriptMgr()
{
    _loaded = _initialized = false;
}

ScriptMgr::~ScriptMgr()
{

}

void ScriptMgr::Init()
{
    sLog.outString("Initializing Scripting System...");

    // we shouldn't be loaded
    ASSERT(!_loaded);
    sLog.outString("  Checking state of script binaries...");
    bool script_state = CheckScriptStatus();
    if(script_state == false)
    {
        // our scripts are dirty or have not been compiled
        script_state = RecompileScripts();
    }

    if(script_state == false)
    {
        // our scripts failed compilation.
        sLog.outError("  Warning: Scripts did not successfully compile.");
    }

    // after we know we have a successful compile, load.
    LoadScripts();

    // initialized!
    _initialized = true;
}
#ifdef WIN32
struct cache_entry
{
    char filename[100];
    _fsize_t filesize;
    time_t modifytime;
};
#else
struct cache_entry
{
    char filename[100];
    int filesize;
    time_t modifytime;
};
#endif

bool ScriptMgr::RecompileScripts()
{
#ifdef WIN32
    int errorcount = 0, warningcount = 0;
    //script_debuglog("Recompiling script binaries...");
    std::map<std::string, cache_entry> caches;
    std::set<std::string> files_to_compile;
    std::set<cache_entry*> all_files;
    // load script compile cache
    FILE *f = fopen("script_bin\\scompile_cache.dat", "rb");
    if(f)
    {
        fseek(f, 0, SEEK_END);
        int fsize = ftell(f);
        fseek(f, 0, SEEK_SET);
        
        int32 count;
        if(fsize <= 4)
            count = -1;
        else
            fread(&count, 4, 1, f);

        if(count == -1 || (count * sizeof(cache_entry)) != fsize-4)
        {
            script_errorlog("Warning: Corrupt script_bin\\scompile_cache.dat!");
        } else {
            for(int i = 0; i < count; ++i)
            {
                cache_entry tmp;
                fread(&tmp, sizeof(tmp), 1, f);
                std::string fname = tmp.filename;
                caches[fname] = tmp;
            }
        }
        fclose(f);
    }
    // fill the files_to_compile set.
    _finddata_t finddata;
    // add files
    const char* path = "script_src\\";
    std::string search_path = path;
    search_path += "*.cpp";
    std::string envpath = Config.MainConfig.GetStringDefault("Compiler.EnvPath",
        "C:/Program Files/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat");
    std::string::value_type pos = envpath.find("/");
    while(pos != std::string::npos)
    {
        envpath[pos] = '\\';
        pos = envpath.find("/");
    }
    FILE *fi = fopen(envpath.c_str(), "r");
    if(!fi)
    {
        sLog.outError("  Could not open compiler environment script!");
        return false;
    }
    int handle = _findfirst(search_path.c_str(), &finddata);
    int result = handle;
    if(handle == -1)
    {
        sLog.outError("  No script sources were found to compile.");
        return false;
    }
    while(result != -1)
    {
        // grab the file, load it
        std::string search = finddata.name;
        _fsize_t fs = finddata.size;
        time_t mod = finddata.time_write;

        result = _findnext(handle, &finddata);

        // create struct and push to all for later saving
        cache_entry *ce = new cache_entry;
        strcpy(ce->filename, search.c_str());
        ce->filesize = fs;
        ce->modifytime = mod;
        all_files.insert(ce);

        std::map<std::string, cache_entry>::iterator itr = caches.find(search);
        if(itr != caches.end())
        {
            // check for edit
            if(itr->second.filesize == fs && itr->second.modifytime == mod)
                continue;   // skip :)
        }
        files_to_compile.insert(search);        
    }
    _findclose(handle);

    if(files_to_compile.size() == 0)
    {
        sLog.outString("  No files need to be compiled.");
    } else {
        sLog.outString("  %u files need to be compiled.", files_to_compile.size());
        std::string headerdir = Config.MainConfig.GetStringDefault("Compiler.IncludePath", "include");
        // create include paths
        std::string include_paths = "/I \"";
        include_paths += headerdir;
        include_paths += "\\game";
        include_paths += "\" /I \"";
        include_paths += headerdir;
        include_paths += "\\shared";
        include_paths += "\" /I \"";
        include_paths += headerdir;
        include_paths += "\" /I \"";
        include_paths += ".\" ";

        char compile_string[1024];

        // create compile batch file
        FILE *fo = fopen("compile.bat", "w");

        fprintf(fo, "@echo Compiling script libraries now. This may take a while.\n@echo.\n");
        char buffer[1024];
        while(fgets(buffer, 1024, fi))
        {
            fputs(buffer, fo);
        }
        
        char *wowdpath = getcwd(NULL, MAX_PATH);
        fprintf(fo, "\n@cd %s\\script_src\n", wowdpath);
        free(wowdpath);

        // add compile exe
#ifdef _DEBUG
        sprintf(compile_string, "@cl /LD %s /D \"WIN32\" /D \"_DEBUG\" /D \"_WINDOWS\" /D \"_USRDLL\" /D \"WOWD_SCRIPTLIB\" /D \"_WINDLL\" /D \"_MBCS\" /Gm /EHsc /MTd /W3 /Wp64 /ZI /TP /nologo /Fd\".\\workdir/\" /Fo\".\\workdir/\" ", include_paths.c_str());
#else
        sprintf(compile_string, "@cl /LD %s /D \"WIN32\" /D \"_WINDOWS\" /D \"_USRDLL\" /D \"WOWD_SCRIPTLIB\" /D \"_WINDLL\" /D \"_MBCS\" /Gm /EHsc /MT /W3 /Wp64 /ZI /TP /nologo /Fd\".\\workdir/\" /Fo\".\\workdir/\" ", include_paths.c_str());
#endif

        // search folder, grab files, load them one by one registering their handlers.
        for(std::set<std::string>::iterator itr = files_to_compile.begin(); itr != files_to_compile.end();
            ++itr)
        {
            fprintf(fo, "%s%s /link ../wowd.lib winmm.lib\n", compile_string, (*itr).c_str());
        }

        fprintf(fo, "\n");
        fprintf(fo, "@echo.\n@echo Compile finished. Cleaning up and copying dll files.\n@echo.\n");
        fprintf(fo, "@copy *.dll ..\\script_bin\n");
        // copy pdb files if we're in debug
#ifdef _DEBUG
        fprintf(fo, "@copy *.pdb ..\\script_bin\n");
#endif
        fprintf(fo, "@del *.ilk\n@del *.lib\n@del *.pdb\n@del *.dll\n@del *.exp\n");
        fprintf(fo, "@echo.\n@echo Compiling is finished, check the output for any errors\n@echo and hit any key to resume loading of the server.\n@echo.\n@pause\n");
        fprintf(fo, "cd ..\ndel compile.bat\n");
        fclose(fo);

        // execute compile
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        memset(&si, 0, sizeof(si));
        memset(&pi, 0, sizeof(pi));
        SHELLEXECUTEINFO sh = {0};
        sh.cbSize = sizeof(sh);
        sh.fMask = SEE_MASK_NOCLOSEPROCESS;
        sh.hwnd = 0;
        sh.lpVerb = 0;
        sh.lpDirectory = 0;
        sh.hInstApp = 0;
        sh.nShow = SW_SHOW;
        sh.lpFile = "compile.bat";
        sh.lpParameters = "";
        if(!ShellExecuteEx(&sh))
        {
            script_errorlog("ShellExecuteEx failed. Something has gone wrong, you will need to compile manually.");
        } else {
            WaitForSingleObject(sh.hProcess, INFINITE);
        }
        CloseHandle(sh.hProcess);

        // dump our new (updated) file.
        f = fopen("script_bin\\scompile_cache.dat", "wb");
        fseek(f, 4, SEEK_SET);
        char tmp[200];
        int wcount = 0;
        for(std::set<cache_entry*>::iterator itr = all_files.begin(); itr != all_files.end(); ++itr)
        {
            // make sure we have a result file
            std::string resultst = (*itr)->filename;
            int endpos = resultst.length();
            resultst[endpos-1] = 'l';
            resultst[endpos-2] = 'l';
            resultst[endpos-3] = 'd';
            sprintf(tmp, "script_bin\\%s", resultst.c_str());
            FILE *ftmp = fopen(tmp, "rb");
            
            if(ftmp)
            {
                // successful compile of that file
                cache_entry *e = (*itr);
                fwrite(e, sizeof(cache_entry), 1, f);
                wcount++;
                fclose(ftmp);
                SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                printf("%s compiled successfully.\n", resultst.c_str());
                SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
            } else {
                script_errorlog("%s did not compile successfully.", resultst.c_str());
            }
        }
        fseek(f, 0, SEEK_SET);
        fwrite(&wcount, 4, 1, f);
        fclose(f);

        script_debuglog("Script compile completed.", errorcount, warningcount);
    }
    for(std::set<cache_entry*>::iterator itr = all_files.begin(); itr != all_files.end(); ++itr)
    {
        cache_entry *e = (*itr);
        delete e;
    }
    all_files.clear();
    fclose(fi);
#endif
    return true;
}

bool ScriptMgr::CheckScriptStatus()
{
    // need to check binary file for compile/modify dates and see if any files have changed since
    // last server start.
    return false;
}

void ScriptMgr::_PrintLoadName(const char* filename)
{
#ifdef WIN32    // win32 - colors
    printf("|");
    SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
        // white
    printf("%35s ", filename);
    SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    printf("|");
#else
    printf("|%35s |", filename);
#endif
}

void ScriptMgr::_PrintLoadStatus(int pass)
{
#ifdef WIN32
    printf(" |");
    SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf(" [ ");
    if(pass == 1) {
        SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    } else if(pass == 2) {
        SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    } else {
        SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    if(pass == 2)
        printf("WARNING");
    else
        printf("%s", (pass ? "OK" : "FAIL"));
    SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf(" ] ");
    SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    printf("|\n");
#else
    if(pass==2)
        printf("WARNING");
    else
        printf("[ %s ] |\n", (pass ? "OK" : "FAIL"));
#endif
}

void ScriptMgr::_PrintLoadError(bool warning, const char* str, ...)
{
    va_list l;
    va_start(l, str);
#ifdef WIN32
    DWORD c = FOREGROUND_RED;
    if(warning)
        c |= FOREGROUND_GREEN;      //transform to yellow
    else
        c |= FOREGROUND_INTENSITY;

    SetConsoleTextAttribute(sLog.stdout_handle, c);
#endif
    vprintf(str, l);
    printf("\n");
    va_end(l);
#ifdef WIN32
    SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#endif
}

void ScriptMgr::UnloadScripts()
{
    script_debuglog("Deleting instanced AI classes...");
    for(InstancedAIMap::iterator itr = _aiInstances.begin(); itr != _aiInstances.end(); ++itr)
    {
        itr->second->Destroy();
    }
    _aiInstances.clear();
    script_debuglog("Deleting Gossip Modules...");
    for(GossipScriptMap::iterator itr = _GossipScripts.begin(); itr != _GossipScripts.end(); ++itr)
    {
        uint32 entry = itr->first;
        ScriptModule *sm = NULL;
        ScriptMap::iterator iter = _gossipModules.find(entry);
        if(iter != _gossipModules.end())
            sm = iter->second;

        itr->second->Destroy();            
    }
    _GossipScripts.clear();
    ASSERT(_loaded);
    script_debuglog("Unloading scripts...");
    for(ScriptSet::iterator itr = this->_scripts.begin(); itr != this->_scripts.end();)
    {
        // unload library
        script_errorlog("Unloading %s", (*itr)->filename.c_str());
        FreeLibrary((*itr)->_handle);
        ScriptModule *sm = (*itr);
        ++itr;
        delete sm;
    }
    script_debuglog("Unloaded all scripts.");
}

void ScriptMgr::LoadScripts()
{
    ASSERT(!_loaded);
    _loaded = true;
    // path is hardcoded for now
    const char* path = "script_bin\\";
    std::string search_path = path;
    search_path += "*.dll";
    int count = 0;
    sLog.outString("  Loading Script Binaries...\n", path);
    uint32 t = getMSTime();

    // search folder, grab files, load them one by one registering their handlers.
#ifdef WIN32
    _finddata_t finddata;
    int handle = _findfirst(search_path.c_str(), &finddata);
    int result = handle;
    if(handle == -1)
#else
    struct dirent** list;
    int filecount = scandir("../lib/", &list, 0, 0);
    if(filecount == 0 || !list)
#endif
    {
        sLog.outError("  No script binaries were found to load.");
        return;
    }

    // output header
    sLog.outString("==============================================================================");
    sLog.outString("| Module Name                        | Load Address |     Error     | Status |");
    sLog.outString("==============================================================================");
#ifdef WIN32
    while(result != -1)
#else
    int32 leng = 0;
    while(filecount--)
    {
    leng = strlen(list[filecount]->d_name);
    if(list[filecount]->d_name[leng-1] == 'o' && list[filecount]->d_name[leng-2] == 's' && list[filecount]->d_name[leng-3] == '.')
#endif
    {
        
#ifdef WIN32
        // grab the file, load it
        std::string filename = path;
        std::string fname = finddata.name;
    result = _findnext(handle, &finddata);
#else
    std::string filename = "../lib/";
    std::string fname = list[filecount]->d_name;
    free(list[filecount]);
#endif
    _PrintLoadName(fname.c_str());        
    filename += fname;
#ifdef WIN32
        HMODULE mhandle = LoadLibrary(filename.c_str());
        printf("  0x%.8X  |              ", (uint32)mhandle);
#else
    HMODULE mhandle = dlopen(filename.c_str(), RTLD_NOW);
    printf("  0x%.8X  |     %s       ", (uint32)mhandle,dlerror());
#endif
              

        if(!mhandle)
        {
            _PrintLoadStatus(0);
            continue;
        } else {
            // create module, load in the info.
            GetModuleVersion _call = (GetModuleVersion)GetProcAddress(mhandle, "GetModuleVersion");
            ScriptInitialize _init = (ScriptInitialize)GetProcAddress(mhandle, "ScriptInitialize");

            if(_call == NULL || _init == NULL)
            {
                // bad module.
                _PrintLoadStatus(0);
                _PrintLoadError(false, "\tCould not find address of GetModuleVersion or ScriptInitialize. Non-WoWd module?\n");
                FreeLibrary(mhandle);
                continue;
            }

            ScriptModule *mod = new ScriptModule;
            mod->_handle = mhandle;
            mod->filename = fname;

            _call(&mod->_version);
            // check the version
            if(((mod->_version & 0xFFFF0000) >> 16) != SCRIPTLIB_VERSION_MAJOR)
            {
                _PrintLoadStatus(0);
                _PrintLoadError(false, "\t%s is an invalid major version. Unable to load.\n", fname.c_str());
                FreeLibrary(mhandle);
                continue;
            }

            if((mod->_version & 0x0000FFFF) != SCRIPTLIB_VERSION_MINOR)
            {
                _PrintLoadStatus(2);
                _PrintLoadError(true, "\t%s has a different minor version. Expect problems!\n", fname.c_str());
            } else {
                _PrintLoadStatus(1);
            }

            // increase counter
            count++;

            // nullify
            mod->_createCreatureAI = NULL;
            mod->_createGossipScript = NULL;
            mod->_dummySpellHandler = NULL;
            mod->_dummyAuraHandler = NULL;
            mod->_createGameObjectAI = NULL;
            mod->_createInstance = NULL;
            mod->_createEvent = NULL;
            mod->_createPluginModule = NULL;

            _scripts.insert(mod);

            // Call ScriptInitialize.
            _init(mod);
        }                
#ifndef WIN32
    }
#endif
    }
#ifdef WIN32
    // free up
    _findclose(handle);
#else
    free(list);
#endif
    _loaded = true;
    sLog.outString("===============================================================================");
    sLog.outString("");
    sLog.outString("  %u script binaries loaded in %ums.", count, getMSTime()-t);
}

CreatureAIScript* ScriptMgr::CreateAIScriptClassForEntry(uint32 uEntryId, Creature* pCreature)
{
    ScriptMap::iterator itr = _creatureModules.find(uEntryId);
    if(itr == _creatureModules.end())
        return NULL;

    // Grab script module
    ScriptModule *mod = itr->second;
    ASSERT(mod);

    // Create an instanced AI class.
    CreatureAIScript* ai_script = mod->_createCreatureAI(uEntryId, pCreature);
    if(!ai_script)
    {
        script_errorlog("Warning: _createCreatureAI returned NULL for entry id %u, possible bad script. Filename: \"%s\"", uEntryId, mod->filename.c_str());
    }

    _aiInstances.insert( InstancedAIMap::value_type( pCreature, ai_script ) );

    return ai_script;
}

InstanceScript* ScriptMgr::CreateInstanceScriptForMap(uint32 uMapId, MapMgr *pInstance)
{
    ScriptMap::iterator itr = _instanceModules.find(uMapId);
    if(itr == _instanceModules.end())
        return NULL;

    // Grab script module
    ScriptModule *mod = itr->second;
    ASSERT(mod);

    // Create an instanced AI class.
    InstanceScript* ai_script = mod->_createInstance(uMapId, pInstance);
    if(!ai_script)
    {
        script_errorlog("Warning: _createInstance returned NULL for map id %u, possible bad script. Filename: \"%s\"", uMapId, mod->filename.c_str());
    }

    return ai_script;
}

GameObjectAIScript* ScriptMgr::CreateAIScriptClassForGameObject(uint32 uEntryId, GameObject* pGameObject)
{
    ScriptMap::iterator itr = _gameobjectModules.find(uEntryId);
    if(itr == _gameobjectModules.end())
        return NULL;

    // Grab script module
    ScriptModule *mod = itr->second;
    ASSERT(mod);

    // Create an instanced AI class.
    GameObjectAIScript* ai_script = mod->_createGameObjectAI(uEntryId, pGameObject);
    if(!ai_script)
    {
        script_errorlog("Warning: _createGameObjectAI returned NULL for entry id %u, possible bad script. Filename: \"%s\"", uEntryId, mod->filename.c_str());
    }

    return ai_script;
}


bool ScriptMgr::Aura_HasDummyHandler(uint32 uSpellId)
{
    return (_auraModules.find(uSpellId) == _auraModules.end() ? false : true);
}

bool ScriptMgr::Spell_HasDummyHandler(uint32 uSpellId)
{
    return (_spellModules.find(uSpellId) == _spellModules.end() ? false : true);
}

bool ScriptMgr::Creature_HasAIScript(uint32 uEntryId)
{
    return (_creatureModules.find(uEntryId) == _creatureModules.end() ? false : true);
}

bool ScriptMgr::CallScriptedDummySpell(uint32 uSpellId, uint32 i, Spell* pSpell)
{
    ScriptMap::iterator itr = _spellModules.find(uSpellId);
    if(itr == _spellModules.end())
        return false;

    // Grab script module
    ScriptModule *mod = itr->second;
    ASSERT(mod);

    // Hand control over to the library, which will handle the spell.
    ASSERT(mod->_dummySpellHandler);
    return mod->_dummySpellHandler(uSpellId, i, pSpell);
}

GossipScript* ScriptMgr::GetGossipScript(uint32 uEntryId)
{
    // Check if we have one
    GossipScriptMap::iterator iter = _GossipScripts.find(uEntryId);
    if(iter != _GossipScripts.end())
        return iter->second;

    // Got to auto create one
    GossipScript * Script = new GossipScript();
    Script->AutoCreated = true;

    //script_debuglog("Using DEFAULT gossip module for entry %u.", uEntryId);
    _GossipScripts.insert( GossipScriptMap::value_type( uEntryId, Script) );
    return Script;
}

/* Aura shiznit */

bool ScriptMgr::CallScriptedDummyAura(uint32 uSpellId, uint32 i, Aura* pAura, bool apply)
{
    ScriptMap::iterator itr = _auraModules.find(uSpellId);
    if(itr == _auraModules.end())
        return false;

    // Grab script module
    ScriptModule *mod = itr->second;
    ASSERT(mod);

    // Hand control over to the library, which will handle the spell.
    ASSERT(mod->_dummyAuraHandler);
    return mod->_dummyAuraHandler(uSpellId, i, pAura, apply);
}

/* Addition Functions, wewt. */

void ScriptMgr::ScriptAddAuraModule(uint32 uSpellId, ScriptModule *mod)
{
    if(mod->_dummyAuraHandler == 0)
        mod->_dummyAuraHandler = (HandleDummyAura)GetProcAddress(mod->_handle, "HandleDummyAura");

    if(mod->_dummyAuraHandler == 0)
    {
        script_errorlog("Could not find address of HandleDummyAura, not adding to script map.");
        return;
    }

    _auraModules.insert( ScriptMap::value_type( uSpellId, mod ) );
    QueueLoadedMessage(LOAD_MSG_AURA, "Aura `%s` [%u] -> %s.",
        sSpellStore.LookupString(sSpellStore.LookupEntry(uSpellId)->Name), uSpellId, mod->filename.c_str());
}

void ScriptMgr::ScriptAddSpellModule(uint32 uSpellId, ScriptModule *mod)
{
    if(mod->_dummySpellHandler == 0)
        mod->_dummySpellHandler = (HandleDummySpell)GetProcAddress(mod->_handle, "HandleDummySpell");

    if(mod->_dummySpellHandler == 0)
    {
        script_errorlog("Could not find address of HandleDummySpell, not adding to script map.");
        return;
    }

    _spellModules.insert( ScriptMap::value_type( uSpellId, mod ) );
    QueueLoadedMessage(LOAD_MSG_AURA, "Spell `%s` [%u] -> %s.",
        sSpellStore.LookupString(sSpellStore.LookupEntry(uSpellId)->Name), uSpellId, mod->filename.c_str());
}

void ScriptMgr::ScriptAddCreatureModule(uint32 uEntryId, ScriptModule *mod)
{
    if(mod->_createCreatureAI == 0)
        mod->_createCreatureAI = (CreateCreatureAIClass)GetProcAddress(mod->_handle, "CreateCreatureAIClass");

    if(mod->_createCreatureAI == 0)
    {
        script_errorlog("Could not find address of CreateCreatureAIClass, not adding to script map.");
        return;
    }

    _creatureModules.insert( ScriptMap::value_type( uEntryId, mod ) );
    QueueLoadedMessage(LOAD_MSG_CREATURE, "Creature %u -> %s.",
        uEntryId, mod->filename.c_str());
}

void ScriptMgr::ScriptAddGossipModule(uint32 uEntryId, ScriptModule *mod)
{
    if(mod->_createGossipScript == 0)
        mod->_createGossipScript = (CreateGossipScript)GetProcAddress(mod->_handle, "CreateGossipScript");
    
    if(mod->_createGossipScript == 0)
    {
        script_errorlog("Could not find address of either CreateGossipScript or DestroyGossipScript. Not adding to script map.");
        return;
    }

    GossipScript* script = mod->_createGossipScript(uEntryId);

    _gossipModules.insert( ScriptMap::value_type( uEntryId, mod ) );
    _GossipScripts.insert( GossipScriptMap::value_type( uEntryId, script ) );
}

void ScriptMgr::ScriptAddBattlegroundModule(uint32 uMapId, ScriptModule *mod)
{

}

void ScriptMgr::ScriptAddInstanceModule(uint32 uMapId, ScriptModule *mod)
{
    if(mod->_createInstance == 0)
        mod->_createInstance = (CreateInstanceClass)GetProcAddress(mod->_handle, "CreateInstanceClass");

    if(mod->_createCreatureAI == 0)
    {
        script_errorlog("Could not find address of CreateInstanceClass, not adding to script map.");
        return;
    }

    _instanceModules.insert( ScriptMap::value_type( uMapId, mod ) );
}

/* CreatureAI Stuff */

CreatureAIScript::CreatureAIScript(Creature* creature) : _unit(creature)
{

}

void CreatureAIScript::RegisterAIUpdateEvent(uint32 frequency)
{
    sEventMgr.AddEvent(_unit, &Creature::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0);
}

void CreatureAIScript::RemoveAIUpdateEvent()
{
    sEventMgr.RemoveEvents(_unit, EVENT_SCRIPT_UPDATE_EVENT);
}

/* GameObjectAI Stuff */

GameObjectAIScript::GameObjectAIScript(GameObject* goinstance) : _gameobject(goinstance)
{

}

void GameObjectAIScript::RegisterAIUpdateEvent(uint32 frequency)
{
    sEventMgr.AddEvent(_gameobject, &GameObject::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0);
}


/* InstanceAI Stuff */

InstanceScript::InstanceScript(MapMgr *instance) : _instance(instance)
{

}


GossipScript::GossipScript()
{
    AutoCreated = false;
}

void GossipScript::GossipEnd(Creature* pCreature, Player* Plr)
{
    Plr->CleanupGossipMenu();
}

void GossipScript::GossipHello(Creature* pCreature, Player* Plr, bool AutoSend)
{
    GossipMenu *Menu;
    uint32 TextID = 2;
    Trainer *pTrainer = pCreature->GetTrainer();
    uint32 Text = objmgr.GetGossipTextForNpc(pCreature->GetEntry());
    if(Text != 0)
    {
        GossipText * text = objmgr.GetGossipText(Text);
        if(text != 0)
            TextID = Text;
    }

    objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), TextID, Plr);
    
    uint32 flags = pCreature->GetUInt32Value(UNIT_NPC_FLAGS);

    if(flags & UNIT_NPC_FLAG_VENDOR)
        Menu->AddItem(1, "I would like to browse your goods", 1);
    
    if(flags & UNIT_NPC_FLAG_TRAINER && pTrainer != 0)
    {
        string name = pCreature->GetName();
        string::size_type pos = name.find(" ");      // only take first name
        if(pos != string::npos)
            name = name.substr(0, pos);

        string msg = "I seek ";

        if(pTrainer->RequiredClass && pTrainer->RequiredClass != Plr->getClass())
        {
            //                               WARRIOR   PALADIN HUNTER  ROGUE   PRIEST  -   SHAMAN  MAGE   WARLOCK  - DRUID
            uint32 notclass[13] =   {0      ,5721     ,3976   ,5839   ,4797   ,4435   ,0  ,5006   ,539    ,5836   ,0,4774};
            Menu->SetTextID(notclass[pTrainer->RequiredClass]);
        } else if(pTrainer->RequiredClass) {
            //                               WARRIOR   PALADIN HUNTER  ROGUE   PRIEST  -   SHAMAN  MAGE   WARLOCK  - DRUID
            uint32 isclass[13] =    {0      ,1040     ,3974   ,4864   ,4835   ,4436   ,0  ,5005   ,538    ,5835   ,0,4775};
            Menu->SetTextID(isclass[pTrainer->RequiredClass]);

            switch(Plr->getClass())
            {
            case MAGE:
                msg += "mage";
                break;
            case SHAMAN:
                msg += "shaman";
                break;
            case WARRIOR:
                msg += "warrior";
                break;
            case PALADIN:
                msg += "paladin";
                break;
            case WARLOCK:
                msg += "warlock";
                break;
            case HUNTER:
                msg += "hunter";
                break;
            case ROGUE:
                msg += "rogue";
                break;
            case DRUID:
                msg += "druid";
                break;
            case PRIEST:
                msg += "priest";
                break;
            }
            msg += " training, ";
            msg += name;
            msg += ".";

            Menu->AddItem(3, msg.c_str(), 2);

        }
        else
        {
            msg += "training, ";
            msg += name;
            msg += ".";

            Menu->AddItem(3, msg.c_str(), 2);
        }
    }

    if(flags & UNIT_NPC_FLAG_TAXIVENDOR)
        Menu->AddItem(2, "Give me a ride.", 3);

    if(flags & UNIT_NPC_FLAG_AUCTIONEER)
        Menu->AddItem(0, "I would like to make a bid.", 4);

    if(flags & UNIT_NPC_FLAG_INNKEEPER)
        Menu->AddItem(5, "Make this inn your home.", 5);

    if(flags & UNIT_NPC_FLAG_BANKER)
        Menu->AddItem(0, "I would like to check my deposit box.", 6);

    if(flags & UNIT_NPC_FLAG_SPIRITHEALER)
        Menu->AddItem(0, "Bring me back to life.", 7);

    if(flags & UNIT_NPC_FLAG_PETITIONER)
        Menu->AddItem(0, "How do I create a guild?", 8);

    if(flags & UNIT_NPC_FLAG_TABARDCHANGER)
        Menu->AddItem(0, "I want to create a guild crest.", 9);

    /*if(flags & UNIT_NPC_FLAG_BATTLEFIELDPERSON)
        Menu->AddItem(0, "I would like to go to the battleground.", 10);*/

    if( pTrainer &&
        pTrainer->RequiredClass &&                      // class trainer
        pTrainer->RequiredClass == Plr->getClass() &&   // correct class
        pCreature->getLevel() > 10 &&                   // creature level
        Plr->getLevel() > 10 )                          // player level
    {
        Menu->AddItem(0, "I would like to reset my talents.", 11);
    }

    if(AutoSend)
        Menu->SendTo(Plr);
}

void GossipScript::GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
{
    switch(IntId)
    {
    case 1:
        // vendor
        Plr->GetSession()->SendInventoryList(pCreature);
        break;
    case 2:
        // trainer
        Plr->GetSession()->SendTrainerList(pCreature);
        break;
    case 3:
        // taxi
        Plr->GetSession()->SendTaxiList(pCreature);
        break;
    case 4:
        // auction
        Plr->GetSession()->SendAuctionList(pCreature);
        break;
    case 5:
        // innkeeper
        Plr->GetSession()->SendInnkeeperBind(pCreature);
        break;
    case 6:
        // banker
        Plr->GetSession()->SendBankerList(pCreature);
        break;
    case 7:
        // spirit
        Plr->GetSession()->SendSpiritHealerRequest(pCreature);
        break;
    case 8:
        // petition
        Plr->GetSession()->SendCharterRequest(pCreature);
        break;
    case 9:
        // guild crest
        Plr->GetSession()->SendTabardHelp(pCreature);
        break;
    case 10:
        // battlefield
        Plr->GetSession()->SendBattlegroundList(pCreature, 2);
        break;
    case 11:
        // switch to talent reset message
        {
            GossipMenu *Menu;
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 5674, Plr);
            Menu->AddItem(0, "I understand, continue.", 12);
            Menu->SendTo(Plr);
        }break;
    case 12:
        // talent reset
        {
            Plr->Gossip_Complete();
            Plr->SendTalentResetConfirm();
        }break;

    default:
        script_errorlog("Unknown IntId %u on entry %u", IntId, pCreature->GetEntry());
        break;
    }    
}

void GossipScript::Destroy()
{
    //delete this;
}

EventScript::EventScript(MapMgr *instance) : _map(instance)
{

}

void ScriptMgr::ScriptAddEventModule(uint32 uMapId, ScriptModule *mod)
{
    if(mod->_createEvent == 0)
        mod->_createEvent = (CreateWoWEvent)GetProcAddress(mod->_handle, "CreateWoWEvent");

    if(mod->_createEvent == 0)
    {
        script_errorlog("Could not find address of CreateEvent, not adding to script map.");
        return;
    }

    _eventModules.insert( make_pair( mod, uMapId) );
}

void ScriptMgr::ExecuteEventScriptsOnMap(uint32 uMapId, MapMgr* instance)
{
    EventScriptMap::iterator iter = _eventModules.begin();
    for(; iter != _eventModules.end(); ++iter)
    {
        if(iter->second == uMapId)
        {
            EventScript * script = iter->first->_createEvent(instance);
            script->Execute();
        }
    }
}

void ScriptMgr::ScriptAddServerModule(ScriptModule *mod)
{
    if(mod->_createPluginModule == 0)
        mod->_createPluginModule = (CreatePluginModule)GetProcAddress(mod->_handle, "CreatePluginModule");
    if(mod->_createPluginModule == 0)
    {
        script_errorlog("Cannot find address of CreatePluginModule, not adding to script map.");
        return;
    }

    _serverPlugins.insert(mod);

    // create the initial module
    PluginModule * cmod = mod->_createPluginModule();
    if(cmod)
    {
        _pluginInstances.push_back(cmod);
    }
}

void ScriptMgr::ScriptAddGameObjectModule(uint32 uEntryId, ScriptModule *mod)
{
    if(mod->_createGameObjectAI == 0)
        mod->_createGameObjectAI = (CreateGameObjectAIClass)GetProcAddress(mod->_handle, "CreateGameObjectAIClass");

    if(mod->_createGameObjectAI == 0)
    {
        script_errorlog("Could not find address of CreateGameObjectAIClass, not adding to script map.");
        return;
    }

    _gameobjectModules.insert( ScriptMap::value_type( uEntryId, mod ) );
}

void ScriptMgr::PostMessageToPlugins(PluginMessage * msg)
{
    PluginInstanceSet::iterator iter = _pluginInstances.begin();
    for(; iter != _pluginInstances.end(); ++iter)
        (*iter)->HandleMessage(msg);
    delete msg;
}

bool ScriptMgr::HasPlugins()
{
    if(_pluginInstances.size() == 0)
        return false;
    return true;
}

void ScriptMgr::QueueLoadedMessage(uint32 Type, const char * Format, ...)
{
    va_list ap;
    va_start(ap, Format);
    char msg[250];
    vsprintf(msg, Format, ap);
    va_end(ap);
    loadmsg.push_back( make_pair( Type, string(msg) ) );
}

void ScriptMgr::FlushLoadedMessages()
{
    sLog.outString(" >> Registering script modules...");
    for(uint32 type = LOAD_MSG_SPELL; type < LOAD_MSG_CREATURE + 1; ++type)
    {
        for(vector< pair< uint32, string > >::iterator itr = loadmsg.begin(); itr != loadmsg.end(); ++itr)
        {
            if(itr->first == type)
            {
                sLog.outDebug("       %s", itr->second.c_str());
            }
        }
    }
    sLog.outString("    >> %u spells, %u auras, %u creatures, %u instances, %u gossip.", 
        _spellModules.size(), _auraModules.size(), _creatureModules.size(), _instanceModules.size(),
        _gossipModules.size());
}

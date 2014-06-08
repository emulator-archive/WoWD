// Copyright (C) 2004 WoW Daemon
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include "Common.h"
#include "Config/ConfigEnv.h"
#include "Log.h"
#include <stdarg.h>

createFileSingleton( Log );
initialiseSingleton( WorldLog );
createFileSingleton(Anticheat_Log);
createFileSingleton(GMCommand_Log);

#ifndef WIN32
static char* colorstrings[TBLUE+1] = {
"",
"\033[22;31m",
"\033[22;32m",
"\033[01;33m",
//"\033[22;37m",
"\033[0m",
"\033[01;37m",
"\033[22;34m",
};
#endif

void script_debuglog(const char* str, ...)
{
    if(loglevel < 2) return;
    if(!str)
        return;
    va_list l;
    va_start(l, str);
    // yellow
#ifdef WIN32
    SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif
    fprintf(stdout, "[script] ");
    vfprintf(stdout, str, l);
    fprintf(stdout, "\n");
#ifdef WIN32
    SetConsoleTextAttribute(sLog.stdout_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
    va_end(l);
}

void script_errorlog(const char* str, ...)
{
    if(loglevel < 2) return;
    if(!str)
        return;
    va_list l;
    va_start(l, str);
    // yellow
#ifdef WIN32
    SetConsoleTextAttribute(sLog.stderr_handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
    fprintf(stderr, "[script] ");
    vfprintf(stderr, str, l);
    fprintf(stderr, "\n");
#ifdef WIN32
    SetConsoleTextAttribute(sLog.stderr_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
    va_end(l);
}

void Log::outString( const char * str, ... ) {
    if( !str ) return;
    va_list ap;
    va_start(ap, str);
    if(fileenabled)
    {
        vfprintf(outfile,str,ap);
        fprintf(outfile,"\n");
    }
    if(screenenabled)
    {
        vprintf(str, ap );    
        printf( "\n" );
		fflush(stdout);
    }

    va_end(ap);
}

void Log::outError( const char * err, ... ) {
    if( !err || loglevel < 1) return;
    va_list ap;
    va_start(ap, err);
    if(fileenabled)
    {
        vfprintf(outfile,err,ap);
        fprintf(outfile,"\n");
    }
    if(screenenabled)
    {
#ifdef WIN32
        SetConsoleTextAttribute(stderr_handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
	printf(colorstrings[TRED]);
#endif
        vfprintf( stderr, err, ap );
        fprintf( stderr, "\n" );
		fflush(stderr);
#ifdef WIN32
        SetConsoleTextAttribute(stderr_handle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#else
	printf(colorstrings[TNORMAL]);
#endif
    }
    va_end(ap);
}

void Log::outBasic( const char * str, ... ) {
    if( !str || loglevel < 1) return;
    va_list ap;
    va_start(ap, str);
    if(fileenabled)
    {
        vfprintf(outfile,str,ap);
        fprintf(outfile,"\n");
    }
    if(screenenabled) {
        vprintf( str, ap );
        printf( "\n" );
		fflush(stdout);
    }
    va_end(ap);
}

void Log::outDetail( const char * str, ... ) {
    if( !str || loglevel < 2) return;
    va_list ap;
    va_start(ap, str);
    if(fileenabled)
    {
        vfprintf(outfile,str,ap);
        fprintf(outfile,"\n");
    }
    if(screenenabled) {
        vprintf( str, ap );
        printf( "\n" );
        fflush(stdout);
    }
    va_end(ap);
}

void Log::outDebug( const char * str, ... ) {
    if( !str || loglevel < 3) return;
    va_list ap;
    va_start(ap, str);
    if(fileenabled)
    {
        vfprintf(outfile,str,ap);
        fprintf(outfile,"\n");
    }
    if(screenenabled) {
        vprintf( str, ap );
        printf( "\n" );
        fflush(stdout);
    }
    va_end(ap);
}

void Log::outMenu( const char * str, ... ) {
    if( !str ) return;
    va_list ap;
    va_start(ap, str);
    if(fileenabled)
    {
        vfprintf(outfile,str,ap);
        fprintf(outfile,"\n");
    }
    if(screenenabled)
        vprintf( str, ap );

    va_end(ap);
    fflush(stdout);
}

void Log::CloseFile()
{
    if(fileenabled)
        fclose(outfile);
}

void Log::InitFile(const char *filename)
{
    outfile = fopen(filename,"at");
    if(!outfile)
        fileenabled = false;
    else
        fileenabled = true;
}

void Log::SetLogging(bool enabled)
{
    screenenabled = enabled;
}

void Log::Init()
{
    screenenabled = true;
    fileenabled = false;
    loglevel = 1;
    // get error handle
#ifdef WIN32
    stderr_handle = GetStdHandle(STD_ERROR_HANDLE);
    stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}

WorldLog::WorldLog()
{
    if (Config.MainConfig.GetBoolDefault("LogWorld", false))
    {
        sLog.outString("  Enabling packetlog output to \"world.log\"");
        fout = fopen("world.log", "w");
        bEnabled = true;
    } else {
        fout = NULL;
        bEnabled = false;
    }
}

WorldLog::~WorldLog()
{
    if(fout)
    {
        fclose(fout);
    }
}

Anticheat_Log::Anticheat_Log()
{
    out = 0;
}

Anticheat_Log::~Anticheat_Log()
{
    close();
}

void Anticheat_Log::init(const char* filename)
{
    out = fopen(filename, "a");
    write("new anti cheat log opened");
}

void Anticheat_Log::write(const char* format, ...)
{
    if(!out) return;

    va_list ap;
    va_start(ap, format);
    MUTEX.Acquire();
    
    // write timestamp to log
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    fprintf(out,"[%-4d-%02d-%02d %02d:%02d:%02d] ",aTm->tm_year+1900,aTm->tm_mon+1,aTm->tm_mday,aTm->tm_hour,aTm->tm_min,aTm->tm_sec);

    // write out text
    vfprintf(out, format, ap);
    fprintf(out, "\n");
    fflush(out);

    // finish up
    MUTEX.Release();
    va_end(ap);
}

void Anticheat_Log::close()
{
    if(out)
    {
        fclose(out);
        out = 0;
    }
}

GMCommand_Log::GMCommand_Log()
{
    out = 0;
}

GMCommand_Log::~GMCommand_Log()
{
    close();
}

void GMCommand_Log::init(const char* filename)
{
    out = fopen(filename, "a");
    write("new gm command log opened");
}

void GMCommand_Log::write(const char* format, ...)
{
    if(!out) return;

    va_list ap;
    va_start(ap, format);
    MUTEX.Acquire();

    // write timestamp to log
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    fprintf(out,"[%-4d-%02d-%02d %02d:%02d:%02d] ",aTm->tm_year+1900,aTm->tm_mon+1,aTm->tm_mday,aTm->tm_hour,aTm->tm_min,aTm->tm_sec);

    // write out text
    vfprintf(out, format, ap);
    fprintf(out, "\n");
    fflush(out);

    // finish up
    MUTEX.Release();
    va_end(ap);
}

void GMCommand_Log::close()
{
    if(out)
    {
        fclose(out);
        out = 0;
    }
}

void Log::outColor(uint32 colorcode, const char * str, ...)
{
    if( !str ) return;
    va_list ap;
    va_start(ap, str);
#ifdef WIN32
    SetConsoleTextAttribute(stdout_handle, colorcode);
#else
	printf(colorstrings[colorcode]);
#endif
    vprintf( str, ap );
    fflush(stdout);
    va_end(ap);
}

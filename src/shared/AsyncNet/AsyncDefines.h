/************************************************************************/
/* Copyright (C) 2006 WoWD                                              */
/************************************************************************/

#ifndef __ASYNCDEFINES_H
#define __ASYNCDEFINES_H

enum IOType 
{
	IOReadRequest = 0,
    IOReadCompleted,
    IOWriteRequest,
	IOWriteCompleted,
    IOAccept,
	IOShutdownRequest,
    IOShutdownCompleted
};

class OverLapped
{
public:
    OVERLAPPED m_ol;
    IOType m_io;

    OverLapped(IOType io) : m_io(io) {};
};

#endif
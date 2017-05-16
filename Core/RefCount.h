#ifndef __REF_COUNT_H_
#define __REF_COUNT_H_

//#include "stdafx.h"
#include "Includes/PrecompileConfig.h"

class CRefCount
{
public:
    CRefCount();
    virtual ~CRefCount();

public:
    long AddRef();
    long Release();

private:
	long            m_nRefCount;
	pthread_mutex_t		m_Mutex;
};

#endif




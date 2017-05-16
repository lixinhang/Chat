//#include "stdafx.h"
#include "RefCount.h"

// CRefCount
CRefCount::CRefCount()
: m_nRefCount(1)
{
	m_Mutex = PTHREAD_MUTEX_INITIALIZER;
}

CRefCount::~CRefCount()
{
}

long CRefCount::AddRef()
{
	long nRef = 0;

	pthread_mutex_lock(&m_Mutex);

	m_nRefCount++;
	nRef = m_nRefCount;

	pthread_mutex_unlock(&m_Mutex);

    return nRef;
}

long CRefCount::Release()
{
	long nRef = 0;

	pthread_mutex_lock(&m_Mutex);

	m_nRefCount--;
	nRef = m_nRefCount;

	pthread_mutex_unlock(&m_Mutex);

	if (!nRef){ delete this; }  
	return nRef;
}

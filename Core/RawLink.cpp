//#include "stdafx.h"
#include "RawLink.h"
#pragma warning(disable: 4996)

CRawLink::CRawLink()
{
    m_bOpen = false;
    m_bActive = false;
	m_pSink = NULL;
	m_pNetPeer = NULL;

	m_SynchMutex = PTHREAD_MUTEX_INITIALIZER;
	m_CallBackMutex = PTHREAD_MUTEX_INITIALIZER;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	m_hConnect = CreateEvent(NULL, TRUE, FALSE, NULL);
#else
	sem_init(&m_semConnect, 0, 0);
#endif
}

CRawLink::~CRawLink()
{
	Close();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	CloseHandle(m_hConnect);
#else
	sem_destroy(&m_semConnect);
#endif
}

void CRawLink::AttachSink(ILinkSink* pSink)
{

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_ASSERT(pSink != NULL);
#else
	assert(pSink != NULL);
#endif

	ILinkSink* pLastSink = NULL;

	{
		pthread_mutex_lock(&m_SynchMutex);

		if (m_pSink != NULL)
		{
			pLastSink = m_pSink;
		}

		m_pSink = pSink;
		m_pSink->AddRef();

		pthread_mutex_unlock(&m_SynchMutex);
	}

	if (pLastSink != NULL)
	{
		pLastSink->Release();
	}
}

bool CRawLink::Open(INetPeer* pNetPeer)
{
	pthread_mutex_lock(&m_SynchMutex);

	if (m_pNetPeer != NULL)
	{
		#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			_ASSERT(0);
		#else
			assert(0);
		#endif
		return false;
	}

	m_pNetPeer = pNetPeer;
	m_pNetPeer->AddRef();
	INetPeer::PEER_CONFIG peerConfig;
	m_pNetPeer->GetConfig(peerConfig);
	peerConfig.pSink = this;
	m_pNetPeer->UpdateConfig(peerConfig);
	bool bResult = m_pNetPeer->Open();
    if (bResult)
    {
        m_bOpen = true;
    }

	pthread_mutex_unlock(&m_SynchMutex);
	return bResult;
}

bool CRawLink::Open(const char* pIP, unsigned short nPort)
{
	pthread_mutex_lock(&m_SynchMutex);

    m_bActive = true;

    if (m_pNetPeer != NULL)
    {
        return false;
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	strcpy_s(m_sIP, pIP);
#else
	strcpy(m_sIP, pIP);
#endif

    m_nPort = nPort;

    m_pNetPeer = CreateNetPeer(this);
    INetPeer::PEER_CONFIG peerConfig;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    strcpy_s(peerConfig.sRemoteIP, sizeof(peerConfig.sRemoteIP), pIP);
#else
	strcpy(peerConfig.sRemoteIP, pIP);
#endif
    peerConfig.nRemotePort = nPort;
    peerConfig.transType = INetPeer::trans_tcp;
    peerConfig.pSink = this;
    m_pNetPeer->UpdateConfig(peerConfig);

    bool bResult = m_pNetPeer->Connect();
    if (!bResult)
    {
        return false;
    }

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	WaitForSingleObject(m_hConnect, CONNECTWAIT * MULTIPLENUM);
#else
	
	gettimeofday(&m_tTodayTime,NULL);
	m_tOutTime.tv_sec = m_tTodayTime.tv_sec;

	int nNum = MULTIPLENUM * MULTIPLENUM * MULTIPLENUM;
	m_tOutTime.tv_nsec = m_tTodayTime.tv_usec * MULTIPLENUM + CONNECTWAIT * MULTIPLENUM * MULTIPLENUM;
	m_tOutTime.tv_sec += m_tTodayTime.tv_usec / nNum;
	m_tOutTime.tv_sec %= nNum;

	//sem_timedwait(&m_semConnect, &m_tOutTime);
#endif

	pthread_mutex_unlock(&m_SynchMutex);
    return m_bOpen;
}

void CRawLink::Close()
{
	INetPeer* pNetPeer = NULL;

	{
		pthread_mutex_lock(&m_SynchMutex);

		pNetPeer = m_pNetPeer;
		m_pNetPeer = NULL;
        m_bOpen = false;

		while (m_PacketList.size() > 0)
		{
			CPacket* pPacket = m_PacketList.front();
			pPacket->Release();
			m_PacketList.pop_front();
		}

		pthread_mutex_unlock(&m_SynchMutex);
	}

	if (pNetPeer != NULL)
	{
        if (m_bActive)
        {
            pNetPeer->Close();
        }

		pNetPeer->Release();
        DeleteNetPeer(m_pNetPeer);
        m_pNetPeer = NULL;
	}

	ILinkSink* pSink = NULL;

	{
		pthread_mutex_lock(&m_CallBackMutex);

		pSink = m_pSink;
		m_pSink = NULL;

		pthread_mutex_unlock(&m_CallBackMutex);
	}

	if (pSink != NULL)
	{
		pSink->Release();
	}
}

long CRawLink::AddRef()
{
    return CRefCount::AddRef();
}

long CRawLink::Release()
{
    return CRefCount::Release();
}

bool CRawLink::SendPacket(CPacket* pPacket)
{
	pthread_mutex_lock(&m_SynchMutex);

	pPacket->AddRef();
	m_PacketList.push_back(pPacket);
	CPacket* pFirstPacket = m_PacketList.front();
	bool bResult = m_pNetPeer->SendData(pFirstPacket->GetTotal(), pFirstPacket->GetTotalSize());

	if (bResult)
	{
		pFirstPacket->Release();
		m_PacketList.pop_front();
	}

	pthread_mutex_unlock(&m_SynchMutex);
	return bResult;
}

void CRawLink::OnRecvData(char* pData, int nLen)
{
	ILinkSink* pSink = GetSink();

	if (pSink == NULL)
	{
		return;
	}

	int nUsed = 0;
	bool bErr = false;

	while (true)
	{
		if (nLen < sizeof(PACKET_HADER))
		{
			break;
		}

		PACKET_HADER* pPacketHeader = (PACKET_HADER*)(pData + nUsed);
		int nPacketSize = 0;
		char* pPacketSize = (char*)&nPacketSize;
		memcpy(pPacketSize, &pPacketHeader->packetSize, 4);
		int nSize = ntohl(nPacketSize);

		if (nLen < nSize)
		{
			break;
		}

		CPacket* pPacket = CPacket::CreateFromTotal(pData + nUsed, nSize);

		if (pPacket == NULL)
		{
			bErr = true;
			break;
		}

		pSink->OnLinkPacket(this, pPacket);
		pPacket->Release();

		nUsed += nSize;
		nLen -= nSize;
	}

	if (bErr)
	{
		pSink->OnLinkErr(this);
	}

	pSink->Release();
}

void CRawLink::OnSendOver()
{
	pthread_mutex_lock(&m_SynchMutex);

	if (m_pNetPeer == NULL)
	{
		return;
	}

	bool bResult = true;

	while (bResult && m_PacketList.size() > 0)
	{
		CPacket* pFirstPacket = m_PacketList.front();
		bResult = m_pNetPeer->SendData(pFirstPacket->GetTotal(), pFirstPacket->GetTotalSize());

		if (bResult)
		{
			pFirstPacket->Release();
			m_PacketList.pop_front();
		}
	}

	pthread_mutex_unlock(&m_SynchMutex);
}

void CRawLink::OnNetErr(long nErr)
{
	ILinkSink* pSink = GetSink();

	if (pSink == NULL)
	{
		return;
	}

	pSink->OnLinkErr(this);
	pSink->Release();
}

void CRawLink::OnConnect(bool bSuc)
{
    if (bSuc)
    {
        m_bOpen = true;
        m_pNetPeer->Open();
    }

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    SetEvent(m_hConnect);
#else
	sem_post(&m_semConnect);
#endif

}

ILinkSink* CRawLink::GetSink()
{
	ILinkSink* pSink = NULL;

	{
		pthread_mutex_lock(&m_CallBackMutex);

		if (m_pSink != NULL)
		{
			pSink = m_pSink;
			pSink->AddRef();
		}

		pthread_mutex_unlock(&m_CallBackMutex);
	}

	return pSink;
}

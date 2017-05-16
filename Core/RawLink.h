#ifndef __RAW_LINK_H_
#define __RAW_LINK_H_

#include "Packet.h"
#include "network.h"

using namespace NetWork;

#define CONNECTWAIT 3
#define MULTIPLENUM	1000

class CRawLink;

class ILinkSink
{
public:

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	virtual long __stdcall AddRef() = 0;
	virtual long __stdcall Release() = 0;
	virtual void __stdcall OnLinkPacket(CRawLink* pLink, CPacket* pPacket) = 0;
	virtual void __stdcall OnLinkErr(CRawLink* pLink) = 0;
#else

	virtual long __attribute__((__stdcall__)) AddRef() = 0;
	virtual long __attribute__((__stdcall__)) Release() = 0;
	virtual void __attribute__((__stdcall__)) OnLinkPacket(CRawLink* pLink, CPacket* pPacket) = 0;
	virtual void __attribute__((__stdcall__)) OnLinkErr(CRawLink* pLink) = 0;
#endif
};

// CRawLink
class CRawLink : public CRefCount, public INetPeerSink
{
public:
	CRawLink();

protected:
	virtual ~CRawLink();

public:
	void AttachSink(ILinkSink* pSink);
	bool Open(INetPeer* pNetPeer);
    bool Open(const char* pIP, unsigned short nPort);
	void Close();
	virtual bool SendPacket(CPacket* pPacket);

public:

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    virtual long __stdcall AddRef();
    virtual long __stdcall Release();
#else
	virtual long __attribute__((__stdcall__)) AddRef();
	virtual long __attribute__((__stdcall__)) Release();
#endif

protected:

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	virtual void __stdcall OnRecvData(char* pData, int nLen);
	virtual void __stdcall OnSendOver();
	virtual void __stdcall OnNetErr(long nErr);
	virtual void __stdcall OnConnect(bool bSuc);
#else

	virtual void __attribute__((__stdcall__)) OnRecvData(char* pData, int nLen);
	virtual void __attribute__((__stdcall__)) OnSendOver();
	virtual void __attribute__((__stdcall__)) OnNetErr(long nErr);
	virtual void __attribute__((__stdcall__)) OnConnect(bool bSuc);
#endif

private:
	ILinkSink* GetSink();

private:

	typedef std::list<CPacket*> PACKET_LIST;

    bool                m_bOpen;
    bool                m_bActive;
	ILinkSink*          m_pSink;
	INetPeer*           m_pNetPeer;
	PACKET_LIST         m_PacketList;

    char                m_sIP[32];
    unsigned short      m_nPort;

	pthread_mutex_t		m_SynchMutex;
	pthread_mutex_t		m_CallBackMutex;


#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	HANDLE              m_hConnect;
#else
	sem_t				m_semConnect;

	struct timeval		m_tTodayTime; 
	struct timespec		m_tOutTime;
#endif
};

#endif

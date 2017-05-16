// ChatRoom.h
//

#ifndef __CHAT_ROOM_H_
#define __CHAT_ROOM_H_

#include "cxgChatRoom.h"
#include "RawLink.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "Timer.h"
#endif

using namespace ChatRoom;

std::string unescape(char* str);
static CRawLink*	g_pLink = NULL;

class CChatRoom
	: public IChatRoom
	, public ILinkSink
	, public CRefCount
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	, public CTimer
#endif
{
public:
	CChatRoom(IChatRoomObserver* pObserver);
	~CChatRoom();

public:

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	virtual bool __stdcall IsEntered(void);
	virtual bool __stdcall IsConnect(void);

	virtual int  __stdcall EnterChatRoom(void);
	virtual int  __stdcall ReentryChatRoom(int nNodeNum =0);
	virtual void __stdcall ExitChatRoom(void);

	virtual void __stdcall SetToken(std::string strToken);
	virtual void __stdcall SetChatRoomInfo(ChatRoomInfo RoomInfo);
	virtual void __stdcall SendChatMsg(std::string strMsg, unsigned long long nMasterid, bool bPrivate);

	virtual std::string __stdcall GetErrMsg(void);
	virtual void __stdcall SendChatMsgPacket();
	virtual void __stdcall SendHreatBeatPacket();
#else

	virtual int __attribute__((__stdcall__)) EnterChatRoom(void);
	virtual int __attribute__((__stdcall__)) ReentryChatRoom(int nNodeNum = 0);
    
	virtual bool __attribute__((__stdcall__)) IsEntered(void);
	virtual bool __attribute__((__stdcall__)) IsConnect(void);

	virtual void __attribute__((__stdcall__)) ExitChatRoom(void);
	virtual void __attribute__((__stdcall__)) SetToken(std::string strToken);
	virtual void __attribute__((__stdcall__)) SetChatRoomInfo(ChatRoomInfo RoomInfo);
	virtual void __attribute__((__stdcall__)) SendChatMsg(std::string strMsg,unsigned long long nMasterid, bool bPrivate);
    
	virtual std::string __attribute__((__stdcall__)) GetErrMsg(void);
#endif

private:

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	virtual long __stdcall AddRef();
	virtual long __stdcall Release();
	virtual void __stdcall OnLinkPacket(CRawLink* pLink, CPacket* pPacket);
	virtual void __stdcall OnLinkErr(CRawLink* pLink);

	bool OnTimer(int nTimeId);
#else

	virtual long __attribute__((__stdcall__)) AddRef();
	virtual long __attribute__((__stdcall__)) Release();
	virtual void __attribute__((__stdcall__)) OnLinkPacket(CRawLink* pLink, CPacket* pPacket);
	virtual void __attribute__((__stdcall__)) OnLinkErr(CRawLink* pLink);

    void KeepLive();
    bool IsEnterChatRoom(time_t current);
    static void* SendHeartBeat(void *arg);
    
#endif

	void OpenTCPLink(int nNodeNum);

	std::string EscapeCode(std::string& str);
	bool EscapeCode(std::string& str, std::string oldchar, std::string newchar);

	std::string Utf_8ToANSI(const std::string& strUtf8);
	std::string ANSIToUtf_8(const std::string& strANSI);
	bool Utf_8ToANSI(const char* pUtf8, int nUtf_8Len, char* pANSI, int* ANSILen);
	bool ANSIToUtf_8(const char* pANSI, int nANSILen, char* pUtf_8, int* Utf_8Len);

private:

	bool m_bEnter;
	bool m_bConnect;

	unsigned long long m_nMasterId;
	unsigned long long m_nRoomId;
	unsigned long long m_nMasterNo;
	unsigned int       m_nKeepLiveTimer;

	ChatRoomMsg	m_eMsgType;
	const char*	m_strChatMsg;
    
    int         m_nRoomPort;
	std::string	m_strToken;
	std::string	m_strRoomIp[MAX_CHAT_NODE];

	pthread_mutex_t    m_SynchMutex;
    IChatRoomObserver* m_pObserver;
    CRawLink*          m_pLink;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	HANDLE      m_hEnter;
#else
    time_t      m_tLastTime;
    time_t      m_tEvent;
    pthread_t   m_HeartBeat;
#endif
};

#endif

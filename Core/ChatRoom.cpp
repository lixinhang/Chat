// ChatRoom.cpp
//

//#include "stdafx.h"
#include "ChatRoom.h"

#pragma warning(disable: 4996)

long hex2dec(char * s)
{
	int L = (int)strlen(s);
	char c;
	long re = 0;
	while (c = s++[0])
	{
		if (c >= '0' && c <= '9')
		{
			c -= 48;
		}		
		else
		{
			c = c>'Z' ? c - 32 : c;
			c -= 'A' - 10;
		}
		re += c*pow(16.0, --L);
	}

	return re;
}

std::string unescape(char* str)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	const int nMultiByte = 2;
#else
	int nByte = 0;
	const int nMultiByte = 3;
#endif

	char* re = (char*)calloc(strlen(str) + 1, 1);
	char* _str;
	char* _re = re;
	char code[5] = { 0 };
	wchar_t wc;
	int n;
	

	while (str)
	{
		_str = strchr(str, '%');
		if (!_str)
		{
			break;
		}

		if (n = _str - str)
		{
			memcpy(_re, str, n);
			_re += n;
		}

		memset(code, 0, sizeof(code));
		if (_str[1] == 'u')
		{
			memcpy(code, _str + 2, 4);
			str = _str + 6;
			wc = hex2dec(code);
			memset(code, 0, 5);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			WideCharToMultiByte(CP_OEMCP, NULL, &wc, -1, (char*)code, nMultiByte, NULL, FALSE);
			memcpy(_re, code, nMultiByte);
			_re += nMultiByte;
#else
			wcstombs(code, &wc, nMultiByte);
			nByte = (int)strlen(code);
			memcpy(_re, code, nByte);
			_re += nByte;
#endif
		}
		else
		{
			memcpy(code, _str + 1, 2);
			str = _str + 3;
			_re[0] = hex2dec(code);
			_re++;
		}
	}

	strcpy(_re, str);
	std::string rt(re);
	free(re);
	re = NULL;

	return rt;
}

CChatRoom::CChatRoom(IChatRoomObserver* pObserver)
{
	m_pObserver = pObserver;
	m_pLink = NULL;

	m_bEnter = false;
	m_bConnect = false;
	m_nKeepLiveTimer = 0;
	m_nMasterId = 0;
	m_nMasterNo = 0;
	m_nRoomId = 0;
	m_nRoomPort = 0;

	m_strToken.clear();
	m_strChatMsg = "";
	m_eMsgType = CR_MSG;

	m_SynchMutex = PTHREAD_MUTEX_INITIALIZER;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	m_hEnter = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
}

CChatRoom::~CChatRoom()
{
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    StopTimer(m_nKeepLiveTimer);
#else
    pthread_detach(m_HeartBeat);
#endif
	
	if (m_pLink)
	{
		m_pLink->Close();
		m_pLink->Release();
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	CloseHandle(m_hEnter);
#endif
}

void CChatRoom::SetToken(std::string strToken)
{
	m_strToken = strToken;
}

void CChatRoom::SetChatRoomInfo(ChatRoomInfo RoomInfo)
{
	m_nRoomId	= RoomInfo.nRoomId;
	m_nRoomPort = RoomInfo.nPort;
	m_nMasterId = RoomInfo.nMasterId;
	m_nMasterNo = RoomInfo.nMasterNo;

	for (int i = 0; i < MAX_CHAT_NODE; i++)
	{
		m_strRoomIp[i] = RoomInfo.strIp[i];
	}
}

void CChatRoom::OpenTCPLink(int nNodeNum)
{
	if (m_pLink)
	{
		m_pLink->Close();
		m_pLink->Release();
		m_pLink = NULL;
	}

	int nTry = 0;
	m_bConnect = false;
	while (nTry++ < 3)
	{
		m_pLink = new CRawLink();
		if (g_pLink != m_pLink){ g_pLink = m_pLink; }
		m_pLink->AttachSink(this);

		bool bRet = m_pLink->Open(m_strRoomIp[nNodeNum].c_str(), m_nRoomPort);
		if (bRet)
		{
			m_bConnect = true;
			break;
		}

		if (m_pLink)
		{
			m_pLink->Close();
			m_pLink->Release();
			m_pLink = NULL;
		}
	}
}

int CChatRoom::EnterChatRoom()
{
	if (m_bEnter)
	{
		return 0;
	}

	int nNodeNum = 0;
	{
		pthread_mutex_lock(&m_SynchMutex);

		while (nNodeNum < MAX_CHAT_NODE)
		{
			OpenTCPLink(nNodeNum);
			if (!m_bConnect){ nNodeNum++; }
			else{ break; }
		}

		if (!m_bConnect)
		{
			m_strChatMsg = ERR_CONNECT_FAILD;
			m_eMsgType = CR_ERROR;
			m_pObserver->OnChatRoom(m_eMsgType, m_strChatMsg);
			ExitChatRoom();
			return 0;
		}

		std::string jsonEnter = "";

		CPacket* pPacket = CPacket::CreateFromPayload((char*)jsonEnter.c_str(), jsonEnter.length());
		pPacket->SetPacketType(ENTER);
		m_pLink->SendPacket(pPacket);

		pthread_mutex_unlock(&m_SynchMutex);
	}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	unsigned long dwCode = ::WaitForSingleObject(m_hEnter, MAX_WAIT_TIME * MULTIPLENUM);
	if (dwCode != WAIT_OBJECT_0)
	{
		m_strChatMsg = ERR_ENTER_FAILD;
		m_eMsgType = CR_ERROR;
		m_pObserver->OnChatRoom(m_eMsgType, m_strChatMsg);
		return 0;
	}

	m_nKeepLiveTimer = StartTimer(true, KEEPLIVE_INTERVAL * MULTIPLENUM, 50);
#else

    time_t curtime = time(NULL);
    if(!IsEnterChatRoom(curtime))
    {
        m_strChatMsg = ERR_ENTER_FAILD;
        m_eMsgType = CR_ERROR;
        m_pObserver->OnChatRoom(m_eMsgType,m_strChatMsg);
    }
	KeepLive();
#endif
	return nNodeNum;
}

int CChatRoom::ReentryChatRoom(int nNodeNum)
{
	if (m_bEnter){ return nNodeNum; }

	{
		pthread_mutex_lock(&m_SynchMutex);

		OpenTCPLink(nNodeNum);
		if (!m_bConnect){ return EnterChatRoom(); }

		std::string jsonEnter = "";

		CPacket* pPacket = CPacket::CreateFromPayload((char*)jsonEnter.c_str(), jsonEnter.length());
		pPacket->SetPacketType(ENTER);
		m_pLink->SendPacket(pPacket);

		pthread_mutex_unlock(&m_SynchMutex);
	}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	unsigned long dwCode = ::WaitForSingleObject(m_hEnter, MAX_WAIT_TIME * MULTIPLENUM);
	if (dwCode != WAIT_OBJECT_0)
	{
		m_strChatMsg = ERR_REENTRY_FAILD;
		m_pObserver->OnChatRoom(CR_ERROR, m_strChatMsg);
		return nNodeNum;
	}

	m_nKeepLiveTimer = StartTimer(true, KEEPLIVE_INTERVAL * MULTIPLENUM, 50);
#else

    time_t curtime = time(NULL);
    if(!IsEnterChatRoom(curtime))
    {
        m_strChatMsg = ERR_ENTER_FAILD;
        m_eMsgType = CR_ERROR;
        m_pObserver->OnChatRoom(m_eMsgType, m_strChatMsg);
    }

	KeepLive();
#endif
 
    return nNodeNum;
}

void CChatRoom::ExitChatRoom()
{
	if (!m_bEnter)
	{
		return;
	}

	m_bEnter = false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	StopTimer(m_nKeepLiveTimer);
#else
	pthread_detach(m_HeartBeat);
#endif

	if (m_pLink != NULL)
	{
		m_pLink->Close();
		m_pLink->Release();
		m_pLink = NULL;
	}
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

bool CChatRoom::OnTimer(int nTimeID)
{
	if (nTimeID == m_nKeepLiveTimer)
	{
		pthread_mutex_lock(&m_SynchMutex);

		time_t tStamp = time(NULL);

		std::string HreatBeat = "";

		CPacket* pPacket = CPacket::CreateFromPayload((char*)HreatBeat.c_str(), HreatBeat.length());
		pPacket->SetPacketType(KEEPLIVE);
		if (m_pLink)
		{
			m_pLink->SendPacket(pPacket);
		}

		pthread_mutex_unlock(&m_SynchMutex);
	}
	return true;
}

#else

void CChatRoom::KeepLive()
{
	if (m_bEnter)
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		time(&m_tLastTime);
		pthread_create(&m_HeartBeat, &attr, CChatRoom::SendHeartBeat, this);
	}
}

bool CChatRoom::IsEnterChatRoom(time_t current)
{
	time_t tstamp = time(NULL);
	while (tstamp - current <= MAX_WAIT_TIME)
	{
		if (m_bEnter){ break; }
		usleep(500000);
		time(&tstamp);
	}

	return m_bEnter;
}

void* CChatRoom::SendHeartBeat(void *arg)
{
	CChatRoom* pCt = (CChatRoom*)arg;
	time_t tStamp;
	while (pCt->m_bEnter)
	{
		time(&tStamp);
		if (tStamp - pCt->m_tLastTime >= KEEPLIVE_INTERVAL)
		{
			std::string HreatBeat = "";

			CPacket* pPacket = CPacket::CreateFromPayload((char*)HreatBeat.c_str(), HreatBeat.length());
			pPacket->SetPacketType(KEEPLIVE);
			if (pCt->m_pLink)
			{
				pCt->m_pLink->SendPacket(pPacket);
			}
			pCt->m_tLastTime = tStamp;
		}
		sleep(TICKER_TIME);
	}

	return 0;
}
#endif

void CChatRoom::SendChatMsg(std::string strMsg,unsigned long long nMasterid, bool bPrivate)
{
   	pthread_mutex_lock(&m_SynchMutex);
    
	std::string jsonChatMsg = "";
    
    CPacket* pPacket = CPacket::CreateFromPayload((char*)jsonChatMsg.c_str(), jsonChatMsg.length());
    
    if(!nMasterid)
        pPacket->SetPacketType(SENDMSG);
    else
    {
        if(bPrivate)
            pPacket->SetPacketType(SENDMSG_PRI);
        else
            pPacket->SetPacketType(SENDMSG_PUB);
    }
    
    if (m_pLink)
    {
        m_pLink->SendPacket(pPacket);
    }
    
    pthread_mutex_unlock(&m_SynchMutex);
}

void CChatRoom::OnLinkErr(CRawLink* pLink)
{
	if (pLink != m_pLink)
	{
		return;
	}

	pthread_mutex_lock(&m_SynchMutex);

	m_bEnter = false;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	StopTimer(m_nKeepLiveTimer);
#else
    pthread_detach(m_HeartBeat);
#endif

	if (m_pLink != NULL)
	{
		m_pLink->Close();
		m_pLink->Release();
		m_pLink = NULL;
	}

	m_strChatMsg = MSG_RECONNECT_CHAT;
	m_eMsgType = CR_RECONNECT;
	m_pObserver->OnChatRoom(m_eMsgType, m_strChatMsg);

	pthread_mutex_unlock(&m_SynchMutex);
}

void CChatRoom::OnLinkPacket(CRawLink* pLink, CPacket* pPacket)
{
	if (pPacket == NULL)
	{
		return;
	}

	if (pPacket->GetPacketType() == KEEPLIVE)
	{
		return;
	}

	uLongf dlen = sizeof(char)* (MAX_BUF_SIZE);
	char* dbuf = (char*)malloc(dlen);
	memset(dbuf, 0, dlen);
	int nRet = uncompress((Bytef*)dbuf, &dlen, (Bytef*)pPacket->GetPayload(), pPacket->GetPacketSize());
	while (nRet != Z_OK)
	{
		if (dlen / MAX_BUF_SIZE > 10)
		{
			break;
		}

		dlen *= 2;
		free(dbuf);
		dbuf = (char*)malloc(dlen);
		memset(dbuf, 0, dlen);
		nRet = uncompress((Bytef*)dbuf, &dlen, (Bytef*)pPacket->GetPayload(), pPacket->GetPacketSize());
	}

	Json::Reader reader;
	Json::Value object;

	try
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		if (!reader.parse(Utf_8ToANSI(dbuf), object))
#else
        if (!reader.parse(dbuf, object))
#endif
		{
			return;
		}
	}
	catch (std::exception& e)
	{
		return;
	}

	std::string code = object[""].asString();
	Json::Value msgObj = object[""];
	if (!msgObj.empty())
	{
		Json::Value::iterator itc = msgObj.begin();
		std::string action = (*itc)[""].asString();
		std::string msgtype = (*itc)[""].asString();
		int nMsgType = atoi(msgtype.c_str());

		switch (nMsgType)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;

		default:
			break;
		}
	}

	if (dbuf)
	{
		free(dbuf);
	}

	return;
}

long CChatRoom::AddRef()
{
	return CRefCount::AddRef();
}

long CChatRoom::Release()
{
	return CRefCount::Release();
}

std::string CChatRoom::GetErrMsg()
{
	return m_strChatMsg;
}

bool CChatRoom::IsEntered()
{
	return m_bEnter;
}

bool CChatRoom::IsConnect()
{
	return m_bConnect;
}

#define STACK_ARRAY(TYPE, LEN) static_cast<TYPE*>(::alloca((LEN)*sizeof(TYPE)))
std::string CChatRoom::Utf_8ToANSI(const std::string& strUtf8)
{
	int nLen = strUtf8.size() * 4;
	char* pANSI = STACK_ARRAY(char, nLen);
	Utf_8ToANSI(strUtf8.c_str(), strUtf8.size(), pANSI, &nLen);
	std::string strANSI = pANSI;
	strANSI.assign(pANSI, nLen);
	return strANSI;
}

std::string CChatRoom::ANSIToUtf_8(const std::string& strANSI)
{
	int nLen = strANSI.size() * 4;
	char* pUtf_8 = STACK_ARRAY(char, nLen);
	ANSIToUtf_8(strANSI.c_str(), strANSI.size(), pUtf_8, &nLen);
	std::string strUtf_8;
	strUtf_8.assign(pUtf_8, nLen);
	return strUtf_8;
}

bool CChatRoom::Utf_8ToANSI(const char* pUtf8, int nUtf_8Len, char* pANSI, int* ANSILen)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	int nUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, pUtf8, nUtf_8Len, NULL, 0);
	wchar_t* pUnicode = new wchar_t[nUnicodeLen + 1];
	memset(pUnicode, 0, nUnicodeLen * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, pUtf8, nUtf_8Len, pUnicode, nUnicodeLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pUnicode, nUnicodeLen, NULL, 0, NULL, NULL);
	if (*ANSILen < nLen)
	{
		return false;
	}

	memset(pANSI, 0, *ANSILen);
	WideCharToMultiByte(CP_ACP, 0, pUnicode, nUnicodeLen, pANSI, nLen, NULL, NULL);
	*ANSILen = nLen;

	delete[] pUnicode;

#else
	size_t nUnicodeLen = mbstowcs(nullptr, pUtf8, 0);
	wchar_t* pUnicode = new wchar_t[nUnicodeLen + 1];
	memset(pUnicode, 0, nUnicodeLen * 2 + 2);
	mbstowcs(pUnicode, pUtf8, nUtf_8Len);

	size_t nLen = wcstombs(nullptr, pUnicode, 0);
	if (*ANSILen < nLen)
	{
		return false;
	}

	memset(pANSI, 0, *ANSILen);
	wcstombs(pANSI, pUnicode, nLen);

	delete[] pUnicode;

#endif
	return true;
}

bool CChatRoom::ANSIToUtf_8(const char* pANSI, int nANSILen, char* pUtf_8, int* Utf_8Len)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	int nUnicodeLen = MultiByteToWideChar(CP_ACP, 0, pANSI, nANSILen, NULL, 0);
	wchar_t* pUnicode = new wchar_t[nUnicodeLen + 1];
	memset(pUnicode, 0, nUnicodeLen * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, pANSI, nANSILen, pUnicode, nUnicodeLen);

	int nLen = WideCharToMultiByte(CP_UTF8, 0, pUnicode, nUnicodeLen, NULL, 0, NULL, NULL);
	if (*Utf_8Len < nLen)
	{
		return false;
	}

	memset(pUtf_8, 0, *Utf_8Len);
	WideCharToMultiByte(CP_UTF8, 0, pUnicode, nUnicodeLen, pUtf_8, nLen, NULL, NULL);
	*Utf_8Len = nLen;

	delete[] pUnicode;
#endif

	return true;
}

std::string CChatRoom::EscapeCode(std::string& str)
{
	EscapeCode(str, "&amp;", "&");
	EscapeCode(str, "&nbsp;", " ");
	EscapeCode(str, "&middot;", "¡¤");
	EscapeCode(str, "&rsquo;", "'");
	EscapeCode(str, "&lsquo;", "'");
	EscapeCode(str, "&ldquo;", "\"");
	EscapeCode(str, "&rdquo;", "\"");
	EscapeCode(str, "&quot;", "\"");
	EscapeCode(str, "&mdash;", "¡ª");
	EscapeCode(str, "&hellip;", "¡­¡­");
	EscapeCode(str, "&lt;", "<");
	EscapeCode(str, "&rarr;", "¡ú");
	EscapeCode(str, "&cap;", "¡É");

	return str;
}

bool CChatRoom::EscapeCode(std::string& str, std::string oldchar, std::string newchar)
{
	std::string::size_type pos = str.find(oldchar);
	while (pos != std::string::npos)
	{
		str.replace(pos, oldchar.size(), newchar);
		pos = str.find(oldchar);
	}

	return true;
}

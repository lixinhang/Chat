#ifndef __CXGCHATROOM_
#define __CXGCHATROOM_

#include "Includes/PrecompileConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#ifdef CXGCHATROOM_EXPORTS
	#define CXGCHATROOM_API __declspec(dllexport)
#else
	#define CXGCHATROOM_API __declspec(dllimport)
#endif
#endif

#include "ChatMsgInfo.h"

namespace ChatRoom
{
	enum ChatRoomMsg
	{
		CR_ERROR = 0,
		CR_ENTER_OK,
		CR_ENTER_ERR,
		CR_CONNECT_OK,
		CR_CONNECT_ERR,
		CR_RECONNECT,
		CR_EXIT,

		CR_SPEAK,
		CR_TRUMPET,
		CR_PRESENT,
		CR_SYSMSG,

		CR_USERLIST,
		CR_USERCOUNT,
		CR_USERADD,
		CR_USERDEL,

		CR_LIVE_OK,
		CR_LIVE_FAIL,
		CR_LIVE_RESTART,
		CR_LIVE_INTERRUPT,
		CR_LIVE_STOP,
		CR_LIVE_QUIT,

		CR_MSG
	};

	struct ChatRoomInfo
	{
		int	nPort;
		unsigned long long nMasterNo;
		unsigned long long nRoomId;
		unsigned long long nMasterId;

		std::string strIp[MAX_CHAT_NODE];

		ChatRoomInfo()
		{
			nPort	= 0;
			nRoomId	= 0;
			nMasterId = 0;
			nMasterNo = 0;

			for (int i = 0; i < MAX_CHAT_NODE; i++)
			{
				strIp[i].clear();
			}	
		}
	};

	class IChatRoomObserver
	{
	public:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		virtual void __stdcall OnChatRoom(ChatRoomMsg crMsg, const char* strMsg) = 0;
#else
		virtual void __attribute__((__stdcall__)) OnChatRoom(ChatRoomMsg crMsg, const char* strMsg) = 0;
#endif
	};

	class IChatRoom
	{
	public:

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

		virtual bool __stdcall IsEntered(void) = 0;
		virtual bool __stdcall IsConnect(void) = 0;

		virtual int  __stdcall EnterChatRoom(void) = 0;
		virtual int  __stdcall ReentryChatRoom(int nNodeNum = 0) = 0;
		virtual void __stdcall ExitChatRoom(void) = 0;

		virtual void __stdcall SetToken(std::string strToken) = 0;
		virtual void __stdcall SetChatRoomInfo(ChatRoomInfo RoomInfo) = 0;

		virtual std::string __stdcall GetErrMsg(void) = 0;
		virtual void __stdcall SendChatMsgPacket() = 0;
		virtual void __stdcall SendHreatBeatPacket() = 0;
#else

		virtual int __attribute__((__stdcall__)) EnterChatRoom(void) = 0;
		virtual int __attribute__((__stdcall__)) ReentryChatRoom(int nNodeNum = 0) = 0;
        
		virtual bool __attribute__((__stdcall__)) IsEntered(void) = 0;
		virtual bool __attribute__((__stdcall__)) IsConnect(void) = 0;

		virtual void __attribute__((__stdcall__)) ExitChatRoom(void) = 0;
		virtual void __attribute__((__stdcall__)) SetToken(std::string strToken) = 0;
		virtual void __attribute__((__stdcall__)) SetChatRoomInfo(ChatRoomInfo RoomInfo) = 0;
		virtual void __attribute__((__stdcall__)) SendChatMsg(std::string strMsg,unsigned long long nMasterid, bool bPrivate) = 0;

		virtual std::string __attribute__((__stdcall__)) GetErrMsg(void) = 0;
        
#endif
	};

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	extern "C"
	{
		CXGCHATROOM_API IChatRoom* CreateChatRoom(IChatRoomObserver* pCb);
		CXGCHATROOM_API void DeleteChatRoom(IChatRoom* pVChat);
	}
#else
	extern "C"
	{
		IChatRoom* CreateChatRoom(IChatRoomObserver* pCb);
		void DeleteChatRoom(IChatRoom* pVChat);
	}
#endif
}

#endif /*__CXGCHATROOM_*/

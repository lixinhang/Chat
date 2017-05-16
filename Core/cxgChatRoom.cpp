// cxgChatRoom.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"
#include "ChatRoom.h"

namespace ChatRoom
{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	CXGCHATROOM_API IChatRoom* CreateChatRoom(IChatRoomObserver* pObserver)
	{
		CChatRoom* pChatRoom = new CChatRoom(pObserver);
		return pChatRoom;
	}

	CXGCHATROOM_API void DeleteChatRoom(IChatRoom* pChatRoom)
	{
		if (pChatRoom)
		{
			CChatRoom* _pChatRoom = (CChatRoom*)pChatRoom;
			delete _pChatRoom;
		}
	}
#else

	IChatRoom* CreateChatRoom(IChatRoomObserver* pObserver)
	{
		CChatRoom* pChatRoom = new CChatRoom(pObserver);
		return pChatRoom;
	}

	void DeleteChatRoom(IChatRoom* pChatRoom)
	{
		if (pChatRoom)
		{
			CChatRoom* _pChatRoom = (CChatRoom*)pChatRoom;
			delete _pChatRoom;
		}
	}
#endif
}
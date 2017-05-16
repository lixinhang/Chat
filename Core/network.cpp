//#include "stdafx.h"
#include "NetPeer.h"

namespace NetWork
{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	NETWORK_API INetPeer* CreateNetPeer(INetPeerSink* pNb)
	{
		CNetPeer* pNetPeer = new CNetPeer(pNb);
		return pNetPeer;
	}

	NETWORK_API void DeleteNetPeer(INetPeer* pNetPeer)
	{
		if (pNetPeer)
		{
			CNetPeer* _pNetPeer = (CNetPeer*)pNetPeer;
			delete _pNetPeer;
		}
	}

#else

	INetPeer* CreateNetPeer(INetPeerSink* pNb)
	{
		CNetPeer* pNetPeer = new CNetPeer(pNb);
		return pNetPeer;
	}

	void DeleteNetPeer(INetPeer* pNetPeer)
	{
		if (pNetPeer)
		{
			CNetPeer* _pNetPeer = (CNetPeer*)pNetPeer;
			delete _pNetPeer;
		}
	}
#endif
}

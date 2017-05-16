#ifndef __PACKET_H_
#define __PACKET_H_

#include "RefCount.h"

typedef unsigned char PACKET_TYPE;

#pragma pack(push, 1)

struct PACKET_HADER
{
	int packetSize;
    int packetType;
};

#pragma pack(pop)

// CPacket
class CPacket
	: public CRefCount
{
public:
	static CPacket* CreateFromPayload(char* pPayload, int nPayload);
	static CPacket* CreateFromTotal(char* pTotal, int nTotal);

    void SetPacketType(int ptType);
    int GetPacketType() const;
	int GetPacketSize() const;
	char* GetPayload() const;
	int GetPayloadSize() const;
	char* GetTotal() const;
	int GetTotalSize() const;

protected:
	CPacket();
	virtual ~CPacket();
	bool InitPayload(char* pPayload, int nPayload);
	bool InitTotal(char* pTotal, int nTotal);

private:
	PACKET_HADER*    m_pHeader;
	char*            m_pTotal;
	int              m_nTotal;
	char*            m_pPayload;
	int              m_nPayload;
};

#endif

//#include "stdafx.h"
#include "Packet.h"

CPacket* CPacket::CreateFromPayload(char* pPayload, int nPayload)
{
	CPacket* pPacket = new CPacket();

	if (pPacket->InitPayload(pPayload, nPayload))
	{
		return pPacket;
	}
	else
	{
		delete pPacket;
		return NULL;
	}
}

CPacket* CPacket::CreateFromTotal(char* pTotal, int nTotal)
{
	CPacket* pPacket = new CPacket();

	if (pPacket->InitTotal(pTotal, nTotal))
	{
		return pPacket;
	}
	else
	{
		delete pPacket;
		return NULL;
	}
}

CPacket::CPacket()
{
	m_pHeader = NULL;
	m_pTotal = NULL;
	m_nTotal = 0;
	m_pPayload = NULL;
	m_nPayload = 0;
}

CPacket::~CPacket()
{
	if (m_pTotal != NULL)
	{
		delete [] m_pTotal;
	}
}

void CPacket::SetPacketType(int ptType)
{
    int nPacketType = htonl(ptType);
    char* pPacketType = (char*)&nPacketType;
    memcpy(&m_pHeader->packetType, pPacketType, 4);
}

int CPacket::GetPacketType() const
{
    int nPacketType = 0;
    char* pPacketType = (char*)&nPacketType;
    memcpy(pPacketType, &m_pHeader->packetType, 4);
    nPacketType = ntohl(nPacketType);

    return nPacketType;
}

int CPacket::GetPacketSize() const
{
	int nPacketSize = 0;
	char* pPacketSize = (char*)&nPacketSize;
	memcpy(pPacketSize, &m_pHeader->packetSize, 4);
	nPacketSize = ntohl(nPacketSize);

	return nPacketSize;
}

char* CPacket::GetPayload() const
{
	return m_pPayload;
}

int CPacket::GetPayloadSize() const
{
	return m_nPayload;
}

char* CPacket::GetTotal() const
{
	return m_pTotal;
}

int CPacket::GetTotalSize() const
{
	return m_nTotal;
}

bool CPacket::InitPayload(char* pPayload, int nPayload)
{
	m_nTotal = sizeof(PACKET_HADER) + nPayload;
	m_pTotal = new char[m_nTotal];
	m_pHeader = (PACKET_HADER*)m_pTotal;
	m_pPayload = m_pTotal + sizeof(PACKET_HADER);
	m_nPayload = nPayload;
	memcpy(m_pPayload, pPayload, nPayload);
	memset(m_pHeader, 0, sizeof(PACKET_HADER));

	int nPacketSize = htonl(m_nTotal);
	char* pPacketSize = (char*)&nPacketSize;
	memcpy(&m_pHeader->packetSize, pPacketSize, 4);

	return true;
}

bool CPacket::InitTotal(char* pTotal, int nTotal)
{
	if (nTotal <= sizeof(PACKET_HADER))
	{
		return false;
	}

	PACKET_HADER* pHeader = (PACKET_HADER*)pTotal;

	int nPacketSize = 0;
	char* pPacketSize = (char*)&nPacketSize;
	memcpy(pPacketSize, &pHeader->packetSize, 4);
	nPacketSize = ntohl(nPacketSize);

	if (nPacketSize != nTotal)
	{
		return false;
	}

	m_nTotal = nTotal;
	m_pTotal = new char[m_nTotal];
	m_pHeader = (PACKET_HADER*)m_pTotal;
	m_pPayload = m_pTotal + sizeof(PACKET_HADER);
	m_nPayload = m_nTotal - sizeof(PACKET_HADER);
	memcpy(m_pTotal, pTotal, nTotal);

	return true;
}

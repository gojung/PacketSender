// IPLayer.cpp: implementation of the CIPLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PacketSender.h"
#include "IPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPLayer::CIPLayer( char* pName )
: CBaseLayer( pName )
{
	ResetHeader( );
}

CIPLayer::~CIPLayer()
{
}

void CIPLayer::ResetHeader()
{
	///////////////////////////// Fill in the blank. ////////////////////////////////////////
	m_sHeader.ip_verlen = 0x45;	//fil - IP verdion & length ver
	/////////////////////////////////////////////////////////////////////////////////////////

	m_sHeader.ip_tos = 0x00;		//16bit
	m_sHeader.ip_len = 0x0000;		//32bit
	m_sHeader.ip_id = 0x0000;		//28bit
	m_sHeader.ip_fragoff = 0x0000;	//32bit??
	m_sHeader.ip_ttl = 0x80;		//8bit

	///////////////////////////// Fill in the blank. ////////////////////////////////////////
	m_sHeader.ip_proto = IP_PROTO_SCTP;	// SCTP (132)	//fill
	/////////////////////////////////////////////////////////////////////////////////////////

	m_sHeader.ip_cksum = 0x0000;	//32bit
	memset(m_sHeader.ip_src.addrs_i, 0, 4);
	memset(m_sHeader.ip_dst.addrs_i, 0, 4);
	memset(m_sHeader.ip_data, 0, IP_DATA_SIZE);
}

void CIPLayer::SetSrcIPAddress(u_char* src_ip)
{
	memcpy( m_sHeader.ip_src.addrs_i, src_ip, 4);
}

void CIPLayer::SetDstIPAddress(u_char* dst_ip)
{
	memcpy( m_sHeader.ip_dst.addrs_i, dst_ip, 4);
}

void CIPLayer::SetFragOff(u_short fragoff)
{
	m_sHeader.ip_fragoff = fragoff;
}

BOOL CIPLayer::Send(u_char* ppayload, int nlength)
{
	memcpy( m_sHeader.ip_data, ppayload, nlength ) ;
	m_sHeader.ip_len = ntohs(nlength+IP_HEADER_SIZE);
	
	BOOL bSuccess = FALSE ;
	bSuccess = mp_UnderLayer->Send((u_char*)&m_sHeader,IP_HEADER_SIZE + nlength);

	return bSuccess;
}

BOOL CIPLayer::Receive(u_char* ppayload)
{
	PIPLayer_HEADER pFrame = (PIPLayer_HEADER) ppayload ;
	
	BOOL bSuccess = FALSE ;

#if 1
	if(memcmp((char *)pFrame->ip_dst.addrs_i,(char *)m_sHeader.ip_src.addrs_i,4) == 0 &&
		memcmp((char *)pFrame->ip_src.addrs_i,(char *)m_sHeader.ip_src.addrs_i,4) != 0 )
	{
#endif
		// * IP의 프로토콜 타입 필터
		// 1. 프로토콜 타입이 SCTP인 패킷만 걸러낸다.
		// 2. 걸러진 SCTP타입 패킷을 decapsulation하여, 상위 레이어로 data를 올린다.
		if(pFrame->ip_proto == IP_PROTO_SCTP)
			bSuccess = mp_aUpperLayer[0]->Receive((u_char*)pFrame->ip_data);
#if 1
	}
#endif
	return bSuccess ;
}

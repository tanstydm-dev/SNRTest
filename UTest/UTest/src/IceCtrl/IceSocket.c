// IceSocket.cpp: implementation of the CIceSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "IceErrCtrl.h"
#include "IceSocket.h"

#pragma comment(lib,"ws2_32.lib")

/** 初始化SOCK库
@return 是否正常执行
*/
BOOL IceSock_InitSockEnv(void){
	WSADATA wsaData;
	if ( WSAStartup(0x0202, &wsaData) != 0)
	{
		//LOG_WINSOCK_ERR();
		//LOGERR0("WSAStartup");
		return FALSE;
	}
	return TRUE;
}


/** 创建地址
@param pSockAddr OUT
@param ip IP地址，可为空或NULL表示任意地址
@param port 端口号，可为0表示任意端口
*/
void IceSock_InitAddr(SOCKADDR_IN *pSockAddr, const char *ip, unsigned short port)
{
	VERIFY_LOG_RET(pSockAddr!=NULL);

	(*pSockAddr).sin_addr.s_addr=htonl(INADDR_ANY);
	if(ip!=0){
		if(strlen(ip)>0){
			(*pSockAddr).sin_addr.s_addr = inet_addr(ip);
		}
	}
	
	(*pSockAddr).sin_family = AF_INET;

	if(port==0){
		(*pSockAddr).sin_port = 0;
	}
	else{
		(*pSockAddr).sin_port = htons((unsigned short)port);
	}
	return;
}


/** 创建Socket
@param interfaceIp 本地绑定IP，可为NULL表示任意
@param localPort 本地端口，可为0表示任意，可用于发送
@return 是否正常执行
*/
BOOL IceSock_CreateSocketUdp(SOCKET *pSocket, const char *interfaceIp, unsigned short localPort){
	SOCKADDR_IN sockAddr={0};

	//create socket 
	(*pSocket)=socket(AF_INET, SOCK_DGRAM, 0);

	if( (*pSocket)==INVALID_SOCKET ){
		//LOG_WINSOCK_ERR();
		return FALSE;
	}
	
	//init address
	IceSock_InitAddr(&sockAddr, interfaceIp, localPort);

	//bind
//	VERIFY_ERR_SOCKETRET(bind((*pSocket),(SOCKADDR *)&sockAddr,sizeof(sockAddr)),FALSE);
	bind((*pSocket),(SOCKADDR *)&sockAddr,sizeof(sockAddr));

	return TRUE;
}

/** 加入多播并设置TTL为8
@todo 没有做加入特定网卡的多播?imr_interface
@param socket 待加入多播的SOCKET
@param multiIp 多播地址
@return 是否正常执行
*/
BOOL IceSock_AddMulti(SOCKET socket, const char *multiIp){
	DWORD optval = 8;
	struct ip_mreq mreq;/* Join the multicast group */

	// set ttl
// 	VERIFY_ERR_SOCKETRET(setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL,
// 		(char*)&optval, sizeof(int)), FALSE);
	setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL,
		(char*)&optval, sizeof(int));

	//add multi
	mreq.imr_multiaddr.s_addr = inet_addr((LPCTSTR)multiIp);	/* group addr */ 
	mreq.imr_interface.s_addr = INADDR_ANY;		/* use default */ 
//	VERIFY_ERR_SOCKETRET(setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char FAR*) & mreq, sizeof(mreq)), FALSE);
	setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char FAR*) & mreq, sizeof(mreq));
	return TRUE;
}

/** 设置接收缓冲
@param socket 待设置的SOCKET
@param bufferSize 缓冲区字节大小
@return 是否正常执行
*/
BOOL IceSock_RecBuff(SOCKET socket, int bufferSize){
	//VERIFY_ERR_SOCKETRET(setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char FAR*) & bufferSize, sizeof(int)), FALSE);
	setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char FAR*) & bufferSize, sizeof(int));
	return TRUE;
}

/** UDP发送
@param socket 
@param pSockAddr 已经填写好的发送地址
@param  pData 待发送数据
@param dataLen 待发送数据字节长度
@return 
*/
BOOL IceSock_UDPSend(SOCKET socket, const SOCKADDR_IN *pSockAddr, const char *pData, int dataLen){
// 	VERIFY_LOG_RETVAL(pSockAddr!=NULL, FALSE);
// 	VERIFY_LOG_RETVAL(pData!=NULL, FALSE);
// 	VERIFY_LOG_RETVAL(dataLen>0, FALSE);
// 	VERIFY_LOG_RETVAL((*pSockAddr).sin_port!=0, FALSE);

// 	VERIFY_ERR_SOCKETRET(sendto(socket,(const char *)pData,dataLen,0,
// 			(const SOCKADDR*)pSockAddr,sizeof(SOCKADDR)),FALSE);	
	
	sendto(socket, (const char*)pData, dataLen, 0,
		(const SOCKADDR*)pSockAddr, sizeof(SOCKADDR));	
	return TRUE;
}
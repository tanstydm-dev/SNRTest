// IceSocket.h: interface for the CIceSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ICESOCKET_H__20070801_XW)
#define ICESOCKET_H__20070801_XW

#include <WINSOCK2.H>
#include <WS2TCPIP.H>

#ifdef __cplusplus
extern "C"{
#endif

/** 初始化SOCK库
@return 是否正常执行
*/
BOOL IceSock_InitSockEnv(void);

/** 创建地址
@param pSockAddr OUT
@param ip IP地址，可为空或NULL表示任意地址
@param port 端口号，可为0表示任意端口
*/
void IceSock_InitAddr(SOCKADDR_IN *pSockAddr, const char *ip, unsigned short port);

/** 创建Socket
@param interfaceIp 本地绑定IP，可为NULL表示任意
@param localPort 本地端口，可为0表示任意，可用于发送
@return 是否正常执行
*/
BOOL IceSock_CreateSocketUdp(SOCKET *pSocket, const char *interfaceIp, unsigned short localPort);

/** 加入多播并设置TTL为8
@todo 没有做加入特定网卡的多播?imr_interface
@param socket 待加入多播的SOCKET
@param multiIp 多播地址
@return 是否正常执行
*/
BOOL IceSock_AddMulti(SOCKET socket, const char *multiIp);

/** 设置接收缓冲
@param socket 待设置的SOCKET
@param bufferSize 缓冲区字节大小
@return 是否正常执行
*/
BOOL IceSock_RecBuff(SOCKET socket, int bufferSize);

/** UDP发送
@param socket 
@param pSockAddr 已经填写好的发送地址
@param  pData 待发送数据
@param dataLen 待发送数据字节长度
@return 
*/
BOOL IceSock_UDPSend(SOCKET socket, const SOCKADDR_IN *pSockAddr, const char *pData, int dataLen);



#ifdef __cplusplus
}
#endif


#endif // !defined(ICESOCKET_H__20070801_XW)

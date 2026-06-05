/**
@file		IceErrCtrl.c 调试信息输出
@author		
@date 		2007/8/1   18:20
@version	v1.0 Copyright (c) 2007 - All Rights Reserved
@warning 	
*********************************************************************/
#include "IceErrCtrl.h"
#include "icetype.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


void IceExternLogWin32(int logID, const char* pStr);

BOOL IceCheckStrLen(const S8_CHAR *str,S32 nMaxStrLen)
{
	S32 nLen = 0;
	while(*str++ != 0)
	{
		nLen++;
		if (nLen > nMaxStrLen-1)
		{
			return FALSE;
		}
	}
	return TRUE;
}

#define MAX_TRACE_STR_LEN 2048
S8_CHAR szBuffer_IceLog[MAX_TRACE_STR_LEN]={0};


void IceLog(S32 logID, const S8_CHAR *lpszFormat, ...)
{
	va_list args;

	if(!IceCheckStrLen(lpszFormat, MAX_TRACE_STR_LEN))
	{
		return;
	}

	/*lint -save -e970 -e912 */
	va_start(args, lpszFormat);
	
	/*lint -restore */
#ifdef _WIN32
	_vsnprintf(szBuffer_IceLog, MAX_TRACE_STR_LEN, lpszFormat, args);
#else
	if(strlen(lpszFormat)>(MAX_TRACE_STR_LEN/2)){
		strcat(szBuffer_IceLog, "Error@IceLog: Too Long"); 
	}
	else{
		//VSPRINTF_EX(szBuffer, lpszFormat, args);
		vsnprintf(szBuffer_IceLog, MAX_TRACE_STR_LEN, lpszFormat, args);
	}
	
#endif

#if (defined _WIN32)
	IceExternLogWin32(logID, szBuffer_IceLog);		/**< WIN32专用测试 */
#else
	IceExternLog(logID, szBuffer_IceLog);
#endif /* defined WIN32 && !defined UNDER_CE */

	va_end(args);
}


#define MAX_TRACE_LOG_LEN	2048
S8_CHAR szBuffer[MAX_TRACE_LOG_LEN/2]={0};
S8_CHAR szBufferHead[MAX_TRACE_LOG_LEN]={0};

void IceLogTrace(S32 logID, const S8_CHAR *fileName, S32 nLine, const S8_CHAR *lpszFormat, ...)
{
#if 1
	va_list args;

	if(!IceCheckStrLen(lpszFormat, MAX_TRACE_LOG_LEN))
	{
		return;
	}
	/*lint -save -e970 -e912 */
	va_start(args, lpszFormat);
	/*lint -restore */

	sprintf(szBufferHead, "%s@%d:", fileName, nLine);
	if(strlen(szBufferHead)>(MAX_TRACE_LOG_LEN/2)){
		strcat(szBuffer, "err@szBufferHead"); 
	}
	#ifdef WIN32
		if(strlen(lpszFormat)>(MAX_TRACE_LOG_LEN/2)){
			strcat(szBuffer, "Error@IceLog: Too Long"); 
		}
		else{
			//VSPRINTF_EX(szBuffer, lpszFormat, args);
			_vsnprintf(szBuffer, MAX_TRACE_LOG_LEN, lpszFormat, args);
		}
	#else
		if(strlen(lpszFormat)>(MAX_TRACE_LOG_LEN/2)){
			strcat(szBuffer, "Error@IceLog: Too Long"); 
		}
		else{
			//VSPRINTF_EX(szBuffer, lpszFormat, args);
			vsnprintf(szBuffer, MAX_TRACE_LOG_LEN, lpszFormat, args);
		}
	#endif

	strcat(szBufferHead, szBuffer);

	#if (defined _WIN32)
		IceExternLogWin32(logID, szBufferHead);
	#else
		IceExternLog(logID, szBufferHead);
	#endif /* defined WIN32 && !defined UNDER_CE */

	va_end(args);	
#endif
}

void IceLogHex(S32 logID, const U8 *pData, U32 nByteLen)
{
	U32 i;
	S8_CHAR pDstChar[(S32)MAX_TRACE_STR_LEN*3] = {0};
	if ( (nByteLen > MAX_TRACE_STR_LEN	) ||
		 (pData == NULL					)
		)
	{
		LOGINFO0("Err IceLogHex Input");
		return;
	}

	for ( i=0; i<nByteLen; i++ )
	{
		sprintf(pDstChar+((S32)i*3),"%02X ", pData[i]);
	}
	
	IceLog(logID, pDstChar);

}

#define MAX_LOG_BYTE_LEN 2048	/**< 打印原始数据长度 */
S8_CHAR pDstChar[(MAX_LOG_BYTE_LEN*3)] = {0};

void IceLogHexStr(const S8_CHAR *lpszFormat, const U8 *pData, U32 nByteLen)
{
	U32 i;
	U32 inputStrLen = strlen(lpszFormat);

	if ( (nByteLen > MAX_LOG_BYTE_LEN	) ||
		 (pData == NULL					)	
		)
	{
		LOGINFO0("Err IceLogHex Input");
		return;
	}

	memcpy( pDstChar, lpszFormat, inputStrLen );
	sprintf(pDstChar + inputStrLen, " [%d]: ", nByteLen);

	inputStrLen = strlen(pDstChar);
	for ( i=0; i<nByteLen; i++ )
	{
		sprintf((pDstChar+((S32)i*3))+inputStrLen, "%02X ", pData[i]);
	}

	IceLog(1, pDstChar);

}

/*lint -restore */

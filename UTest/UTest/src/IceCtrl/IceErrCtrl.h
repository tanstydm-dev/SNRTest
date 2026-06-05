/**
@file		IceErrCtrl.h 错误调试信息输出
@author		
@date 		2007/8/1   16:19
@version	v1.0 Copyright (c) 2007 - All Rights Reserved
@warning 	适用于vxworks、wince、windows操作系统.特别注意，调试输出信息每次不可大于MAX_TRACE_STR_LEN字节，多者截断
默认情况下，输入的调试信息自动在最后加换行
根据所在的操作系统需配合下列中一个文件使用：
IceErrCtrlVxwork.c
IceErrCtrlWin32.cpp
IceErrCtrlCE.cpp
*********************************************************************/
/*lint -save -e971 (Note -- Use of 'char' without 'signed' or 'unsigned')*/
/*lint -save -e960 : (Note -- Violates MISRA Required Rule 69, function has variable number of arguments) */
#ifndef _ICEERRCTRL_H_WP_2010_04_28_
#define _ICEERRCTRL_H_WP_2010_04_28_

#define EN_DEBUG_INFO	1

#ifdef __cplusplus
extern "C"{
#endif

#include "icetype.h"

enum ELogId{
	eLogErr,
	eLogTrace,
	eLogInfo,
	eLogDump,
	eLogWarning0,
	eLogWarning1,
	eLogWarning2,
	eLogWarning3,
	eLogWarning4,
	eLogWarning5,
	eLogWarning6
};

#if (defined _USRDLL) || (EN_DEBUG_INFO == 0)
#define LOG			LOGD//{} 	/* ExSerialWriteLog_NO_USE */
#define LOGD		{}

#define LOGERR0(a) {}
#define LOGERR1(a,b) {}
#define LOGERR2(a,b,c) {}
#define LOGERR3(a,b,c,d) {}
#define LOGERR4(a,b,c,d,e) {}
#define LOGERR5(a,b,c,d,e,f) {}

#define LOGTRACE0(a) {}
#define LOGTRACE1(a,b) {}
#define LOGTRACE2(a,b,c) {}

#define LOGINFO0(a) {}
#define LOGINFO1(a,b) {}
#define LOGINFO2(a,b,c) {}
#define LOGINFO3(a,b,c,d) {}

/*#define LOGBYTE01(nHeadInfo, pStr, nByteLen) LOGINFO1(nHeadInfo, nByteLen);LOGU8(pStr, nByteLen)*/
#define LOGBYTE01(nHeadInfo, pStr, nByteLen) 

#else /*<_USRDLL*/

#define LOG			IceLog 	/* ExSerialWriteLog_NO_USE */
#define LOGD		IceLog

void IceLog(S32 logID, const S8_CHAR *Format, ...);
void IceLogTrace(S32 logID, const S8_CHAR *fileName, S32 nLine, const S8_CHAR *Format, ...);
void IceLogHex(S32 logID, const U8 *pData, U32 nByteLen);
void IceLogHexStr(const S8_CHAR *lpszFormat, const U8 *pData, U32 nByteLen);

#define LOGERR0(a) IceLogTrace(eLogErr,__FILE__,__LINE__,(a))
#define LOGERR1(a,b) IceLogTrace(eLogErr,__FILE__,__LINE__,(a),(b))
#define LOGERR2(a,b,c) IceLogTrace(eLogErr,__FILE__,__LINE__,(a),(b),(c))
#define LOGERR3(a,b,c,d) IceLogTrace(eLogErr,__FILE__,__LINE__,(a),(b),(c),(d))
#define LOGERR4(a,b,c,d,e) IceLogTrace(eLogErr,__FILE__,__LINE__,(a),(b),(c),(d), (e))
#define LOGERR5(a,b,c,d,e,f) IceLogTrace(eLogErr,__FILE__,__LINE__,(a),(b),(c),(d),(e),(f))

#define LOGTRACE0(a) IceLog(eLogTrace,(a))
#define LOGTRACE1(a,b) IceLog(eLogTrace,(a),(b))
#define LOGTRACE2(a,b,c) IceLog(eLogTrace,(a),(b),(c))

#define LOGINFO0(a) IceLog(eLogInfo,(a))       //("a = 8")
#define LOGINFO1(a,b) IceLog(eLogInfo,(a),(b)) //("a = %d", b)
#define LOGINFO2(a,b,c) IceLog(eLogInfo,(a),(b),(c))
#define LOGINFO3(a,b,c,d) IceLog(eLogInfo,(a),(b),(c),(d))

#define LOGBYTE01(nHeadInfo, pStr, nByteLen) IceLogHexStr(nHeadInfo, pStr, nByteLen);

#endif /*<_USRDLL*/

#define VERIFY_LOG(a) if(!(a)){LOGERR0(#a);}
#define VERIFY_LOG_RET(a) if(!(a)){LOGERR0(#a);return;}
#define VERIFY_LOG_RETVAL(a,retVal) if(!(a)){LOGERR0(#a); return retVal;}

#define VERIFY_RET(a) VERIFY_LOG_RETVAL((a), FALSE);
#define VERIFY_S32(a) VERIFY_LOG_RETVAL((a)==ERR_NO_ERROR, FALSE);



#ifdef __cplusplus
}
#endif


#endif /* _ICEERRCTRL_H_WP_2010_04_28_ */

/*lint -restore */

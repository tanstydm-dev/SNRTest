/**
@file		AscFuncs.h 计算机语言处理ASCII码类型String的操作函数 
@author		
@date		2010/04/05 09:39
@version	v1.0 Copyright(c) 1995-2010 - All Rights Reserved
@warning 	

*************************************************************************/
#ifndef _ASCFUNCS_H_2010_04_05_WP_ENJOY_
#define _ASCFUNCS_H_2010_04_05_WP_ENJOY_

#ifdef UNINAV_LIB_ENABLE
#define UNINAV_LIB	__declspec(dllexport)
#else 
#define UNINAV_LIB
#endif

#include "icetype.h"
#include "IceErrCtrl.h"

#ifdef __cplusplus
extern "C"{
#endif


/************************************************************************/
/**********以下为解析处理************************************************/
/************************************************************************/

typedef enum
{
	eS32 = 0x00,
	eU32 = 0x01,
	eF64 = 0x02,
	eCHR = 0x03,
	eSTR = 0x04,
	eHEX = 0x05,
	eORG = 0x06,

	eS32_COMMA = 0x10,
	eU32_COMMA = 0x11,
	eF64_COMMA = 0x12,
	eCHR_COMMA = 0x13,
	eSTR_COMMA = 0x14,
	eHEX_COMMA = 0x15,
	eORG_COMMA = 0x16

}EnumIceType;

/** 串口解析数据初始化
@param pData  
@return 
*************************************************************/ 
UNINAV_LIB void Asc_GetInit( const S8_CHAR *pData );

/** 获取解析处理的工作状态
@return TRUE: 工作正常; FALSE: 工作发生异常
*************************************************************/ 
UNINAV_LIB BOOL  Asc_GetStatus(void);

/** 从ASC码中获得U32类型数据，并将其按照一定bit位转成U32或S32
@param pVar  
@param eType  
@param nBitUse  
@return 注意不可超过32bit, 输入数据长度必须和nBitUse一致
*************************************************************/ 
UNINAV_LIB void Asc_GetHexType( void *pVar, EnumIceType eType, U32 nBitUse);

UNINAV_LIB void Asc_GetIceTypeS32( S32 *pVar, const EnumIceType eType );
UNINAV_LIB void Asc_GetIceTypeU32( U32 *pVar, const EnumIceType eType );
UNINAV_LIB void Asc_GetIceTypeF64( F64 *pVar, const EnumIceType eType );
UNINAV_LIB void Asc_GetIceTypeCHR( S8_CHAR *pVar, const EnumIceType eType );
UNINAV_LIB void Asc_GetIceTypeSTR( S8_CHAR *pVar, const EnumIceType eType );

#define ASC_GET_S32(a)			Asc_GetIceTypeS32(a, eS32);
#define ASC_GET_U32(a)			Asc_GetIceTypeU32(a, eU32);
#define ASC_GET_F64(a)			Asc_GetIceTypeF64(a, eF64);
#define ASC_GET_CHR(a)			Asc_GetIceTypeCHR(a, eCHR);
#define ASC_GET_STR(a)			Asc_GetIceTypeSTR(a, eSTR);
#define ASC_GET_HEX_U32(a)		Asc_GetHexType(a, eU32, 0);
#define ASC_GET_HEX_S32(a,nBit)	Asc_GetHexType(a, eS32, nBit);
#define ASC_GET_START(a)		Asc_GetInit((S8_CHAR *)a);
#define ASC_GET_END				Asc_GetStatus();
/*VERIFY_LOG_RETVAL( Asc_GetStatus(), ERR_NORAML_ERROR );*/

/************************************************************************/
/**********以下为组帧处理************************************************/
/************************************************************************/

/** 组帧初始化函数,在每个函数开始时调用
@param bPointExist  
@return 
*************************************************************/ 
UNINAV_LIB void Asc_SetInit(const BOOL bPointExist);

/** 计算CRC并将组好的数据帧memcpy至pData中
@param pData  
@return 
*************************************************************/ 
UNINAV_LIB U32 Asc_SetCrcTail(S8 *pData);

/** 根据输入频点参数，组帧为S8_CHAR数据
@param nFreqId  
@return 
*************************************************************/ 
UNINAV_LIB void Asc_SetFreqTpye(const U32 nFreqId);

/** 根据输入支路参数，组帧为S8_CHAR数据
@param nBrchId  
@return 
*************************************************************/ 
UNINAV_LIB void Asc_SetBranchTpye(const U32 nBrchId);

/** 计算数据的CRC(按BD2协议,为$和*之间数据异或)
@param pData  
@param nByteLen  
@return 
*************************************************************/ 
UNINAV_LIB U8 Asc_GetCRC(const U8 *pData, U32 nByteLen);

/** 把BYTE数组转成十六进制表示的字符串
@param pSrc  
@param pDst  
@param nSrcLength  
@return 
*************************************************************/ 
UNINAV_LIB S32 Asc_U8toChar(const U8* pSrc, S8_CHAR* pDst, S32 nSrcLength);
UNINAV_LIB S32 Asc_U8toChar_Space(const U8* pSrc, S8_CHAR* pDst, S32 nSrcLength);

/** 把十六进制输入字符串,转成BYTE数组
@param pSrc  
@param pDst  
@param nSrcLength  
@return 
*************************************************************/ 
UNINAV_LIB S32 Asc_ChartoU8(const S8_CHAR* pSrc, U8* pDst, S32 nSrcLength);


UNINAV_LIB void Asc_SetIceTpyeS32(const S32 *pVar, const EnumIceType eType, const S8_CHAR *pProcType );
UNINAV_LIB void Asc_SetIceTpyeU32(const U32 *pVar, const EnumIceType eType, const S8_CHAR *pProcType );
UNINAV_LIB void Asc_SetIceTpyeF64(const F64 *pVar, const EnumIceType eType, const S8_CHAR *pProcType );
UNINAV_LIB void Asc_SetIceTpyeSTR(const S8_CHAR *pVar, const EnumIceType eType, const S8_CHAR *pProcType );
UNINAV_LIB void Asc_SetIceTpyeCHR(const S8_CHAR *pVar, const EnumIceType eType, const S8_CHAR *pProcType );
UNINAV_LIB void Asc_SetIceTpyeHEX(const U32 *pVar, const EnumIceType eType, const S8_CHAR *pProcType );
UNINAV_LIB void Asc_SetIceTpyeORG(const F64 *pVar, const EnumIceType eType, const S8_CHAR *pProcType );

#define ASC_SET_START(a)			Asc_SetInit((a));
#define ASC_SET_END(a)				Asc_SetCrcTail(a);			
#define ASC_SET_FERQ(a)				Asc_SetFreqTpye(a);
#define ASC_SET_BRANCH(a)			Asc_SetBranchTpye(a);

#define ASC_SET_S32(a)				Asc_SetIceTpyeS32(&(a), eS32, "%d");
#define ASC_SET_S32_2BYTES(a)		Asc_SetIceTpyeS32(&(a), eS32, "%02d");
#define ASC_SET_S32_4BYTES(a)		Asc_SetIceTpyeS32(&(a), eS32, "%04d");
#define ASC_SET_S32_COMMA(a)		Asc_SetIceTpyeS32(&(a), eS32_COMMA, "%d");
#define ASC_SET_S32_2BYTES_COMMA(a)	Asc_SetIceTpyeS32(&(a), eS32_COMMA, "%02d");
#define ASC_SET_S32_3BYTES_COMMA(a)	Asc_SetIceTpyeS32(&(a), eS32_COMMA, "%03d");
#define ASC_SET_S32_4BYTES_COMMA(a)	Asc_SetIceTpyeS32(&(a), eS32_COMMA, "%04d");

#define ASC_SET_U32(a)				Asc_SetIceTpyeU32(&(a), eS32, "%d");
#define ASC_SET_U32_2BYTES(a)		Asc_SetIceTpyeU32(&(a), eS32, "%02d");
#define ASC_SET_U32_3BYTES(a)		Asc_SetIceTpyeU32(&(a), eS32, "%03d");		   /**< YLee add */
#define ASC_SET_U32_COMMA(a)		Asc_SetIceTpyeU32(&(a), eS32_COMMA, "%d");
#define ASC_SET_U32_2BYTES_COMMA(a)	Asc_SetIceTpyeU32(&(a), eS32_COMMA, "%02d");
#define ASC_SET_U32_3BYTES_COMMA(a)	Asc_SetIceTpyeU32(&(a), eS32_COMMA, "%03d");
#define ASC_SET_U32_4BYTES_COMMA(a)	Asc_SetIceTpyeU32(&(a), eS32_COMMA, "%04d");   /**< YLee add */

#define ASC_SET_F64(a)				Asc_SetIceTpyeF64(&(a), eF64, "%d+%.3lf+");
#define ASC_SET_F64_7DIGS(a)		Asc_SetIceTpyeF64(&(a), eF64, "%d+%.7lf+");
#define ASC_SET_F64_COMMA(a)		Asc_SetIceTpyeF64(&(a), eF64_COMMA, "%d+%.3lf+");
#define ASC_SET_F64_2BYTES_COMMA(a)	Asc_SetIceTpyeF64(&(a), eF64_COMMA, "%02d+%lf+");
#define ASC_SET_F64_3DIGS_COMMA(a)	Asc_SetIceTpyeF64(&(a), eF64_COMMA, "%d+%.3lf+");
#define ASC_SET_F64_5DIGS_COMMA(a)	Asc_SetIceTpyeF64(&(a), eF64_COMMA, "%d+%.5lf+");
#define ASC_SET_F64_7DIGS_COMMA(a)	Asc_SetIceTpyeF64(&(a), eF64_COMMA, "%d+%.7lf+");
#define ASC_SET_F64_2BYTES_8DIGS_COMMA(a)	Asc_SetIceTpyeF64(&(a), eF64_COMMA, "%02d+%.8lf+");
#define ASC_SET_F64_1DIGS_COMMA(a)	Asc_SetIceTpyeF64(&(a), eF64_COMMA, "%d+%.1lf+");
#define ASC_SET_F64_1DIGS(a)		Asc_SetIceTpyeF64(&(a), eF64, "%d+%.1lf+");


#define ASC_SET_HEX(a)				Asc_SetIceTpyeHEX(&(a), eHEX, "%X");
#define ASC_SET_HEX_2BYTES(a)		Asc_SetIceTpyeHEX(&(a), eHEX, "%02X");
#define ASC_SET_HEX_3BYTES(a)		Asc_SetIceTpyeHEX(&(a), eHEX, "%03X");
#define ASC_SET_HEX_4BYTES(a)		Asc_SetIceTpyeHEX(&(a), eHEX, "%04X");
#define ASC_SET_HEX_5BYTES(a)		Asc_SetIceTpyeHEX(&(a), eHEX, "%05X");
#define ASC_SET_HEX_8BYTES(a)		Asc_SetIceTpyeHEX(&(a), eHEX, "%08X");
#define ASC_SET_HEX_COMMA(a)				Asc_SetIceTpyeHEX(&(a), eHEX_COMMA, "%X");
#define ASC_SET_HEX_2BYTES_COMMA(a)		Asc_SetIceTpyeHEX(&(a), eHEX_COMMA, "%02X");
#define ASC_SET_HEX_3BYTES_COMMA(a)		Asc_SetIceTpyeHEX(&(a), eHEX_COMMA, "%03X");
#define ASC_SET_HEX_4BYTES_COMMA(a)	Asc_SetIceTpyeHEX(&(a), eHEX_COMMA, "%04X");
#define ASC_SET_HEX_5BYTES_COMMA(a)		Asc_SetIceTpyeHEX(&(a), eHEX_COMMA, "%05X");
#define ASC_SET_HEX_6BYTES_COMMA(a)		Asc_SetIceTpyeHEX(&(a), eHEX_COMMA, "%06X");		/**< YLee add */
#define ASC_SET_HEX_8BYTES_COMMA(a)		Asc_SetIceTpyeHEX(&(a), eHEX_COMMA, "%08X");		/**< YLee add */

#define ASC_SET_ORG_COMMA(a)		Asc_SetIceTpyeORG(&(a), eORG_COMMA, "%lf");
#define ASC_SET_ORG_3DIGS_COMMA(a)		Asc_SetIceTpyeORG(&(a), eORG_COMMA, "%.3lf");
#define ASC_SET_ORG_3DIGS(a)		Asc_SetIceTpyeORG(&(a), eORG, "%.3lf");

#define ASC_SET_STR(a)				Asc_SetIceTpyeSTR( a, eSTR, "");
#define ASC_SET_STR_COMMA(a)		Asc_SetIceTpyeSTR( a, eSTR_COMMA, "");

#define ASC_SET_CHR(a)				Asc_SetIceTpyeCHR( &(a), eCHR, "");
#define ASC_SET_CHR_COMMA(a)		Asc_SetIceTpyeCHR( &(a), eCHR_COMMA, "");

#define ASC_SET_COMMA				ASC_SET_STR(",");

#define ASC_SET_BANK_ORG_3DIGS_COMMA(a) \
{\
	if(a == 0.0)\
{\
	ASC_SET_COMMA;\
}\
else\
{\
	ASC_SET_ORG_3DIGS_COMMA(a);\
}\
}

#define ASC_SET_BANK_ORG_3DIGS(a)	\
{\
	if(a != 0.0)\
{\
	ASC_SET_ORG_3DIGS_COMMA(a);\
}\
}

#define ASC_SET_TIME_COMMA(pSTime) \
	ASC_SET_U32_2BYTES(pSTime->nHour);\
	ASC_SET_U32_2BYTES(pSTime->nMin);\
	ASC_SET_F64_2BYTES_COMMA(pSTime->nSec);

#define ASC_SET_DATE_COMMA(pSDate) \
	ASC_SET_U32_2BYTES(pSDate->nYear);\
	ASC_SET_U32_2BYTES(pSDate->nMonth);\
	ASC_SET_U32_2BYTES_COMMA(pSDate->nDay);


#define ASC_SET_TIME_COMMA_SU30(pSTime) \
	ASC_SET_U32_2BYTES(pSTime->nHour);\
	ASC_SET_U32_2BYTES(pSTime->nMin);\
	ASC_SET_F64_2BYTES_8DIGS_COMMA(pSTime->nSec);

#define ASC_SET_LAT_LON(pSPoint) \
	ASC_SET_U32_2BYTES((pSPoint->nLatDegree));\
	ASC_SET_F64_2BYTES_COMMA(pSPoint->nLatMin);\
	ASC_SET_CHR_COMMA(pSPoint->sLatDir);\
	ASC_SET_U32_3BYTES(pSPoint->nLonDegree);\
	ASC_SET_F64_2BYTES_COMMA(pSPoint->nLonMin);\
	ASC_SET_CHR_COMMA(pSPoint->sLonDir);

#define ASC_SET_MAG_LON(pSMag) \
	ASC_SET_F64_2BYTES_COMMA((pSMag->nDegree));\
	ASC_SET_CHR_COMMA(pSMag->Vardir);

UNINAV_LIB F64 Asc_GetFree(BOOL bBefore, U32 nPointNum, F64 nOrig);

#if ( defined __ARM || defined _VXWORKS || defined _KEIL)
	S32 *_itoa( U8 value, S8 *string, S32 radix );
#endif /* defined __ARM || defined _VXWORKS */

/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/
#ifdef ICE_AUTO_TEST_ENABLE
	
void Test_AscFuncs();

#endif /* #ifdef ICE_AUTO_TEST_ENABLE */
/************************************************************************/
/**********以上为测试代码************************************************/
/************************************************************************/


#ifdef __cplusplus
}
#endif


#endif /* _ASCFUNCS_H_2010_04_05_WP_ENJOY_ */

/** @base1
@file		AscFuncs.c 计算机语言处理ASCII码类型String的操作函数 
@author		
@date		2010/04/05 09:40
@version	v1.0 Copyright(c) 1995-2010 - All Rights Reserved
@warning 	

*************************************************************************/
/*lint -save -e829 : (Info -- A +headerwarn option was previously issued for header 'stdio.h')  */
/*lint -save -e605 : (Warning -- Increase in pointer capability (arg. no. 1)) */
/*lint -save -e961 : (Note -- Violates MISRA Advisory Rule 44, redundant explicit casting) */
/*lint -save -e912 : (Note -- Implicit binary conversion from int to unsigned int) */
/*lint -save -e421 : (Warning -- Caution -- function 'atoi(const char *)' is considered dangerous */
/*lint -save -e668 : (Warning -- Possibly passing a null pointer to function 'sprintf(char *, const char *, ...) */
/*lint -save -e946 : (Note -- Relational or subtract operator applied to pointers)*/
/*lint -save -e960 : (Note -- Violates MISRA Required Rule 57, continue statement detected)*/
/*lint -save -e956 : (Note -- Non const, non volatile static or external variable */

#include "AscFuncs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define	ASC_GET_MAX_STRING_LEN	256				/**< 解析-允许最大处理String字节长度 */
#define	ASC_GET_BASE_HEX		16				/**< 解析-16进制转换 */
const S8_CHAR		m_sGetSeps[4] = "$,*";					/**< 解析-strtok 处理时的停止标记, 注意4个标记为'$',',','*','\0' */
static S8_CHAR	m_sGetFrmTmp[ASC_GET_MAX_STRING_LEN*2];	/**< 解析-处理字符串的存储拷贝 */
static BOOL		m_bGetRight = FALSE;					/**< 解析-处理正常标记, 若出现错误暂停后续处理, 初始化Asc_GetInit需要置TRUE */

#define			ASC_SET_MAX_PDATA_LEN	1024			/**< 组帧-对外输出ASC码的字节数,考虑PNnnc内部协议,这里较大 */
static BOOL	m_bSetRight = FALSE;					/**< 组帧-处理正常标记, 若出现错误暂停后续处理, 初始化Asc_SetInit需要置TRUE */
static U32	m_bSetBytePos = 0;						/**< 组帧-组帧字节数 */	
static S8		m_sSetFrm[ASC_SET_MAX_PDATA_LEN];

typedef enum
{
	eFrqB1I = 0,		/**<BD-B1*/
		eFrqG1,			/**<GLONASS-L1*/
		eFrqB3I,			/**<BD-B3*/	
		eFrqL1,			/**<GPS-L1*/	/* BP测试中L1定义为3 */
		eFrqAll			/**<All*/
}EFrqId;

/** 从ASC码中获得指定EnumIceType类型的数据
@param pVar  
@param eType  
@return 
*************************************************************/ 
static void Asc_GetIceType( void *pVar, const EnumIceType eType);

void Asc_GetIceTypeS32( S32 *pVar, const EnumIceType eType )
{
	Asc_GetIceType( (void*)pVar, eType);
}

void Asc_GetIceTypeU32( U32 *pVar, const EnumIceType eType )
{
	Asc_GetIceType( (void*)pVar, eType);
}

void Asc_GetIceTypeF64( F64 *pVar, const EnumIceType eType )
{
	Asc_GetIceType( (void*)pVar, eType);
}

void Asc_GetIceTypeCHR( S8_CHAR *pVar, const EnumIceType eType )
{
	Asc_GetIceType( (void*)pVar, eType);
}

void Asc_GetIceTypeSTR( S8_CHAR *pVar, const EnumIceType eType )
{
	Asc_GetIceType( (void*)pVar, eType);
}

/** 组帧格式转换函数	
@param pVar			待转换数据的指针
@param eType		待转换数据的类型
@param pProcType	仅用于浮点转换时的参数输入
@return 
*************************************************************/ 
static void Asc_SetIceTpye(const void *pVar, const EnumIceType eType, const S8_CHAR *pProcType );

void Asc_SetIceTpyeS32(const S32 *pVar, const EnumIceType eType, const S8_CHAR *pProcType )
{
	Asc_SetIceTpye( (void*)pVar, eType, pProcType );
}

void Asc_SetIceTpyeU32(const U32 *pVar, const EnumIceType eType, const S8_CHAR *pProcType )
{
	Asc_SetIceTpye( (void*)pVar, eType, pProcType );
}

void Asc_SetIceTpyeF64(const F64 *pVar, const EnumIceType eType, const S8_CHAR *pProcType )
{
	Asc_SetIceTpye( (void*)pVar, eType, pProcType );
}

void Asc_SetIceTpyeCHR(const S8_CHAR *pVar, const EnumIceType eType, const S8_CHAR *pProcType )
{
	Asc_SetIceTpye( (void*)pVar, eType, pProcType );
}

void Asc_SetIceTpyeSTR(const S8_CHAR *pVar, const EnumIceType eType, const S8_CHAR *pProcType )
{
	Asc_SetIceTpye( (void*)pVar, eType, pProcType );
}

void Asc_SetIceTpyeHEX(const U32 *pVar, const EnumIceType eType, const S8_CHAR *pProcType )
{
	Asc_SetIceTpye( (void*)pVar, eType, pProcType );
}

void Asc_SetIceTpyeORG(const F64 *pVar, const EnumIceType eType, const S8_CHAR *pProcType )
{
	Asc_SetIceTpye( (void*)pVar, eType, pProcType );
}
/************************************************************************/
/**********以下为解析处理************************************************/
/************************************************************************/

void Asc_GetInit(const S8_CHAR *pData)
{
	U32 i = 0, index = 0;

	if (pData == NULL)
	{
		m_bGetRight = FALSE;
		return;
	}	

	memset( m_sGetFrmTmp, 0, sizeof(m_sGetFrmTmp) );
	strcpy( m_sGetFrmTmp, (const S8_CHAR *)(pData) );	
	
	/* 在连逗号之间补零 */
	for ( i=0; (i<ASC_GET_MAX_STRING_LEN); i++ )
	{
		m_sGetFrmTmp[index] = pData[i];
		index ++;
		if ( ( pData[i] == ','								) && 
			 ( (pData[i+1] == ',') || (pData[i+1] == '*')	)
			)
		{
			m_sGetFrmTmp[index] = '0';
			index ++;
		}
	}

	/* 越过"$CCROM,"之类的head */
	/*lint -save -e534 : (Warning -- Ignoring return value of function */
	strtok( m_sGetFrmTmp, m_sGetSeps); 
	/*lint -restore */

	m_bGetRight = TRUE;	
}

BOOL Asc_GetStatus(void)
{
	return m_bGetRight;
}

static void Asc_GetIceType( void *pVar, const EnumIceType eType )
{
	S8_CHAR *pToken;

	if ( ( m_bGetRight == FALSE	) ||
		 ( pVar == NULL			)
		)
	{
		/* 前端状态异常,直接返回 */
		m_bGetRight = FALSE;
		return;
	}
	
	pToken = strtok(NULL, m_sGetSeps);

	if ( pToken == NULL )
	{
		m_bGetRight = FALSE;
		return;
	}

	/* 尽量回避使用switch语句 */
	if ( eType == eS32 )
	{
		(* (S32*)(pVar) ) = (S32)atoi(pToken);
	}
	else if ( eType == eU32 )
	{
		(* (U32*)(pVar) ) = (U32)atoi(pToken);
	}
	else if ( eType == eF64 )
	{
		(* (F64*)(pVar) ) = atof(pToken);
	}
	else if ( eType == eCHR )
	{
		(* (S8_CHAR*)(pVar) ) = (*pToken);
	}
	else if ( eType == eSTR )
	{
		strcpy( (S8_CHAR *)(pVar) , pToken);
	}
	else
	{
		m_bGetRight = FALSE;
	}
	
	return;
}

void Asc_GetHexType( void *pVar, EnumIceType eType, U32 nBitUse )
{
	S8_CHAR *pTmp;	/**< 指示所转换字符串取数字的结尾位置的指针, 无意义 */
	U32 res = 0;
	S8_CHAR sTmp[ASC_GET_MAX_STRING_LEN];

	if ( ( m_bGetRight == FALSE					) ||
		 ( pVar == NULL							) ||
		 ( (eType == eS32) && (nBitUse == 0)	)
		)
	{
		/* 前端状态异常,直接返回 */
		m_bGetRight = FALSE;
		return;
	}

	/* 先获取String */
	Asc_GetIceType( sTmp, eSTR );

	if ( eType == eU32 )
	{
		/* Type 转换 */
		res = (U32)strtoul( sTmp, &pTmp, ASC_GET_BASE_HEX );
		(* (U32*)(pVar) ) = res;
	}
	else if ( eType == eS32 )
	{
		/* Type 转换 */
		res = (U32)strtol( sTmp, &pTmp, ASC_GET_BASE_HEX );
		/* 判断正负 */
		if ( res >= (U32)( 1<<(nBitUse-1) ) )
		{
			(* (S32*)(pVar) ) = (S32)( res - (U32)(1<<nBitUse) );/**< 负数 */
		}
		else
		{
			(* (S32*)(pVar) ) = (S32)( res );					/**< 正数 */
		}
	}
	else
	{
		m_bGetRight = FALSE;
	}
	
	return;
}

/************************************************************************/
/**********以下为组帧处理************************************************/
/************************************************************************/

void Asc_SetInit(const BOOL bPointExist)
{	
	if ( bPointExist != TRUE )
	{
		m_bSetRight = FALSE;
		return;
	}	
	
	memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
	m_bSetRight = TRUE;	
	m_bSetBytePos = 0;

	return;
}

static void Asc_SetIceTpye(const void *pVar, const EnumIceType eType, const S8_CHAR *pProcType )
{
	S8_CHAR sTmp[64];
	U32 nByteLen = 0;

	/*lint -save -e506 -e774 -e831 : (Warning -- Constant value Boolean) */
	if ( ( m_bSetRight  == FALSE					)	||
		 ( m_bSetBytePos > ASC_SET_MAX_PDATA_LEN	)	||
		 ( pVar == NULL								)	||
		 ( m_sSetFrm == NULL						)
		)
	{
		/* 前端状态异常,直接返回 */
		m_bSetRight = FALSE;
		return;
	} 
	/*lint -restore */

	if ( (eType & 0x0F) == eS32 )
	{
		/* 负号处理增加"-" */
		if ( (*(S32*)(pVar)) < 0 )
		{
			memcpy(m_sSetFrm+m_bSetBytePos, "-", sizeof(U8));
			m_bSetBytePos += 1;
		}

		sprintf( sTmp, pProcType, abs(*(S32*)pVar) );
	
		nByteLen = strlen(sTmp);
		memcpy(m_sSetFrm+m_bSetBytePos, sTmp, nByteLen);
		m_bSetBytePos += nByteLen;
	}
	else if ( (eType & 0x0F) == eU32 )
	{
		sprintf(sTmp, pProcType, (*(U32*)pVar) );
		
		nByteLen = strlen(sTmp);
		memcpy(m_sSetFrm+m_bSetBytePos, sTmp, nByteLen);
		m_bSetBytePos += nByteLen;
	}
	else if ( (eType & 0x0F) == eF64 )
	{	
		/* F64类型需要把 "%03d+%.04lf+" 分别处理后再合并 */
		const S8_CHAR seps[2] = "+";	/**< 停止标记 */
		S8_CHAR *pTokenInt;
		S8_CHAR *pTokenDig;
		S8_CHAR sCopyProcType[64];
		S8_CHAR sTmpInt[64];
		S8_CHAR sTmpDig[64];
		F64 aValue = (*(F64*)pVar);
		F64 fDig = 0;
		/* 分离format格式 */
		memcpy(sCopyProcType, pProcType, strlen(pProcType));
		/*lint -save -e668 -e831 : (Warning -- Possibly passing a null pointer to function 'sprintf(char *, const char *, ...)' */
		pTokenInt = strtok(sCopyProcType, seps);
		pTokenDig = strtok(NULL, seps);

		/* 取出整数和小数 */
		sprintf(sTmpDig, pTokenDig, fabs( aValue - (S32)(aValue) ) ); 
		/* 四舍五入后整数进位处理 */
		fDig = atof(sTmpDig);
		aValue += (S32)fDig;
		sprintf(sTmpInt, pTokenInt, (S32)(aValue) );
		
		/*lint -restore */

		/* "-0.xxx"对负号处理 */
		if ( aValue > -1.0 && aValue < 0.0 )
		{
			memcpy(m_sSetFrm+m_bSetBytePos, "-", sizeof(U8));
			m_bSetBytePos += 1;
		}
		/* copy整数 */
		nByteLen = strlen(sTmpInt);
		memcpy(m_sSetFrm+m_bSetBytePos, sTmpInt, nByteLen);
		m_bSetBytePos += nByteLen;
		/* copy小数 */
		nByteLen = strlen(sTmpDig);
		memcpy(m_sSetFrm+m_bSetBytePos, sTmpDig+1, nByteLen-1);	/**< 只连接.xx */
		m_bSetBytePos += (nByteLen - 1);
	}
	else if ( (eType & 0x0F) == eCHR )
	{
		memcpy( m_sSetFrm+m_bSetBytePos, pVar, sizeof(U8));
		m_bSetBytePos ++;
	}
	else if ( (eType & 0x0F) == eSTR )
	{
		U32 strLen = strlen(pVar);
		memcpy( m_sSetFrm+m_bSetBytePos, pVar, strLen);
		m_bSetBytePos += strLen;
	}
	else if ( (eType & 0x0F) == eHEX )
	{
		sprintf(sTmp, pProcType, (*(U32*)pVar) );
		
		nByteLen = strlen(sTmp);
		memcpy(m_sSetFrm+m_bSetBytePos, sTmp, nByteLen);
		m_bSetBytePos += nByteLen;
	}
	else if( (eType & 0x0F) == eORG )
	{
		sprintf(sTmp, pProcType, (*(F64*)pVar) );
		
		nByteLen = strlen(sTmp);
		memcpy(m_sSetFrm+m_bSetBytePos, sTmp, nByteLen);
		m_bSetBytePos += nByteLen;

	}
	else
	{
		m_bGetRight = FALSE;
	}

	/* 从输入的枚举判断逗号处理 */
	if ( (eType & 0x10) == 0x10 )
	{
		memcpy( m_sSetFrm+m_bSetBytePos, ",", sizeof(U8));
		m_bSetBytePos ++;
	}

	return;
}

void Asc_SetFreqTpye(const U32 nFreqId)
{
	if ( ( m_bSetRight  == FALSE					)	||
		 ( m_bSetBytePos > ASC_SET_MAX_PDATA_LEN	)	||
		 ( nFreqId > eFrqAll						)
		)
	{
		/* 前端状态异常,直接返回 */
		m_bSetRight = FALSE;
		return;
	}
	
	if ( nFreqId == eFrqB1I )
	{
		Asc_SetIceTpye("B1", eSTR, "");
	}
	else if ( nFreqId == eFrqG1 )
	{
		Asc_SetIceTpye("R1"/*"G1"*/, eSTR, "");
	}
	else if ( nFreqId == eFrqB3I )
	{
		Asc_SetIceTpye("B3", eSTR, "");
	}
	else if ( nFreqId == eFrqL1 )
	{
		Asc_SetIceTpye("L1", eSTR, "");
	} 
	else if ( nFreqId == eFrqAll )
	{
		Asc_SetIceTpye("A", eSTR, "");
	}
	else
	{
		m_bSetRight = FALSE;
	}
	
	/* 逗号 */
	memcpy( m_sSetFrm+m_bSetBytePos, ",", sizeof(U8));
	m_bSetBytePos ++;
	
	return;
}

void Asc_SetBranchTpye(const U32 nBrchId)
{
	if ( ( m_bSetRight  == FALSE				)	||
		 ( m_bSetBytePos > ASC_SET_MAX_PDATA_LEN)	||
		 ( nBrchId != 0 && nBrchId != 1			)
		)
	{
		/* 前端状态异常,直接返回 */
		m_bSetRight = FALSE;
		return;
	}
	
	if ( nBrchId == 0 )
	{
		Asc_SetIceTpye("I", eSTR, "");
	}
	else
	{
		Asc_SetIceTpye("Q", eSTR, "");
	}
	
	/* 逗号 */
	memcpy( m_sSetFrm+m_bSetBytePos, ",", sizeof(U8));
	m_bSetBytePos ++;
	
	return;
}

U32 Asc_SetCrcTail( S8 *pData )
{
	U32 i = 0;
	U8 crc = 0;
	
	if ( pData == NULL )	
	{
		m_bSetRight = FALSE;
		return 0;	/**< 异常返回 */
	}

	/* $和* 之间的异或CRC */
	for(i=1;i<m_bSetBytePos;i++)
	{
		crc ^= m_sSetFrm[i];
	}

	m_sSetFrm[m_bSetBytePos] = '*';
	m_bSetBytePos++;
	sprintf((S8_CHAR*)(m_sSetFrm+m_bSetBytePos),"%X",(crc>>4));
	m_bSetBytePos++;
	sprintf((S8_CHAR*)(m_sSetFrm+m_bSetBytePos),"%X",(crc&0x0F));
	m_bSetBytePos++; 
	
	/* 回车换行 */
	m_sSetFrm[m_bSetBytePos] = 0x0D;
	m_bSetBytePos++;
	m_sSetFrm[m_bSetBytePos] = 0x0A;
	m_bSetBytePos++;

	memcpy(pData, m_sSetFrm, m_bSetBytePos);

	return m_bSetBytePos;
}

S32 Asc_U8toChar(const U8* pSrc, S8_CHAR* pDst, S32 nSrcLength)
{
	S32 i;
	
	if ( ( pSrc == NULL		)	||
		 ( pDst == NULL		)	||
		 ( nSrcLength <= 0	)
		)
	{
		return 0;
	}

	for(i=0;i<nSrcLength;i++)
	{
		sprintf(pDst+i*2,"%02X",pSrc[i]);
	}
	
	return (nSrcLength*2);
}

S32 Asc_U8toChar_Space(const U8* pSrc, S8_CHAR* pDst, S32 nSrcLength)
{
	S32 i;
	
	if ( ( pSrc == NULL		)	||
		( pDst == NULL		)	||
		( nSrcLength <= 0	)
		)
	{
		return 0;
	}
	
	for(i=0;i<nSrcLength;i++)
	{
		sprintf(pDst+i*3,"%02X ",pSrc[i]);
	}
	
	return (nSrcLength*3);
}

S32 Asc_ChartoU8(const S8_CHAR* pSrc, U8* pDst, S32 nSrcLength)
{
	U8 *pHead;
	const S8_CHAR *pSrcBound;
	
	if (( pSrc == NULL		)	||
		( pDst == NULL		)	||
		( nSrcLength <= 0	)
		)
	{
		return 0;
	}

	pSrcBound=pSrc+nSrcLength;
	pHead=pDst;
	while(pSrc<pSrcBound)
	{
		/*输出高4位*/
		/*lint -save -e701 -e734 : (Info -- Shift left of signed quantity (int)) */
		if(*pSrc>='0' && *pSrc<='9')
		{
			*pDst = ( (*pSrc) - '0')<<4;
		}
		else
		if(*pSrc>='a' && *pSrc<='f')
		{
			*pDst = ( (*pSrc - 'a') + 0x0A)<<4;
		}
		else
		if(*pSrc>='A' && *pSrc<='F')
		{
			*pDst = ( (*pSrc - 'A') + 0x0A)<<4;
		}
		else
		{
			pSrc++;
			continue;	/**< 跳至While开头重新开始, 即if(*pSrc>='0' && *pSrc<='9') */
		} 
		/*lint -restore */
				
		pSrc++;

		/*输出低四位*/
		if(*pSrc>='0' && *pSrc<='9')
		{
			*pDst |= ( (*pSrc) - '0');
		}
		else
		if(*pSrc>='a' && *pSrc<='f')
		{
			*pDst |= ( (*pSrc - 'a') + 0x0A);
		}
		else
		if(*pSrc>='A' && *pSrc<='F')
		{
			*pDst |= ( (*pSrc - 'A') + 0x0A);
		}
						
		pSrc++;
		pDst++;
	}
		
	return (pDst-pHead);
}

U8 Asc_GetCRC( const U8 *pData, U32 nByteLen )
{
	U32 i = 0;
	U8 crc = 0;
	
	if ( pData == NULL )
	{
		return 0;
	}

	for(i=1;i<nByteLen;i++)
	{
		crc ^= pData[i];
	}
	
	return crc;
}

F64 Asc_GetFree(BOOL bBefore, U32 nPointNum, F64 nOrig)
{
	F64 nRes = 0.0;
	F64 TransPara = pow(10.0, (F64)nPointNum);
	F64 nCmp;

	/* 计算补偿,防止C语言取整时将0.999999999当做0 */
	if ( nOrig > 0.0 )
	{
		nCmp = 0.0000000001;
	}
	else
	{
		nCmp = -0.0000000001;	
	}

	/* 取前半部分数据 */
	if ( bBefore )
	{
		nRes = (F64)( (S32)(nOrig*TransPara) )/TransPara + nCmp;
	}
	else
	{
		nRes = (F64)( nOrig*TransPara - (S32)(nOrig*TransPara))/TransPara + nCmp ;
	}
	
	return nRes;
}

/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/
#ifdef ICE_AUTO_TEST_ENABLE
#include "IceAutoTestC.h"
 
void Test_Asc_ChartoU8();
void Test_Asc_GetCRC();
void Test_Asc_GetHexType();
void Test_Asc_GetIceType();
void Test_Asc_GetInit();
void Test_Asc_GetStatus();
void Test_Asc_SetBranchTpye();
void Test_Asc_SetCrcTail();
void Test_Asc_SetFreqTpye();
void Test_Asc_SetIceTpye();
void Test_Asc_SetInit();
void Test_Asc_U8toChar();
void Test_Asc_GetFree();

void Test_AscFuncs()
{
	Test_Asc_ChartoU8();
	Test_Asc_GetCRC();
	Test_Asc_GetHexType();
	Test_Asc_GetIceType();
	Test_Asc_GetInit();
	Test_Asc_GetStatus();
	Test_Asc_SetBranchTpye();
	Test_Asc_SetCrcTail();
	Test_Asc_SetFreqTpye();
	Test_Asc_SetIceTpye();
	Test_Asc_SetInit();
	Test_Asc_U8toChar();
	Test_Asc_GetFree();

	IceAT_ReportAll();
}

void Test_Asc_ChartoU8()
{
	S8_CHAR pSrc[] = "_1F2DEC0AB397FAAFFFAAAA";
	U8 pDst[32];
	S32 len;

	IceAT_Start("Test_Asc_ChartoU8");
	len = Asc_ChartoU8(pSrc, pDst, 16);
	_TCASSERT(len == 8);	/**< 转换成功8个字节 */
	_TCASSERT(pDst[0] == 0x1F );
	_TCASSERT(pDst[1] == 0x2D );
	_TCASSERT(pDst[2] == 0xEC );
	_TCASSERT(pDst[3] == 0x0A );
	_TCASSERT(pDst[4] == 0xB3 );
	_TCASSERT(pDst[5] == 0x97 );
	_TCASSERT(pDst[6] == 0xFA );
	_TCASSERT(pDst[7] == 0xAF );

	IceAT_End_Report();
}

void Test_Asc_GetCRC()
{
	U8 pData[] = {0x1F, 0x2D, 0xEC, 0x0A, 0xB3, 0x97, 0xFA, 0xAF};
	U8 crc;

	IceAT_Start("Test_Asc_GetCRC");

	crc = Asc_GetCRC( pData, 8 );
	_TCASSERT(crc == 0xBA);

	IceAT_End_Report();
}

void Test_Asc_GetHexType()
{	
	IceAT_Start("Test_Asc_GetHexType");
	{
		U32 aVar = 0;
		U8 pData[] = "$CCALF,123445*FC";
		U32 nTmp = 0;
		
		ASC_GET_START(pData);
		Asc_GetHexType(&nTmp, eU32, 0);	/**< ASC_GET_HEX_U32(nTmp) */

		_TCASSERT(nTmp == 1193029);
	}
	{
		U32 aVar = 0;
		U8 pData[] = "$CCALF,FF*FC";
		S32 nTmp = 0;
		
		ASC_GET_START(pData);
		Asc_GetHexType(&nTmp, eS32, 8);	/**< #define ASC_GET_HEX_U32(a)		Asc_GetHexType(a, eU32, 0); */
		
		_TCASSERT(nTmp == -1);
	}
	{
		U32 aVar = 0;
		U8 pData[] = "$CCALF, FFFFFF*FC";
		S32 nTmp = 0;
		
		ASC_GET_START(pData);
		Asc_GetHexType(&nTmp, eS32, 24); /**< #define ASC_GET_HEX_S32(a,nBit)	Asc_GetHexType(a, eS32, nBit);*/
		
		_TCASSERT(nTmp == -1);
	}
	{
		U32 aVar = 0;
		U8 pData[] = "$CCALF, 1FFF*FC";
		S32 nTmp = 0;
		
		ASC_GET_START(pData);
		Asc_GetHexType(&nTmp, eS32, 13); /**< #define ASC_GET_HEX_S32(a,nBit)	Asc_GetHexType(a, eS32, nBit);*/
		
		_TCASSERT(nTmp == -1);
	}
	{
		U32 aVar = 0;
		U8 pData[] = "$CCALF, 7F*FC";
		S32 nTmp = 0;
		
		ASC_GET_START(pData);
		Asc_GetHexType(&nTmp, eS32, 8);	/**< #define ASC_GET_HEX_S32(a,nBit)	Asc_GetHexType(a, eS32, nBit);*/
		
		_TCASSERT(nTmp == 127);
	}
	IceAT_End_Report();
}

void Test_Asc_GetIceType()
{
	IceAT_Start("Test_Asc_GetIceType");
	/* S32 test */
	{
		S32 aVar = 0;
		U8 pData[] = "$CCRIS,*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eS32);
		_TCASSERT(aVar == 0);
	}
	{
		S32 aVar = 0;
		U8 pData[] = "$CCRIS,-123*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eS32);
		_TCASSERT(aVar == -123);
	}
	{
		S32 aVar = 0;
		U8 pData[] = "$CCRIS,-123.0*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eS32);
		_TCASSERT(aVar == -123);
	}
	{
		S32 aVar = 0;
		U8 pData[] = "$CCRIS,1123423*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eS32);
		_TCASSERT(aVar == 1123423);
	}
	/* U32 test */
	{
		U32 aVar = 0;
		U8 pData[] = "$CCRIS,*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eU32);
		_TCASSERT(aVar == 0);
	}
	{
		U32 aVar = 0;
		U8 pData[] = "$CCRIS,-123*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eU32);
		_TCASSERT(aVar == 4294967173);
	}
	{
		U32 aVar = 0;
		U8 pData[] = "$CCRIS,-123.0*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eU32);
		_TCASSERT(aVar == 4294967173);
	}
	{
		U32 aVar = 0;
		U8 pData[] = "$CCRIS,1123423*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eU32);
		_TCASSERT(aVar == 1123423);
	}
	/* F64 test */
	{
		F64 aVar = 0;
		U8 pData[] = "$CCRIS,*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eF64);
		_TCASSERT_EQU_E(aVar, 0.0);
	}
	{
		F64 aVar = 0;
		U8 pData[] = "$CCRIS,-123*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eF64);
		_TCASSERT_EQU_E(aVar, -123.0);
	}
	{
		F64 aVar = 0;
		U8 pData[] = "$CCRIS,-123.0*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eF64);
		_TCASSERT_EQU_E(aVar, -123.0);
	}
	{
		F64 aVar = 0;
		U8 pData[] = "$CCRIS,1123423*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eF64);
		_TCASSERT_EQU_E(aVar, 1123423.0);
	}
	/* CHR test */
	{
		S8_CHAR aVar;
		U8 pData[] = "$CCRIS,*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eCHR);
		_TCASSERT(aVar == '0');
	}
	{
		S8_CHAR aVar;
		U8 pData[] = "$CCRIS,S*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eCHR);
		_TCASSERT(aVar == 'S');
	}
	{
		S8_CHAR aVar;
		U8 pData[] = "$CCRIS,TT*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eCHR);
		_TCASSERT(aVar == 'T');
	}
	{
		S8_CHAR aVar;
		U8 pData[] = "$CCRIS,W,*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eCHR);
		_TCASSERT(aVar == 'W');
	}
	/* STR test */
	{
		S8_CHAR aVar[10];
		U8 pData[] = "$CCRIS,*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eSTR);
		_TCASSERT(aVar[0] == '0');
	}
	{
		S8_CHAR aVar[10];
		U8 pData[] = "$CCRIS,-Sasf*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eSTR);
		_TCASSERT(aVar[0] == '-');
		_TCASSERT(aVar[1] == 'S');
		_TCASSERT(aVar[2] == 'a');
		_TCASSERT(aVar[3] == 's');
		_TCASSERT(aVar[4] == 'f');
	}
	{
		S8_CHAR aVar[10];
		U8 pData[] = "$CCRIS,TT*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eSTR);
		_TCASSERT(aVar[0] == 'T');
		_TCASSERT(aVar[1] == 'T');
	}
	{
		S8_CHAR aVar[10];
		U8 pData[] = "$CCRIS,W4567,*FC";
		
		ASC_GET_START(pData);
		Asc_GetIceType(&aVar, eSTR);
		_TCASSERT(aVar[0] == 'W');
		_TCASSERT(aVar[1] == '4');
		_TCASSERT(aVar[2] == '5');
		_TCASSERT(aVar[3] == '6');
		_TCASSERT(aVar[4] == '7');
	}
	IceAT_End_Report();
}

void Test_Asc_GetInit()
{
	IceAT_Start("Test_Asc_GetInit");
	{
		U8 pData[] = "$CCRIS,W,*FC\r\n";
		U8 pDateRes[] = "$CCRIS,W,0*FC\r\n";

		m_bGetRight = FALSE;
		Asc_GetInit(pData);
		_TCASSERT(m_bGetRight == TRUE);
		_TCASSERT(m_sGetFrmTmp[0] == '$');
		_TCASSERT(m_sGetFrmTmp[1] == 'C');
		_TCASSERT(m_sGetFrmTmp[2] == 'C');
		_TCASSERT(m_sGetFrmTmp[3] == 'R');
		_TCASSERT(m_sGetFrmTmp[4] == 'I');
		_TCASSERT(m_sGetFrmTmp[5] == 'S');
	}
	IceAT_End_Report();
}

void Test_Asc_GetStatus()
{
	IceAT_Start("Test_Asc_GetStatus");
	{
		m_bGetRight = FALSE;
		_TCASSERT(Asc_GetStatus() == FALSE);
		m_bGetRight = TRUE;
		_TCASSERT(Asc_GetStatus() == TRUE);
	}
	IceAT_End_Report();
}

void Test_Asc_SetBranchTpye()
{
	IceAT_Start("Test_Asc_SetBranchTpye");
	{
		U8 pData[1024] = {0};
		ASC_SET_START( pData != NULL );
		Asc_SetBranchTpye(0);
		_TCASSERT(m_sSetFrm[0] == 'I');
		_TCASSERT(m_sSetFrm[1] == ',');
		_TCASSERT(m_bSetBytePos == 2);
	}
	{
		U8 pData[1024] = {0};
		ASC_SET_START( pData != NULL );
		Asc_SetBranchTpye(1);
		_TCASSERT(m_sSetFrm[0] == 'Q');
		_TCASSERT(m_sSetFrm[1] == ',');
		_TCASSERT(m_bSetBytePos == 2);
	}
	IceAT_End_Report();
}

void Test_Asc_SetCrcTail()
{
	IceAT_Start("Test_Asc_SetCrcTail");
	{
		S8 pData[1024] = {0};
		U32 aLen = 0, i;
		S8 pDataTest[]    = "$PNnnc,CRO,005,B1,13,C,4,176000.020,-28880000.9870000,28880000.987,0.0000123";
		S8 pDataTestRes[] = "$PNnnc,CRO,005,B1,13,C,4,176000.020,-28880000.9870000,28880000.987,0.0000123*0f\r\n";
		m_bSetBytePos = 76;
		
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		memcpy(m_sSetFrm, pDataTest, 76);
		
		aLen = Asc_SetCrcTail(pData);
		_TCASSERT(aLen == 81);
		for ( i=0; i<aLen; i++ )
		{
			_TCASSERT(pData[i] == pDataTestRes[i]);
		}
	}
	IceAT_End_Report();
}

void Test_Asc_SetFreqTpye()
{
	IceAT_Start("Test_Asc_SetFreqTpye");
	{
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		Asc_SetFreqTpye(0);
		_TCASSERT(m_bSetBytePos == 3);
		_TCASSERT(m_sSetFrm[0] == 'B');
		_TCASSERT(m_sSetFrm[1] == '1');
		_TCASSERT(m_sSetFrm[2] == ',');
	}
	{
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		Asc_SetFreqTpye(1);
		_TCASSERT(m_bSetBytePos == 3);
		_TCASSERT(m_sSetFrm[0] == 'G');
		_TCASSERT(m_sSetFrm[1] == '1');
		_TCASSERT(m_sSetFrm[2] == ',');
	}
	{
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		Asc_SetFreqTpye(2);
		_TCASSERT(m_bSetBytePos == 3);
		_TCASSERT(m_sSetFrm[0] == 'B');
		_TCASSERT(m_sSetFrm[1] == '3');
		_TCASSERT(m_sSetFrm[2] == ',');
	}
	{
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		Asc_SetFreqTpye(3);
		_TCASSERT(m_bSetBytePos == 3);
		_TCASSERT(m_sSetFrm[0] == 'L');
		_TCASSERT(m_sSetFrm[1] == '1');
		_TCASSERT(m_sSetFrm[2] == ',');
	}
	{
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		Asc_SetFreqTpye(4);
		_TCASSERT(m_bSetBytePos == 2);
		_TCASSERT(m_sSetFrm[0] == 'S');
		_TCASSERT(m_sSetFrm[1] == ',');
	}
	{
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		Asc_SetFreqTpye(5);
		_TCASSERT(m_bSetBytePos == 2);
		_TCASSERT(m_sSetFrm[0] == 'A');
		_TCASSERT(m_sSetFrm[1] == ',');
	}
	IceAT_End_Report();
}

void Test_Asc_SetIceTpye()
{
	U32 i;
	IceAT_Start("Test_Asc_SetIceTpye");
	/* eS32 */
	{
		S32 aVar = -324;
		S8 pDataRes[] = "-324";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;

		Asc_SetIceTpye( &aVar, eS32, "%d" );
		_TCASSERT( m_bSetBytePos == 4 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	{
		S32 aVar = -324;
		S8 pDataRes[] = "-0324,";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eS32_COMMA, "%05d" );
		_TCASSERT( m_bSetBytePos == 6 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	/* eU32 */
	{
		U32 aVar = 472389;
		S8 pDataRes[] = "000472389";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eU32, "%09d" );
		_TCASSERT( m_bSetBytePos == 9 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	{
		U32 aVar = 4723;
		S8 pDataRes[] = "4723,";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eU32_COMMA, "%d" );
		_TCASSERT( m_bSetBytePos == 5 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	/* eF64 */
	{
		F64 aVar = -472389.23542;
		S8 pDataRes[] = "-472389.23542000";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eF64, "%d+%.8lf+" );
		_TCASSERT( m_bSetBytePos == 16 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	{
		F64 aVar = -0.23542;
		S8 pDataRes[] = "-0.23542000,";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eF64_COMMA, "%d+%.8lf+" );
		_TCASSERT( m_bSetBytePos == 12 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	{
		F64 aVar = 4294919213.870407;
		S8 pDataRes[] = "-0.23542000,";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eORG_COMMA, "%lf" );
		_TCASSERT( m_bSetBytePos == 12 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	/* eCHR */
	{
		S8_CHAR aVar = 'S';
		S8 pDataRes[] = "S";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eCHR, "" );
		_TCASSERT( m_bSetBytePos == 1 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	{
		S8_CHAR aVar = 'T';
		S8 pDataRes[] = "T,";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eCHR_COMMA, "" );
		_TCASSERT( m_bSetBytePos == 2 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	/* eSTR */
	{
		S8_CHAR aVar[] = "yatsdasfu2134";
		S8 pDataRes[] = "yatsdasfu2134";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eSTR, "" );
		_TCASSERT( m_bSetBytePos == 13 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	{
		S8_CHAR aVar[] = "yatsdasfu2134";
		S8 pDataRes[] = "yatsdasfu2134,";
		memset(m_sSetFrm, 0, sizeof(m_sSetFrm));
		m_bSetBytePos = 0;
		
		Asc_SetIceTpye( &aVar, eSTR_COMMA, "" );
		_TCASSERT( m_bSetBytePos == 14 );
		for ( i=0; i<m_bSetBytePos; i++ )
		{
			_TCASSERT( m_sSetFrm[i] == pDataRes[i] );
		}
	}
	IceAT_End_Report();
}

void Test_Asc_SetInit()
{
	U32 i;
	IceAT_Start("Test_Asc_SetInit");
	{
		m_bSetRight = FALSE;
		m_sSetFrm[1] = 0x02;
		m_bSetBytePos = 124;

		Asc_SetInit(TRUE);
		_TCASSERT(m_bSetRight == TRUE);
		_TCASSERT(m_bSetBytePos == 0);
		for ( i=0; i<ASC_SET_MAX_PDATA_LEN; i++ )
		{
			_TCASSERT(m_sSetFrm[i] == 0x00);
		}
	}
	IceAT_End_Report();
}

void Test_Asc_U8toChar()
{
	IceAT_Start("Test_Asc_U8toChar");
	{
		S32 i;
		U8 pData[] = {0xAB, 0xCD, 0xEF};
		S8_CHAR pStr[1024];
		S8_CHAR pStrRes[] = "ABCDEF";

		S32 aLen = 0;

		aLen = Asc_U8toChar(pData, pStr, 3);
		_TCASSERT(aLen == 6);
		for ( i=0; i<aLen; i++ )
		{
			_TCASSERT(pStrRes[i] == pStr[i]);
		}
	}
	IceAT_End_Report();
}

void Test_Asc_GetFree()
{
	IceAT_Start("Test_Asc_GetFree");
	{
		F64 nOrig = 25.123456789;
		F64 res = 0.0;
		res = Asc_GetFree(TRUE, 3, nOrig);
		_TCASSERT_F64(res,25.123);
	}
	IceAT_Start("Test_Asc_GetFree");
	{
		F64 nOrig = 25.123456789;
		F64 res = 0.0;
		res = Asc_GetFree(FALSE, 3, nOrig);
		_TCASSERT_F64(res,0.000456789);
	}
	{
		F64 nOrig = -25.123456789;
		F64 res = 0.0;
		res = Asc_GetFree(TRUE, 3, nOrig);
		_TCASSERT_F64(res,-25.123);
	}
	IceAT_Start("Test_Asc_GetFree");
	{
		F64 nOrig = -25.123456789;
		F64 res = 0.0;
		res = Asc_GetFree(FALSE, 3, nOrig);
		_TCASSERT_F64(res,-0.000456789);
	}
	IceAT_End_Report();
}


#endif /* #ifdef ICE_AUTO_TEST_ENABLE */
/************************************************************************/
/**********以上为测试代码************************************************/
/************************************************************************/
/*lint -restore */

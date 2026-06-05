#include "SfrmExtract.h"
#include "IceErrCtrl.h"
#include "icetype.h"
#include <string.h>
#include <stdio.h>
typedef signed int S32;
typedef	unsigned char	U8;			/**<  unsigned 8 bit data  */
typedef	unsigned short	U16;		/**<  unsigned 16 bit data */
typedef	unsigned int	U32;		/**<  unsigned 32 bit data */
typedef	signed char		S8;			/**<  signed 8 bit data  */

#define MAX_FRAME_HEADTAIL_LEN	16		/**< 最大帧头与帧尾长度	*/

static S32 IsFrameHead(FrameBufferStruct *pBuffer);
static S32 FBufferGetFrameLen(FrameBufferStruct *pBuffer, const U8 *pData, U32 nHeaderType);
static S32 IsFrameIcHead(FrameBufferStruct *pBuffer, const U8 *pData);
static S32 CheckHeadAndTail(FrameBufferStruct *pBuffer, U32 nHeaderType);
static BOOL CrcCheck(FrameBufferStruct* pBuffer, S32 nLength);

BOOL AddHeader(FrameBufferStruct * pBuffer, U8 * pFrameHead, S32 frameHeadLen, S32 dataLenIndex, S32 nFrameHeadTailLen, S32 maxFrameLen)
{

	return TRUE;
}

BOOL FBufferCreate(FrameBufferStruct * pBuffer, S32 bufferSize, S8 *pStaticBuffer)
{

	VERIFY_RET(
		(pBuffer != NULL		)	&&
		(bufferSize > 0			)	&&
		(pStaticBuffer != NULL	)
		);
	pBuffer->nHeaderNum = 0;
	VERIFY_RET(Sr_Create(bufferSize, &pBuffer->aRingBuffer, pStaticBuffer));

	return TRUE;
}

BOOL FBufferInit( FrameBufferStruct * pBuffer, U8 * pFrameHead, S32 frameHeadLen, S32 frameTailLen, S32 dataLenIndex, S32 dataLenBytes, S32 nFrameHeadTailLen, S32 maxFrameLen )
{
	U32 nIndex = 0;
	VERIFY_RET( 
		(pBuffer != NULL							)	&& 
		(nFrameHeadTailLen <= MAX_FRAME_HEADTAIL_LEN)	&& 
		(frameHeadLen <= MAX_FRAME_HEAD_LEN			)	&& 
		(dataLenIndex <= nFrameHeadTailLen			)	
		);


// 	pBuffer->nFrameHeadTailLen=nFrameHeadTailLen;
// 	memcpy(pBuffer->pFrameHead, pFrameHead, (U32)(frameHeadLen));
// 	pBuffer->nFrameHeadLen = frameHeadLen;
// 	pBuffer->nDataLenPos = dataLenIndex;
// 	pBuffer->nMaxFrameLen = maxFrameLen;
// 	pBuffer->nDataLenBytes = dataLenBytes;

	nIndex = pBuffer->nHeaderNum;
	pBuffer->sHeader[nIndex].nFrameHeadTailLen=nFrameHeadTailLen;
	memcpy(pBuffer->sHeader[nIndex].pFrameHead, pFrameHead, (U32)(frameHeadLen));
	pBuffer->sHeader[nIndex].nFrameHeadLen = frameHeadLen;
	pBuffer->sHeader[nIndex].nDataLenPos = dataLenIndex;
	pBuffer->sHeader[nIndex].nMaxFrameLen = maxFrameLen;
	pBuffer->sHeader[nIndex].nDataLenBytes = dataLenBytes;
	pBuffer->sHeader[nIndex].bLsb = FALSE;
	pBuffer->sHeader[nIndex].bIsFrameLen = FALSE;
	pBuffer->sHeader[nIndex].nFrameTailLen = frameTailLen;
	pBuffer->nHeaderNum++;

	return TRUE;
} 

BOOL FBufferExInit( FrameBufferStruct * pBuffer, U8 * pFrameHead, U8 *pFameTail, S32 frameHeadLen,  S32 frameTailLen, S32 dataLenIndex, S32 dataLenBytes, S32 nFrameHeadTailLen, BOOL bIsFramLen, BOOL bLSB, S32 maxFrameLen)
{
	U32 nIndex = 0;
	VERIFY_RET( 
		(pBuffer != NULL							)	&& 
		(nFrameHeadTailLen <= MAX_FRAME_HEADTAIL_LEN)	&& 
		(frameHeadLen <= MAX_FRAME_HEAD_LEN			)	&& 
		(dataLenIndex <= nFrameHeadTailLen			)	
		);

	nIndex = pBuffer->nHeaderNum;
	pBuffer->sHeader[nIndex].nFrameHeadTailLen=nFrameHeadTailLen;
	memcpy(pBuffer->sHeader[nIndex].pFrameHead, pFrameHead, (U32)(frameHeadLen));
	memcpy(pBuffer->sHeader[nIndex].pFrameTail, pFameTail, (U32)(frameTailLen));
	pBuffer->sHeader[nIndex].nFrameHeadLen = frameHeadLen;
	pBuffer->sHeader[nIndex].nDataLenPos = dataLenIndex;
	pBuffer->sHeader[nIndex].nMaxFrameLen = maxFrameLen;
	pBuffer->sHeader[nIndex].nDataLenBytes = dataLenBytes;
	pBuffer->sHeader[nIndex].bLsb = bLSB;
	pBuffer->bHasFrame = 0;
	pBuffer->nFrameLen = 0;
	pBuffer->sHeader[nIndex].bIsFrameLen = bIsFramLen;
	pBuffer->sHeader[nIndex].nFrameTailLen = frameTailLen;
	pBuffer->nHeaderNum++;
	return TRUE;
}

S32 FBufferPush( FrameBufferStruct *pBuffer,const U8 * pData, S32 dataByteLen )
{
	VERIFY_LOG_RETVAL(
		(pBuffer != NULL) &&
		(pData != NULL	) ,
		0
		);

	return Sr_BufPut(&pBuffer->aRingBuffer, (S8*)pData, dataByteLen);
}

S32 FBufferPushForce( FrameBufferStruct *pBuffer,const U8 * pData, S32 dataByteLen )
{
	VERIFY_LOG_RETVAL(
		(pBuffer != NULL) &&
		(pData != NULL	) ,
		0
		);

	return Sr_BufPutForce(&pBuffer->aRingBuffer, (S8*)pData, dataByteLen);
}

S32 FBufferHasFrame(FrameBufferStruct *pBuffer)
{
	S8 pData[MAX_FRAME_HEADTAIL_LEN];
	U32 nFrmLen = 0;
	U32 nFromBuf = 0;

	VERIFY_LOG_RETVAL(
		(pBuffer != NULL	) ,
		0
		);

	/* pBuffer->bHasFrame在POP时清空 */
	if(pBuffer->bHasFrame==1)
	{
		return 1;
	}

	nFromBuf = pBuffer->aRingBuffer.pFromBuf;
	while(Sr_BufGetNoDel(&pBuffer->aRingBuffer,pData, 1) == 1)		
	{
		if((nFrmLen = IsFrameHead(pBuffer)) > 0)
		{
			pBuffer->bHasFrame = 1;
			pBuffer->nFrameLen = nFrmLen;
			return pBuffer->bHasFrame;
		}
		if(Sr_BufGet(&pBuffer->aRingBuffer,pData, 1) != 1)
		{
			break;
		}
	}
	pBuffer->aRingBuffer.pFromBuf = nFromBuf;
	return pBuffer->bHasFrame;
}

static S32 FBufferGetFrameLen(FrameBufferStruct* pBuffer, const U8* pData, U32 nHeaderType)
{
	U32 nResult = 0;
	FrameBufferHeader* pFrameHeader = NULL;

	VERIFY_LOG_RETVAL(
		(pBuffer != NULL) &&
		(pData != NULL),
		0
	);

	pFrameHeader = &pBuffer->sHeader[nHeaderType];

	if (pFrameHeader->nDataLenBytes > 0)
	{
		U32 i = 0;
		U32 nPos = 0;


		for (i = 0; i < pFrameHeader->nDataLenBytes; i++)
		{
			if (pFrameHeader->bLsb)
			{
				nPos = pFrameHeader->nDataLenPos + i;
			}
			else
			{
				nPos = pFrameHeader->nDataLenPos + pFrameHeader->nDataLenBytes - 1 - i;
			}

			nResult |= (U32)pData[nPos] << (i * 8);
		}
		if (!pFrameHeader->bIsFrameLen)
		{
			nResult += pFrameHeader->nFrameHeadTailLen;
		}
	}
	else
	{
		U8 pTail[MAX_FRAME_HEAD_LEN] = { 0 };
		S32 nSaveFromBuf = pBuffer->aRingBuffer.pFromBuf;
		//		U32 nTmpFromBuf = pBuffer->aRingBuffer.pFromBuf;
		U32 nTailLen = pFrameHeader->nFrameHeadTailLen - pFrameHeader->nFrameHeadLen;

		if (Sr_BufGet(&pBuffer->aRingBuffer, pTail, pFrameHeader->nFrameHeadLen) != pFrameHeader->nFrameHeadLen)
		{
			return 0;
		}

		while (Sr_BufGetNoDel(&pBuffer->aRingBuffer, pTail, nTailLen) == nTailLen)
		{
			U32 i = 0;

			if (memcmp(pFrameHeader->pFrameHead, &pTail[i], pFrameHeader->nFrameHeadLen) == 0)
			{
				break;
			}

			if (memcmp(pFrameHeader->pFrameTail, &pTail[i], nTailLen) == 0)
			{
				nResult = pBuffer->aRingBuffer.pFromBuf + nTailLen - nSaveFromBuf;
				if (pBuffer->aRingBuffer.pFromBuf < nSaveFromBuf)
				{
					nResult += (pBuffer->aRingBuffer.bufSize - 1);
				}

				pBuffer->aRingBuffer.pFromBuf = nSaveFromBuf;
				break;
			}

			if (Sr_BufGet(&pBuffer->aRingBuffer, pTail, 1) != 1)
			{
				break;
			}
		}

		pBuffer->aRingBuffer.pFromBuf = nSaveFromBuf;
	}

	return (S32)nResult;
}


/* 
返回值说明 ：0：帧头未找到，或校验没过 ，-1：帧头已经找到，但缓冲区中数据不够或异常
*/
static S32 CheckHeadAndTail(FrameBufferStruct *pBuffer, U32 nHeaderType)
{
	S32 len = 0;
	U32 nTailLen = 0;
	U8 pData[MAX_FRAME_HEAD_TAIL_LEN] = { 0 };
	U8 pTempTail[MAX_FRAME_HEAD_LEN] = { 0 };	/**<当前帧格式帧尾*/

	FrameBufferHeader* pHeader = &pBuffer->sHeader[nHeaderType];
	if (Sr_BufGetNoDel(&pBuffer->aRingBuffer, (S8*)pData, pHeader->nFrameHeadTailLen) != pHeader->nFrameHeadTailLen)
	{
		return 0;
	}

	if (memcmp(pHeader->pFrameHead, pData, (U32)(pHeader->nFrameHeadLen)) != 0)
	{
		return 0;
	}

	len = FBufferGetFrameLen(pBuffer, pData, nHeaderType);

	if (len == 0)
	{
		return 0;
	}

	if (	(len < pHeader->nFrameHeadTailLen) 
		 || (len > pHeader->nMaxFrameLen	 ))
	{
		return ERR_NORAML_ERROR;
	}

	/*crc 校验*/
	if (CrcCheck(pBuffer, len) == FALSE)
	{
		return ERR_NORAML_ERROR;
	}

	nTailLen = pHeader->nFrameTailLen;

	/* @wp 精简 改进一下，就是判断缓冲区有没有这么多字节，并且帧尾是不是pFrameTail，根本不用缓冲 */
	if (Sr_MoveGetByte(&pBuffer->aRingBuffer, len, nTailLen, pTempTail) == nTailLen)
	{
		if (memcmp(pHeader->pFrameTail, pTempTail, nTailLen) == 0)
		{
			return len;
		}
		else
		{
			return ERR_NORAML_ERROR;
		}
	}
	/*
		if ( Sr_BufGetNoDel(&pBuffer->aRingBuffer,(S8*)m_TempU8,len) == len )
		{

			if (memcmp(pHeader->pFrameTail, &m_TempU8[len - nTailLen], nTailLen) == 0)
			{
				return len;
			}
			else
			{
				return ERR_NORAML_ERROR;
			}
		}
	*/
	return ERR_NORAML_ERROR;
}

static BOOL CrcCheck(FrameBufferStruct* pBuffer, S32 nLength)
{
	/*	crc 校验 */
	U8* u8Data;
	U8 u8Crc = 0;
	U8 u8CrcCdst = 0;
	U8 u8CrcCdnd = 0;
	S32 s32CkCdst = 0;	/*	第一个校验码 */
	S32 s32CkCdnd = 0;	/*	第二个校验码 */
	S32 s32FrmTail = 0;	/*	帧尾的位置	*/
	S32 s32FrmBuf = 0;
	SStaticRng* pStaticRing;

	if ((pBuffer == NULL) || (nLength == 0))
	{
		return FALSE;
	}

	pStaticRing = &pBuffer->aRingBuffer;
	u8Data = (U8*)&pStaticRing->buf[0];

	s32FrmTail = (pStaticRing->pFromBuf + nLength);
	if (s32FrmTail <= (pStaticRing->bufSize + 1))
	{
		s32CkCdst = (s32FrmTail - 4);
		s32CkCdnd = (s32FrmTail - 3);
	}
	else if (s32FrmTail == (pStaticRing->bufSize + 2))
	{
		s32CkCdst = (s32FrmTail - 4);
		s32CkCdnd = 0;
	}
	else
	{
		s32CkCdst = (s32FrmTail - (pStaticRing->bufSize - 1) - 4);
		s32CkCdnd = (s32FrmTail - (pStaticRing->bufSize - 1) - 3);
	}
	for (S32 i = 1; i < (nLength - 5); i++)	/*	对$到*之间的数据求校验和 */
	{
		s32FrmBuf = (pStaticRing->pFromBuf + i);
		if (s32FrmBuf >= (pStaticRing->bufSize - 1))
		{
			s32FrmBuf -= (pStaticRing->bufSize - 1);
		}
		u8Crc ^= u8Data[s32FrmBuf];
	}
	u8CrcCdst = u8Crc >> 4;
	u8CrcCdnd = u8Crc & 0x0F;
	u8CrcCdst = u8CrcCdst <= 0x09 ? u8CrcCdst + 0x30 : u8CrcCdst + 0x37;
	u8CrcCdnd = u8CrcCdnd <= 0x09 ? u8CrcCdnd + 0x30 : u8CrcCdnd + 0x37;
	if ((u8CrcCdst == u8Data[s32CkCdst]) && (u8CrcCdnd == u8Data[s32CkCdnd]))
	{
		return TRUE;
	}

	return FALSE;
}

static S32 IsFrameHead(FrameBufferStruct *pBuffer)
{
	S32 len = 0;
	U32 i = 0;

	for (i = 0; i < pBuffer->nHeaderNum; i++)
	{
		if ((len = CheckHeadAndTail(pBuffer, i)) > 0)
		{
			return len;
		}
		/* 帧头已经匹配过则不再遍历查找帧 */
		if (len < 0)
		{
			break;
		}
	}

	return 0;
}

S32 IsRdssCrcAdap(const SStaticRngId rngId)
{
	U32 nDataLen = 0;
	U8	crc = 0;
	S32 i;
	U8 *pData;
	U8 Ck_A = 0;
	U8 Ck_B = 0;

	/* 获取缓冲区中的有效数据 */
	S32 TotalLen = 0;

	pData =(U8 *)&rngId->buf [rngId->pFromBuf];

	TotalLen = Sr_NBytes(rngId);
	if ((TotalLen < 8)   &&
		(TotalLen > 2048	)
		)	
	{
		return 0;	
	}

	/* 计算长度 */
	memcpy(&nDataLen, &pData[4], 2);
	nDataLen += 8;

	if ( (nDataLen < 8		)	||
		(nDataLen > 2048	)   ||
		(nDataLen > TotalLen)
		)	
	{
		return 0;	
	}	

	for (i= 2;i < nDataLen - 2;i++)
	{
		Ck_A += pData[i];
		Ck_B += Ck_A;
	}

	if ((pData[nDataLen - 2] != Ck_A) || (pData[nDataLen - 1] != Ck_B))
	{
		return 0;
	}

	return nDataLen;
}


S32 FBufferHasExFrame(FrameBufferStruct *pBuffer)
{
	S8 pData[MAX_FRAME_HEADTAIL_LEN];
	U32 nFrmLen = 0;
	U32 nFromBuf = 0;

	VERIFY_LOG_RETVAL(
		(pBuffer != NULL),
		0
	);

	/* pBuffer->bHasFrame在POP时清空 */
	if (pBuffer->bHasFrame == 1)
	{
		return 1;
	}

	nFromBuf = pBuffer->aRingBuffer.pFromBuf;
	while (Sr_BufGetNoDel(&pBuffer->aRingBuffer, pData, 1) == 1)
	{
		if ((nFrmLen = IsRdssCrcAdap(&pBuffer->aRingBuffer)) > 0)
		{
			pBuffer->bHasFrame = 1;
			pBuffer->nFrameLen = nFrmLen;
			return pBuffer->bHasFrame;
		}
		if ((nFrmLen = IsFrameHead(pBuffer)) > 0)
		{
			pBuffer->bHasFrame = 1;
			pBuffer->nFrameLen = nFrmLen;
			return pBuffer->bHasFrame;
		}

		if (Sr_BufGet(&pBuffer->aRingBuffer, pData, 1) != 1)
		{
			break;
		}
	}
	pBuffer->aRingBuffer.pFromBuf = nFromBuf;

	return pBuffer->bHasFrame;
}

S32 FBufferHasData( FrameBufferStruct *pBuffer, S32 byteLen )
{
	VERIFY_LOG_RETVAL(
		(pBuffer != NULL) &&
		(byteLen > 0	) ,
		0
		);
	
	if(byteLen <= Sr_NBytes(&pBuffer->aRingBuffer))
	{
		pBuffer->nFrameLen = byteLen;
		pBuffer->bHasFrame = 1;
		return 1;
	}
	else
	{
		pBuffer->nFrameLen=0;
		return 0;
	}
}

S32 FBufferPop( FrameBufferStruct * pBuffer, U8 * pData, S32 maxByteLen )
{
	S32 bOverLoad;
	S32 dataByteLen = 0;

	VERIFY_LOG_RETVAL(
		(pBuffer != NULL	) &&
		(pData != NULL		) &&
		(maxByteLen > 0	) ,
		0
		);

	if(pBuffer->bHasFrame!=1)
	{
		dataByteLen=0;
		return 0;
	}
	
	if( pBuffer->nFrameLen > maxByteLen)
	{
		dataByteLen = maxByteLen;
		bOverLoad=1;
	}
	else
	{
		dataByteLen = pBuffer->nFrameLen;
		bOverLoad=0;
	}

	if (pBuffer->nFrameLen + pBuffer->aRingBuffer.pFromBuf >= pBuffer->aRingBuffer.bufSize - 1)
	{
		S32 nPrePart = 0;
		S32 nLatterPart = 0;

		nPrePart = pBuffer->aRingBuffer.bufSize - 1 - pBuffer->aRingBuffer.pFromBuf;
		nLatterPart = pBuffer->nFrameLen - nPrePart;
		nPrePart = Sr_BufGet(&pBuffer->aRingBuffer, (S8*)pData, nPrePart);
		nLatterPart = Sr_BufGet(&pBuffer->aRingBuffer, (S8*)&pData[nPrePart], nLatterPart);
		dataByteLen = nPrePart + nLatterPart;
	}
	else
	{
		dataByteLen = Sr_BufGet(&pBuffer->aRingBuffer, (S8*)pData, dataByteLen);
	}

	pBuffer->bHasFrame=0;
	pBuffer->nFrameLen=0;

	if(bOverLoad)
	{
		return 0;
	}
	else
	{
		return dataByteLen;
	}
}

S32 FBufferPopBareData( FrameBufferStruct * pBuffer, U8 * pData, S32 maxByteLen )
{
	S32 bOverLoad;
	S32 dataByteLen = 0;

	VERIFY_LOG_RETVAL(
		(pBuffer != NULL	) &&
		(pData != NULL		) &&
		(maxByteLen > 0	) ,
		0
		);

	/*  */

	if( pBuffer->nFrameLen > maxByteLen)
	{
		dataByteLen = maxByteLen;
		bOverLoad=1;
	}
	else
	{
		dataByteLen = Sr_NBytes(&pBuffer->aRingBuffer);
		if (dataByteLen >= maxByteLen) /**< 判断，输入最大长度小于缓冲区数据长度，发送最大长度数据 */
		{
			dataByteLen = maxByteLen;
		}
		bOverLoad=0;
	}

	dataByteLen=Sr_BufGet( &pBuffer->aRingBuffer,(S8*)pData,dataByteLen);

	pBuffer->bHasFrame=0;
	pBuffer->nFrameLen=0;

	if(bOverLoad)
	{
		return 0;
	}
	else
	{
		return dataByteLen;
	}
}

BOOL FBufferClear(FrameBufferStruct* pBuffer, S32 bufferSize, S8* pStaticBuffer)
{
	VERIFY_RET(
		(pBuffer != NULL) &&
		(bufferSize > 0) &&
		(pStaticBuffer != NULL)
	);

	VERIFY_RET(Sr_Create(bufferSize, &pBuffer->aRingBuffer, pStaticBuffer));

	return TRUE;
}

S32 FBufferPopBytes( FrameBufferStruct * pBuffer, U8 * pData, S32 dataByteLen )
{
	S32 popLen = 0;

	VERIFY_LOG_RETVAL(
		(pBuffer != NULL	) &&
		(pData != NULL		) ,
		0
		);

	popLen = Sr_BufGet(&pBuffer->aRingBuffer, (S8*)pData, dataByteLen);
	
	if (popLen < dataByteLen)
	{
		return 0;
	}
	else
	{
		return popLen;
	}
/*lint -save -e818 : (Info -- Pointer parameter 'pBuffer' (line 234) could be declared as pointing to const) */
} 
/*lint -restore */


static S32 IsFrameIcHead(FrameBufferStruct *pBuffer, const U8 *pData)
{
	S32 i;
	U8 crcCalc = 0;
	U8 crcRead = 0;
	S32 canUseBuf = 0;
	U8 tempOneU8 = 0;
	S32 origFromBuf = 0;

	S32 nLen = FBufferGetFrameLen(pBuffer,pData,0);
	if ( (nLen == 0) || (nLen > 512) )
	{
		return 0;
	}

	/* @wp 同上 精简 改进一下，就是判断缓冲区有没有这么多字节，并且帧尾是不是pFrameTail，根本不用缓冲 */
	/* 判断一下能否读到这么长数据 */
	canUseBuf = Sr_NBytes(&pBuffer->aRingBuffer);
	if (canUseBuf < nLen)
	{
		return 0;
	}
/*
	if ( Sr_BufGetNoDel(&pBuffer->aRingBuffer, (S8*)m_TempU8, nLen) < nLen )
	{
		return 0;
	}
*/

	/* 缓存一下起始位置，然后挨个Byte读取数据，最后再还原起始位置 */
	origFromBuf = pBuffer->aRingBuffer.pFromBuf;
	for ( i=0; i<nLen-1 ; i++ )
	{
		Sr_BufGet(&pBuffer->aRingBuffer, (S8*)(&tempOneU8), 1);
		crcCalc ^= tempOneU8;
	}
	Sr_BufGet(&pBuffer->aRingBuffer, (S8*)(&tempOneU8), 1);
	crcRead = tempOneU8;
	pBuffer->aRingBuffer.pFromBuf = origFromBuf;

	if(	( memcmp(pBuffer->sHeader[0].pFrameHead, pData, (U32)(pBuffer->sHeader[0].nFrameHeadLen) ) == 0	) &&	/**< 帧头 */
		( (nLen > pBuffer->sHeader[0].nFrameHeadTailLen ) && (nLen <= pBuffer->sHeader[0].nMaxFrameLen)	) &&	/**< 帧长度 */
		( crcCalc == crcRead																			) &&	/**< CRC校验 */
		( 1/*( nLen > 4 && nLen < 128 )*/	) 
		)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/* IC比较特殊，要根据帧尾校验才知道如果长度不够一帧最小长度，不能轻易删除帧
	1首先确认帧头满足，2其次长度未超过最大帧长度，3且缓冲区实际数据内容也不够这个长度
	此时不要删除字节

	return 0 不可删除
		   1 可删除	
*/
static S32 IsFrameBufIcCanDel(FrameBufferStruct *pBuffer, U8 *pData)
{
	S32 needLen = 0;
	S32 existlen = 0;

	/* 是否符合帧头 */
	if ( memcmp(pBuffer->sHeader[0].pFrameHead, pData, (U32)(pBuffer->sHeader[0].nFrameHeadLen) ) != 0 )
	{
		return 1;
	}

	needLen = FBufferGetFrameLen(pBuffer, (U8*)pData,0);
	if (needLen >= pBuffer->sHeader[0].nMaxFrameLen)
	{
		return 1;
	}

	existlen = Sr_NBytes(&pBuffer->aRingBuffer);
	if (existlen >= needLen)
	{
		return 1;
	}

	return 0;
}

S32 FBufferHasIcFrame(FrameBufferStruct *pBuffer)
{
	S8 pData[MAX_FRAME_HEADTAIL_LEN];
	S8 pTempData[2];

	VERIFY_LOG_RETVAL(
		( pBuffer != NULL	) ,
		0
		);

	/* pBuffer->bHasFrame在POP时清空 */
	if(pBuffer->bHasFrame==1)
	{
		return 1;
	}

	/**
	先检测BUFFER的头是否指向帧头.否则丢弃数据并只到指向帧头.
	判断BUFFER中的数据长度是否足够一帧.是的话,标记已有一帧.	
	*/
	if(Sr_BufGetNoDel(&pBuffer->aRingBuffer,pData,pBuffer->sHeader[0].nFrameHeadTailLen)<pBuffer->sHeader[0].nFrameHeadTailLen)
	{
		return 0;
	}

	while( !IsFrameIcHead(pBuffer,(U8*)pData) )
	{
		/* IC比较特殊，要根据帧尾校验才知道如果长度不够一帧最小长度，不能轻易删除帧 */
		if ( IsFrameBufIcCanDel(pBuffer,(U8*)pData) == 0)
		{
			return 0;
		}

		/* 取不到帧头后，删掉一个字节继续 */
		if(Sr_BufGet(&pBuffer->aRingBuffer,pTempData,1)<1)
		{
			return 0;
		}

		/* 判断删掉之后，够不够判断完整帧头 */
		if(Sr_BufGetNoDel(&pBuffer->aRingBuffer,pData,pBuffer->sHeader[0].nFrameHeadTailLen)<pBuffer->sHeader[0].nFrameHeadTailLen)
		{
			return 0;
		}


	}
	
	pBuffer->nFrameLen = FBufferGetFrameLen(pBuffer, (U8*)pData,0);

	if(pBuffer->nFrameLen <= Sr_NBytes(&pBuffer->aRingBuffer) )
	{
		pBuffer->bHasFrame=1;
		return 1;
	}
	else if(pBuffer->nFrameLen > pBuffer->sHeader[0].nMaxFrameLen)
	{
		Sr_MoveRead(&pBuffer->aRingBuffer,pBuffer->sHeader[0].nFrameHeadTailLen);
		pBuffer->nFrameLen = 0;
		return 0;
	}
	else
	{
		pBuffer->nFrameLen=0;
		return 0;
	}

}

/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/
#ifdef ICE_AUTO_TEST_ENABLE
#include "IceAutoTestC.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "bitfuncs.h"

#define BUF_LEN_TEST 1024

void Bit_SetRandData(U8 *pData, S32 bitLen, U32 index);

void Test_FBufferHasExFrame();
void Test_IsFrameIcHead();
void Test_FBufferHasIcFrame();
void Test_FBufferHasFrame();

/* 随机数生成 */
void Bit_SetRandData(U8 *pData, S32 bitLen, U32 index)
{
	S32 i;
	S32 n,m;	/*n为整倍数,m为余数*/
	U32 aRandNum;
	
	memset(pData, 0, bitLen/8*sizeof(U8));
	srand( (unsigned)time( NULL )+index );
	
	n = bitLen/32;
	m = bitLen%32;
	
	for (i=0;i<n;i++)
	{
		aRandNum = (U32)( ((double)rand()/(double) RAND_MAX) * ((double)(pow(2.0,32))-1.0) );
		Bit_CatUData(pData, i*32, bitLen,aRandNum,32);
	}
	aRandNum = (U32)( ((double)rand()/(double) RAND_MAX) * ((double)(pow(2.0,32))-1.0) );
	Bit_CatUData(pData, i*32, bitLen,aRandNum,m);
}

void Test_SfrmExtract()
{
	Test_FBufferHasExFrame();
	Test_IsFrameIcHead();
	Test_FBufferHasIcFrame();
	Test_FBufferHasFrame();
	
	IceAT_ReportAll();
}


void Test_FBufferHasExFrame()
{
	FrameBufferStruct aBuffer = {0};
	static SStaticRng m_RngTest;
	static S8 m_BufTest[BUF_LEN_TEST*2];
	U8 pHeadRN[2]	= {0x24, 0x00};
	U8 pTailRN[2] = {0x0d, 0x0a};

	FBufferCreate((&aBuffer), BUF_LEN_TEST*2,  m_BufTest);
	FBufferExInit(&aBuffer,pHeadRN,pTailRN,1,2,0,0,3, FALSE, FALSE, BUF_LEN_TEST);

	{
		//U8 pInPut[] = "$CCRIS,*FC\r\n$CCRIS,*FC\r\nret";
		U8 pInPut[] = "$CCMIS,C,1,B3,P,B3,P,B3,P*FC\r\n";
		//U8 pOutPut[128];
		//S32 nLen,i;
		S32 nLen ,nPart,i;
		
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.bHasFrame = 0;
		
 		LOGINFO0("======00======");
		nLen = sizeof(pInPut);
		nPart = 2;
		
		FBufferPush(&aBuffer, pInPut, nPart);
		FBufferHasExFrame(&aBuffer);
		//_TCASSERT(aBuffer.bHasFrame == 1);
		FBufferPush(&aBuffer, pInPut+nPart, nLen - nPart);
		FBufferHasExFrame(&aBuffer);
		_TCASSERT(aBuffer.bHasFrame == 1);

		memset(aBuffer.aRingBuffer.buf, 0, nLen);
		LOGINFO0("======00======");

		LOGINFO0("======11======");
		memset(aBuffer.aRingBuffer.buf, 0, nLen);
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = 0;
		aBuffer.bHasFrame = 0;
		nPart = 1;
		FBufferPush(&aBuffer, pInPut, nPart);
		FBufferHasExFrame(&aBuffer);
		//_TCASSERT(aBuffer.bHasFrame == 1);
		FBufferPush(&aBuffer, pInPut+nPart, nLen - nPart);
		FBufferHasExFrame(&aBuffer);
		_TCASSERT(aBuffer.bHasFrame == 1);
		LOGINFO0("======11======");

		LOGINFO0("======22======");
		memset(aBuffer.aRingBuffer.buf, 0, nLen);
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = 0;
		aBuffer.bHasFrame = 0;
		for (i=0;i<nLen;i++)
		{
			S32 bHasFrame;
			//aBuffer.aRingBuffer.pToBuf = i+1;
			FBufferPush(&aBuffer, pInPut+i, 1);
			bHasFrame = FBufferHasExFrame(&aBuffer);
		}
		
		_TCASSERT(aBuffer.bHasFrame == 1);
		LOGINFO0("======22======");
		
// 		aBuffer.aRingBuffer.pFromBuf = 24;
// 		memcpy(aBuffer.aRingBuffer.buf+24,pInPut,nLen);		
// 		aBuffer.bHasFrame = 0;
// 		for (i=0;i<24;i++)
// 		{
// 			aBuffer.aRingBuffer.pToBuf = 24+i+1;
// 			FBufferHasExFrame(&aBuffer);
// 		}
		
	}

	IceAT_Start("Test_FBufferHasFrame");
	{
		U8 pInPut[] = "ret$CCRIS,*FC\r\n$CCRIS,*FC\r\n";
		S32 nLen = 24;
		aBuffer.bHasFrame = 0;
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = nLen;
		memcpy(aBuffer.aRingBuffer.buf,pInPut,nLen);

		FBufferHasExFrame(&aBuffer);
		_TCASSERT(aBuffer.bHasFrame == 1);
	}
	{
		U8 pInPut[] = {0x00,0x01,0xF0,0x24,0x47,0x45,0x55,0x49,0x00,0x0B,0x00,0x00,0x01,0x3,0x0D,0X0A};
		S32 nLen = 13+3;	/**< 前3字节错误数据 */
		aBuffer.bHasFrame = 0;
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = nLen;
		memcpy(aBuffer.aRingBuffer.buf,pInPut,nLen);

		FBufferHasExFrame(&aBuffer);
		_TCASSERT(aBuffer.bHasFrame == 1);
	}
	IceAT_End_Report();

	{
		U8 pInPut[] = "$CCRIS,*FC\r\n$CCRIS,*FC\r\nret";
		S32 nLen = 24,i;
		memcpy(aBuffer.aRingBuffer.buf,pInPut,nLen);
		
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.bHasFrame = 0;
		for (i=0;i<24;i++)
		{
			S32 bHasFrame;
			aBuffer.aRingBuffer.pToBuf = i+1;
			bHasFrame = FBufferHasExFrame(&aBuffer);
		}
		
		_TCASSERT(aBuffer.bHasFrame == 1);
		
		aBuffer.aRingBuffer.pFromBuf = 24;
		memcpy(aBuffer.aRingBuffer.buf+24,pInPut,nLen);		
		aBuffer.bHasFrame = 0;
		for (i=0;i<24;i++)
		{
			aBuffer.aRingBuffer.pToBuf = 24+i+1;
			FBufferHasExFrame(&aBuffer);
		}
		
	}

	IceAT_Start("Test_PushData");
	{
		U8 pInPut[] = "$CCMIS,C,1,B3,P,B3,P,B3,P*FC\r\n$CCMIS,C,1,B3,P,B3,P,B3,P*FC\r\n";
		S32 nLen = strlen(pInPut);
		S32 i = 0;
		for (i=0; i<1000; i++)
		{
			FBufferPush(&aBuffer, pInPut, nLen);
			FBufferPush(&aBuffer, pInPut, nLen);
		}
		_TCASSERT(i == 1000);
	}
	IceAT_End_Report();
}

#define TEST_MAXLEN_07IC 2048

void Test_IsFrameIcHead()
{
	FrameBufferStruct aBuffer = {0};
	static SStaticRng m_RngTest;
	static S8 m_BufTest[TEST_MAXLEN_07IC*2];
	U8 pHeadIC[2]	= {0x00, 0x00};
	
	FBufferCreate((&aBuffer), TEST_MAXLEN_07IC*2, m_BufTest);
	FBufferInit(&aBuffer,pHeadIC,1,0,0,2,2,TEST_MAXLEN_07IC);

	IceAT_Start("Test_IsFrameIcHead");
 	{
		U8 pData[] = {0x00,0x1F,0xA1,0x00,0x24,0x11,0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 33;
		S32 res = 0;
		U8 pDataHead[2];
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = 33;
		memcpy(aBuffer.aRingBuffer.buf, pData, 33);
		Sr_BufGetNoDel(&aBuffer.aRingBuffer,pDataHead,aBuffer.sHeader[0].nFrameHeadTailLen);
		res = IsFrameIcHead(&aBuffer, pDataHead);
		_TCASSERT(res == 1);
	}
	{
		U8 pData1[] = {0x00,0x1F,0xA1,0x00,0x24,0x11};
		U8 pData2[] = {0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 33;
		S32 dataHead = 6;
		S32 res = 0;
		U8 pDataHead[2];
		aBuffer.aRingBuffer.pFromBuf = aBuffer.aRingBuffer.bufSize - dataHead;
		aBuffer.aRingBuffer.pToBuf = dataLen - dataHead;
		memcpy(aBuffer.aRingBuffer.buf + (aBuffer.aRingBuffer.bufSize - dataHead), pData1, dataHead);
		memcpy(aBuffer.aRingBuffer.buf, pData2, dataLen - dataHead);
		
		Sr_BufGetNoDel(&aBuffer.aRingBuffer,pDataHead,aBuffer.sHeader[0].nFrameHeadTailLen);
		res = IsFrameIcHead(&aBuffer, pDataHead);
		_TCASSERT(res == 1);
	}
	{
		U8 pData1[] = {0x00};
		U8 pData2[] = {0x1F,0xA1,0x00,0x24,0x11,0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 33;
		S32 dataHead = 1;
		S32 res = 0;
		U8 pDataHead[2];
		aBuffer.aRingBuffer.pFromBuf = aBuffer.aRingBuffer.bufSize - dataHead;
		aBuffer.aRingBuffer.pToBuf = dataLen - dataHead;
		memcpy(aBuffer.aRingBuffer.buf + (aBuffer.aRingBuffer.bufSize - dataHead), pData1, dataHead);
		memcpy(aBuffer.aRingBuffer.buf, pData2, dataLen - dataHead);
		
		Sr_BufGetNoDel(&aBuffer.aRingBuffer,pDataHead,aBuffer.sHeader[0].nFrameHeadTailLen);
		res = IsFrameIcHead(&aBuffer, pDataHead);
		_TCASSERT(res == 1);
	}
	{	/* 异常: 帧头不对 */
		U8 pData[] = {0x00,0x00,0x1F,0xA1,0x00,0x24,0x11,0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 33+2;
		S32 res = 0;
		U8 pDataHead[2];
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = dataLen;
		memcpy(aBuffer.aRingBuffer.buf, pData, dataLen);
		Sr_BufGetNoDel(&aBuffer.aRingBuffer,pDataHead,aBuffer.sHeader[0].nFrameHeadTailLen);
		res = IsFrameIcHead(&aBuffer, pDataHead);
		_TCASSERT(res == 0);
	}
	{	/* 异常: 长度错误 */
		U8 pData1[] = {0x00,0x1F,0xA1,0x00,0x24,0x11};
		U8 pData2[] = {/*0x05,*/0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 32;
		S32 dataHead = 6;
		S32 res = 0;
		U8 pDataHead[2];
		aBuffer.aRingBuffer.pFromBuf = aBuffer.aRingBuffer.bufSize - dataHead;
		aBuffer.aRingBuffer.pToBuf = dataLen - dataHead;
		memcpy(aBuffer.aRingBuffer.buf + (aBuffer.aRingBuffer.bufSize - dataHead), pData1, dataHead);
		memcpy(aBuffer.aRingBuffer.buf, pData2, dataLen - dataHead);
		
		Sr_BufGetNoDel(&aBuffer.aRingBuffer,pDataHead,aBuffer.sHeader[0].nFrameHeadTailLen);
		res = IsFrameIcHead(&aBuffer, pDataHead);
		_TCASSERT(res == 0);
	}
	{	/* 异常: CRC错误 */
		U8 pData1[] = {0x00};
		U8 pData2[] = {0x1F,0xA1,0x00,0x24,0x11,0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x17};
		S32 dataLen = 33;
		S32 dataHead = 1;
		S32 res = 0;
		U8 pDataHead[2];
		aBuffer.aRingBuffer.pFromBuf = aBuffer.aRingBuffer.bufSize - dataHead;
		aBuffer.aRingBuffer.pToBuf = dataLen - dataHead;
		memcpy(aBuffer.aRingBuffer.buf + (aBuffer.aRingBuffer.bufSize - dataHead), pData1, dataHead);
		memcpy(aBuffer.aRingBuffer.buf, pData2, dataLen - dataHead);
		
		Sr_BufGetNoDel(&aBuffer.aRingBuffer,pDataHead,aBuffer.sHeader[0].nFrameHeadTailLen);
		res = IsFrameIcHead(&aBuffer, pDataHead);
		_TCASSERT(res == 0);
	}
	IceAT_End_Report();
}
 
void Test_FBufferHasIcFrame()
{
	FrameBufferStruct aBuffer = {0};
	static SStaticRng m_RngTest;
	static S8 m_BufTest[TEST_MAXLEN_07IC*2];
	U8 pHeadIC[2]	= {0x00, 0x00};
	S32 i;

	FBufferCreate((&aBuffer), TEST_MAXLEN_07IC*2,  m_BufTest);
	FBufferInit(&aBuffer,pHeadIC,1,0,0,2,2,TEST_MAXLEN_07IC);
	
	IceAT_Start("Test_FBufferHasIcFrame");
	{
		U8 pData[] = {0x00,0x1F,0xA1,0x00,0x24,0x11,0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 33;
		S32 res = 0;
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = 33;
		aBuffer.bHasFrame = 0;
		aBuffer.nFrameLen = 0;
		memcpy(aBuffer.aRingBuffer.buf, pData, 33);
		res = FBufferHasIcFrame(&aBuffer);
		_TCASSERT(res == 1);
		_TCASSERT(aBuffer.nFrameLen == 33);
		_TCASSERT(aBuffer.bHasFrame == 1);
	}
	{
		U8 pData1[] = {0x00,0x1F,0xA1,0x00,0x24,0x11};
		U8 pData2[] = {0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 33;
		S32 dataHead = 6;
		S32 res = 0;
		aBuffer.aRingBuffer.pFromBuf = aBuffer.aRingBuffer.bufSize - dataHead;
		aBuffer.aRingBuffer.pToBuf = dataLen - dataHead;
		aBuffer.bHasFrame = 0;
		aBuffer.nFrameLen = 0;
		memcpy(aBuffer.aRingBuffer.buf + (aBuffer.aRingBuffer.bufSize - dataHead), pData1, dataHead);
		memcpy(aBuffer.aRingBuffer.buf, pData2, dataLen - dataHead);	
		res = FBufferHasIcFrame(&aBuffer);
		_TCASSERT(res == 1);
		_TCASSERT(aBuffer.nFrameLen == 33);
		_TCASSERT(aBuffer.bHasFrame == 1);
	}
	{
		U8 pData1[] = {0x00};
		U8 pData2[] = {0x1F,0xA1,0x00,0x24,0x11,0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 33;
		S32 dataHead = 1;
		S32 res = 0;
		aBuffer.aRingBuffer.pFromBuf = aBuffer.aRingBuffer.bufSize - dataHead;
		aBuffer.aRingBuffer.pToBuf = dataLen - dataHead;
		aBuffer.bHasFrame = 0;
		aBuffer.nFrameLen = 0;
		memcpy(aBuffer.aRingBuffer.buf + (aBuffer.aRingBuffer.bufSize - dataHead), pData1, dataHead);
		memcpy(aBuffer.aRingBuffer.buf, pData2, dataLen - dataHead);		
		res = FBufferHasIcFrame(&aBuffer);
		_TCASSERT(res == 1);
		_TCASSERT(aBuffer.nFrameLen == 33);
		_TCASSERT(aBuffer.bHasFrame == 1);
	}
	{	/* 异常: 帧头不对,多一字节可以解出 */
		U8 pData[] = {0x00,0x00,0x1F,0xA1,0x00,0x24,0x11,0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 33+1;
		S32 res = 0;
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = dataLen;
		aBuffer.bHasFrame = 0;
		aBuffer.nFrameLen = 0;
		memcpy(aBuffer.aRingBuffer.buf, pData, dataLen);
		res = FBufferHasIcFrame(&aBuffer);
		_TCASSERT(res == 1);
		_TCASSERT(aBuffer.nFrameLen == 33);
		_TCASSERT(aBuffer.bHasFrame == 1);
	}
	{	/* 异常: 长度错误 */
		U8 pData1[] = {0x00,0x1F,0xA1,0x00,0x24,0x11};
		U8 pData2[] = {/*0x05,*/0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x15};
		S32 dataLen = 32;
		S32 dataHead = 6;
		S32 res = 0;
		aBuffer.aRingBuffer.pFromBuf = aBuffer.aRingBuffer.bufSize - dataHead;
		aBuffer.aRingBuffer.pToBuf = dataLen - dataHead;
		aBuffer.bHasFrame = 0;
		aBuffer.nFrameLen = 0;
		memcpy(aBuffer.aRingBuffer.buf + (aBuffer.aRingBuffer.bufSize - dataHead), pData1, dataHead);
		memcpy(aBuffer.aRingBuffer.buf, pData2, dataLen - dataHead);
		res = FBufferHasIcFrame(&aBuffer);
		_TCASSERT(res == 0);
		_TCASSERT(aBuffer.nFrameLen == 0);
		_TCASSERT(aBuffer.bHasFrame == 0);
	}
	{	/* 异常: CRC错误 */
		U8 pData1[] = {0x00};
		U8 pData2[] = {0x1F,0xA1,0x00,0x24,0x11,0x05,0xDC,0x03,0x00,0x00,0x3C,0x5C,0x07,0x7B,0x7D,0x88,0xD0,0x75,0x89,0x3A,0x2B,0x39,0xC7,0x92,0x23,0x54,0x4C,0xC5,0xF7,0xB4,0x41,0x17};
		S32 dataLen = 33;
		S32 dataHead = 1;
		S32 res = 0;
		aBuffer.aRingBuffer.pFromBuf = aBuffer.aRingBuffer.bufSize - dataHead;
		aBuffer.aRingBuffer.pToBuf = dataLen - dataHead;
		aBuffer.bHasFrame = 0;
		aBuffer.nFrameLen = 0;
		memcpy(aBuffer.aRingBuffer.buf + (aBuffer.aRingBuffer.bufSize - dataHead), pData1, dataHead);
		memcpy(aBuffer.aRingBuffer.buf, pData2, dataLen - dataHead);
		res = FBufferHasIcFrame(&aBuffer);
		_TCASSERT(res == 0);
		_TCASSERT(aBuffer.nFrameLen == 0);
		_TCASSERT(aBuffer.bHasFrame == 0);
	}
	for ( i=0; i<100; i++ )
	{
		/* 随机数生成测试: 强大 */
		U8 pData[1024] = {0};
		U32 byteLen = 0;
		
		/* 生成1024字节随机数填入缓冲 */
		Bit_SetRandData(pData, 1024, i*100);
		LOG(1, "%02X%02X%02X%02X%02X%02X%02X", pData[0],pData[1],pData[2],pData[3],pData[4],pData[5],pData[6] );
		memcpy(aBuffer.aRingBuffer.buf,pData, 1024);
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = 1024;
		aBuffer.bHasFrame = 0;
		aBuffer.nFrameLen = 0;

		while(FBufferHasIcFrame(&aBuffer))
		{
			if( (byteLen = FBufferPop(&aBuffer, pData, 2048)) == 0 )
			{
				LOGINFO0("IC Serial Ring buffer overload!");
				return;
			}
			/*VERIFY_RSLT_RETSELF(ProcICFrame(pData, byteLen, pUserStatus));*/
			LOGINFO2("ProcIC pFrameBuf [%d]->[%d]", 
				(&aBuffer)->aRingBuffer.pFromBuf,
				(&aBuffer)->aRingBuffer.pToBuf);
		}
	}
	IceAT_End_Report();
}
/*#include "PtlNav.h"*/

void Test_FBufferHasFrame()
{
#if 1
	FrameBufferStruct aBuffer = {0};
	static SStaticRng m_RngTest;
	static S8 m_BufTest[TEST_MAXLEN_07IC*2];
	U8 pHeadPrm[2]	= {0x08, 0x00};
	U8 pTailPrm[1] = {0x77};
//	SPrmNavMsg aSPrmNavMsg = {0};

	FBufferCreate((&aBuffer), TEST_MAXLEN_07IC*2,  m_BufTest);
	FBufferExInit(&aBuffer,pHeadPrm,pTailPrm,2,1,2,1,5, FALSE,FALSE,TEST_MAXLEN_07IC);

	IceAT_Start("Test_FBufferHasFrame");
	{
		U8 pData[] = {0x08,0x00,0x02,0x05,0x00,0x0F,0x77,0x08,0x00,0x0B,0x06,0x01,0x01,0x01,0x01,0x03,0x37,0x98,0x00,0x68,0x00,0xC1,0x77,0x08,0x00,0x47,0x06,0x01,0x01,0x09,0x01,0x00,0x68,0x03,0x37,0x98,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x77 };
		U8 pBuf[1024] = {0};
		S32 dataLen = sizeof(pData);
		S32 byteLen;
		S32 res = 0;
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.aRingBuffer.pToBuf = 33;
		aBuffer.bHasFrame = 0;
		aBuffer.nFrameLen = 0;
		memcpy(aBuffer.aRingBuffer.buf, pData, 33);
		while(FBufferHasExFrame(&aBuffer))
		{
			if( (byteLen = FBufferPop(&aBuffer, pBuf, 2048)) == 0 )
			{
				LOGINFO0("IC Serial Ring buffer overload!");
				return;
			}
			LOGBYTE01("DATA: ", pBuf, byteLen);
			/*VERIFY_RSLT_RETSELF(ProcICFrame(pData, byteLen, pUserStatus));*/
			LOGINFO2("ProcPRM pFrameBuf [%d]->[%d]", 
				(&aBuffer)->aRingBuffer.pFromBuf,
				(&aBuffer)->aRingBuffer.pToBuf);
		}

		aBuffer.aRingBuffer.pToBuf = 33 + 33;
		memcpy(aBuffer.aRingBuffer.buf+33, pData+33, 33);
		while(FBufferHasExFrame(&aBuffer))
		{
			if( (byteLen = FBufferPop(&aBuffer, pBuf, 2048)) == 0 )
			{
				LOGINFO0("IC Serial Ring buffer overload!");
				return;
			}
			LOGBYTE01("DATA: ", pBuf, byteLen);
			/*VERIFY_RSLT_RETSELF(ProcICFrame(pData, byteLen, pUserStatus));*/
			LOGINFO2("ProcPRM pFrameBuf [%d]->[%d]", 
				(&aBuffer)->aRingBuffer.pFromBuf,
				(&aBuffer)->aRingBuffer.pToBuf);
		}

		aBuffer.aRingBuffer.pToBuf = 33 + 33 + 33;
		memcpy(aBuffer.aRingBuffer.buf+33+33, pData+33+33, 33);
		while(FBufferHasExFrame(&aBuffer))
		{
			if( (byteLen = FBufferPop(&aBuffer, pBuf, 2048)) == 0 )
			{
				LOGINFO0("IC Serial Ring buffer overload!");
				return;
			}

			LOGBYTE01("DATA: ", pBuf, byteLen);
			/*VERIFY_RSLT_RETSELF(ProcICFrame(pData, byteLen, pUserStatus));*/
			LOGINFO2("ProcPRM pFrameBuf [%d]->[%d]", 
				(&aBuffer)->aRingBuffer.pFromBuf,
				(&aBuffer)->aRingBuffer.pToBuf);
		}
		
		_TCASSERT(1 == 1);
	}
	{
		U8 pData[] = {0x08,0x00,0x44,0x06,0x06,0x03,0x04,0x06,0x00,0x54,0x60,0x00,0x0A,0x36,0xBD,0xEA,0x00,0x06,0xCE,0x0E,0xB4,0x00,0x27,0x1F,0xD8,0x1F,0x00,0x01,0x69,0x40,0x00,0x03,0x40,0x98,0x40,0x00,0x01,0xD1,0x2D,0x00,0x4E,0x80,0x66,0xEE,0x00,0x5E,0xEF,0x00,0xB1,0x99,0x00,0x0C,0x08,0x00,0xCA,0xEA,0x0B,0xD7,0x40,0x00,0x14,0xF2,0x00,0x0A,0x40,0x64,0x9C,0x00,0x77};
		U8 pData2[] = {0x08,0x00,0x02,0x05,0x00,0x0F,0x77};
		U8 pBuf[1024] = {0};
		S32 dataLen = sizeof(pData);
		S32 byteLen;
		S32 res = 0;
		aBuffer.aRingBuffer.pFromBuf = 0;
		aBuffer.bHasFrame = 0;
		aBuffer.nFrameLen = 0;
		aBuffer.aRingBuffer.pToBuf = dataLen;
		memcpy(aBuffer.aRingBuffer.buf, pData, dataLen);
		while(FBufferHasExFrame(&aBuffer))
		{
			if( (byteLen = FBufferPop(&aBuffer, pBuf, 2048)) == 0 )
			{
				LOGINFO0("IC Serial Ring buffer overload!");
				return;
			}
			LOGBYTE01("DATA: ", pBuf, byteLen);
			/*VERIFY_RSLT_RETSELF(ProcICFrame(pData, byteLen, pUserStatus));*/
			LOGINFO2("ProcPRM pFrameBuf [%d]->[%d]", 
				(&aBuffer)->aRingBuffer.pFromBuf,
				(&aBuffer)->aRingBuffer.pToBuf);
		}

		aBuffer.aRingBuffer.pToBuf = dataLen+7;;
		memcpy(aBuffer.aRingBuffer.buf+7, pData2, 7);
		while(FBufferHasExFrame(&aBuffer))
		{
			if( (byteLen = FBufferPop(&aBuffer, pBuf, 2048)) == 0 )
			{
				LOGINFO0("IC Serial Ring buffer overload!");
				return;
			}
			LOGBYTE01("DATA: ", pBuf, byteLen);
			/*VERIFY_RSLT_RETSELF(ProcICFrame(pData, byteLen, pUserStatus));*/
			LOGINFO2("ProcPRM pFrameBuf [%d]->[%d]", 
				(&aBuffer)->aRingBuffer.pFromBuf,
				(&aBuffer)->aRingBuffer.pToBuf);
		}
		

	}
#endif /**< 0 */
}

#define TEST_MAXLEN_SR	4096

void Test_FBufferHasFrame_PRM()
{
	FrameBufferStruct aBuffer = {0};
	static SStaticRng m_RngTest;
	static S8 m_BufTest[TEST_MAXLEN_07IC*2];
	U8 pData[] ={0x24, 0x54, 0x4F, 0x50, 0x4D, 0x00, 0x96, 0x00, 0x00, 0x02, 0x08, 0x00, 0x86, 0x12, 0x80, 0xD4, 0x80, 0x00, 0x4A, 0xFD, 0x72, 0xD3, 0x20, 0xDE, 0xE8, 0x6A, 0xAE, 0x76, 0x95, 0x46, 0xB3, 0xDD, 0x40, 0xE0, 0x55, 0xED, 0x6F, 0x7A, 0x5A, 0xA5, 0x34, 0xA7, 0x9A, 0xCB, 0x49, 0x0D, 0xB4, 0xD5, 0x5F, 0x6A, 0x01, 0x0A, 0xB4, 0xD5, 0xC6, 0xE5, 0xE4, 0x47, 0xAE, 0xBA, 0x01, 0x59, 0x77, 0x9B, 0xE5, 0x4F, 0xEB, 0x7D, 0xC7, 0x56, 0x12, 0x97, 0x0D, 0x0A, 0xA6, 0x0B, 0x61, 0x3B, 0xC7, 0xC8, 0x07, 0xF1, 0xF0, 0xAF, 0x6E, 0xAD, 0x2A, 0xF5, 0x05, 0xF2, 0x7F, 0xC8, 0x4F, 0x04, 0xB2, 0x35, 0x3A, 0xF0, 0x2D, 0x49, 0xAD, 0xCC, 0x3B, 0x76, 0x60, 0x40, 0x77, 0x39, 0xF0, 0x46, 0x85, 0xFA, 0x33, 0x2D, 0xFF, 0x34, 0xC9, 0xE4, 0xB2, 0xD5, 0xF3, 0x75, 0x0B, 0xE8, 0xA8, 0xD0, 0xCC, 0x71, 0xE6, 0xCA, 0x03, 0x98, 0x47, 0x65, 0x3D, 0x0A, 0xD4, 0x52, 0xF7, 0x9D, 0x1D, 0xB4, 0x55, 0x33, 0x8A, 0x27, 0x0B, 0xFE, 0x77, 0xC1};
	U8 pHeadPrm[]	= {'$','T','O','P','M'};
	U8 pTailPrm[1] = {0};
	U32 i = 0; 
	U32 nPushNum = 0;
	U8 pOutData[2048] = {0};
	U32 nByteLen = 0;
	U32 nPopNum = 0;

	FBufferCreate((&aBuffer), TEST_MAXLEN_SR*2,  m_BufTest);
	FBufferExInit(&aBuffer,pHeadPrm,pTailPrm,5,0,5,2,8, TRUE,FALSE,TEST_MAXLEN_SR);
	IceAT_Start("Test_FBufferHasFrame");
	{
		for (i = 0; i < 10000; i++)
		{
			U32 nRoll = rand() % 100;
			//LOGINFO2("Roll[%08d] = %d", i, nRoll);
			if (nRoll >= 40)
			{
				FBufferPush(&aBuffer, pData, sizeof(pData));
				nPushNum++;
			}
			else
			{
				while(FBufferHasExFrame(&aBuffer))
				{
					nByteLen = FBufferPop(&aBuffer, pOutData, 2048);
					if (nByteLen != 0)
					{
						nPopNum++;
						if(memcmp(pData, pOutData, nByteLen) != 0)
						{
							LOGINFO1("Err Org = %d, pop = %d", nByteLen);
						}
					}
				}
				_TCASSERT(nPushNum == nPopNum);
				if(nPushNum != nPopNum)
				{
					LOG(1, "Push Num = %d , Pop = %d", nPushNum, nPopNum);
				}
			}
		}
	}
	IceAT_End_Report();
	
}
#endif /* #ifdef ICE_AUTO_TEST_ENABLE */
/************************************************************************/
/**********以上为测试代码************************************************/
/************************************************************************/

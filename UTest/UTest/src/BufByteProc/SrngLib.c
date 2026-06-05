
#include "SrngLib.h"
#include <string.h>

#define BYTE_COPY(src,dest,len)		memcpy((dest), (src), (len))
#define MIN(a,b)					(((a) < (b)) ? (a) : (b))

/*
 * The following macros are designed to do various operations on
 * the SStaticRng object.  By using them, users can avoid having to know
 * the structure of SStaticRng.  However they are intended to be very
 * efficient and this is why they are macros in the first place.
 * In general the parameters to them should be register variables
 * for maximum efficiency.
 */

/*******************************************************************************
*
* RNG_ELEM_GET - get one character from a ring buffer
*
* This macro gets a single character from the specified ring buffer.
* Must supply temporary variable (register S32) 'fromP'.
*
* RETURNS: 1 if there was a S8 in the buffer to return, 0 otherwise
*
* NOMANUAL
*/

#define RNG_ELEM_GET(ringId, pCh, fromP)		\
    (						\
    fromP = (ringId)->pFromBuf,			\
    ((ringId)->pToBuf == fromP) ?		\
	0 					\
    :						\
	(					\
	*pCh = (ringId)->buf[fromP],		\
	(ringId)->pFromBuf = ((++fromP == (ringId)->bufSize) ? 0 : fromP), \
	1					\
	)					\
    )

/*******************************************************************************
*
* STATIC_RNG_ELEM_PUT - put one character into a ring buffer
*
* This macro puts a single character into the specified ring buffer.
* Must supply temporary variable (register S32) 'toP'.
*
* RETURNS: 1 if there was room in the buffer for the S8, 0 otherwise
*
* NOMANUAL
*/

#define STATIC_RNG_ELEM_PUT(ringId, ch, toP)		\
    (						\
    toP = (ringId)->pToBuf,			\
    (toP == (ringId)->pFromBuf - 1) ?		\
	0 					\
    :						\
	(					\
	(toP == (ringId)->bufSize - 1) ?	\
	(					\
	((ringId)->pFromBuf == 0) ?		\
	0				\
	:					\
	(				\
	(ringId)->buf[toP] = ch,	\
	(ringId)->pToBuf = 0,		\
	1				\
	)				\
	)					\
	:					\
	(					\
	(ringId)->buf[toP] = ch,		\
	(ringId)->pToBuf++,			\
	1					\
	)					\
	)					\
)

/*******************************************************************************
*
* Sr_Create - create an empty ring buffer
*
* This routine creates a ring buffer of size <nbytes>, and initializes
* it.  Memory for the buffer is allocated from the system memory partition.
*
* RETURNS
* The ID of the ring buffer, or NULL if memory cannot be allocated.
*/
BOOL Sr_Create( S32 nbytes,SStaticRngId pStaticRING,S8 *pStaticBuffer )
{
	if ( pStaticRING == NULL || pStaticBuffer == NULL )
	{
		return FALSE;
	}

	pStaticRING->bufSize	= nbytes + 1;			/**< 缓冲区"大小"要比实际空间大一字节 */
	pStaticRING->buf	    = pStaticBuffer;

	Sr_Flush (pStaticRING);

	return TRUE;
}

/*******************************************************************************
*
* Sr_Flush - make a ring buffer empty
*
* This routine initializes a specified ring buffer to be empty.
* Any data currently in the buffer will be lost.
*
* RETURNS: N/A
*/

void Sr_Flush(SStaticRngId ringId)         /* ring buffer to initialize */
{
	ringId->pToBuf   = 0;
	ringId->pFromBuf = 0;
}

/*******************************************************************************
*
* Sr_BufGet - get characters from a ring buffer
*
* This routine copies bytes from the ring buffer <rngId> into <buffer>.
* It copies as many bytes as are available in the ring, up to <maxbytes>.
* The bytes copied will be removed from the ring.
*
* RETURNS:
* The number of bytes actually received from the ring buffer;
* it may be zero if the ring buffer is empty at the time of the call.
*/

S32 Sr_BufGet( SStaticRngId rngId, S8 *buffer, S32 maxbytes )
{
	S32 bytesgot = 0;
	S32 pToBuf = rngId->pToBuf;
	S32 bytes2;
	S32 pRngTmp = 0;

	if (pToBuf >= rngId->pFromBuf)
	{
		/* pToBuf has not wrapped around */

		bytesgot = MIN (maxbytes, pToBuf - rngId->pFromBuf);
		BYTE_COPY (&rngId->buf [rngId->pFromBuf], buffer, bytesgot);
		rngId->pFromBuf += bytesgot;
	}
	else
	{
		/* pToBuf has wrapped around.  Grab chars up to the end of the
		* buffer, then wrap around if we need to. */

		bytesgot = MIN (maxbytes, rngId->bufSize - rngId->pFromBuf);
		BYTE_COPY (&rngId->buf [rngId->pFromBuf], buffer, bytesgot);
		pRngTmp = rngId->pFromBuf + bytesgot;

		/* If pFromBuf is equal to bufSize, we've read the entire buffer,
		* and need to wrap now.  If bytesgot < maxbytes, copy some more chars
		* in now. */

		if (pRngTmp == rngId->bufSize)
		{
			if (bytesgot == 2)
			{
				/*处理帧尾分布在缓冲区头部和尾部的情况*/
				bytes2 = MIN(maxbytes - (bytesgot - 1), pToBuf);
				BYTE_COPY(rngId->buf, buffer + (bytesgot - 1), bytes2);
				rngId->pFromBuf = bytes2;
			}
			else
			{
				/*经调试，当帧数据分布在缓冲区尾部和头部时，不需要buffer原来的内容*/
				bytes2 = MIN(maxbytes, pToBuf);
				BYTE_COPY(rngId->buf, buffer, bytes2);
				rngId->pFromBuf = bytes2;
				bytesgot = bytes2;
			}
		}
		else
			rngId->pFromBuf = pRngTmp;
	}
	return (bytesgot);
}
/*******************************************************************************
*
* Sr_BufPut - put bytes into a ring buffer
*
* This routine puts bytes from <buffer> into ring buffer <ringId>.  The
* specified number of bytes will be put into the ring, up to the number of
* bytes available in the ring.
*
* INTERNAL
* Always leaves at least one byte empty between pToBuf and pFromBuf, to
* eliminate ambiguities which could otherwise occur when the two pointers
* are equal.
*
* RETURNS:
* The number of bytes actually put into the ring buffer;
* it may be less than number requested, even zero,
* if there is insufficient room in the ring buffer at the time of the call.
*/


S32 Sr_BufPut( SStaticRngId rngId, const S8 *buffer, S32 nbytes )
{
	S32 bytesput = 0;
	S32 pFromBuf = rngId->pFromBuf;
	S32 bytes2;
	S32 pRngTmp = 0;
	
	if (nbytes > rngId->bufSize || nbytes < 0)
	{
		return 0;
	}

// 	if (nbytes <= (rngId->bufSize - rngId->pToBuf - 1))
// 	{
// 		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], nbytes);
// 		rngId->pToBuf += nbytes; /**< fromBuf不变 */
// 	}
// 	else
// 	{
// 		S32 n1 = (rngId->bufSize - rngId->pToBuf - 1);
// 		S32 n2 = nbytes - n1;
// 		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], n1);
// 		BYTE_COPY (&buffer[n1], &rngId->buf[0], n2);
// 		rngId->pToBuf = n2;
// 
// 		if (rngId->pToBuf > rngId->pFromBuf )
// 		{
// 			rngId->pFromBuf = rngId->pToBuf;
// 		}
// 	}

// 	if (pFromBuf > rngId->pToBuf)
// 	{
// 		/* pFromBuf is ahead of pToBuf.  We can fill up to two bytes
// 		* before it */
// 
// 		bytesput = MIN ( nbytes, ( pFromBuf - rngId->pToBuf ) - 1 );
// 		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], bytesput);
// 		rngId->pToBuf += bytesput;
// 	}
// 	else if (pFromBuf == 0)
// 	{
// 		/* pFromBuf is at the beginning of the buffer.  We can fill till
// 		* the next-to-last element */
// 
// 		bytesput = MIN ( nbytes, ( rngId->bufSize - rngId->pToBuf ) - 1 );
// 		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], bytesput);
// 		rngId->pToBuf += bytesput;
// 	}
// 	else
// 	{
// 		/* pFromBuf has wrapped around, and its not 0, so we can fill
// 		* at least to the end of the ring buffer.  Do so, then see if
// 		* we need to wrap and put more at the beginning of the buffer. */
// 
// 		bytesput = MIN (nbytes, rngId->bufSize - rngId->pToBuf);
// 		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], bytesput);
// 		pRngTmp = rngId->pToBuf + bytesput;
// 
// 		if (pRngTmp == rngId->bufSize)
// 		{
// 			/* We need to wrap, and perhaps put some more chars */
// 
// 			bytes2 = MIN (nbytes - bytesput, pFromBuf - 1);
// 			BYTE_COPY (buffer + bytesput, rngId->buf, bytes2);/**< @todo */
// 			rngId->pToBuf = bytes2;
// 			bytesput += bytes2;
// 		}
// 		else
// 			rngId->pToBuf = pRngTmp;
// 	}


	if (pFromBuf > rngId->pToBuf)
	{
		/* pFromBuf is ahead of pToBuf.  We can fill up to two bytes
		* before it */

		bytesput = MIN ( nbytes, ( pFromBuf - rngId->pToBuf ) - 1 );
		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], bytesput);
		rngId->pToBuf += bytesput;
	}
	else if (pFromBuf == 0)
	{
		/* pFromBuf is at the beginning of the buffer.  We can fill till
		* the next-to-last element */

		bytesput = MIN ( nbytes, ( rngId->bufSize - rngId->pToBuf ) - 1 );
		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], bytesput);
		rngId->pToBuf += bytesput;
	}
	else
	{
		/* pFromBuf has wrapped around, and its not 0, so we can fill
		* at least to the end of the ring buffer.  Do so, then see if
		* we need to wrap and put more at the beginning of the buffer. */

		bytesput = MIN (nbytes, rngId->bufSize - rngId->pToBuf);
		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], bytesput);
		pRngTmp = rngId->pToBuf + bytesput;

		if (pRngTmp == rngId->bufSize)
		{
			/* We need to wrap, and perhaps put some more chars */

			bytes2 = MIN (nbytes - bytesput, pFromBuf - 1);
			BYTE_COPY (buffer + bytesput, rngId->buf, bytes2);/**< @todo */
			rngId->pToBuf = bytes2;
			bytesput += bytes2;
		}
		else
			rngId->pToBuf = pRngTmp;
	}
	return (bytesput);
}

S32 Sr_BufPutForce( SStaticRngId rngId, const S8 *buffer, S32 nbytes )
{
	S32 pFromBuf = rngId->pFromBuf;
	S32 pRngTmp = 0;
	S32 nowDataLen = 0;

	if (nbytes > rngId->bufSize || nbytes < 0)
	{
		return 0;
	}
	/* Copy Data To Buf */
	if (nbytes <= (rngId->bufSize - rngId->pToBuf - 1))
	{
		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], nbytes);
		rngId->pToBuf += nbytes; /**< fromBuf不变 */
	}
	else
	{
		S32 n1 = (rngId->bufSize - rngId->pToBuf - 1);
		S32 n2 = nbytes - n1;
		BYTE_COPY (buffer, &rngId->buf [rngId->pToBuf], n1);
		BYTE_COPY (&buffer[n1], &rngId->buf[0], n2);
		rngId->pToBuf = n2;
	}

	/* Fresh new FromBuf */
	if (rngId->pToBuf >= rngId->pFromBuf)
	{
		nowDataLen = rngId->pToBuf - rngId->pFromBuf;
	}
	else
	{
		nowDataLen = rngId->bufSize + (rngId->pToBuf - rngId->pFromBuf);
	}

	if (nbytes + nowDataLen >= rngId->bufSize)
	{
		rngId->pFromBuf = rngId->pToBuf + 1;
	}

	return (nbytes);
}

/*******************************************************************************
*
* Sr_IsEmpty - test if a ring buffer is empty
*
* This routine determines if a specified ring buffer is empty.
*
* RETURNS:
* TRUE if empty, FALSE if not.
*/

S32 Sr_IsEmpty( const SStaticRngId ringId )
{
	return (ringId->pToBuf == ringId->pFromBuf);
/*lint -save -e818 : (Info -- Pointer parameter 'ringId' (line 192) could be declared as pointing to const) */
} 
/*lint -restore */

/*******************************************************************************
*
* Sr_IsFull - test if a ring buffer is full (no more room)
*
* This routine determines if a specified ring buffer is completely full.
*
* RETURNS:
* TRUE if full, FALSE if not.
*/

S32 Sr_IsFull( const SStaticRngId ringId )
{
	S32 n = ( ringId->pToBuf - ringId->pFromBuf ) + 1;

	return ((n == 0) || (n == ringId->bufSize));
/*lint -save -e818 : (Info -- Pointer parameter 'ringId' (line 192) could be declared as pointing to const) */
} 
/*lint -restore */

/*******************************************************************************
*
* Sr_FreeBytes - determine the number of free bytes in a ring buffer
*
* This routine determines the number of bytes currently unused in a specified
* ring buffer.
*
* RETURNS: The number of unused bytes in the ring buffer.
*/

S32 Sr_FreeBytes( const SStaticRngId ringId )
{
	S32 n = ( (S32)ringId->pFromBuf - (S32)ringId->pToBuf ) - 1;

	if ( n < 0 )
	{
		n += (S32)(ringId->bufSize);
	}

	return (n);
/*lint -save -e818 : (Info -- Pointer parameter 'ringId' (line 192) could be declared as pointing to const) */
} 
/*lint -restore */

/*******************************************************************************
*
* Sr_NBytes - determine the number of bytes in a ring buffer
*
* This routine determines the number of bytes currently in a specified
* ring buffer.
*
* RETURNS: The number of bytes filled in the ring buffer.
*/

S32 Sr_NBytes( const SStaticRngId ringId )
{
	S32 n = ringId->pToBuf - ringId->pFromBuf;

	if (n < 0)
	{
		n += (ringId->bufSize - 1);
	}

	return (n);
/*lint -save -e818 : (Info -- Pointer parameter 'ringId' (line 192) could be declared as pointing to const) */
} 
/*lint -restore */


S32 Sr_BufGetNoDel( SStaticRngId rngId, S8 *buffer, S32 maxbytes )
{
	S32 retval;
	S32 temp=rngId->pFromBuf;
	retval=Sr_BufGet(rngId, buffer,  maxbytes);
	rngId->pFromBuf=temp;
	return retval;
}

void Sr_MoveRead( SStaticRngId ringId, S32 n )
{
	n += ringId->pFromBuf;

	if (n >= ringId->bufSize - 1)
	{
		n -= (ringId->bufSize - 1);
	}

	ringId->pFromBuf = n;
}

typedef struct
{
	U8 satelLite;  //卫星号
	U8 sateStatus; //卫星状态
	U8 chanStatus;  //通道状态
	S8 SateElevation;        //卫星1仰角
	U16 SateAngle;           //卫星1方位角
	U8 SignalToNosiesRatiop; //信噪比1
	int Doppler;             //多普勒1
	U16 code_dco_phase;
	S32 code_phase;
	U16 bitCdtion;

}SSigSatInfoTemp;
#define  MAX_SAT_NUM	12
typedef struct
{
	U32 TicCount;	    	// TicCount
	U8  timValdSym;	     	// 时间有效标志
	U16 timeYear;           //UTC年
	U8  timYearMinute[4];	// 4维代表月日时分
	F64 timeSecnd;          //UTC秒
	U8 locRslt;             //BD定位状态
	F32 locRsltX_Y_Z[3];    //BD定位X_Y_Z结果
	F32 locRsltVX_VY_VZ[3];        //BD定位结果VX_VY_YZ
	U16 locRsltPDOP_HDOP_VDOP[3];  //BD定位结果PDOP
	F32 locRslt_ClockMinus;        //BD定位结果钟差
	F32 locRslt_ClockFloat;         //BD定位结果钟飘
	U8 chanNum;  //通道数
	U8 EditNum;  //软件版本号
	U32 resever48Bite[12];       //预留48个字节 
	U16 inferRight;          //校验和

	SSigSatInfoTemp  stSatInfo[MAX_SAT_NUM];

}SDzNavInfoTemp;


/*构建该函数用作字符串协议的校对,return -1为错误数据，return 0正常
*/
// S32 editFuction(const SDzNavInfoTemp StcomTest)
// {
// 	S32 i;
// 	U32 head[12] = StcomTest.resever48Bite;
// 	S32 sum = 0;
// 	for (i = 0;i < 12;i++)
// 	{
// 		sum = sum + head[i];
// 	}
// 	if (sum != 0)
// 	{
// 		return -1;   
// 	}
// 	else
// 	{
// 		return 0;
// 	}
//}

/* 缓冲区从From开始，读取backBytes以后的tailLen个字节
若能读取，返回BOOL，若不能，返回FALSE

相当于先移动，再读几个字节数
*/
S32 Sr_MoveGetByte(const SStaticRngId rngId, const S32 backBytes, const S32 tailLen, S8* pTailData)
{
	S32 temp=rngId->pFromBuf;
	S32 res = 0;	/**< 最终返回长度值 */
	S32 canUseBuf = 0;

	/* 确定能够移动 */
	canUseBuf = Sr_NBytes(rngId);
	if (canUseBuf < backBytes)
	{
		return 0;
	}

	/* 取出短数据 */
	Sr_MoveRead( rngId, backBytes-tailLen );
	res = Sr_BufGetNoDel( rngId, pTailData, tailLen );

	rngId->pFromBuf=temp;

	return res;
}
/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/
#ifdef ICE_AUTO_TEST_ENABLE
#include "IceAutoTestC.h"
#include <STDLIB.H>
#include <MATH.H>

void Test_Sr_BufPut();
void Test_Sr_MoveGetByte();

void Test_SrngLib()
{
	Test_Sr_BufPut();
	Test_Sr_MoveGetByte();

	IceAT_ReportAll();
}

void Test_Sr_MoveGetByte()
{
	/* 三类正常情况 */
	{
		S8 m_Buf06Ps[2048*2] = {0};
		SStaticRng aSStaticRng = {0};
		S32 backBytes = 12;
		S32 tailLen = 2;
		S8 pTailData[2] = {0};
		S32 res = 0;
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1200;
		aSStaticRng.pToBuf = 1230;
		aSStaticRng.buf[1210] = 0xAA;
		aSStaticRng.buf[1211] = 0xBB;

		res = Sr_MoveGetByte( &aSStaticRng, backBytes, tailLen, pTailData);
		_TCASSERT(res==2);
		_TCASSERT(pTailData[0] == 0xAA);
		_TCASSERT(pTailData[1] == 0xBB);
	}
	{
		S8 m_Buf06Ps[2048*2] = {0};
		SStaticRng aSStaticRng = {0};
		S32 backBytes = 12;
		S32 tailLen = 2;
		S8 pTailData[2] = {0};
		S32 res = 0;
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 4095-10;
		aSStaticRng.pToBuf = 10;
		aSStaticRng.buf[4095] = 0xAA;
		aSStaticRng.buf[4096] = 0xBB;

		res = Sr_MoveGetByte( &aSStaticRng, backBytes, tailLen, pTailData);
		_TCASSERT(res==2);
		_TCASSERT(pTailData[0] == 0xAA);
		_TCASSERT(pTailData[1] == 0xBB);
	}
	{
		S8 m_Buf06Ps[2048*2] = {0};
		SStaticRng aSStaticRng = {0};
		S32 backBytes = 12;
		S32 tailLen = 2;
		S8 pTailData[2] = {0};
		S32 res = 0;
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 4096-10;
		aSStaticRng.pToBuf = 10;
		aSStaticRng.buf[4096] = 0xAA;
		aSStaticRng.buf[0] = 0xBB;

		res = Sr_MoveGetByte( &aSStaticRng, backBytes, tailLen, pTailData);
		_TCASSERT(res==2);
		_TCASSERT(pTailData[0] == 0xAA);
		_TCASSERT(pTailData[1] == 0xBB);
	}
	/* 异常情况 移动超出缓冲区有效数据长度 */
	{
		S8 m_Buf06Ps[2048*2] = {0};
		SStaticRng aSStaticRng = {0};
		S32 backBytes = 102;
		S32 tailLen = 2;
		S8 pTailData[2] = {0};
		S32 res = 0;
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1200;
		aSStaticRng.pToBuf = 1230;
		aSStaticRng.buf[1210] = 0xAA;
		aSStaticRng.buf[1211] = 0xBB;

		res = Sr_MoveGetByte( &aSStaticRng, backBytes, tailLen, pTailData);
		_TCASSERT(res==0);
	}
	/* 异常情况 跨边界，移动超出缓冲区有效数据长度 */
	{
		S8 m_Buf06Ps[2048*2] = {0};
		SStaticRng aSStaticRng = {0};
		S32 backBytes = 102;
		S32 tailLen = 2;
		S8 pTailData[2] = {0};
		S32 res = 0;
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 4095;
		aSStaticRng.pToBuf = 11;
		aSStaticRng.buf[1210] = 0xAA;
		aSStaticRng.buf[1211] = 0xBB;

		res = Sr_MoveGetByte( &aSStaticRng, backBytes, tailLen, pTailData);
		_TCASSERT(res==0);
	}
	/* 异常情况 移动可以满足，但移动后无法取数据 */
	{
		S8 m_Buf06Ps[2048*2] = {0};
		SStaticRng aSStaticRng = {0};
		S32 backBytes = 15;
		S32 tailLen = 2;
		S8 pTailData[2] = {0};
		S32 res = 0;
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 4095;
		aSStaticRng.pToBuf = 11;
		aSStaticRng.buf[1210] = 0xAA;
		aSStaticRng.buf[1211] = 0xBB;

		res = Sr_MoveGetByte( &aSStaticRng, backBytes, tailLen, pTailData);
		_TCASSERT(res==0);
	}
}

void Test_Sr_BufPut()
{
	IceAT_Start("Test_Sr_BufPut");
	/* 1 正常不越界 */
	{
		S8 pData[33];
		U32 nbytes = 33;
		U32 res,res2;

		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		SStaticRng aSStaticRng2 = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1268;
		aSStaticRng.pToBuf = 1271;
		Sr_Create(2048*2, &aSStaticRng2, m_Buf06Ps);
		aSStaticRng2.pFromBuf = 1268;
		aSStaticRng2.pToBuf = 1271;

		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		res2 = Sr_BufPutForce( &aSStaticRng2, pData, nbytes);
		_TCASSERT(res==2);
	}
	/* 2 越界 */
	{
		S8 pData[10240];
		U32 nbytes = 3000;
		U32 res,res2;

		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		SStaticRng aSStaticRng2 = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1268;
		aSStaticRng.pToBuf = 1271;
		Sr_Create(2048*2, &aSStaticRng2, m_Buf06Ps);
		aSStaticRng2.pFromBuf = 1268;
		aSStaticRng2.pToBuf = 1271;

		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		res2 = Sr_BufPutForce( &aSStaticRng2, pData, nbytes);
		_TCASSERT(res==2);
	}
	/* 3 覆盖但不越界 */
	{
		S8 pData[10240];
		U32 nbytes = 30;
		U32 res,res2;

		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		SStaticRng aSStaticRng2 = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1271;
		aSStaticRng.pToBuf = 1268;
		Sr_Create(2048*2, &aSStaticRng2, m_Buf06Ps);
		aSStaticRng2.pFromBuf = 1271;
		aSStaticRng2.pToBuf = 1268;

		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		res = Sr_BufGet( &aSStaticRng, pData, 5000 );
		res2 = Sr_BufPutForce( &aSStaticRng2, pData, nbytes);
		res2 = Sr_BufGet( &aSStaticRng2, pData, 5000 );
		_TCASSERT(res==2);
	}
	/* 4 覆盖并且越界 */
	{
		S8 pData[10240];
		U32 nbytes = 3000;
		U32 res,res2;

		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		SStaticRng aSStaticRng2 = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1271;
		aSStaticRng.pToBuf = 1268;
		Sr_Create(2048*2, &aSStaticRng2, m_Buf06Ps);
		aSStaticRng2.pFromBuf = 1271;
		aSStaticRng2.pToBuf = 1268;

		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		res = Sr_BufGet( &aSStaticRng, pData, 5000 );
		res2 = Sr_BufPutForce( &aSStaticRng2, pData, nbytes);
		res2 = Sr_BufGet( &aSStaticRng2, pData, 5000 );
		_TCASSERT(res==2);
	}
	/* 5 覆盖但不越界（边界值pFromBuf=4097最大，to=4096） */
	{
		S8 pData[10240];
		U32 nbytes = 4096-1268;
		U32 res,res2;

		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		SStaticRng aSStaticRng2 = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1271;
		aSStaticRng.pToBuf = 1268;
		Sr_Create(2048*2, &aSStaticRng2, m_Buf06Ps);
		aSStaticRng2.pFromBuf = 1271;
		aSStaticRng2.pToBuf = 1268;

		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		res = Sr_BufGet( &aSStaticRng, pData, 5000 );
		res2 = Sr_BufPutForce( &aSStaticRng2, pData, nbytes);
		res2 = Sr_BufGet( &aSStaticRng2, pData, 5000 );
		_TCASSERT(res==2);
	}
	/* 6 覆盖但不越界（边界值pFromBuf=1最小，to=0） */
	{
		S8 pData[10240];
		U32 nbytes = 4096-1268+1;
		U32 res,res2;

		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		SStaticRng aSStaticRng2 = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1271;
		aSStaticRng.pToBuf = 1268;
		Sr_Create(2048*2, &aSStaticRng2, m_Buf06Ps);
		aSStaticRng2.pFromBuf = 1271;
		aSStaticRng2.pToBuf = 1268;

		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		res = Sr_BufGet( &aSStaticRng, pData, 5000 );
		res2 = Sr_BufPutForce( &aSStaticRng2, pData, nbytes);
		res2 = Sr_BufGet( &aSStaticRng2, pData, 5000 );
		_TCASSERT(res==2);
	}
	/* 7 覆盖但不越界（边界值pFromBuf=2最小，to=1） */
	{
		S8 pData[10240];
		U32 nbytes = 4096-1268+2;
		U32 res,res2;

		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		SStaticRng aSStaticRng2 = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1271;
		aSStaticRng.pToBuf = 1268;
		Sr_Create(2048*2, &aSStaticRng2, m_Buf06Ps);
		aSStaticRng2.pFromBuf = 1271;
		aSStaticRng2.pToBuf = 1268;

		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		res = Sr_BufGet( &aSStaticRng, pData, 5000 );
		res2 = Sr_BufPutForce( &aSStaticRng2, pData, nbytes);
		res2 = Sr_BufGet( &aSStaticRng2, pData, 5000 );
		_TCASSERT(res==2);
	}
	//////////////////////////////////////////////////////////////////////////
	{
		S8 pData[33];
		U32 nbytes = 33;
		U32 res;
		
		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 1271;
		aSStaticRng.pToBuf = 1268;
		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		_TCASSERT(res==2);
	}
	{
		S8 pData[33];
		U32 nbytes = 1;
		U32 res;
		
		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 4090;
		aSStaticRng.pToBuf = 4096;
		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		_TCASSERT(res==1);
	}
	{
		S8 pData[33];
		U32 nbytes = 1;
		U32 res;
		
		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 4096;
		aSStaticRng.pToBuf = 4097;
		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		_TCASSERT(res==1);
	}
	{
		S8 pData[33];
		U32 nbytes = 2;
		U32 res;
		
		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		aSStaticRng.pFromBuf = 4090;
		aSStaticRng.pToBuf = 4096;
		res = Sr_BufPut( &aSStaticRng, pData, nbytes );
		_TCASSERT(res==2);
	}
	{
	S8 pData[33];
	U32 nbytes = 2;
	U32 res;
	
	  S8 m_Buf06Ps[2048*2];
	  SStaticRng aSStaticRng = {0};
	  Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
	  aSStaticRng.pFromBuf = 0;
	  aSStaticRng.pToBuf = 0;
	  res = Sr_BufPut( &aSStaticRng, pData, 4097 );
	  _TCASSERT(res==4096);
	}

	{
		/* 缓冲区无限数据测试(包含异常数据) */
		S8 pData[40960];
		S32 res;
		S32 i;
		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);

		for (i=0;i<100;i++)
		{
			S32 aRandNum = (S32)( ((double)rand()/(double) RAND_MAX) * (/*2048*/(double)(pow(2.0,32))-1.0) );
			res = Sr_BufPut( &aSStaticRng, pData, aRandNum );
			LOGINFO2("toBuf[%d], push[%d]", aSStaticRng.pToBuf, res);
		}
	}

	{
		/* 缓冲区无限数据测试(每帧清除缓冲区) */
		S8 pData[40960];
		S32 res;
		S32 i;
		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		
		for (i=0;i<10000;i++)
		{
			S32 aRandNum = (S32)( ((double)rand()/(double) RAND_MAX) * (2048 ));
			aSStaticRng.pFromBuf = 0;
			aSStaticRng.pToBuf = 0;
			res = Sr_BufPut( &aSStaticRng, pData, aRandNum );
			_TCASSERT(res==aRandNum);
		}
		
	}
	{
		/* 缓冲区无限数据测试(不清除缓冲区，故意堵死) */
		S8 pData[40960];
		S32 res;
		S32 i;
		S8 m_Buf06Ps[2048*2];
		SStaticRng aSStaticRng = {0};
		Sr_Create(2048*2, &aSStaticRng, m_Buf06Ps);
		
		for (i=0;i<100;i++)
		{
			S32 aRandNum = (S32)( ((double)rand()/(double) RAND_MAX) * (2048 ));
			res = Sr_BufPut( &aSStaticRng, pData, aRandNum );
			LOGINFO2("toBuf[%d], push[%d]", aSStaticRng.pToBuf, res);
		}
		
	}

	IceAT_End_Report();
}

#endif /* #ifdef ICE_AUTO_TEST_ENABLE */
/************************************************************************/
/**********以上为测试代码************************************************/
/************************************************************************/

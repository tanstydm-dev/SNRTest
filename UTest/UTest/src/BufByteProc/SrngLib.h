
#ifndef STATIC_RNGBUFFER_H_20100428_WP_ENJOY_
#define STATIC_RNGBUFFER_H_20100428_WP_ENJOY_

#ifdef __cplusplus
extern "C" {
#endif

#include "icetype.h"

#ifdef UNINAV_LIB_ENABLE
	#define UNINAV_LIB	__declspec(dllexport)
#else 
	#define UNINAV_LIB
#endif

/* typedefs */
typedef struct		/* SStaticRng - ring buffer */
{
	S32 pToBuf;		/* offset from start of buffer where to write next */
	S32 pFromBuf;	/* offset from start of buffer where to read next */
	S32 bufSize;	/* size of ring in bytes */
	S8 *buf;		/* pointer to start of buffer */
} SStaticRng;

typedef SStaticRng *SStaticRngId;

/**缓冲区是否满
*/
UNINAV_LIB S32 Sr_IsFull (const SStaticRngId ringId);

/**创建一个环形缓冲区
*/
UNINAV_LIB BOOL Sr_Create (S32 nbytes,SStaticRngId pStaticRING,S8 *pStaticBuffer);

/**取出环形缓冲区的数据
@return The number of bytes actually received from the ring buffer;
*/
UNINAV_LIB S32 Sr_BufGet (SStaticRngId rngId, S8 *buffer, S32 maxbytes);

/**向环形缓冲区添加数据
@return The number of bytes actually put into the ring buffer;
*/
UNINAV_LIB S32 Sr_BufPut (SStaticRngId rngId, const S8 *buffer, S32 nbytes);


/**向环形缓冲区添加数据（无视已有数据，只保证最新数据，用于各类发送缓冲区）
@return The number of bytes actually put into the ring buffer;
*/
UNINAV_LIB S32 Sr_BufPutForce( SStaticRngId rngId, const S8 *buffer, S32 nbytes );

/**复制出环形缓冲区的数据，但不删除
@return The number of bytes actually received from the ring buffer;
*/
UNINAV_LIB S32 Sr_BufGetNoDel (SStaticRngId rngId, S8 *buffer, S32 maxbytes);

UNINAV_LIB S32 Sr_IsEmpty( const SStaticRngId ringId);      /* ring buffer to test */

/**缓冲区空闲的字节数
*/
UNINAV_LIB S32 Sr_FreeBytes (const SStaticRngId ringId);

/**缓冲区已使用的字节数
*/
UNINAV_LIB S32 Sr_NBytes (const SStaticRngId ringId);

/**清空缓冲区
*/
UNINAV_LIB void Sr_Flush (SStaticRngId ringId);

/**read指针向后移n个字节
*/
UNINAV_LIB void Sr_MoveRead (SStaticRngId ringId, S32 n);

UNINAV_LIB S32 Sr_MoveGetByte(const SStaticRngId rngId, const S32 backBytes, const S32 tailLen, S8* pTailData);

/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/
#ifdef ICE_AUTO_TEST_ENABLE
 
void Test_SrngLib();

#endif /* #ifdef ICE_AUTO_TEST_ENABLE */
/************************************************************************/
/**********以上为测试代码************************************************/
/************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* STATIC_RNGBUFFER_H_20100428_WP_ENJOY_ */

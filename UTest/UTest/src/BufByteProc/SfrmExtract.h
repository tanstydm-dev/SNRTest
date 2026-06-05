

#ifndef _STATIC_FRAME_EXTRACT_H_20100428_WP_ENJOY_
#define _STATIC_FRAME_EXTRACT_H_20100428_WP_ENJOY_

#ifdef UNINAV_LIB_ENABLE
#define UNINAV_LIB	__declspec(dllexport)
#else 
#define UNINAV_LIB
#endif

#ifdef __cplusplus
extern "C"{
#endif

#include "Srnglib.h"
#ifdef UNINAV_LIB_ENABLE
//#pragma comment(lib, "srnglib.lib")
#endif
#define MAX_FRAME_HEAD_LEN	4	// 13 2B
#define MAX_HEADER_NUM	7
#define MAX_FRAME_HEAD_TAIL_LEN	 4
/**帧环形缓冲区结构
*/
typedef struct
{
	S32 nMaxFrameLen;		/**<当前帧格式中最大帧长度*/

	U8	pFrameHead[MAX_FRAME_HEAD_LEN];		/**<当前帧格式帧头SOH*/
	U8 nFrameHeadLen;		/**<当前帧格式帧头的长度*/

	U8	pFrameTail[MAX_FRAME_HEAD_LEN];	/**<当前帧格式帧尾*/
	U8 nFrameTailLen;		/**<当前帧格式帧头的长度*/

	U8 nDataLenPos;			/**<当前帧格式中数据长度的位置*/
	U8 nDataLenBytes;		/**<当前帧格式中帧长度占用的字节数*/

	U8 nFrameHeadTailLen;	/**<当前帧格式中头与尾的长度，数据长度字段包含在帧头内，例如长度为5，实际剩下2字节不算在其中，则此值为2*/
	
	BOOL bLsb;	/** 小端模式 */
	BOOL bIsFrameLen;

}FrameBufferHeader;

typedef struct {
	SStaticRng aRingBuffer;	/**< 环形缓冲区 */
	S32 bHasFrame;			/**<是否已经有了一帧 */
	S32 nFrameLen;			/**<当前帧长 */
// 	S32 nMaxFrameLen;		/**<当前帧格式中最大帧长度*/
// 	U8	pFrameHead[MAX_FRAME_HEAD_LEN];		/**<当前帧格式帧头SOH*/
// 	U8	pFrameTail[MAX_FRAME_HEAD_LEN];	/**<当前帧格式帧尾*/
// 	S32 nFrameHeadLen;		/**<当前帧格式帧头的长度*/
// 	S32 nDataLenPos;		/**<当前帧格式中数据长度的位置*/
// 	S32 nFrameHeadTailLen;	/**<当前帧格式中头与尾的长度，数据长度字段包含在帧头内*/
// 	U32 nDataLenBytes;		/**<当前帧格式中帧长度占用的字节数*/

	U32 nHeaderNum;
	FrameBufferHeader sHeader[MAX_HEADER_NUM];
} FrameBufferStruct;



typedef struct {
	SStaticRng aRingBuffer;	/**< 环形缓冲区 */
	S32 bHasFrame;			/**<是否已经有了一帧 */
	S32 nFrameLen;			/**<当前帧长 */
	U32 nHeaderNum;
	FrameBufferHeader sHeader[MAX_HEADER_NUM];
} FrameBufferStructExt;

/** 把接收到的字节数据压入缓冲区pBuffer.
@param pBuffer 帧环形缓冲区
@param pData 待插入的数组指针头
@param dataByteLen 插入数据的字节长度
@return 实际压入多少个字节
*/
UNINAV_LIB S32 FBufferPush(FrameBufferStruct *pBuffer,const U8 * pData, S32 dataByteLen);
UNINAV_LIB S32 FBufferPushForce( FrameBufferStruct *pBuffer,const U8 * pData, S32 dataByteLen );

/** 检测缓冲区pBuffer中是否有一帧数据
@param pBuffer 帧环形缓冲区
@retval 0 无
@retval 1 有
*/
UNINAV_LIB S32 FBufferHasFrame(FrameBufferStruct *pBuffer);

/** 从缓冲区pBuffer中取出一帧数据
@param pBuffer 帧环形缓冲区
@param pData 取出数据存入的地址.pData==0时,表示只删除环形缓冲区中的东东
@param maxByteLen 取出的数据帧最大长度
@return 取出数据的字节长度 >0正常，=0错误
*/
UNINAV_LIB S32 FBufferPop(FrameBufferStruct * pBuffer, U8 * pData, S32 maxByteLen);


UNINAV_LIB S32 FBufferPopBareData( FrameBufferStruct * pBuffer, U8 * pData, S32 maxByteLen );

/**用于创建缓冲区
@param pBuffer 需要创建的缓冲区指针
@param bufferSize 缓冲区大小
@return ERR_NO_ERROR ERR_MEM_NONE
*/
UNINAV_LIB BOOL FBufferCreate(FrameBufferStruct * pBuffer, S32 bufferSize, S8 *pStaticBuffer);

/*清空缓冲区*/
BOOL FBufferClear(FrameBufferStruct* pBuffer, S32 bufferSize, S8* pStaticBuffer);

/** 初始化缓冲区，设置帧格式等
@param pBuffer				缓冲区指针
@param pFrameHead			当前帧格式帧头
@param frameHeadLen			帧格式帧头的长度
@param dataLenIndex			帧格式中数据长度的位置
@param frameLenDelta		帧格式中帧总长与数据长度的差别
@param nFrameHeadTailLen	帧格式中头与尾的长度，即帧总长度与帧格式中数据长度之差
@param maxFrameLen			帧格式中最大帧长度
@return						ERR_BAD_ARGS ERR_NO_ERROR
*/
UNINAV_LIB BOOL FBufferInit( FrameBufferStruct * pBuffer, U8 * pFrameHead, S32 frameHeadLen, S32 frameTailLen, S32 dataLenIndex, S32 dataLenBytes, S32 nFrameHeadTailLen, S32 maxFrameLen );

UNINAV_LIB BOOL FBufferExInit( FrameBufferStruct * pBuffer, U8 * pFrameHead, U8 *pFameTail, S32 frameHeadLen, S32 frameTailLen, S32 dataLenIndex, S32 dataLenBytes, S32 nFrameHeadTailLen, BOOL bIsFramLen, BOOL bLSB, S32 maxFrameLen);

UNINAV_LIB S32 FBufferPopBytes(FrameBufferStruct * pBuffer, U8 * pData, S32 dataByteLen);

UNINAV_LIB S32 FBufferHasData(FrameBufferStruct *pBuffer, S32 byteLen);

UNINAV_LIB S32 FBufferHasExFrame(FrameBufferStruct *pBuffer);

UNINAV_LIB S32 FBufferHasIcFrame(FrameBufferStruct *pBuffer);

/*UNINAV_LIB S32 IsRdssCrcAdap(const SStaticRngId rngId);*/

/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/
#ifdef ICE_AUTO_TEST_ENABLE

void Test_SfrmExtract(); 
void Test_FBufferHasFrame_PRM();

#endif /* #ifdef ICE_AUTO_TEST_ENABLE */
/************************************************************************/
/**********以上为测试代码************************************************/
/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /*#ifdef _STATIC_FRAME_EXTRACT_H_20100428_WP_ENJOY_*/

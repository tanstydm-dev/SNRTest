
#ifndef _PTLNEMA0183IN_H_2019_12_30_
#define _PTLNEMA0183IN_H_2019_12_30_

#ifndef PI
#define		PI					3.1415926535897931
#endif
#include "PtlNEMA0183Out.h"


#ifdef __cplusplus
extern "C" {
#endif
#ifndef MAX_SAT_INDEX
#define MAX_SAT_INDEX	76		/**< 最大卫星索引, 考虑和GPS公用 BD:1~14 GPS:15~47 GLO:48~76*/
#endif

	S32 GetSysId(U8* pData);
	S32 GetGSV(U8* pData, SGSVFrm* psGSV);
	S32 GetGGA(U8* pData, SGGA* psGGA);
	S32 GetDHV(U8* pData, SDHV* psDHV);
	S32 GetZDA(U8* pData, SZDA* psZDA);
	S32 GetGSA(U8* pData, SGSA* psGSA);
	S32 GetCOV(U8* pData, SCOV* psCOV);
	S32 GetCNO(U8* pData, SCNO* psCNO);

/*	U32 GetPRO(U8* pData, )*/


/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/
#ifdef ICE_AUTO_TEST_ENABLE

void Test_PtlNEMA0183In();

#endif

#ifdef __cplusplus
}
#endif

#endif
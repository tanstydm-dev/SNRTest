#pragma once
/*	字符和十六进制的相互转换 */

#include "icetype.h"
#include <afxext.h>


/*	十六进制转字符串显示函数 */
S32 ICEByte2HexStr(const unsigned char* pSrc, char* pDst, S32 nSrcLength);

S32 HexCStr2ByteBas(unsigned char* pDst, const CString& pSrc, int nSrcLength);

/*	字符串转十六进制函数 */
S32 HexCStr2Byte(unsigned char* pDst, const CString& pSrc, S32 nSrcLength);
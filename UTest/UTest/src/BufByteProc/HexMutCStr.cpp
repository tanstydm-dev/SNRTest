#include "HexMutCStr.h"



/*	十六进制转字符串显示函数 */
S32 ICEByte2HexStr(const unsigned char* pSrc, char* pDst, S32 nSrcLength)
{
	S32 i;
	for (i = 0; i < nSrcLength; i++)
	{
		sprintf(pDst + i * 3, "%02X ", pSrc[i]);
	}

	return nSrcLength * 3;
}


/*	字符串转十六进制函数 */
S32 HexCStr2ByteBas(unsigned char* pDst, const CString& pSrc, int nSrcLength)
{
	int i = 0;
	int j;
	char* stops;


	for (j = 0; (j < nSrcLength / 2) && (i < nSrcLength); )
	{
		char s[3] = "  ";
		while (s[0] == ' ' && i < nSrcLength)
		{
			s[0] = (char)pSrc.GetAt(i);
			i++;
		}
		while (s[1] == ' ' && i < nSrcLength)
		{
			s[1] = (char)pSrc.GetAt(i);
			i++;
		}
		if (s[0] != ' ' && s[1] != ' ')
		{
			s[2] = 0x0;
			pDst[j] = (unsigned char)strtoul(s, &stops, 16);
			j++;
		}
	}
	return j;
}

/*	字符串转十六进制函数 */
S32 HexCStr2Byte(unsigned char* pDst, const CString& pSrc, S32 nSrcLength)
{
	S32 i;
	char* stops, s[3];

	for (i = 0; i < nSrcLength / 3; i++)
	{
		s[0] = (char)pSrc.GetAt(i * 3);
		s[1] = (char)pSrc.GetAt(i * 3 + 1);
		s[2] = 0x0;
		pDst[i] = (unsigned char)strtoul(s, &stops, 16);
	}

	return (nSrcLength / 3);
}






#include "PtlNEMA0183In.h"
#include "AscFuncs.h"
#include <math.h>
#include <stdlib.h>

U8 u8Bd[3] = { 0x24, 0x42 ,0x44 };
U8 u8Gp[3] = { 0x24, 0x47 ,0x50 };
U8 u8Gl[3] = { 0x24, 0x47 ,0x4C };
U8 u8All[3] = { 0x24, 0x47 ,0x4E };

S32 GetSysId(U8* pData)
{
	if (pData == NULL)
	{
		return ERR_BAD_ARGS;
	}

	S32 s32SysId = 0;

	if (memcmp(pData, u8Bd, 3) == 0)
	{
		s32SysId = eSysBd;
	}
	else if(memcmp(pData, u8Gp, 3) == 0)
	{
		s32SysId = eSysGp;
	}
	else if (memcmp(pData, u8Gl, 3) == 0)
	{
		s32SysId = eSysGl;
	}
	else if (memcmp(pData, u8All, 3) == 0)
	{
		s32SysId = eSysGn;
	}
	else
	{
		return ERR_BAD_ARGS;
	}
	return s32SysId;
}


U32 GetDHV(U8* pData, SDHV* psDHV)
{
	F64 nTemp;
	S8_CHAR sTemp[20];

	if ((pData == NULL)                                  /* 判断是否为有效数据 */
		|| (psDHV == NULL)
		|| (psDHV->pSTime == NULL)
		)
	{
		return ERR_BAD_ARGS;
	}

	/* 初始化数据 */
	ASC_GET_START(pData);

	ASC_GET_STR(sTemp);              /* UTC时间 */
	nTemp = atof(sTemp);
	psDHV->pSTime->nHour = (U32)(nTemp / 10000);            /* hour */
	psDHV->pSTime->nMin = (U32)(nTemp - psDHV->pSTime->nHour * 10000) / 100;      /* min */
	psDHV->pSTime->nSec = nTemp - (F64)(psDHV->pSTime->nHour) * 10000.0 - (F64)(psDHV->pSTime->nMin) * 100.0;  /* sec */

	ASC_GET_F64(&psDHV->speed);                                                        /*速度*/
	ASC_GET_F64(&psDHV->xSpeed);
	ASC_GET_F64(&psDHV->ySpeed);
	ASC_GET_F64(&psDHV->zSpeed);
	ASC_GET_F64(&psDHV->eSpeed);
	ASC_GET_F64(&psDHV->maxSpeed);
	ASC_GET_F64(&psDHV->avgSpeed);
	ASC_GET_F64(&psDHV->wholeSpeed);
	ASC_GET_F64(&psDHV->validSpeed);
	ASC_GET_CHR(&psDHV->speedUnit);
	ASC_GET_END;

	return ERR_NO_ERROR;


}

U32 GetGGA(U8* pData, SGGA* psGGA)
{
	F64 nTemp;
	S8_CHAR sTemp[20];
	S8_CHAR cTemp;

	if (	(pData == NULL			)                               /* 判断是否为有效数据 */
		||	(psGGA == NULL			)
		||	(psGGA->pSPoint == NULL	)
		||	(psGGA->pSTime == NULL	)
		)                                 
	{
		return ERR_BAD_ARGS;
	}

	/* 初始化数据 */
	ASC_GET_START(pData);

	ASC_GET_STR(sTemp);              /* UTC时间 */
	nTemp = atof(sTemp);
	psGGA->pSTime->nHour = (U32)(nTemp / 10000);            /* hour */
	psGGA->pSTime->nMin = (U32)(nTemp - psGGA->pSTime->nHour * 10000) / 100;      /* min */
	psGGA->pSTime->nSec = nTemp - (F64)(psGGA->pSTime->nHour) * 10000.0 - (F64)(psGGA->pSTime->nMin) * 100.0;  /* sec */


	ASC_GET_STR(sTemp);                        /* 经度 */
	nTemp = atof(sTemp);
	psGGA->pSPoint->nLatDegree = (U32)(nTemp / 100);                      /* 度 */
	psGGA->pSPoint->nLatMin = nTemp - psGGA->pSPoint->nLatDegree * 100.0;  /* 分 */
	ASC_GET_CHR(&psGGA->pSPoint->sLatDir);                                /* 方向 */


	ASC_GET_STR(sTemp);                                                          /* 经度 */
	nTemp = atof(sTemp);
	psGGA->pSPoint->nLonDegree = (U32)nTemp / 100;								/* 度 */
	psGGA->pSPoint->nLonMin = nTemp - psGGA->pSPoint->nLonDegree * 100.0;       /* 分 */
	ASC_GET_CHR(&psGGA->pSPoint->sLonDir);										/* 方向 */

	ASC_GET_S32( &(psGGA->nState) );

	if (psGGA->nState == 0)          /* 判断 状态指示 是否有效  */
	{
		return ERR_BAD_ARGS;        /* 无效 返回异常 */
	}
	else
	{
		ASC_GET_U32(&psGGA->nSvNum);                                                     /* 卫星数 */
		ASC_GET_F64(&psGGA->dHDOP);
		ASC_GET_F64(&psGGA->pSPoint->nHeight);                                             /* 高程 */
		ASC_GET_CHR(&psGGA->pSPoint->sHightUnit);										 /* 单位 */
		ASC_GET_F64(&psGGA->pSPoint->nHeightDif);                                        /* 高程异常 */
		ASC_GET_CHR(&cTemp);
		ASC_GET_U32(&psGGA->dDiffDataLife);                                              /* 差分延迟 */
		ASC_GET_U32(&psGGA->nDiffStaID);                                                 /* 基站号 */
		ASC_GET_F64(&psGGA->dVDOP);
	}
	ASC_GET_END;

	return ERR_NO_ERROR;
}


U32 GetGSV(U8* pData, SGSVFrm* psGSV)
{
	U32 cTmp = 0;
	U32 index = 0;
	U32 i = 0;

	if ((pData == NULL)                 /* 判断数据是否有效 */
		||(psGSV->pAzimuth == NULL)
		||(psGSV->pSatId == NULL) 
		||(psGSV->pElevation == NULL)
		||(psGSV->pfSatCnr == NULL))
	{
		return ERR_BAD_ARGS;
	}

	ASC_GET_START(pData);
	ASC_GET_U32(&psGSV->nTotalNum);
	ASC_GET_U32(&psGSV->nIndex);
	ASC_GET_U32(&psGSV->s32RcvdSatNum);
	for (i = 0; (i < 4) && (i < psGSV->s32RcvdSatNum - 4*(psGSV->nIndex-1)); i++)   /* 判断每条语句的卫星数 */
	{
		ASC_GET_U32(&index);
		if ((index > MAX_PROC_SAT_NUM) || 
			(index == 0))
		{
			return ERR_BAD_ARGS;
		}
		psGSV->pSatId[index-1] = index;

		ASC_GET_F64(&psGSV->pElevation[index-1]);                          /* 卫星仰角 */
		ASC_GET_F64(&psGSV->pAzimuth[index - 1]);                            /* 卫星方位角 */
		ASC_GET_F64(&psGSV->pfSatCnr[index - 1]);                                /* 信噪比 */
	}

	ASC_GET_END;

	return ERR_NO_ERROR;
}

U32 GetZDA(U8* pData, SZDA* psZDA)
{
	U32 u32revise = 0;
	S8_CHAR sTemp[20];
	F64 nTemp;

	if ((pData == NULL)                 /* 判断数据是否有效 */
		|| (psZDA->psTime == NULL)
		|| (psZDA->psDate == NULL))
	{
		return ERR_BAD_ARGS;
	}

	ASC_GET_START(pData);
	ASC_GET_U32(&psZDA->u32zdaMode);
	ASC_GET_STR(sTemp);              /* UTC时间 */
	nTemp = atof(sTemp);
	psZDA->psTime->nHour = (U32)(nTemp / 10000);            /* hour */
	psZDA->psTime->nMin = (U32)(nTemp - psZDA->psTime->nHour * 10000) / 100;      /* min */
	psZDA->psTime->nSec = nTemp - (F64)(psZDA->psTime->nHour) * 10000.0 - (F64)(psZDA->psTime->nMin) * 100.0;  /* sec */
	ASC_GET_U32(&psZDA->psDate->u32Day);
	ASC_GET_U32(&psZDA->psDate->u32Month);
	ASC_GET_U32(&psZDA->psDate->u32Year);
	ASC_GET_S32(&psZDA->s32TimeZone);
	ASC_GET_S32(&psZDA->s32ZoneMin);
	ASC_GET_S32(&u32revise);
	ASC_GET_S32(&u32revise);
	ASC_GET_U32(&u32revise);
	ASC_GET_END;

	return ERR_NO_ERROR;
}
// $GNZDA, 2, 061524.00, 04, 09, 2023, -08, 02, 00, 00, 00, Y * 12

S32 GetGSA(U8* pData, SGSA* psGSA)
{
	U32 pu32SatNum = 0;

	if ((pData == NULL)
		||(psGSA == NULL)
		||(psGSA->pu32PRNCode == NULL))
	{
		return ERR_BAD_ARGS;
	}

	psGSA->eType = GetSysId(pData);
	ASC_GET_START(pData);
	ASC_GET_CHR(&psGSA->s8Mode);
	ASC_GET_U32(&psGSA->u32Mode);
	ASC_GET_U32(&pu32SatNum);
	for (S32 i = 0;i < pu32SatNum;i++)
	{
		ASC_GET_U32(psGSA->pu32PRNCode+i);
	}
	ASC_GET_F64(&psGSA->f64Pdop);
	ASC_GET_F64(&psGSA->f64Hdop);
	ASC_GET_F64(&psGSA->f64Vdop);
	if (psGSA->eType == eSysBd)
	{
		ASC_GET_F64(&psGSA->f64Tdop);
	}
	ASC_GET_END;

	return ERR_NO_ERROR;
}

S32 GetCOV(U8* pData, SCOV* psCOV)
{
	U32 u32Temp = 0;

	if ((pData == NULL)
		|| (psCOV == NULL))
	{
		return ERR_BAD_ARGS;
	}

	ASC_GET_START(pData);
	ASC_GET_U32(&u32Temp);
	ASC_GET_U32(&psCOV->u32Sec);
	ASC_GET_U32(&u32Temp);
	ASC_GET_END;

	return ERR_NO_ERROR;
}

S32 GetCNO(U8* pData, SCNO* psCNO)
{	
	U32 pu32SatId = 0;
	U8 pu8SatFrq[5] = { 0 };
	U8 pu8FrqB1I[3] = {"B1I"};
	U8 pu8FrqB1C[3] = { "B1C" };
	U8 pu8FrqB2a[3] = { "B2a" };
	U8 pu8FrqB3I[3] = {"B3I"};
	U8 pu8FrqL1[4] = {"L1"};
	U8 pu8FrqGL[3] = {"GL"};

	if ((pData == NULL)
		|| (psCNO == NULL))
	{
		return ERR_BAD_ARGS;
	}

	psCNO->eType = GetSysId(pData);
	ASC_GET_START(pData);
	ASC_GET_STR(pu8SatFrq);
	ASC_GET_U32(&psCNO->u32SatNum);
	for (S32 i = 0;i < psCNO->u32SatNum;i++)
	{
		ASC_GET_U32(&pu32SatId);
		ASC_GET_F64(&psCNO->pfSatCnr[pu32SatId - 1]);
		psCNO->pu32SatId[pu32SatId - 1] = pu32SatId;
	}
	ASC_GET_END;

	if (memcmp(pu8SatFrq, pu8FrqB1I,3) == 0)
	{
		psCNO->eFrqId = eFrqB1I;
	}
	if (memcmp(pu8SatFrq, pu8FrqB1C, 3) == 0)
	{
		psCNO->eFrqId = eFrqB1C;
	}
	if (memcmp(pu8SatFrq, pu8FrqB2a, 3) == 0)
	{
		psCNO->eFrqId = eFrqB2a;
	}
	if (memcmp(pu8SatFrq, pu8FrqB3I,3) == 0)
	{
		psCNO->eFrqId = eFrqB3I;
	}
	if (memcmp(pu8SatFrq, pu8FrqGL,3) == 0)
	{
		psCNO->eFrqId = eFrqG1;
	}
	if (memcmp(pu8SatFrq, pu8FrqL1,4) == 0)
	{
		psCNO->eFrqId = eFrqL1;
	}

	return ERR_NO_ERROR;
}

/*
static s32 DisposeGetInfoOutCNOSatellite(SCNOSatellite* pCNOSatellite, u8* pData)
{
	u32 u32ByteLen = 0;
	s32 s32CopynTotalNum;
	u8 i;
	u32 u32DataLen = 0;
	f64 f64Tmp;
	VERIFY_RETVAL(pCNOSatellite != DX_NULL, 0);
	VERIFY_RETVAL(pData != DX_NULL, 0);

	ASC_SET_START((pCNOSatellite->u8SatNum) > 0 && pData != DX_NULL && pCNOSatellite != DX_NULL);
	ASC_SET_STR("$BD");
	ASC_SET_STR_COMMA("CNO");
	ASC_SET_STR_COMMA(GetFrqStrById(pCNOSatellite->u8FrqID));

	ASC_SET_U8_COMMA(pCNOSatellite->u8SatNum);  / **< 语句序号 * /

	for (i = 0; i < pCNOSatellite->u8SatNum; i++)
	{
		ASC_SET_U8_COMMA(pCNOSatellite->u8PUsedSatId[i]);
		f64Tmp = ((f64)pCNOSatellite->u16SNR[i]) / 10.0;
		if (i == pCNOSatellite->u8SatNum - 1)
		{
			ASC_SET_F64_1DIGS(f64Tmp);	/ **< 10*log10(nCnr)* /
		}
		else
		{
			ASC_SET_F64_1DIGS_COMMA(f64Tmp);	/ **< 10*log10(nCnr)* /
		}
	}

	u32DataLen = ASC_SET_END(pData);                                 //GSV  
	return u32DataLen;

}
*/
// $BDCNO, B1I, 18, 1, 45.3, 2, 41.8, 3, 46.9, 4, 41.1, 5, 38.3, 59, 47.6, 60, 45.5, 16, 48.6, 8, 46.7, 9, 44.0, 13, 49.1, 19, 48.9, 35, 44.8, 22, 41.2, 29, 44.9, 36, 42.6, 38, 49.9, 39, 49.6 * 4F
/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/

#ifdef ICE_AUTO_TEST_ENABLE
#include "IceAutoTestC.h"

//void Test_GetGGA();
void Test_GetGSV();
//void Test_GetGGA();

void Test_PtlNEMA0183In(void)
{

	
	Test_GetGSV();
//	Test_GetGGA();

	IceAT_ReportAll();
}

// void Test_GetGGA()
// {
// 	IceAT_Start("Test_GetGGA_NORMAL");
// 	{
// 		S8 pData[10240] = { 0 };
// 		//S8 pDateRes[] ={"$BDGGA,122340.124000,0000.000000,,0000.000000,,1,23,123.000,0.000,M,0.000,M,234.210,8,12.230*56\r\n"};
// 		//S8 pDateRes[] ={"$BDGGA,122340.124000,0000.000000,.,00000.000000,.,1,23,123.000,0.000,M,0.000,M,0234,8,12.230*4B\r\n"};
// 		S8 pDateRes[] = { "$BDGGA,122340.124000,1503.159800,N,01620.483000,E,1,23,123.000,1563.200,M,0.000,M,0234,8,12.230*7B\r\n" };
// 		U32 result = 0;
// 		U32 i = 0;
// 		SGGA psGGA;
// 		STime aSTime = { 0 };       	//UTC时间
// 		SSrOutPoint aSPoint = { 0 };	//定位点坐标指针
// 		psGGA.pSPoint = &aSPoint;
// 		psGGA.pSTime = &aSTime;
// 
// 		result = GetGGA(pDateRes, &psGGA);
// 
// 		_TCASSERT(psGGA.pSTime->nHour == 12);
// 		_TCASSERT(psGGA.pSTime->nMin == 23);
// 		_TCASSERT_F64(psGGA.pSTime->nSec, 40.124000);
// 		_TCASSERT(psGGA.pSPoint->nLatDegree == 15);
// 		_TCASSERT_F64(psGGA.pSPoint->nLatMin, 3.159800);
// 		_TCASSERT(psGGA.pSPoint->sLatDir == 'N');
// 		_TCASSERT(psGGA.pSPoint->nLonDegree == 16);
// 		_TCASSERT_F64(psGGA.pSPoint->nLonMin, 20.183000);
// 		_TCASSERT(psGGA.pSPoint->sLonDir == 'E');
// 		_TCASSERT(psGGA.nState == 1);
// 		_TCASSERT(psGGA.nSvNum == 23);
// 		_TCASSERT_F64(psGGA.dHDOP, 123.000);
// 		_TCASSERT_F64(psGGA.pSPoint->nHeight, 1563.200);
// 		_TCASSERT(psGGA.pSPoint->sHightUnit == 'M');
// 		_TCASSERT_F64(psGGA.pSPoint->nHeightDif, 0.00);
// //		_TCASSERT(psGGA.pSPoint->sHightUnit == 'M');
// 		_TCASSERT(psGGA.dDiffDataLife == 234);
// 		_TCASSERT(psGGA.nDiffStaID == 8);
// 
// 
// 		_TCASSERT(result == ERR_NO_ERROR);
// 
// 	}
// 	{
// 		U8* pData = NULL;
// 		SGGA asGAA = { 0 };
// 		IceAT_Start("Test_GetGGA_abnormity1");
// 		_TCASSERT(GetGGA(pData, &asGAA) == ERR_BAD_ARGS);
// 		IceAT_End_Report();
// 	}
// 
// 	{
// 		U8 pData[] = "$BDGGA,062938.00,3110.4700719,N,12123.2657056,E,1,25,0.6,58.9666,M,0.000,M,99,AA*4\r\n";
// 		SGGA* asGGA = NULL;
// 		IceAT_Start("Test_GetGGA_abnormity2");
// 		_TCASSERT(GetGGA(pData, asGGA) == ERR_BAD_ARGS);
// 		IceAT_End_Report();
// 	}
// 
// }



void Test_GetGSV()
{
	{
		U8 pData[] = "$BDGSV,3,1,11,1,0.000,0.000,46.021,2,0.667,1.334,46.031,3,1.334,2.668,46.042,4,2.001,4.002,46.053*67\r\n";
		SGSVFrm asGSV = { 0 };
		IceAT_Start("Test_GetGSV");
		_TCASSERT(GetGSV(pData, &asGSV) == ERR_NO_ERROR);
		_TCASSERT(asGSV.nTotalNum == 3);
		_TCASSERT(asGSV.nIndex == 1);
		_TCASSERT(asGSV.s32RcvdSatNum == 11);

		_TCASSERT(asGSV.pSatId[1] == 1);
		_TCASSERT_F64(asGSV.pElevation[1], 0.0);
		_TCASSERT_F64(asGSV.pAzimuth[1], 0.0);
		_TCASSERT_F64(asGSV.pfSatCnr[1], 46.021);

		_TCASSERT(asGSV.pSatId[4] == 4);
		_TCASSERT_F64(asGSV.pElevation[4], 2.001);
		_TCASSERT_F64(asGSV.pAzimuth[4], 4.002);
		_TCASSERT_F64(asGSV.pfSatCnr[4], 46.053);

		IceAT_End_Report();
	}

	{
		U8 pData[] = "$GPGSV,3,3,09,13,34,304,38,,,,,,,,,,,,*49\r\n";
		SGSVFrm asGSV = { 0 };
		S32 i;
		IceAT_Start("Test_GetGSV");

		_TCASSERT(GetGSV(pData, &asGSV) == ERR_NO_ERROR);

		_TCASSERT(asGSV.nTotalNum == 3);
		_TCASSERT(asGSV.nIndex == 3);
		_TCASSERT(asGSV.s32RcvdSatNum == 9);

		_TCASSERT(asGSV.pSatId[13] == 13);
		_TCASSERT_F64(asGSV.pElevation[13], 34.00);
		_TCASSERT_F64(asGSV.pAzimuth[13], 304.000);
		_TCASSERT_F64(asGSV.pfSatCnr[13], 38.00);

// 		for (i = 0; (i < MAX_PROC_SAT_NUM); i++)
// 		{
// 			if (i != 13)
// 			{
// 				_TCASSERT(asGSV.pSatId[i] == 1);
// 				_TCASSERT_F64(asGSV.pElevation[i], 0);
// 				_TCASSERT_F64(asGSV.pAzimuth[i], 0);
// 				_TCASSERT_F64(asGSV.pfSatCnr[i], 0);
// 			}
// 		}

		IceAT_End_Report();
	}

}

#endif
#include "AscFuncs.h"
#include "stdio.h"
#include <math.h>
#include <string.h>
#include "IceErrCtrl.h"
#include <stdlib.h>
#include "PtlNEMA0183Out.h"



U32 SetPreStr(S8* pData, S32 eType)
{
	if (eType == eFrqAll)
	{
		ASC_SET_STR("$GN");
	}
	else if (eType == eFrqL1)
	{
		ASC_SET_STR("$GP");
	}
	else if (eType == eFrqB3I)
	{
		ASC_SET_STR("$BD");
	}
	else if (eType == eFrqB1I)
	{
		ASC_SET_STR("$BD");
	}
	else if (eType == eFrqG1)
	{
		ASC_SET_STR("$GL");
	}
	else
	{
		ASC_SET_STR("$BD");
		LOGINFO1("Err SysType = %d", eType);
	}
	return 0;
}


/* 5.19 GGA 定位的时间、位置与相关的定位数据*/
U32 SetGGA(S8* pData, const SGGA* psGGA)
{

	if (!(pData != NULL && psGGA != NULL && psGGA->pSTime != NULL && psGGA->pSPoint != NULL))
	{
		return 0;
	}

	ASC_SET_START(pData != NULL && psGGA != NULL && psGGA->pSTime != NULL && psGGA->pSPoint != NULL);
	SetPreStr(pData, psGGA->eType);
	ASC_SET_STR_COMMA("GGA");	  				 /**< 设定GGA */

	ASC_SET_TIME_COMMA(psGGA->pSTime);					 /**< 设定UTC时间,加逗号 */

	if (psGGA->nState == 0)								/**< 若定位无效，则字段为空 */
	{
		ASC_SET_COMMA;
		ASC_SET_STR_COMMA("N");
		ASC_SET_COMMA;
		ASC_SET_STR_COMMA("W");
	}
	else
	{
		ASC_SET_LAT_LON(psGGA->pSPoint);					 /**< 设定 纬度 纬度方向 经度 经度方向 */
	}

ASC_SET_U32_COMMA(psGGA->nState);			   	 	 /**< 设定 状态指示 */

if (psGGA->nState == 0)								 /**< 如果定位无结果，则信息字段为空  YLee add*/
	{
		ASC_SET_COMMA;	    	 				/**< 设定 视野内的卫星数   为空*/
		ASC_SET_COMMA;			   		 		/**< 设定 HDOP值  为空 */
		ASC_SET_COMMA;	   		 				/**< 设定 天线大地主高   为空*/
		ASC_SET_COMMA;				   			/**< 设定 单位 米   为空*/
		ASC_SET_COMMA;		 					/**< 设定 高程异常   为空*/
		ASC_SET_COMMA;						 	/**< 设定 单位 米   为空*/
		ASC_SET_COMMA;			 				/**< 设定 差分数据龄期  为空 */
		ASC_SET_COMMA;				 			/**< 设定 差分总台ID号   为空*/
	}
	else
	{
		ASC_SET_U32_2BYTES_COMMA(psGGA->nSvNum);	    	 /**< 设定 视野内的卫星数 */
		ASC_SET_F64_COMMA(psGGA->dHDOP);			   		 /**< 设定 HDOP值 */
		ASC_SET_F64_COMMA(psGGA->pSPoint->nHeight);	   		 /**< 设定 天线大地主高 */
		ASC_SET_STR_COMMA("M");				   		 /**< 设定 单位 米 */
		ASC_SET_F64_COMMA(psGGA->pSPoint->nHeightDif);		 /**< 设定 高程异常 */
		ASC_SET_STR_COMMA("M");						 /**< 设定 单位 米 */
		ASC_SET_U32_4BYTES_COMMA(psGGA->dDiffDataLife);			 /**< 设定 差分数据龄期 */
		ASC_SET_U32_COMMA(psGGA->nDiffStaID);				 /**< 设定 差分总台ID号 */
		ASC_SET_F64(psGGA->dVDOP);							 /**< 设定 VDOP值 */
	}

	return ASC_SET_END(pData);
}




U32 SetGSV(S8* pData, const SGSV* psGSV)
{
	S32 i;
	F64 nTmpF64;
	S32 pSatId;
	if (!(psGSV->nTotalNum > 0 
		&& pData != NULL
		&& psGSV != NULL 
		&& psGSV->pUsedSatId != NULL
		&& psGSV->pFrqElevation != NULL
		&& psGSV->pFrqAzimuth != NULL
		&& psGSV->pFrqSatCNR != NULL))
	{
		return 0;
	}

	ASC_SET_START(psGSV->nTotalNum > 0 && pData != NULL && psGSV != NULL && psGSV->pUsedSatId != NULL\
		&& psGSV->pFrqElevation != NULL && psGSV->pFrqAzimuth != NULL && psGSV->pFrqSatCNR != NULL);
	//ASC_SET_STR_COMMA("$BDGSV");
	SetPreStr(pData, psGSV->eType);
	ASC_SET_STR_COMMA("GSV");

	ASC_SET_S32_COMMA(psGSV->nTotalNum);
	ASC_SET_S32_COMMA(psGSV->nIndex);
	ASC_SET_U32_2BYTES_COMMA(psGSV->s32RcvdSatNum);
//	ASC_SET_U32_COMMA(psGSV->pUsedSatId);

	for (i = (psGSV->nIndex - 1) * 4; i < 4 * psGSV->nIndex && i < psGSV->s32RcvdSatNum; i++)
	{
		pSatId = psGSV->pUsedSatId[i];                                          /* 卫星号 */

		VERIFY_LOG_RETVAL(pSatId > 0 && pSatId <= MAX_SAT_INDEX, 0);
		ASC_SET_S32_2BYTES_COMMA(pSatId);

		nTmpF64 = ((F64)(psGSV->pFrqElevation[i]) / PI) * 180.0;
		ASC_SET_F64_COMMA(nTmpF64);

		nTmpF64 = ((F64)(psGSV->pFrqAzimuth[i]) / PI) * 180.0;
		ASC_SET_F64_COMMA(nTmpF64);

		if (psGSV->pFrqSatCNR[i] > 0)
		{
			nTmpF64 = 10.0 * log10((F64)psGSV->pFrqSatCNR[i]);
		}
		else
		{
			nTmpF64 = 0.0;
		}
		if (i == (4 * psGSV->nIndex - 1) || i == (psGSV->s32RcvdSatNum - 1))
		{
			ASC_SET_F64(nTmpF64);	         /**< 10*log10(nCnr)  */
		}
		else
		{
			ASC_SET_F64_COMMA(nTmpF64);   	/**< 10*log10(nCnr)  */
		}
	}

	return ASC_SET_END(pData);
}
















/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/

#ifdef ICE_AUTO_TEST_ENABLE
#include "IceAutoTestC.h"

// void Test_SetGGA();
// 
// 
// void Test_PtlNEMA0183Out(void)
// {
// 
// 	Test_SetGGA();
// 
// 	IceAT_ReportAll();
// }
// 
// void Test_SetGGA()
// {
// 	IceAT_Start("Test_SetDHV_NORMAL");
// 	{
// 		S8 pData[10240] = { 0 };
// 		//S8 pDateRes[] ={"$BDGGA,122340.124000,0000.000000,,0000.000000,,1,23,123.000,0.000,M,0.000,M,234.210,8,12.230*56\r\n"};
// 		//S8 pDateRes[] ={"$BDGGA,122340.124000,0000.000000,.,00000.000000,.,1,23,123.000,0.000,M,0.000,M,0234,8,12.230*4B\r\n"};
// 		S8 pDateRes[] = { "$BDGGA,122340.124000,1503.159800,N,01620.483000,E,1,23,123.000,1563.200,M,0.000,M,0234,8,12.230*7B\r\n" };
// 		U32 len = 0;
// 		U32 i = 0;
// 		SSrOutPoint point = { 0 };
// 		STime Time;
// 		SGGA psGGA = { 0 };
// 
// 		point.nLatDegree = 15;
// 		point.nLatMin = 3.1598;
// 		point.sLatDir = 'N';
// 		point.nLonDegree = 16;
// 		point.nLonMin = 20.483;
// 		point.sLonDir = 'E';
// 		point.nHeight = 1563.2;
// 		//STime Time = {0};
// 		Time.nHour = 12;
// 		Time.nMin = 23;
// 		Time.nSec = 40.124;
// 		psGGA.pSTime = &Time;
// 		psGGA.pSPoint = &point;
// 		psGGA.nState = 1;
// 		psGGA.nSvNum = 23;
// 		psGGA.dHDOP = 123;
// 		psGGA.dDiffDataLife = 234;
// 		psGGA.nDiffStaID = 8;
// 		psGGA.dVDOP = 12.23;
// 		psGGA.eType = eFrqB3I;
// 
// 		len = SetGGA(pData, &psGGA);
// 		_TCASSERT(len == 100);
// 		LOG(1, "len==%d", len);
// 		for (i = 0; i < len; i++)
// 		{
// 			_TCASSERT(pData[i] == pDateRes[i]);
// 			if (pData[i] != pDateRes[i])
// 			{
// 				LOG(1, "%d,%d,%d", i, pData[i], pDateRes[i]);
// 			}
// 		}
// 		LOG(1, "pData   ==%s", pData);
// 		LOG(1, "pDateRes==%s", pDateRes);
// 	}
// 	IceAT_End_Report();
// }




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




void Test_SetGSV();


void Test_PtlNEMA0183Out(void)
{

	Test_SetGSV();

	IceAT_ReportAll();
}
void Test_SetGSV(void)
{

	//SPseudorange aPseudorange = {0};
	//SBd2_SolveResult aSolve = {0}; 
	U32 outputFrq = 0;//aSolve.stdFrqId;	
	S8 pData[SERIAL_MAX_FRAME_BYTES] = { 0 };
	S32 nTotalNum = 0;
	S32 nIndex = 0;
	S8 pDataRes[] = "$GPGSV,3,1,09,19,46,206,41,16,55,015,43,31,24,120,37,06,82,060,43*7,$GPGSV,3,2,09,27,82,183,45,21,15,061,33,23,47,268,43,03,72,228,44* 72$GPGSV,3,3,09,13,34,304,38, * 49\r\n";
	S32 len, i;
	SGSV psGSV = { 0 };
	IceAT_Start("Test_SetGSV");
	{

		S32 usedSatId[12] = { 0 };
		F64 frqElevation[12] = { 0 };
		F64 frqAzimuth[12] = { 0 };
		U32 frqSatCNR[12] = { 0 };
		
		nTotalNum = 3;
		nIndex = 1;
		for (i = 0; i <nTotalNum*4; i++)
		{
			usedSatId[i] = i + 1;
			frqElevation[i] = i * 100000000;
			frqAzimuth[i] = i * 200000000;
			frqSatCNR[i] = i;
		}
		psGSV.nTotalNum = nTotalNum;
		psGSV.nIndex = nIndex;
		psGSV.s32RcvdSatNum = 2;
		for (i = 0; i < nTotalNum * 4; i++)
		{
			psGSV.pUsedSatId[i] = usedSatId[i];
			psGSV.pFrqElevation[i] = frqElevation[i];
			psGSV.pFrqAzimuth [i]= frqAzimuth[i];
			psGSV.pFrqSatCNR [i]= frqSatCNR[i];
			
		}
		psGSV.eType = 2;


		len = SetGSV(pData, &psGSV);
		_TCASSERT(len == 211);
		LOG(1, "len==%d", len);
		for (i = 0; i < len - 4; i++)

		{
			_TCASSERT(pData[i] == pDataRes[i]);
			if (pData[i] != pDataRes[i])
			{
				LOG(1, "%c,%c,%d", pData[i], pDataRes[i], i);
			}
		}

	}


// 	IceAT_Start("Test_SetGSV_abnormity1");
// 	{
// 		S8* pDataOut = NULL;
// 		SGSV psGSV = { 0 };
// 		U32 len;
// 		len = SetGSV(pDataOut, &psGSV);
// 		_TCASSERT(len == 0);
// 		LOG(1, "len==%d", len);
// 		IceAT_End_Report();
// 	}
// 
// 
// 	IceAT_Start("Test_SetGSV_abnormity2");
// 	{
// 		S8 pDataOut[SERIAL_MAX_FRAME_BYTES] = { 0 };
// 		SGSV psGSV;
// 		psGSV = NULL;
// 		U32 len;
// 		len = SetGSV(pDataOut, psGSV);
// 		_TCASSERT(len == 0);
// 		LOG(1, "len==%d", len);
// 		IceAT_End_Report();
// 	}


	IceAT_End_Report();
}







#endif
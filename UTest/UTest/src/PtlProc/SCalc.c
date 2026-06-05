#include "SCalc.h"
#include "math.h"
#include "IceAutoTestC.h"
#define ZERO_NEAR (1e-10) /* 用来判断浮点数的宏定义，取一个极小数，来判断两数之差 */

//************************************
// Method:    CalcCnr
// FullName:  CalcCnr
// Access:    public 
// Returns:   BOOL   FALSE 异常
// Qualifier:
// Parameter: F64 * pCnrData 卫星号没有对应存储，0位置也存数，比如15颗星数据，pCnrData就得pCnrData[14]，及卫星号比存储多一;
// Parameter: U32 satNum     传进来的卫星个数;
// Parameter: BOOL bCnrOk	 数据是否完整
// Parameter: SCalcMeanStd * pSCalcMeanStd  用来中间过程处理和计算结果的存储的结构体;
//************************************
BOOL CalcCnr(F64* pCnrData, U32 satNum, BOOL bCnrOk, SCalcMeanStd* pSCalcMeanStd)
{
	S32 i = 0;
	F64 CnrInter = 0;  

	if (!bCnrOk)
	{
		return FALSE;
	}

	if ((pCnrData == NULL )
		|| (pSCalcMeanStd == NULL)
		)
	{
		return FALSE;
	}

	if ((pSCalcMeanStd->nCtrlVer != 0)
		&& (pSCalcMeanStd->nCtrlVer != 1))
	{
		return FALSE;
	}

	if ((satNum <= 0) 
		|| (satNum > MAX_PROC_SAT_NUM))
	{
		return FALSE;
	}

	for (i=0; i< satNum;i++)
	{
		if (pCnrData[i] >= 0)
		{
			pSCalcMeanStd->ps32Total[i]++;  //对每个数据进行计数
			if ((fabs(pSCalcMeanStd->pf64FmrCnr[i] - 0) < ZERO_NEAR) && (pCnrData[i] == 0))  //将第一次数据为0的载噪比不加入计算
			{
			}
			else if ((fabs(pCnrData[i] - 0) < ZERO_NEAR) && (pSCalcMeanStd->pf64FmrCnr[i] != 0) && (pSCalcMeanStd->nCtrlVer == 0)) //将前面有数据，后续数据为0的情况不加入计算
			{
			}
			else  //一直都有载噪比，且前面有数据，后续数据为0的情况仍需要加入计算的情况
			{
				pSCalcMeanStd->pf64CnrCount[i]++;  //对计算的有效数据进行计数，在计算过程之前先加一，是为了在函数外对整个结构体初始化时更加方便，可以全部赋值为0，使计算正常进行
				pSCalcMeanStd->pf64CnrAvg[i] = pCnrData[i] / (pSCalcMeanStd->pf64CnrCount[i]) + (pSCalcMeanStd->pf64CnrAvg[i]) * (pSCalcMeanStd->pf64CnrCount[i] - 1) / (pSCalcMeanStd->pf64CnrCount[i]);
				CnrInter = pow((pCnrData[i] - (pSCalcMeanStd->pf64FmrAvg[i])), 2) * ((pSCalcMeanStd->pf64CnrCount[i]) - 1) / pow((pSCalcMeanStd->pf64CnrCount[i]), 2);
				pSCalcMeanStd->pf64FmrAvg[i] = pSCalcMeanStd->pf64CnrAvg[i];
				pSCalcMeanStd->pf64CnrVar[i] = CnrInter + (pSCalcMeanStd->pf64CnrVar[i]) * ((pSCalcMeanStd->pf64CnrCount[i]) - 1) / (pSCalcMeanStd->pf64CnrCount[i]);
				pSCalcMeanStd->pf64CnrStdErr[i] = sqrt(pSCalcMeanStd->pf64CnrVar[i]);
			}
			memcpy(&pSCalcMeanStd->pf64FmrCnr[i], &pCnrData[i], sizeof(pCnrData[i]));
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////以下为测试代码
#ifdef ICE_AUTO_TEST_ENABLE
void Test_SCALCC()
{
	SCalcMeanStd m_SCalcMeanStd;
	//测试用例1：输入空指针，程序正常返回，不计算不死机
	IceAT_Start((S8*)("测试1"));
	{
		SCalcMeanStd* m_SCalcMeanStd;
		memset(&m_SCalcMeanStd, 0, sizeof(m_SCalcMeanStd));

		F64 cnr[54] = { 50,60,50,0,03,01,50 };
		BOOL res = FALSE;

		res = CalcCnr(cnr, 7, m_SCalcMeanStd);

		_TCASSERT(res == FALSE);

	}
	IceAT_End_Report();




	// 测试用例2：输入空指针，程序正常返回，不计算不死机
	IceAT_Start((S8*)("测试2"));
	{

		F64 cnr[54] = { 50,60,50,0,03,01,50 };
		BOOL res = FALSE;

		res = CalcCnr(NULL, 7, &m_SCalcMeanStd);
		_TCASSERT(res == FALSE);

	}
	IceAT_End_Report();

	// 测试用例3：satnum变量测试
	IceAT_Start((S8*)("测试3"));
	{
		memset(&m_SCalcMeanStd, 0, sizeof(m_SCalcMeanStd));
		F64 cnr[54] = { 50,60,50,0,03,01,50 };
		BOOL res1 = FALSE;
		BOOL res2 = FALSE;
		BOOL res3 = FALSE;
		BOOL res4 = FALSE;
		res1 = CalcCnr(cnr, -1, &m_SCalcMeanStd);
		res2 = CalcCnr(cnr, 0, &m_SCalcMeanStd);
		res3 = CalcCnr(cnr, 54, &m_SCalcMeanStd);
		res4 = CalcCnr(cnr, 55, &m_SCalcMeanStd);
		_TCASSERT(res1 == FALSE);
		_TCASSERT(res2 == TRUE);
		_TCASSERT(res3 == TRUE);
		_TCASSERT(res4 == FALSE);

	}
	IceAT_End_Report();

	// 测试用例4：pCnrData数据测试
	IceAT_Start((S8*)("测试4"));
	{
		memset(&m_SCalcMeanStd, 0, sizeof(m_SCalcMeanStd));

		F64 cnr[54] = { 50,60,50,0,03,01,50 };
		BOOL res = FALSE;
		res = CalcCnr(cnr, 7, &m_SCalcMeanStd);


		_TCASSERT(res == TRUE);

	}
	IceAT_End_Report();

	// 测试用例5：pCnrData数据测试
	IceAT_Start((S8*)("测试5"));
	{
		memset(&m_SCalcMeanStd, 0, sizeof(m_SCalcMeanStd));
		F64 cnr[54] = { 50,60,50,60,03,01,50 };
		BOOL res = FALSE;

		res = CalcCnr(cnr, 7, &m_SCalcMeanStd);
		_TCASSERT(res == TRUE);

	}
	IceAT_End_Report();

	// 测试用例6：pCnrData数据测试
	IceAT_Start((S8*)("测试6"));
	{

		memset(&m_SCalcMeanStd, 0, sizeof(m_SCalcMeanStd));
		F64 cnr[54] = { 50,60,50,-1,03,01,50 };
		BOOL res = FALSE;

		res = CalcCnr(cnr, 7, &m_SCalcMeanStd);
		_TCASSERT(res == FALSE);

	}
	IceAT_End_Report();

	// 测试用例7：pCnrData数据测试
	IceAT_Start((S8*)("测试7"));
	{
		memset(&m_SCalcMeanStd, 0, sizeof(m_SCalcMeanStd));
		F64 cnr[54] = { 50,60,50,61,03,01,50 };
		BOOL res = FALSE;

		res = CalcCnr(cnr, -1, &m_SCalcMeanStd);
		_TCASSERT(res == FALSE);

	}
	IceAT_End_Report();


	// 测试用例8：结构体成员值数据
	IceAT_Start((S8*)("测试8"));
	{

		memset(&m_SCalcMeanStd, -1, sizeof(m_SCalcMeanStd));
		F64 cnr[54] = { 50,60,50,0,03,01,50 };
		BOOL res = FALSE;

		res = CalcCnr(cnr, 7, &m_SCalcMeanStd);
		_TCASSERT(res == FALSE);

	}
	IceAT_End_Report();

	// 测试用例9：第一帧数据前无后有情况
	IceAT_Start((S8*)("测试9"));
	{
		memset(&m_SCalcMeanStd, 0, sizeof(m_SCalcMeanStd));
		F64 cnr[7] = { 50,60,50,2,03,01,50 };
		BOOL res = FALSE;
		U32 c = 7;
		res = CalcCnr(cnr, c, &m_SCalcMeanStd);
		_TCASSERT(res == TRUE);
	}
	IceAT_End_Report();////////////////////////////

	//测试数据为0首次为0情况且不代入计算
	IceAT_Start((S8*)("测试10"));
	{
		memset(&m_SCalcMeanStd, 0, sizeof(m_SCalcMeanStd));
		F64 cnr[54] = { 50,60,50,0,03,01,50 };
		;
		BOOL res1 = FALSE;

		res1 = CalcCnr(cnr, 7, &m_SCalcMeanStd);
		if (m_SCalcMeanStd.pf64CnrCount[3] == 0)
		{
			_TCASSERT(res1 == TRUE);
		}


	}
	IceAT_End_Report();

	//测试二帧数据 都有数据情况,前无后有情况不加零计算，数据一直都无情况,
	IceAT_Start((S8*)("测试11"));
	{
		S32 i = 0;
		memset(&m_SCalcMeanStd, 0, sizeof(m_SCalcMeanStd));
		F64 cnr1[7] = { 50,60,50,0,03,01,50 };
		F64 cnr2[7] = { 56,20,15,35,20,34 ,20 };
		F64 cnr3[8] = { 54,35,36,0,0,0,0 };
		BOOL res1 = FALSE;
		BOOL res2 = FALSE;
		BOOL res3 = FALSE;
		for (i = 0; i < 3; i++)
		{
			if (0 == i)
			{
				res1 = CalcCnr(cnr1, 7, &m_SCalcMeanStd);
				if (m_SCalcMeanStd.pf64CnrCount[3] == 0)
				{
					_TCASSERT(res1 == TRUE);
				}
			}
			if (1 == i)
			{
				res2 = CalcCnr(cnr2, 7, &m_SCalcMeanStd);
				if (m_SCalcMeanStd.pf64CnrCount[3] == 1)
				{
					_TCASSERT(res2 == TRUE);
				}
			}
			if (2 == i)
			{
				res3 = CalcCnr(cnr3, 7, &m_SCalcMeanStd);
				if (m_SCalcMeanStd.pf64CnrCount[3] == 1 && m_SCalcMeanStd.pf64CnrCount[4] == 2 && m_SCalcMeanStd.pf64CnrCount[5] == 2 && m_SCalcMeanStd.pf64CnrCount[6] == 2)
				{
					_TCASSERT(res3 == TRUE);
				}
			}
		}

	}
	IceAT_End_Report();


	IceAT_ReportAll();
}
#endif
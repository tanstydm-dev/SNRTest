#ifndef _ERRORSNRCALC_H_2022
#define _ERRORSNRCALC_H_2022

#include "icetype.h"
#include "PtlNEMA0183Out.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct
	{
		F64 pf64CnrAvg[MAX_PROC_SAT_NUM];  //每个卫星载噪比对应的平均值
		F64 pf64CnrVar[MAX_PROC_SAT_NUM]; //每个卫星载噪比对于的方差
		F64 pf64CnrCount[MAX_PROC_SAT_NUM]; //对每个卫星有效数据进行计数
		F64 pf64FmrAvg[MAX_PROC_SAT_NUM]; //用来存储每个卫星上一帧的平均数
		F64 pf64CnrStdErr[MAX_PROC_SAT_NUM]; //每个卫星载噪比对应的标准差
		F64 pf64FmrCnr[MAX_PROC_SAT_NUM]; //用来存储每个卫星上一次的载噪比
		F64 ps32Total[MAX_PROC_SAT_NUM];  //对每个卫星的所有数据进行计数
		S32 nCtrlVer; //用来判断载噪比数据计算情况的变量
	}SCalcMeanStd;

	BOOL CalcCnr(F64* pCnrData, U32 satNum, BOOL bCnrOk, SCalcMeanStd *pSCalcMeanStd);

	//////////////////////////////////////////////////////////////////////////下面为测试函数
	/*void Test_SCALCC(); */

#ifdef __cplusplus
}
#endif

#endif /*_ERRORSNRCALC_H_2022 */
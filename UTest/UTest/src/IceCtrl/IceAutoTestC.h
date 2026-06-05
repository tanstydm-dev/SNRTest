/**
@file		iceAutoTestC.h 自动测试库（C语言）
@author		xIANG wEi waterknows@gmail.com
@date 		2007/8/1   17:33
@version	v1.0 Copyright (c) 2007 - All Rights Reserved
@warning 	需要IceErrCtrl配合
sample:
	IceAT_Start("TestConnect");
	_TCASSERT(2>3);
	_TCASSERT(3>2);
	easdf=funcsample(asdf);
	_TCASSERT(easdf==3);
	_TCASSERT(asdf==4);
	IceAT_End_Report();

	IceAT_ReportAll();

*********************************************************************/
#ifndef ICEAUTOTESTC_H_20070801_XW
#define ICEAUTOTESTC_H_20070801_XW

#include "IceErrCtrl.h"

#ifdef __cplusplus
extern "C"{
#endif


#ifdef _VXWORKS
	#define ICEAUTO_FORMAT_S64 "ll"
#else
	#define ICEAUTO_FORMAT_S64 "I64d"
#endif


#define _TCASSERT(a)  if(!(a))\
{\
IceAT_ErrAdd();\
LOGERR0(#a);\
}\
IceAT_TestAdd();

#define MIN_ERR (1e-6)
#define _TCASSERT_F64(a, b) _TCASSERT( ((a)-(b)<MIN_ERR) && ((a)-(b)>(-MIN_ERR)))

	
#define _TCASSERT_EQU_D(a, b)  _TCASSERT_EQU(a, b, S32, "%d", 0)

#define _TCASSERT_EQU_E(a, b)  _TCASSERT_EQU(a, b, F64, "%e", 0)
	
	
#define _TCASSERT_EQU(a, b, myType, myformat, index)  {\
myType tempa;\
myType tempb;\
tempa = (a);\
tempb = (b);\
if(!(tempa==tempb))\
{\
IceAT_ErrAdd();\
LOGERR5("(%d) , %s==%s , %"##myformat##" == %"##myformat, (index), #a, #b, tempa, tempb);\
}\
	IceAT_TestAdd();}



#define _TCASSERTfor(a,b)  if(!(a))\
{\
IceAT_ErrAdd();\
LOGERR0(#a);\
LOGERR1("%d",b);\
}\
IceAT_TestAdd();


/** 开始自动测试
@param pStr 本次测试的名称
*/
void IceAT_Start(const S8 *pStr);

/** 停止自动测试，并报告错误数
*/
void IceAT_End_Report(void);

/** 报告总的累及测试数与错误数
*/
void IceAT_ReportAll(void);



/*********************************************************************************/

/** 此函数请勿直接调用。累加错误数
*/
void IceAT_ErrAdd(void);

/** 此函数请勿直接调用。累加测试数
*/
void IceAT_TestAdd(void);
/*********************************************************************************/

#ifdef __cplusplus
}
#endif


#endif

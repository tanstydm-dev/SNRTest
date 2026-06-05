/**
@file		iceAutoTestC.c ×Ô¶¯²âÊÔ
@author		xIANG wEi waterknows@gmail.com
@date 		2007/8/1   18:20
@version	v1.0 Copyright (c) 2007 - All Rights Reserved
@warning 	
*********************************************************************/

#include "iceAutoTestC.h"
#include "IceErrCtrl.h"
#include <string.h>

#define TEST_NAME_MAX_LEN 1024

static S32 m_errNum=0;
static S32 m_testNum=0;
static S8 m_testCaseName[TEST_NAME_MAX_LEN]={0};

static S32 m_testUnitTotalNum=0;
static S32 m_errUnitTotalNum=0;

void IceAT_Start(const S8 *pStr){
	U32 nameLen = strlen(pStr);
	m_errNum = 0;
	m_testNum = 0;
	if(nameLen>=(U32)TEST_NAME_MAX_LEN){
		nameLen =(U32)(TEST_NAME_MAX_LEN-1);
	}
	memcpy(m_testCaseName, pStr, nameLen);
	m_testCaseName[nameLen]=0;
}

void IceAT_End_Report(){

	LOGINFO3("\r\n=======================\r\n%s\r\nTotal Test: %d\r\nError No: %d\r\n=======================\r\n",
			(const S8 *)m_testCaseName,m_testNum,m_errNum);

	m_errNum = 0;
	m_testNum = 0;
	m_testCaseName[0]=0;
	return;
}

void IceAT_ErrAdd()
{	
	m_errNum++;
	m_errUnitTotalNum++;
	return;
}

void IceAT_TestAdd(){
	m_testNum++;
	m_testUnitTotalNum++;
	return;
}

void IceAT_ReportAll(){
	LOGINFO2("\r\n***********************\r\n***********************\r\nTotal Count \r\nTotal Test: %d\r\nError No: %d\r\n***********************\r\n***********************\r\n",
			m_testUnitTotalNum,m_errUnitTotalNum);
	return;
}

#pragma once

#include "SfrmExtract.h"
#include "icetype.h"
#include "PtlNEMA0183In.h"
#include "PtlNEMA0183Out.h"
#include "NavConstant.h"
#include "NavCordTrans.h"
#include "GetDzPtl.h"
#include "HexMutCStr.h"
#include "SCalc.h"
#include "ErrorCalcute.h"
#include "icetype.h"
#include "IceAutoTestC.h"
#include "IceErrCtrl.h"
#include "ComSet.h"


#define	 MAX_STATUSBAR_NUM					6
#define	 TIMER_COMOPENTIME_ID				7
#define	 MAX_COM_NUM						20
#define	 SET_TIMER							1
#define  KILL_TIMER							2

/*自定义消息*/
#define  WM_MYMESSAGE_UPDATESTATUSBAR	WM_USER+200		/**<更新状态栏*/
#define  WM_MYMESSAGE_STARTUPTIMER		WM_USER+201		/**<启动定时器*/
#define  WM_MYMESSAGE_SETGGAPOINT		WM_USER+202		/**<地图标点(定位点，GGA格式)*/
#define  WM_MYMESSAGE_SETUSERPOINT		WM_USER+203		/**<地图标点(用户位置，CString格式)*/

typedef struct 
{
	UINT uItemId;
	CString strItem;
}SStatusBar;

typedef struct
{
	UINT uMenuId;
	UINT uItemId;
	UINT uMode;
}SMenuBar;


#pragma once
#ifndef _PTLNEMA0183_H_2019_12_30_
#define _PTLNEMA0183_H_2019_12_30_

#ifndef PI
#define		PI					3.1415926535897931
#endif

#include "icetype.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef MAX_SAT_INDEX
#define MAX_SAT_INDEX	76		/**< 最大卫星索引, 考虑和GPS公用 BD:1~14 GPS:15~47 GLO:48~76*/
#endif

#define MAX_PROC_SAT_NUM		60			/**< 单一系统最大卫星数 */
#define SERIAL_MAX_FRAME_BYTES	1024		/**< 考虑内部协议超过100Bytes 留有余量 */
#define SYS_BDS					0
#define SYS_GPS					1
#define SYS_GLONASS				2
#define SYS_GALILEO				3
#define SYS_GNSS				4
#define MAX_FRQ_NUM				7			/**< 最大频点数量，根据EFrqId */

typedef enum
{
	eFrqB1I = 1,		/**<BD-B1I*/
	eFrqB1C,			/**<BD-B1C*/
	eFrqB2a,			/**<BD-B2a*/
	eFrqB3I,			/**<BD-B3I*/
	eFrqL1,				/**<GPS-L1*/	/* BP测试中L1定义为3 */
	eFrqG1,				/**<GLONASS-L1*/
	eFrqAll				/**<All*/
}EFrqId;

typedef enum
{
	eSysBd = 5,		
	eSysGp,			
	eSysGl,			
	eSysGa,			
	eSysGn			
}ESysId;



/*
U32 SetPreStr(S8* pData, S32 eType);
U32 SetGSV(S8* pData, const SGSV* psGSV);
U32 SetGGA(S8* pData, const SGGA* psGGA);
*/

/* 点坐标 */
typedef struct {
	U32		nLatDegree;		/**<纬度(度)*/
	F64		nLatMin;		/**<纬度(分)*/
// 	U32		nLatDegree1;	/**<纬度(')*/
// 	U32		nLatDegree11;	/**<纬度('')*/
// 	U32		nLatDegree111;	/**<纬度(0.1'')*/
	S8_CHAR	sLatDir;		/**<纬度方向 N/S*/

	U32		nLonDegree;		/**<经度(度)*/
	F64		nLonMin;		/**<经度(分)*/
// 	U32		nLonDegree1;	/**<经度(')*/
// 	U32		nLonDegree11;	/**<经度('')*/
// 	U32		nLonDegree111;	/**<经度(0.1'')*/
	S8_CHAR	sLonDir;		/**<经度方向 E/W*/

	S8_CHAR	sHightUnit;		/**<经度方向 E/W*/
	F64		nHeight;			/**<平均海拔高	x.x M*/
	F64		nHeightDif;		/**<CGS-2000大地高和海拔高的差距*/
}SSrOutPoint;
/*用户位置*/
typedef struct
{
	F64 f64UserLat;
	F64 f64UserLon;
	F64 f64UserElev;
	F64 f64UserPosX;
	F64 f64UserPosY;
	F64 f64UserPosZ;
}SUserPnt;

/*时间结构体*/
typedef struct {
	U32 nHour;
	U32 nMin;
	F64 nSec;
	U32	nSecs;			/**<整s*/
	U32	nSecs01;		/**<0.01s*/
}STime;

typedef struct
{
	U32 u32Year;    //年份
	U32 u32Month;   //月份
	U32 u32Day;     //日
}SDate;

typedef struct
{
	STime* pSTime;       	//UTC时间
	SSrOutPoint* pSPoint;	//定位点坐标指针
	U32 nState;				//状态指示 0:未定位 1:无差分定位 2:差分定位 3:双频定位 4:BD-2/GPS兼容无差分定位 5:BD-2/GPS兼容差分定位//
	U32 nSvNum;				//视野内卫星数(0~12)
	F64 dHDOP;				//HDOP值
	U32 dDiffDataLife;  	//差分数据寿命，从上一次差分数据更新开始算起，以秒计。空表示未采用差分数据(待定)
	U32 nDiffStaID;  		//差分站台ID号(待定)
	F64 dVDOP;       		//VDOP值
	S32 eType;
}SGGA;
							//0-无效,1-定位有效,2-差分定位有效,3-PPS模式,定位有效,4-RTK模式,5-浮动RTK,6-估算模式,7-手动输入模式,8-模拟器模式


typedef struct 
{
	U32 nTotalNum;                      /* GSV语句 */
	S32 nIndex;                         /* GSV语句编号 */
	S32 s32RcvdSatNum;					/* 收星数 */
	S32 s32AvlSatNum;					/* 可见卫星总数 */
	S32 pnAvlSatId[MAX_PROC_SAT_NUM];		/**< 可见卫星号 */
	U32 pSatId[MAX_PROC_SAT_NUM];		/**< base1, 使用时，循环此变量，不为0的对应可以取elevation、azimuth、CNR */
	F64 pElevation[MAX_PROC_SAT_NUM];	/*	仰角 */
	F64 pAzimuth[MAX_PROC_SAT_NUM];		/*	方位角 */
	F64 pfSatCnr[MAX_PROC_SAT_NUM];		/*	载噪比 */
	S32 eType;							/*	系统名 */
}SGSVFrm;


typedef struct
{
	S32 nTotalNum;                      /* GSV语句 */
	S32 nIndex;                         /* GSV语句编号 */
	S32 s32RcvdSatNum;					/* 可见卫星总数 */
	S32 pUsedSatId[MAX_PROC_SAT_NUM];   /* 卫星号 */
	F64 pFrqElevation[MAX_PROC_SAT_NUM];/* 卫星仰角 */
	F64 pFrqAzimuth[MAX_PROC_SAT_NUM];	/* 卫星方位角 */
	U32 pFrqSatCNR[MAX_PROC_SAT_NUM];	/* 信噪比 */
	S32 eType;
}SGSV;

// typedef struct SXXV
// {
// 	F64 pSTrack;//度数 0-360°
// 	F64 pSMag;
// 	F64 pSSpeedkn;
// 	F64 pSSpeedkm;
// 	S32 eType;
// 	S32 pTurerefer;
// 	S32 pMagnetic;
// }SXXV;

typedef struct
{
	F64	nDegree;		     /**<磁偏角(度)*/
	S8_CHAR Vardir;          //磁偏角方向
}SMag;

typedef struct
 {
	STime* pSTime;       	//UTC时间
	U8 u8State;				/*	状态：A——数据有效；D——接收机告警 */
	SSrOutPoint* pSPoint;	//定位点坐标指针
	F64 f64Speed;			//对地速度
	F64 f64TureCourse;		//对地真航向
	F64 f64Date;			//UTC日期
	SMag* pSMag;			//磁偏角 0.00-180.00
	S8  s8Mode;				//模式指示
	S32 eType;       		//信号类型
}SRMC;
// $BDRMC, 064833.00, A, 2814.928352, N, 11301.616654, E, 0.046, 106.600, 000000, 00, E, A * 0F
typedef struct
{
	S8 s8Mode;		/*	定位模式 */
	U32 u32Mode;	/*	定位模式 */
	U32 pu32PRNCode[MAX_PROC_SAT_NUM];	/*	 */
	F64 f64Pdop;	/*	位置精度因子(PDOP) */
	F64 f64Hdop;	/*	水平精度因子(HDOP) */
	F64 f64Vdop;	/*	垂直精度因子(VDOP) */
	F64 f64Tdop;	/*	TDOP,BD协议特有 */
	S32 eType;
}SGSA;

typedef struct
{
	U32 eFrqId;		/*	频点号: B1I,B3I,L1CA... */
	U32 u32SatNum;		/*	卫星数 */
	U32	pu32SatId[MAX_PROC_SAT_NUM];
	F64 pfSatCnr[MAX_PROC_SAT_NUM];	/*	各自的载噪比 */
	S32 eType;
}SCNO;

typedef struct
{
	U32 peFrqId[MAX_FRQ_NUM];						/*	频点号，取EFrqId*/
	U32 pu32SatNum[MAX_FRQ_NUM];					/*	卫星数 */
	U32	pu32SatId[MAX_FRQ_NUM][MAX_PROC_SAT_NUM];	/*  卫星号 */
	F64 pfSatCnr[MAX_FRQ_NUM][MAX_PROC_SAT_NUM];	/*	各自的载噪比 */
}SCNOBuf;

typedef struct
{
	STime* pSTime;       	//UTC时间
	F64 speed;                  //速度
	F64 xSpeed;                //x轴速度
	F64 ySpeed;                //y轴速度
	F64 zSpeed;                //z轴速度
	F64 eSpeed;                //地速
	F64 maxSpeed;           //最大速度
	F64 avgSpeed;             //平均速度
	F64 wholeSpeed;			//全程平均速度
	F64 validSpeed;        //有效速度
	S8_CHAR speedUnit;    //速度单位
	S32 eType;
}SDHV;

typedef struct 
{
	U32  u32zdaMode;	
	STime* psTime;
	SDate* psDate;
	S32 s32TimeZone;	/*	时区 */
	S32 s32ZoneMin;		/*	时区（分） */
}SZDA;

typedef struct
{

	U32  u32Sec;

}SCOV;

/************************************************************************/
/**********以下为测试代码************************************************/
/************************************************************************/
#ifdef ICE_AUTO_TEST_ENABLE

void Test_PtlNEMA0183Out();

#endif

#ifdef __cplusplus
}
#endif

#endif
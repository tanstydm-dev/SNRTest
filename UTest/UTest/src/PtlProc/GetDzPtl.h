#pragma once


#include "icetype.h"
#define MAX_SYATE_NUM 24  //最大通道数，不足的用0补齐
typedef struct
{
	U8 satelLite;             //卫星号
	U8 sateStatus;            //卫星状态
	U8 chanStatus;            //通道状态
	S8 satelEve;              //卫星仰角
	U16 satelAdic;            //卫星方位角
	U8 signalRatio;           //信噪比
	S32 doppler;              //多普勒
	U16 chancode_doc_phase;   //通道code_doc_phase
	U32 chancode_phase;       //通道code_phase
	U16 bitState;             //比特位同步状态
}SSsigstateInfo;

typedef struct
{
	U32 TicCount;		// TicCount
	U8 timValdSym;		// 时间有效标志
	U16 timeYear;       //年
	U8 timMonHour[4];	// 6维代表月日时分
	F64 timeSecnd;      //8维代表秒
	U8 posStatus;       //定位状态
	F32 locRest[3];     //代表定位结果XYZ
	F32 locRestV[3];    //代表定位结果VXYZ
	U16 locRestPdop;    //定位结果PDOP
	U16 locRestHdop;    //定位结果HDOP
	U16 locRestVdop;    //定位结果VDOP
	F32 clokDiff;       //定位结果钟差
	F32 clokFlot;       //定位结果钟漂
	U8 chanNum;         //通道数
	U8 softNum;         //软件版本号

	SSsigstateInfo stInfo[MAX_SYATE_NUM];  //每个卫星的信息


	U16 obligate[24];   //预留
	U16 checksum;       //校验和

}SDzNavInfo;


#define MAX_NUMWORDS 10   //消息内包含的最大数据字数
typedef struct
{
	U8 GNSSID;                //GNSS标识符
	U8 SVID;                  //卫星标识符
	U8 Reserved0;             //预留0
	U8 FreqID;                //仅用于GLO，频槽+7
	U8 numWords;              //此消息中包含的数据字数
	U8 chn;                   //跟踪通道号
	U8 version;               //消息版本
	U8 Reserved1;             //预留1
	U32 dwrd[MAX_NUMWORDS];   //数据字
}SSFRBX;


#define MAX_RAWX_NUM 20   //RAWX协议最大测量值数
typedef struct
{
	F64 prMes;      //伪范围测量值
	F64 cpMes;      //载波相位测量值
	F32 doMes;      //多普勒测量值
	U8 gnssID;      //GNSS标识符
	U8 svID;        //卫星标识符
	U8 sigID;       //信号标识符
	U8 freqID;      //仅用于GLO，频槽+7
	U16 lockTime;   //载波相位锁时计数
	U8 cno;         //信号强度
	U32 prStd;      //估计的伪范围测量值
	F64 prStdDev;   //估计的伪范围测量标准偏差
	U32 cpStd;      //估计的载流子相位标准偏差
	F64 cpStdDev;   //估计的载波相位测量标准偏差
	U32 doStd;      //估计的多普勒标准差
	F64 doStdDev;   //估计的多普勒测量标准差
	U8 prValid;     //伪排列有效
	U8 cpValid;     //承运商阶段有效
	U8 halfCyc;     //半周期有效
	U8 subHalfCyc;  //从相减的半周期
	U8 reserved1;   //预留1
}SRAWX1;

typedef struct
{
	F64 rcvTow;                    //本地周时间
	U16 week;                      //本地GPS周数
	S8 leaps;                      //GPS跳跃秒
	U8 numMeas;                    //测量值数（消息数）
	U8 leapSec;                    //已确定跳跃秒
	U8 clkReset;                   //应用时钟重置
	U8 version;                    //消息版本
	U8 reserved0[2];               //预留0
	SRAWX1 aSRAWX[MAX_RAWX_NUM];   //每个卫星的消息
}SRAWX;



//const U8* pData          输入：待解析的原始数据
//U32 dataLen              输入：传入数据的长度
//SSFRBX* pSFRBX           输出：解析数据到结构体中
BOOL GetSFRBXPtl(const U8* pData, U32 dataLen, SSFRBX* pSFRBX);

//const U8* pData          输入：待解析的原始数据
//U32 dataLen              输入：传入数据的长度
//SRAWX* pRAWX             输出：解析数据到结构体中
BOOL GetRAWXPtl(const U8* pData, U32 dataLen, SRAWX* pRAWX);

//const U8* pData          输入：待解析的原始数据
//U32 dataLen              输入：传入数据的长度
//SDzNavInfo* pDzNavInfo   输出：解析数据到结构体中
BOOL GetDzPtl(const U8* pData, U32 dataLen, SDzNavInfo* pDzNavInfo);
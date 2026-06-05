#ifndef _ERRORCACULATE_H_2022_
#define _ERRORCACULATE_H_2022_

#include "icetype.h"
#ifdef __cplusplus
extern "C" {
#endif
//pStdBlh  输入标准点坐标	单位：弧度
    //pUsrBlh  输入当前坐标	单位：弧度
    //pLatErr  纬度方向误差 北方为正 : 单位m
    //pLonErr  经度方向误差 东方为正 : 单位m
    //HeighErr 高程误差   单位m
    //lineerr  水平误差   单位m
	BOOL CalcPosErr(const F64* pStdBlh, const F64* pUsrBlh, F64* HeighErr, F64* lineerr, S32 m);//计算每次的水平误差和高程误差
	//f64Lat				ECEF坐标系下的纬度（弧度）
	//longitude				ECEF坐标系下的经度（弧度）
	//velocity_ECEF[3]		ECEF坐标系下的三维速度（m/s）
	//velocity_ENU[5]		前三个为vx, vy, vz东北天坐标下的三围速度（m / s）
	//velocityerr           为测速误差（m/s）
	BOOL velocityErr(F64 f64Lat, F64 f64Lon, const F64* pVelECEF, F64* velocityerr, S32 n);//计算每次的速度误差

#ifdef __cplusplus
}
#endif

#endif /* _ERRORCACULATE_H_2022_ */
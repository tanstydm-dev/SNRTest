#include "ErrorCalcute.h"
#include "NavCordTrans.h"
#include "math.h"
#include "NavConstant.h"

//pStdBlh  输入标准点坐标	单位：弧度
//pUsrBlh  输入当前坐标	单位：弧度
//pLatErr  纬度方向误差 北方为正 : 单位m
//pLonErr  经度方向误差 东方为正 : 单位m
//HeighErr 高程误差   单位m
//lineerr  水平误差   单位m
BOOL CalcPosErr(const F64* pStdBlh, const F64* pUsrBlh, F64* HeighErr, F64* lineerr, S32 m)//计算高程误差和水平误差
{
	int i;
	F64 pLatErr[3] = { 0 };
	F64 pLonErr[3] = { 0 };
	F64 pHeighErr[3] = { 0 };
	F64 radian1[3] = { 0 };
	F64 radian2[3] = { 0 };
	for (i = 0; i < 2; i++)
	{
		radian1[i] = pStdBlh[i] * PI / 180;
		radian2[i] = pUsrBlh[i] * PI / 180;
	}
	radian1[2] = pStdBlh[2];
	radian2[2] = pUsrBlh[2];
	Nav_CalcBlhHoriErr(radian1, radian2, pLatErr, pLonErr, pHeighErr);
	*lineerr = sqrt(pow(*pLatErr, 2) + pow(*pLonErr, 2)) / m + *lineerr * (m - 1) / m;
	*HeighErr = *pHeighErr / m + *HeighErr * (m - 1) / m;

	return TRUE;
}


//f64Lat				ECEF坐标系下的纬度（弧度）
//longitude				ECEF坐标系下的经度（弧度）
//velocity_ECEF[3]		ECEF坐标系下的三维速度（m/s）
//velocity_ENU[5]		前三个为vx, vy, vz东北天坐标下的三围速度（m/s）
//velocityerr           为测速误差（m/s）
BOOL velocityErr(F64 f64Lat, F64 f64Lon, const F64* pVelECEF, F64* velocityerr, S32 n)//计算测速误差
{
	F64 VelENU[5] = { 0 };
	if ((pVelECEF == NULL) || (velocityerr == NULL))
	{
		return FALSE;
	}
	f64Lat = f64Lat * PI / 180;
	f64Lon = f64Lon * PI / 180;
	Nav_VelFromECEFtoENU(f64Lat, f64Lon, pVelECEF, VelENU);
	*velocityerr = sqrt(pow(VelENU[0], 2) + pow(VelENU[1], 2) + pow(VelENU[2], 2)) / n + *velocityerr * (n - 1) / n;

	return TRUE;
}


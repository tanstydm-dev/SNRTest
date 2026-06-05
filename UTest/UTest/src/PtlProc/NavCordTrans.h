#ifndef NAV_CORDTRANS_H
#define NAV_CORDTRANS_H

#ifdef __cplusplus
extern "C"{
#endif

#include "IceType.h"

#ifndef f64
#define f64 double
#endif

#ifndef bool_t
#define bool_t int
#endif

#ifndef s32
#define s32 int
#endif
/**空间坐标形式XYZ转化为大地坐标BLH.(单位:米 转 弧度)
@param pXyz xyz坐标
@param pBlh blh坐标
@return DX_TRUE
*/
bool_t Nav_XyzToBlh(const f64 *pXyz, f64 *pBlh);

/**BLH 坐标转 XYZ坐标
@param pBlh BLH坐标(unit:rad)
@param pXyz 转换后的xyz坐标(unit:rad)
@return DX_TRUE
*/
bool_t Nav_BlhToXyz(const f64 *pBlh, f64 *pXyz);

/**计算卯酉曲率半径
@param f64Lat 纬度 单位：弧度
@return 卯酉曲率半径
*/
f64 Nav_CurvaRadN(f64 f64Lat);

/**速度转换为本地东北天坐标下的速度
@para f64Lat				ECEF坐标系下的纬度（弧度）
@para longitude				ECEF坐标系下的经度（弧度）
@para velocity_ECEF[3]		ECEF坐标系下的三维速度（m/s）
@para velocity_ENU[5]		为1*5的数组
							前三个为vx,vy,vz东北天坐标下的三围速度（m/s）
							第4个指示地速，理解为水平面的分量
							第5个水平方向的速度与北向的偏角，范围0至2Pi。
*/
bool_t Nav_VelFromECEFtoENU(f64 f64Lat, f64 f64Lon, const f64 *pVelECEF, f64 *pVelENU);


/** 由BLH坐标计算水平误差值
@param pStdBlh  输入标准点坐标	单位：弧度
@param pUsrBlh  输入当前坐标	单位：弧度
@param pLatErr  纬度方向误差 北方为正:单位m
@param pLonErr  经度方向误差 东方为正:单位m
@return DX_FALSE：输入异常 DX_TRUE：正常解算
*/
bool_t Nav_CalcBlhHoriErr(const f64 *pStdBlh, const f64 *pUsrBlh, f64 *pLatErr, f64 *pLonErr, f64 *pHeighErr);

/** 由BLH坐标计算水平误差值
@param pStdBlh  输入标准点坐标	单位：度
@param pUsrBlh  输入当前坐标	单位：度
@param pLatErr  纬度方向误差 北方为正:单位m
@param pLonErr  经度方向误差 东方为正:单位m
@return DX_FALSE：输入异常 DX_TRUE：正常解算
*/
bool_t Nav_CalcBlhHoriErrDegree(const f64* pStdBlhG, const f64* pUsrBlhG, f64* pLatErr, f64* pLonErr, f64* pHeighErr);

bool_t Nav_Pz90_To_Wgs84(const f64 *pPz90, f64 *pWgs84);

bool_t Nav_Pz90_To_Wgs84_Vel(const f64 *pPz90, f64 *pWgs84);

		
#ifdef __cplusplus
}
#endif

#endif

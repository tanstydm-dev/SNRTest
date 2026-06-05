#include <math.h>
#include "NavCordTrans.h"
#include "NavConstant.h"
#include "IceErrCtrl.h"

//#include <dxdbg.h>
#define CORDTRS_EPSILON (0.00000000000001)

#define DX_NULL 0
#define DX_TRUE 1
#define DX_FALSE 0


/**空间坐标形式XYZ转化为大地坐标BLH.(单位:米 转 弧度)
@param pXyz xyz坐标
@param pBlh blh坐标
@return DX_TRUE
*/
bool_t Nav_XyzToBlh(const f64 *pXyz, f64 *pBlh)
{
	f64 f64TempZi0 = 0.0;
	f64 f64TempZi = 0.0;
	f64 f64TempZiZ = 0.0;
	f64 f64NaddH = 0.0;
	f64 f64SinLat = 20.0;
	f64 f64CurvaRadN = 0.0;		/*卯酉圈曲率半径*/

	f64 f64TempLongi = 0.0;
	s32 i = 0;

	VERIFY_LOG_RETVAL((pXyz!=DX_NULL) && (pBlh!=DX_NULL), 0);

	f64TempZi = -EARFIRSTECCEN2*pXyz[2];
	for(i=0; i<10; i++)
	{
		f64TempZi0 = f64TempZi;
		f64TempZiZ = pXyz[2] - f64TempZi;
		f64NaddH = sqrt(((pXyz[0]*pXyz[0])+(pXyz[1]*pXyz[1]))+(f64TempZiZ*f64TempZiZ));
		if (f64NaddH < 1.0)
		{
			return DX_FALSE;
		}
		f64SinLat = f64TempZiZ/f64NaddH;
		f64CurvaRadN = EARSEMI_MAJOR_RAD/(sqrt(1.0-(EARFIRSTECCEN2*(f64SinLat*f64SinLat))));
		f64TempZi = -(f64CurvaRadN*EARFIRSTECCEN2)*f64SinLat;
		if( (fabs(f64TempZi-f64TempZi0)) < CORDTRS_EPSILON)
		{
			break;
		}
	}
	pBlh[0] = asin(f64TempZiZ/f64NaddH);
	pBlh[2] = f64NaddH - f64CurvaRadN;
	f64TempLongi = atan(pXyz[1]/pXyz[0]);
	pBlh[1] = f64TempLongi;

	/*经度范围(-PI)~(PI)*/
	if( (pXyz[0]<0.0) && (pXyz[1]>0.0))
	{
		pBlh[1] = f64TempLongi + PI;
	}
	if( (pXyz[0]<0.0) && (pXyz[1]<0.0) )
	{
		pBlh[1] = f64TempLongi - PI;
	}

	return DX_TRUE;
}

/**BLH 坐标转 XYZ坐标
@param pBlh BLH坐标(unit:rad)
@param pXyz 转换后的xyz坐标(unit:rad)
@return DX_TRUE
*/
bool_t Nav_BlhToXyz(const f64 *pBlh, f64 *pXyz)
{
	f64 f64CurvaRadN = 0.0; /*卯酉圈曲率半径N*/

	VERIFY_LOG_RETVAL(
			(pBlh!=DX_NULL)
		&&	(pXyz!=DX_NULL)
		&&	(fabs(pBlh[0])<=PI/2)
		&&	(fabs(pBlh[1])<=2*PI),
		DX_FALSE);

	f64CurvaRadN = Nav_CurvaRadN(pBlh[0]);

	pXyz[0] = ((f64CurvaRadN+pBlh[2])*cos(pBlh[1]))*cos(pBlh[0]);
	pXyz[1] = ((f64CurvaRadN+pBlh[2])*sin(pBlh[1]))*cos(pBlh[0]);
	pXyz[2] = ((f64CurvaRadN*(1.0-EARFIRSTECCEN2))+pBlh[2])*sin(pBlh[0]);

	return DX_TRUE;
}

/**计算卯酉曲率半径
@param f64Lat 纬度 单位：弧度
@return 卯酉曲率半径
*/
f64 Nav_CurvaRadN(f64 f64Lat)
{
	f64 f64CurvaRadN = 0.0; /*卯酉圈曲率半径N*/

	f64CurvaRadN = EARSEMI_MAJOR_RAD/(sqrt(1.0-(EARFIRSTECCEN2*(sin(f64Lat)*sin(f64Lat)))));

	return f64CurvaRadN;
}

/**速度转换为本地东北天坐标下的速度
@para f64Lat				ECEF坐标系下的纬度（弧度）
@para longitude				ECEF坐标系下的经度（弧度）
@para velocity_ECEF[3]		ECEF坐标系下的三维速度（m/s）
@para velocity_ENU[5]		为1*5的数组
							前三个为vx,vy,vz东北天坐标下的三围速度（m/s）
							第4个指示地速，理解为水平面的分量
							第5个水平方向的速度与北向的偏角，范围0至2Pi。
*/
bool_t Nav_VelFromECEFtoENU(f64 f64Lat, f64 f64Lon, const f64 *pVelECEF, f64 *pVelENU)
{
	f64 f64Grdspeed = 0.0; /**< 地速 */
	f64 f64Theta = 0.0; /*速度方向,北偏东*/
	f64 f64SinLat=0.0;
	f64 f64CosLat=0.0;
	f64 f64SinLon=0.0;
	f64 f64CosLon=0.0;

	VERIFY_LOG_RETVAL(
			(pVelECEF!=DX_NULL	    )
		&&	(pVelENU!=DX_NULL		),
		DX_FALSE);

	if((f64Lat>(PI/2))||(f64Lat<-(PI/2))||(f64Lon<-PI)||(f64Lon>PI))
	{
		//pVelENU[0]=0;
		//pVelENU[1]=0;
		//pVelENU[2]=0;
		//return DX_FALSE;
	}
	f64SinLat = sin(f64Lat);
	f64CosLat = cos(f64Lat);
	f64SinLon= sin(f64Lon);
	f64CosLon= cos(f64Lon);

	pVelENU[0]=(-f64SinLon*pVelECEF[0])+(f64CosLon*pVelECEF[1]);
	pVelENU[1]=(((-f64SinLat*f64CosLon)*pVelECEF[0])-((f64SinLat*f64SinLon)*pVelECEF[1]))+(f64CosLat*pVelECEF[2]);
	pVelENU[2]= (f64CosLat*f64CosLon*pVelECEF[0])+(f64CosLat*f64SinLon*pVelECEF[1])+(f64SinLat*pVelECEF[2]);

	/*增加地速输出(水平速度)*/
	f64Grdspeed=(pVelENU[0]*pVelENU[0])+(pVelENU[1]*pVelENU[1]);
	if (f64Grdspeed < 1e-6){
		pVelENU[0]=0;
		pVelENU[1]=0;
		//pVelENU[2]=0;/* 水平速度为0时，垂直速度不能清零 */
		pVelENU[3]=0;
		pVelENU[4]=0;
		return DX_TRUE;
	}
	pVelENU[3]=sqrt(f64Grdspeed);

	/*增加速度的方位角(北偏东)*/
	f64Theta=atan(pVelENU[0]/pVelENU[1]);

	if(pVelENU[1]<0)     /*北向速度小于0,处于第二三象限*/
	{
		f64Theta=f64Theta+PI;
	}else if((pVelENU[1]>0)&&(pVelENU[0]<0))   /*处于第四象限*/
	{
		f64Theta=f64Theta+(2*PI);
	}
	pVelENU[4]=f64Theta;

	return DX_TRUE;
}

/** 由BLH坐标计算水平误差值
@param pStdBlh  输入标准点坐标	单位：弧度
@param pUsrBlh  输入当前坐标	单位：弧度
@param pLatErr  纬度方向误差 北方为正:单位m
@param pLonErr  经度方向误差 东方为正:单位m
@return DX_FALSE：输入异常 DX_TRUE：正常解算
*/
bool_t Nav_CalcBlhHoriErr(const f64 *pStdBlh, const f64 *pUsrBlh, f64 *pLatErr, f64 *pLonErr, f64 *pHeighErr)
{
	f64 f64StdPos[3];	/**< 输入标准点的XYZ坐标 */
	f64 f64UsrPos[3];
	f64 f64ErrXyz[3];

	f64 f64ErrMatrix[3][3];
	f64 f64Lon;
	f64 f64Lat;
	s32 i;

	memset(f64StdPos, 0, sizeof(f64StdPos));
	memset(f64UsrPos, 0, sizeof(f64UsrPos));
	memset(f64ErrXyz, 0, sizeof(f64ErrXyz));
	memset(f64ErrMatrix, 0, sizeof(f64ErrMatrix));
	 
	VERIFY_LOG_RETVAL(
			(pStdBlh!=DX_NULL)
		&&	(pUsrBlh!=DX_NULL)
		&&	(pLatErr!=DX_NULL)
		&&	(pLonErr!=DX_NULL)
		&&	(pHeighErr!=DX_NULL),
		DX_FALSE);

	/* 从ECEF到ECI的误差转换矩阵 */
	f64Lat = pStdBlh[0];
	f64Lon = pStdBlh[1];

	f64ErrMatrix[0][0] = -cos(f64Lon)*sin(f64Lat);
	f64ErrMatrix[0][1] = -sin(f64Lon)*sin(f64Lat);
	f64ErrMatrix[0][2] = cos(f64Lat);

	f64ErrMatrix[1][0] = -sin(f64Lon);
	f64ErrMatrix[1][1] = cos(f64Lon);
	f64ErrMatrix[1][2] = 0.0;

	f64ErrMatrix[2][0] = cos(f64Lon)*cos(f64Lat);
	f64ErrMatrix[2][1] = sin(f64Lon)*cos(f64Lat);	
	f64ErrMatrix[2][2] = sin(f64Lat);

	/* 转换为ECEF坐标求单轴误差 */
	Nav_BlhToXyz(pStdBlh, f64StdPos);
	Nav_BlhToXyz(pUsrBlh, f64UsrPos);
	for (i=0;i<3;i++)
	{
		f64ErrXyz[i] = f64UsrPos[i] - f64StdPos[i];
	}

	/* 误差转换坐标 */
	*pLatErr = f64ErrMatrix[0][0]*f64ErrXyz[0] + f64ErrMatrix[0][1]*f64ErrXyz[1] + f64ErrMatrix[0][2]*f64ErrXyz[2];
	*pLonErr = f64ErrMatrix[1][0]*f64ErrXyz[0] + f64ErrMatrix[1][1]*f64ErrXyz[1] + f64ErrMatrix[1][2]*f64ErrXyz[2];
	*pHeighErr = pUsrBlh[2] - pStdBlh[2];

	return DX_TRUE;
}

/** 由BLH坐标计算水平误差值
@param pStdBlh  输入标准点坐标	单位：度
@param pUsrBlh  输入当前坐标	单位：度
@param pLatErr  纬度方向误差 北方为正:单位m
@param pLonErr  经度方向误差 东方为正:单位m
@return DX_FALSE：输入异常 DX_TRUE：正常解算
*/

bool_t Nav_CalcBlhHoriErrDegree(const f64* pStdBlhG, const f64* pUsrBlhG, f64* pLatErr, f64* pLonErr, f64* pHeighErr)
{
	f64 f64StdPos[3];	/**< 输入标准点的XYZ坐标 */
	f64 f64UsrPos[3];
	f64 f64ErrXyz[3];

	f64 pStdBlh[3] = { 0 };
	f64 pUsrBlh[3] = { 0 };

	f64 f64ErrMatrix[3][3];
	f64 f64Lon;
	f64 f64Lat;
	s32 i;

	memset(f64StdPos, 0, sizeof(f64StdPos));
	memset(f64UsrPos, 0, sizeof(f64UsrPos));
	memset(f64ErrXyz, 0, sizeof(f64ErrXyz));
	memset(f64ErrMatrix, 0, sizeof(f64ErrMatrix));

	VERIFY_LOG_RETVAL(
		(pStdBlh != DX_NULL)
		&& (pUsrBlh != DX_NULL)
		&& (pLatErr != DX_NULL)
		&& (pLonErr != DX_NULL)
		&& (pHeighErr != DX_NULL),
		DX_FALSE);

	pStdBlh[0] = pStdBlhG[0] * PI / 180;
	pStdBlh[1] = pStdBlhG[1] * PI / 180;
	pStdBlh[2] = pStdBlhG[2];

	pUsrBlh[0] = pUsrBlhG[0] * PI / 180;
	pUsrBlh[1] = pUsrBlhG[1] * PI / 180;
	pUsrBlh[2] = pUsrBlhG[2];
	/* 从ECEF到ECI的误差转换矩阵 */
	f64Lat = pStdBlh[0];
	f64Lon = pStdBlh[1];

	f64ErrMatrix[0][0] = -cos(f64Lon) * sin(f64Lat);
	f64ErrMatrix[0][1] = -sin(f64Lon) * sin(f64Lat);
	f64ErrMatrix[0][2] = cos(f64Lat);

	f64ErrMatrix[1][0] = -sin(f64Lon);
	f64ErrMatrix[1][1] = cos(f64Lon);
	f64ErrMatrix[1][2] = 0.0;

	f64ErrMatrix[2][0] = cos(f64Lon) * cos(f64Lat);
	f64ErrMatrix[2][1] = sin(f64Lon) * cos(f64Lat);
	f64ErrMatrix[2][2] = sin(f64Lat);

	/* 转换为ECEF坐标求单轴误差 */
	Nav_BlhToXyz(pStdBlh, f64StdPos);
	Nav_BlhToXyz(pUsrBlh, f64UsrPos);
	for (i = 0; i < 3; i++)
	{
		f64ErrXyz[i] = f64UsrPos[i] - f64StdPos[i];
	}

	/* 误差转换坐标 */
	*pLatErr = f64ErrMatrix[0][0] * f64ErrXyz[0] + f64ErrMatrix[0][1] * f64ErrXyz[1] + f64ErrMatrix[0][2] * f64ErrXyz[2];
	*pLonErr = f64ErrMatrix[1][0] * f64ErrXyz[0] + f64ErrMatrix[1][1] * f64ErrXyz[1] + f64ErrMatrix[1][2] * f64ErrXyz[2];
	*pHeighErr = pUsrBlh[2] - pStdBlh[2];

	return DX_TRUE;
}

bool_t Nav_Pz90_To_Wgs84(const f64 *pPz90, f64 *pWgs84)
{
	if (pPz90 == DX_NULL || pWgs84 == DX_NULL)
	{
		return DX_FALSE;
	}

	pWgs84[0] =  pPz90[0]-0.36;
	pWgs84[1] =	 pPz90[1]+0.08;
	pWgs84[2] =	 pPz90[2]+0.18;

	return DX_TRUE;
}

bool_t Nav_Pz90_To_Wgs84_Vel(const f64 *pPz90, f64 *pWgs84)
{
	if (pPz90 == DX_NULL || pWgs84 == DX_NULL)
	{
		return DX_FALSE;
	}

	pWgs84[0] = pPz90[0];
	pWgs84[1] =	 pPz90[1];
	pWgs84[2] =	 pPz90[2];

	return DX_TRUE;
}





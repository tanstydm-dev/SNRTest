/**
@file		navConstant.h 共用常数，与北斗无关
@author		xIANG wEi waterknows@gmail.com
@date 		2007/8/16   20:29
@version	v1.0 Copyright (c) 2007 - All Rights Reserved
@warning 	
*********************************************************************/

#ifndef NAV_CONSTANT_H_XW_20070816
#define NAV_CONSTANT_H_XW_20070816

//#include "NavType.h"
#define		PI					3.1415926535897931

/*CGS2000地球椭球参数----有待修正*/
#define		EARSEMI_MAJOR_RAD	6378137.0				/**< CGS2000地球椭球长半轴a-unit:m*/
#define		EARFLATTING			3.3528106e-3			/**< CGS2000地球椭球扁率f (1.0/298.257222101=0.00335281068118232)*/							
#define		EARSEMI_MINOR_RAD	6356752.3				/**< CGS2000地球椭球短半轴b-unit:m (EARSEMI_MAJOR_RAD*(1.0-EARFLATTING))*/
#define		UNIGRAVCONS			3.9860044e14			/**< CGS2000地球引力常数u-unit:m3/s2*/
#define		EARTHANGLEVEL		7.2921150e-5			/**< CGS2000地球自转角速度w-unit: rad/s*/
#define		EARFIRSTECCEN2		6.6943800e-3			/**< CGS2000地球椭球第一偏心率的平方e2 (EARFLATTING*(2.0-EARFLATTING))*/
#define		OMEGADOT			-2.6e-9					/* 升交点赤经变化率 rad/s */

#define		LIGHTVEL			299792458.0				/**< 真空中光速-unit:m/s*/
#define		DAYLONG				86400.0					/**< 每天的秒数-unit:s*/
#define		WEEKLONG			604800.0				/**< 每周的秒数-unit:s*/


/****************"2的X方" 的宏定义(替换已经确定的转换尺度)***********************/
/*星历*/
#define		POW2_POS_3					8.0
#define		POW2_NEG_8					3.90625e-3
#define		POW2_NEG_9					1.953125e-3
#define		POW2_NEG_33					1.1641532182693481e-010
#define		POW2_NEG_34					5.8207660913467407e-011
#define		POW2_NEG_50					8.8817841970012523e-016
#define		POW2_NEG_66					1.3552527156068805e-020
#define		POW2_NEG_19					1.9073486328125000e-006
#define		POW2_POS_32					4294967296

#define		POW2_NEG_13_PLUS_PI			3.8349519697141029e-004
#define		POW2_NEG_14_PLUS_PI	        1.9174759848570514e-004
#define		POW2_NEG_31_PLUS_PI			1.4629180792671596e-009
#define		POW2_NEG_32_PLUS_PI			7.3145903963357984e-010
#define		POW2_NEG_43_PLUS_PI			3.5715773419608389e-013
#define		POW2_NEG_44_PLUS_PI			1.7857886709804195e-013
#define		POW2_NEG_57_PLUS_PI			2.1799178112553949e-017
#define		POW2_NEG_31					4.6566128730773926e-010
#define		POW2_NEG_6					0.015625000000000000
#define		POW10_NEG_10				1.0e-010	/**< TGD使用 */
#define		POW10_NEG_1					0.1			/**< delta_T使用 */
#define		POW8_NEG_1					0.125		/**< IONGrid使用 */
#define		POW10_NEG_9                 0.000000001
#define		POW10_NEG_5				1.0e-005

/*历书*/
#define		POW2_NEG_21					4.7683715820312500e-007
#define		POW2_POS_12					4096.0
#define		POW2_POS_9					512
#define		POW2_NEG_19_PLUS_PI			5.9921124526782858e-006
#define		POW2_NEG_6_PLUS_PI			0.049087385212340625
#define		POW2_NEG_38_PLUS_PI			1.1429047494274685e-011
#define		POW2_NEG_11					0.00048828125000000000
#define		POW2_NEG_23_PLUS_PI			3.7450702829239286e-007
#define		POW2_NEG_20					9.5367431640625000e-007
#define		POW2_NEG_38					3.6379788070917130e-012

#define		POW2_NEG_16					0.0000152587890625
#define		POW2_NEG_4					0.0625
#define		POW2_NEG_37					7.27595761418342590e-12
#define		POW2_NEG_33_PLUS_PI			3.65729519816789920e-10
#define		POW2_NEG_15_PLUS_PI			9.58737992428525768e-5
/*电离层8参数模型*/
#define		POW2_NEG_30					9.3132257461547852e-010
#define		POW2_NEG_27				    7.450580596923828e-09
#define		POW2_NEG_24				    5.960464477539063e-08
#define		POW2_POS_14				    16384
#define		POW2_POS_16				    65536
#define		POW2_POS_17				    131072
#define		PLUS_7					    7
#define		POW2_POS_6				    64
#define		POW2_POS_11				    2048
#define		POW2_POS_14				    16384
#define		POW2_POS_15				    32768

//SBAS

//SBAS
#define		POW2_NEG_34_PLUS_PI						1.8286475990839496e-010
#define		POW2_NEG_22_PLUS_PI_PLUS_POW10_NEG_4		7.4901405658478575e-11
#define		POW2_NEG_21_PLUS_PI_PLUS_POW10NEG6_plus7pluspow6neg1	1.7476994653645000e-12
#define		POW2_NEG_39					1.8189894035458565e-012
#define     POW2_NEG_40					9.09494701772928237e-13
// #ifndef _REPLAY_SET_ 
// #define		POW2_NEG_11                 0.00048828125
// #endif
#define		POW2_NEG_12                 0.00048828125*0.5

#define		NAVBD2_FREQUENCY_B1			1561.098e6			/**< BD-2 B1频点 */
#define		NAVBD2_FREQUENCY_B2			1207.14e6			/**< BD-2 B2频点 */
#define		NAVBD2_FREQUENCY_B3			1268.52e6			/**< BD-2 B3频点 */
#define		NAVGPS_FREQUENCY_L1			1575.42e6
#define		NAVGPS_FREQUENCY_L2C		1227.6e6
#define		NAVGPS_FREQUENCY_G1			1602.00e6			/* 间隔0.56 */
#define		NAVGLO_FREQUENCY_G1_01		1602.00e6    //1598.0625e6				/**< 编号对应-8, 如1对应-7, 2对应-6 */
#define		NAVGLO_FREQUENCY_G1(satIndex) (NAVGLO_FREQUENCY_G1_01 + (satIndex-8)*0.5625e6)	/**< satIndex从1开始 */
#define		NAVBD3_FREQUENCY_B1			1575.42e6			/**< BD-3 B1C/B1A频点 */
#define		NAVBD3_FREQUENCY_B1A		1575.42e6			/**< BD-3 B1C/B1A频点 */
#define		NAVBD3_FREQUENCY_B2a		1176.45e6			/**< BD-3 B2a频点 */
#define		NAVBD3_FREQUENCY_B2b		1207.14e6			/**< BD-3 B2b频点 */
#define		NAVBD3_FREQUENCY_E1OS		1575.42e6			/**< Galileo E1os频点 */
#define		NAVBD3_FREQUENCY_E5a		1176.45e6			/**< Galileo E5a频点 */
#define		NAVBD3_FREQUENCY_E5b		1207.14e6			/**< Galileo E5b频点 */
#define		NAVBD3_FREQUENCY_L5			1176.45e6			/**< GPS L5频点 */


#define		NAVBD2_ION_B1_TO_B2			1.672418845159437	/**< 计算电离层误差时 用 (B1*B1)/(B2*B2) */
#define		NAVBD2_ION_B1_TO_B3			1.514487513007284	/**< 计算电离层误差时 用 (B1*B1)/(B3*B3) */
#define		NAVBD3_ION_B3_TO_B1		    0.648338674312700 //军用双频 k31 = (B3A*B3A)/(B1A*B1A)
#define		NAVBD3_ION_B1_TO_B2a		1.793270321361058 //民用双频 k12 = (B1C*B1C)/(B2a*B2a)
#define		NAVBD3_ION_B1_TO_B2b		1.703246193622522 /* 计算BD3电离层误差时用，(B1C*B1C)/(B2b*B2b) */
#define		NAVBD3_ION_B1_TO_B3			1.542403746097814 /* 计算BD3电离层误差时用，(B1C*B1C)/(B3*B3) */
#define		NAVGPS_ION_L1_TO_L5			1.793270321361059/* GPS ion (L1*L1)/(L5*L5) */
#define		NAVGPS_ION_L1_TO_L2			1.646944444444444/* GPS ion (L1*L1)/(L2*L2) */

//////////////////////////////////////////////////////////////////////////

#define MAX_USE_CHANNEL	(MAX_OBS_CNT)	//最大使用的卫星数量
#define	USE_SAT_NUM		(MAX_OBS_CNT)	/* 使用的最大卫星数 */

#define B1B3FrqCLK  422.8239263803681  /*B1B3频点接收机解算频差（ns）*/

#ifndef MAX_CHANNEL
#define MAX_CHANNEL		MAX_USE_CHANNEL
#endif
#define MAX_TRUSTPOS_NUM 5
#define	GLONASS_BASE_INDEX 80
#define GNSS_FRQ_NUM	(eFrqRNMax) /* 最大频点数 */
#define SOLVE_MODE_NUM	(3)// 解算模式 todo LZJ给的
#define USE_SOLVE_MODE_NUM 2 /* 最大使用的解算模式数量 */
#endif

 

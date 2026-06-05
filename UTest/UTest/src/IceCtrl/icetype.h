/**
@author   XiangWei 2005/3/30  11:36
@email    waterknows@gmail.com
@file     icetype.h
公用类型声明
*********************************************************************/

/*
modification history
--------------------
$Log: icetype.h,v $
Revision 1.4  2005/08/08 02:47:33  xb
加入网络

Revision 1.3.26.1  2005/08/05 09:29:07  lzj
协议理解又错误，需要重新对频点的窄带干扰的显示改动

Revision 1.3  2005/07/08 02:04:26  xb
6-24分支已合并

Revision 1.2.10.1  2005/06/28 16:23:42  xw
改正了帧提取时没有校验和的错误, 加上了帧类型判断函数.

*/

#ifndef ICETYPE_H_XW_2005_01_01_1
#define ICETYPE_H_XW_2005_01_01_1
#ifdef __cplusplus
extern "C"{
#endif

/* Typedefs for integer types */
typedef	unsigned char	U8;			/**<  unsigned 8 bit data  */
typedef	unsigned short	U16;		/**<  unsigned 16 bit data */
typedef	unsigned int	U32;		/**<  unsigned 32 bit data */
typedef	signed char		S8;			/**<  signed 8 bit data  */
typedef	signed short	S16;		/**<  signed 16 bit data */
typedef	signed int		S32;		/**<  signed 32 bit data */
typedef	float			F32;		/**<  unsigned 32 bit data  */
typedef	double			F64;		/**<  unsigned 64 bit data */
typedef	U8 *			P_U8;		/**<  unsigned 8 bit data  */
typedef	U16 *			P_U16;		/**<  unsigned 16 bit data */
typedef	U32 *			P_U32;		/**<  unsigned 32 bit data */
typedef	S8 *			P_S8;		/**<  signed 8 bit data  */
typedef	S16 *			P_S16;		/**<  signed 16 bit data */
typedef	S32 *			P_S32;		/**<  signed 32 bit data */

typedef char			S8_CHAR;

#ifdef WIN32
	typedef unsigned __int64 U64, *P_U64;
	typedef __int64 S64, *P_S64;
#else
	typedef unsigned long long U64, *P_U64;
	typedef long long S64, *P_S64;
#endif

#ifndef BOOL
	#ifdef WIN32
		typedef S32 BOOL;
	#else
		typedef U32 BOOL;
	#endif
#endif

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef NULL
	#define NULL 0
#endif



#define		ERR_NO_ERROR		0	/**< No error				*/
#define		ERR_NORAML_ERROR	-1	/**< Normal error			*/
#define		ERR_BAD_ARGS		-2	/**< Bad arguments			*/
#define		ERR_MEM_NONE		-4	/**< Memeroy is not enough  */
#define		ERR_FILE_ERROR		-3	/**< File error				*/
#define		ERR_ARR_OVERFLOW	-5	/**< Array bound overflow	*/
#define		ERR_TIME_OUT		-6	/**< Communication time out	*/

#ifdef __cplusplus
}
#endif


#endif/* ICETYPE_H_XW_2005_01_01_1*/

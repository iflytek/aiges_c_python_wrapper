/** 
 * @file    ifly_mspp_type.h
 * @brief   iFLY MSPP type Codes header file
 * 
 * This file contains the iFly MSPP type codes.
 * Copyright (C)    1999 - 2018 by IFLYTEK. Co.,LTD.
 *                  All rights reserved.
 * 
 * @author  MSPP Dept. iFLYTEK.
 * @version 2.0.0.2
 * @date    2018/10/11
 * 
 * @see        
 * 
 * @par History:
 * <table border=1>
 *  <tr> <th>version    <th>data    <th>author    <th>notes </tr>
 *  <tr> <td>2.0.0.2    <td>2018/10/11    <td>yfxie5    <td>Create this file  </tr>
 * </table>
 */

#ifndef _ifly_mspp_type_h__
#define _ifly_mspp_type_h__

/**
* C/C++ wchar_t support
*/
#ifdef __cplusplus
# include <cwchar>
#else  /* c */
# include <wchar.h>
#endif /* wchar */


#if defined(_MSC_VER)            /* Microsoft Visual C++ */
#if !defined(MSPPAPI)
#define MSPPAPI __stdcall
#endif
#pragma pack(push, 8)
#elif defined(__BORLANDC__)      /* Borland C++ */
#if !defined(MSPPAPI)
#define MSPPAPI __stdcall
#endif
#pragma option -a8
#elif defined(__WATCOMC__)       /* Watcom C++ */
#if !defined(MSPPAPI)
#define MSPPAPI __stdcall
#endif
#pragma pack(push, 8)
#else                            /* Any other including Unix */
#if !defined(MSPPAPI)
#define MSPPAPI
#endif
#endif


//引擎支持输入语音格式,与audio_format_names 对比相同
typedef enum 
{
	PCM_8K_16Bit_  = 0 ,
	ALAW_8K_8Bit_  = 1 ,
	ULAW_8K_8Bit_  = 2 ,
	PCM_16K_16Bit_ = 3 ,
	ALAW_16K_8Bit_ = 4 ,
	ULAW_16K_8Bit_ = 5 ,
    PCM_6K_16Bit_  = 6 ,  //6k*16bit的PCM数据，AHYD新增
	WAVE_TYPE_END  = 7 ,  //语音类型结束值，为了防止传入枚举之外的值，后续如果增加新类型，此值需要增加
}MsppWaveType;

typedef enum
{
	MSPP_AUDIO_BEGIN     = 0,     /* indicate data input begin */
	MSPP_AUDIO_END       = 1,     /* the end of input data */
	MSPP_AUDIO_CONTINUE  = 2,     /* input data is continuing */
	MSPP_AUDIO_FULL      = 3,     /* data of special type is full */
} MsppAudioStatus;

/***********新接口feature存储结构yfxie2018-9-18***********/

//特征名称
typedef enum
{
	FEA_DIF_PLP_39		= 0,
	FEA_DIF_PNCC_39		= 1,
	FEA_DIF_FB_48       = 2,
	FEA_STATIC_FB_64	= 3,
	FEA_STATIC_SDC56    = 4 //语种20181120
}FeaType;//MsppFeaName

//二级特征头结构
typedef struct SingleFeatureHeadInfo
{
	//char fea_type[16];
	FeaType 		fea_type;		//特征名称
	unsigned int	feature_size;	//特征数据字节数
	const void *	feature_address;//特征数据地址
	char			reserved[16];	//保留参数
}SingleFeaHeadInfo;

//一级特征头结构
typedef struct TotalMsppHeadInfo
{
	char	engine_version[16];//引擎版本号
	int		feature_num;	//特征个数
	int		feature_head_size;//二级特征头结构体字节数
	SingleFeaHeadInfo *fea_head;//二级特征头结构体变量,以feature_num为大小
	char	reserved[16];//保留参数
}MsppHeadInfo;
/*********************************************************/

#endif // ifly_mspp_type_h__

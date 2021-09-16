/**
 * @file    ifly_mspp_api.h
 * @brief   iFLY MSPP SDK header file
 *
 * This file contains the iFly MSPP(multi-system preprocessing) application programming interface(API) declarations
 * of iFLYTEK. MSPP developer can include this file in your project to build applications.
 * For more information, please read the developer guide.
 * Copyright (C)    1999 - 2017 by IFLYTEK. Co.,LTD.
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
 *  <tr> <td>2.0.0.0    <td>2018/9/20    <td>yfxie5    <td>Create this file  </tr>
 *  <tr> <td>2.0.0.1    <td>2018/9/25    <td>yfxie5    <td>rename mspp.cfu mfvpp.cfu  </tr>
 *  <tr> <td>2.0.0.2    <td>2018/10/11   <td>yfxie5    <td>solve bugs  </tr>
 * </table>
 */

#ifndef _ifly_mspp_api_h__
#define _ifly_mspp_api_h__


#include "ifly_mspp_type.h"
#include "ifly_mspp_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IflyMsppSafe{ void* internal_; } MsppSafe, *Mspp_INST;

//__attribute((visibility("default")))
//************************************
// Method:    MsppInitialize
// FullName:  MsppInitialize
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 引擎初始化
// Parameter: const wchar_t * engine 引擎路径
// Parameter: char * conf_dir 配置文件目录
//************************************
int MSPPAPI MsppInitialize (const wchar_t* engine, char* conf_dir);
typedef int (MSPPAPI *Proc_MsppInitialize)(const wchar_t* engine, char* conf_dir);

//************************************
// Method:    MsppCreateInst
// FullName:  MsppCreateInst
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 创建一个MSPP实例
// Parameter: Mspp_INST * vst 引擎实例
// Parameter: const wchar_t * params 暂不支持
//************************************
int MSPPAPI MsppCreateInst(Mspp_INST* vst, const wchar_t* params);
typedef int (MSPPAPI *Proc_MsppCreateInst)(Mspp_INST* vst, const wchar_t* params);

//************************************
// Method:    MsppSetParam
// FullName:  MsppSetParam
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 设置引擎参数
// Parameter: Mspp_INST vst
// Parameter: const wchar_t * param
// Parameter: const wchar_t * value
//************************************
int MSPPAPI MsppSetParam(Mspp_INST vst, const wchar_t* param, const wchar_t* value);
typedef int (MSPPAPI *Proc_MsppSetParam)(Mspp_INST vst, const wchar_t* param, const wchar_t* value);

//************************************
// Method:    MsppGetParam
// FullName:  MsppGetParam
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 获取引擎参数
// Parameter: Mspp_INST vst
// Parameter: const wchar_t * param
// Parameter: wchar_t * value
// Parameter: unsigned int * len
//************************************
int MSPPAPI MsppGetParam(Mspp_INST vst, const wchar_t* param, wchar_t* value, unsigned int* len);
typedef int (MSPPAPI *Proc_MsppGetParam)(Mspp_INST vst, const wchar_t* param, wchar_t* value, unsigned int* len);

//************************************
// Method:    MsppAudioWrite
// FullName:  MsppAudioWrite
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 写入语音到引擎中
// Parameter: Mspp_INST vst
// Parameter: MsppWaveType wav_type
// Parameter: const void * data
// Parameter: unsigned int len_bytes
// Parameter: MsppWaveType * status
//************************************
int MSPPAPI MsppAudioWrite(Mspp_INST vst, MsppWaveType wav_type ,const void* data, unsigned int len_bytes, MsppAudioStatus* status);
typedef int (MSPPAPI *Proc_MsppAudioWrite)(Mspp_INST vst, MsppWaveType wav_type ,const void* data, unsigned int len_bytes, MsppAudioStatus* status);

//************************************
// Method:    MsppGetXmlResult
// FullName:  MsppGetXmlResult
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 结果输出函数，输出结果为XML类型
// Parameter: Mspp_INST vst
// Parameter: const char * * result
// Parameter: unsigned int * len_bytes
int MSPPAPI MsppGetXmlResult(Mspp_INST vst, const void** result, unsigned int* len_bytes);
typedef int (MSPPAPI *Proc_MsppGetXmlResult)(Mspp_INST vst, const void** result, unsigned int* len_bytes);


/*********************************************************/
//声纹新接口函数yfxie2018-9-18
//************************************
// Method:    MsppGetFeature
// FullName:  MsppGetFeature
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 获取前端plp、pncc、fb48和fb64特征
// Parameter: Mspp_INST vst 引擎实例
// Parameter: int cluseter_idx VAD分割ID
// Parameter: void **mspp_fea 结构体指针
//************************************
//int MSPPAPI MsppGetFeature(Mspp_INST vst, int cluseter_idx,  MsppHeadInfo **mspp_fea);//MsppGetFeature
int MSPPAPI MsppGetFeature(Mspp_INST vst, int cluseter_idx,  void **mspp_fea);//MsppGetFeature
typedef int (MSPPAPI *Proc_MsppGetFeature)(Mspp_INST vst, int cluseter_idx, void **mspp_fea);


//语种新接口函数yfxie2018-9-19
//int MSPPAPI MsppGetLIDFeature(Mspp_INST vst, int cluseter_idx, MSPPHead *mspp_lid_fea);
//typedef int (MSPPAPI *Proc_MSPPGetFeature)(Mspp_INST vst, int cluseter_idx, MSPPHead *mspp_lid_fea);
/*********************************************************/

//************************************
// Method:    MsppGetAudioData
// FullName:  MsppGetAudioData
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 获取对应目标说话人的语音结果
// Parameter: Mspp_INST vst
// Parameter: int cluseter_idx
// Parameter: const void **pp_data
// Parameter: unsigned int *len_bytes
//************************************
int MSPPAPI MsppGetAudioData(Mspp_INST vst, int cluseter_idx, const void **pp_data, unsigned int *len_bytes);
typedef int (MSPPAPI *Proc_MsppGetAudioData)(Mspp_INST vst, int cluseter_idx, const void **pp_data, unsigned int *len_bytes);//2018-9-19yfxie

//************************************
// Method:    MsppDestroyInst
// FullName:  MsppDestroyInst
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 销毁实例
// Parameter: Mspp_INST vst
//************************************
int MSPPAPI MsppDestroyInst(Mspp_INST vst);
typedef int (MSPPAPI *Proc_MsppDestroyInst)(Mspp_INST vst);

//************************************
// Method:    MsppUninitialize
// FullName:  MsppUninitialize
// Access:    private 
// Returns:   int MSPPAPI
// Qualifier: 引擎逆初始化
//************************************
int MSPPAPI MsppUninitialize();
typedef int (MSPPAPI *Proc_MsppUninitialize)();

#ifdef __cplusplus
}
#endif
#endif // _ifly_mspp_api_h__

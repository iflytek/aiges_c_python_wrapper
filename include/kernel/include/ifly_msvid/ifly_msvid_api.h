#pragma once
#ifndef __IFLY_MSVID_API_H__
#define __IFLY_MSVID_API_H__

#include "ifly_msvid_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
			MSVID API type
*/
#if defined(_MSC_VER)            /* Microsoft Visual C++ */
#if !defined(MSVIDAPI)
#define MSVIDAPI __stdcall
#endif
#pragma pack(push, 8)
#elif defined(__BORLANDC__)      /* Borland C++ */
#if !defined(MSVIDAPI)
#define MSVIDAPI __stdcall
#endif
#pragma option -a8
#elif defined(__WATCOMC__)       /* Watcom C++ */
#if !defined(MSVIDAPI)
#define MSVIDAPI __stdcall
#endif
#pragma pack(push, 8)
#else                            /* Any other including Unix */
#if !defined(MSVIDAPI)
#define MSVIDAPI
#endif
#endif

//��ʼ��
int MSVIDAPI MsvidInitialize(const wchar_t* engine, char* conf_dir);
typedef int (MSVIDAPI *Proc_MsvidInitialize)(const wchar_t* engine, char* conf_dir);

//���ʼ��
int MSVIDAPI MsvidUninitialize(void);
typedef int (MSVIDAPI *Proc_MsvidUninitialize)(void);

//����ʵ��
int MSVIDAPI MsvidCreateInst(MSVID_INST* inst);
typedef int (MSVIDAPI *Proc_MsvidCreateInst)(MSVID_INST * inst);

//����ʵ��
int MSVIDAPI MsvidDestroyInst(MSVID_INST inst);
typedef int (MSVIDAPI *Proc_MsvidDestroyInst)(MSVID_INST inst);

//д����
//int MSVIDAPI MsvidWriteFeature(MSVID_INST inst, MsppHeadInfo *feature, MsvidFeatureStatus status);
int MSVIDAPI MsvidWriteFeature(MSVID_INST inst, void *feature, MsvidFeatureStatus status);
typedef int (MSVIDAPI *Proc_MsvidWriteFeature)(MSVID_INST inst, void *feature, MsvidFeatureStatus status);

//���ivector
int MSVIDAPI MsvidGetIvector(MSVID_INST inst, void **ivector, unsigned int *len);
typedef int (MSVIDAPI *Proc_MsvidGetIvector)(MSVID_INST inst, void **ivector, unsigned int *len);

int MSVIDAPI MsvidGetSpkInfo(MSVID_INST inst, void *ivector, unsigned int vec_len, void** result, unsigned int *rslt_len, char **label);
typedef int (MSVIDAPI *Proc_MsvidGetSpkInfo)(MSVID_INST inst,  void *ivector, unsigned int vec_len, void** result, unsigned int *rslt_len, char **label);

//дivector
int MSVIDAPI MsvidWriteIvector(MSVID_INST inst, void *ivector, unsigned int len, MsvidIvectorStatus status);
typedef int (MSVIDAPI *Proc_MsvidWriteIvector)(MSVID_INST inst, void *ivector, unsigned int len, MsvidIvectorStatus status);

//���ƽ��ivector
int MSVIDAPI MsvidGetAverageIvector(MSVID_INST inst, void **ivector, unsigned int *len);
typedef int (MSVIDAPI *Proc_MsvidGetAverageIvector)(MSVID_INST inst, void **ivector, unsigned int *len);

//����
int MSVIDAPI MsvidPostProcess(MSVID_INST inst, void *model_in, unsigned int len_in, void **model_out, unsigned int *len_out);
typedef int (MSVIDAPI *Proc_MsvidPostProcess)(MSVID_INST inst, void *model_in, unsigned int len_in, void **model_out, unsigned int *len_out);

//��������
int MSVIDAPI MsvidFeaClassify(MSVID_INST inst, void *fb,  void *feature , void **fea_out1, void **fea_out2 );
typedef int(MSVIDAPI *Proc_MsvidFeaClassify)(MSVID_INST inst, void *fb,  void *feature , void **fea_out1, void **fea_out2 );

//linux���޷��鿴�汾��Ϣ���ʼ���˺�����product_version�ռ����ں����⿪��
int MSVIDAPI MsvidGetVersion(char* product_version);
typedef int(MSVIDAPI *Proc_MsvidGetVersion)(char* product_version);

//define GET_SCORE
#ifdef GET_SCORE
//�÷�
int MSVIDAPI MsvidGetScore(MSVID_INST inst, void * src_model, void *target_model,unsigned int len, PkScore *score);
typedef int(MSVIDAPI *Proc_MsvidGetScore)(MSVID_INST inst, void * src_model, void *target_model,unsigned int len, PkScore *score);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __IFLY_MSVID_API_H__ */

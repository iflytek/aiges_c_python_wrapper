#ifndef cnn_vid_api_h_ 
#define cnn_vid_api_h_

#include "cnn_vid_errors.h"

namespace cnnvid_spksep{
typedef void *CnnInst;
#ifdef WIN32
#define __EXPORT __declspec(dllexport)  
#else
#define __EXPORT 
#endif 

#ifdef __cplusplus
extern "C" {
#endif

__EXPORT int CnnVidInitialize_SpkSep(const wchar_t* engine, char* conf_dir);
typedef int (*Proc_CnnVidInitialize_SpkSep)(const wchar_t* engine, char* conf_dir);

__EXPORT int CnnVidUninitialize_SpkSep();
typedef int (*Proc_CnnVidUninitialize_SpkSep)();

__EXPORT int CnnVidCreateInst_SpkSep(CnnInst *inst);
typedef int (*Proc_CnnVidCreateInst_SpkSep)(CnnInst *inst);

__EXPORT int CnnVidDestroyInst_SpkSep(CnnInst inst);
typedef int (*Proc_CnnVidDestroyInst_SpkSep)(CnnInst inst);

__EXPORT int CnnVidWriteFeature_SpkSep(CnnInst inst, void *feature, int feature_size,  int* index_array, int num);
typedef int (*Proc_CnnVidWriteFeature_SpkSep)(CnnInst inst, void *feature, int feature_size, int* index_array, int num);

__EXPORT int CnnVidGetIvector_SpkSep(CnnInst inst, void **ivector, unsigned int *len, int **out_array, int *out_dim);
typedef int (*Proc_CnnVidGetIvector_SpkSep)(CnnInst inst, void **ivector, unsigned int *len, int **out_array, int *out_dim);

__EXPORT int CnnVidPostproc_SpkSep(CnnInst inst, void *ivector, unsigned int len, void ** ovector, unsigned int *out_len);
typedef int (*Proc_CnnVidPostproc_SpkSep)(CnnInst inst, void *ivector, unsigned int len, void ** ovector, unsigned int *out_len);

__EXPORT int CnnVidGetScore_SpkSep(CnnInst inst, void *srcmodel,void *dstmodel,float* score);
typedef int (*Proc_CnnVidGetScore_SpkSep)(CnnInst inst, void *srcmodel,void *dstmodel,float* score);


//dlopen调用，找到函数指针（dlopen调用demo）
typedef struct CnnVidApi_SpkSep
{
	Proc_CnnVidInitialize_SpkSep			CnnVidInitialize_SpkSep_;
	Proc_CnnVidUninitialize_SpkSep			CnnVidUninitialize_SpkSep_;
	Proc_CnnVidCreateInst_SpkSep			CnnVidCreateInst_SpkSep_;
	Proc_CnnVidDestroyInst_SpkSep			CnnVidDestroyInst_SpkSep_;
	Proc_CnnVidWriteFeature_SpkSep			CnnVidWriteFeature_SpkSep_;
	Proc_CnnVidGetIvector_SpkSep			CnnVidGetIvector_SpkSep_;
    Proc_CnnVidPostproc_SpkSep             CnnVidPostproc_SpkSep_;
    Proc_CnnVidGetScore_SpkSep             CnnGetScore_SpkSep_;
};

__EXPORT const CnnVidApi_SpkSep*  GetApi();
typedef const CnnVidApi_SpkSep* (*Proc_GetApi)();

#ifdef __cplusplus
} /* extern "C" */
#endif
}
#endif


#ifndef __MODEL_DECRYPT_H__
#define __MODEL_DECRYPT_H__

#include "iflyTypeDef.h"

#if defined(_MSC_VER)                 /* Microsoft Visual C++ */
#	if !defined(DECRYPTAPI)
#		define DECRYPTAPI __stdcall
#		define DECRYPTAPITYPE __stdcall
#	endif
#	pragma pack(push, 8)
#else                                /* Any other including Unix */
#	if !defined(DECRYPTAPI)
#		define DECRYPTAPI  __attribute__ ((visibility("default")))
#		define DECRYPTAPITYPE
#	endif
#endif

#ifdef __cplusplus 
extern "C"{
#endif

	//获取加解密组件的版本信息
	//ifly_int32_t DECRYPTAPI iDecryptGetVersionV20190001(void); 

	//执行解密操作
	ifly_int32_t DECRYPTAPI iDecryptDoProcessV20190001(const ifly_int8_t *Key, ifly_uint32_t KeySize, const void *inRes, ifly_uint64_t resLen, void *outRes );

#ifdef __cplusplus
}
#endif

/* Reset the structure packing alignments for different compilers. */
#if defined(_MSC_VER)                /* Microsoft Visual C++ */
#pragma pack(pop)
#endif

#endif //__MODEL_DECRYPT_H__
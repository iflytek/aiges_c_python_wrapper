#pragma once
#ifndef __IFLY_MSVID_ERRORS_H__
#define __IFLY_MSVID_ERRORS_H__
#include <memory.h>
//错误码
enum ErrorCode{
	MSVID_SUCCESS                               = 0,  	
	MSVID_CONFIGFILE_NOT_EXIT                   = 1000001,
	VIDPK_CONFIGFILE_NOT_EXIT                   = 1000002,
	MSVID_CONFIG_ERROR                          = 1000003,	
	MSVID_OUT_OF_MEMORY                         = 1000004,
	MSVID_INVALID_HANDLE                        = 1000005,	
	MSVID_INITIALIZE_FAIL                       = 1000006,	
	MSVID_AMLPRESOURCEADD_FAIL                  = 1000007,	
	MSVID_CREATE_INST_FAILED                    = 1000008,	
	MSVID_GET_IVECTOR_FAILED                    = 1000009,
	MSVID_VIDPKWRITEFEATURE_FAILED              = 1000010,	
	MSVID_POST_PROCESS_FAILED                   = 1000011,	
	MSVID_GET_SCORE_FAILED                      = 1000012,	
	MSVID_GET_FEACLASSIFY_FAILED                = 1000013,	
	MSVID_DESTORY_INST_FAILED                   = 1000014,	
	MSVID_UNINITIALIZE_FAIL                     = 1000015,
	MSVID_ERROR_NO_LICENSE				    	= 1000016,
	MSVID_ERROR_FEATURE_NO_BEGIN				= 1000017,
	MSVID_ERROR_FEATURE_END_ERROR				= 1000018,
	MSVID_ERROR_ONLY_FOR_CNN					= 1000019,
	MSVID_ERROR_PARAM_NULL						= 1000020,
	MSVID_ERROR_FEATURE_INPUT_NULL				= 1000021,
	MSVID_ERROR_FEATURE_VERSION                 = 1000022,
	MSVID_ERROR_FEATURE_MISS                    = 1000023,
    MSVID_ERROR_FEATURE_ALREADY_BEGIN           = 1000024,
    MSVID_ERROR_STRATEGY_NUMBER_EXCEED          = 1000025,
	MSVID_ERROR_IVECTOR_NORM                    = 1000026,
	MSVID_ERROR_MODEL_NOT_EXIT                  = 1000027,
	MSVID_ERROR_MODEL_SYSTEM_ERROR              = 1000028,
};

typedef struct MsvidVerifier { void * internal_; } MsvidVerifier, *MSVID_INST;

typedef enum tagMsvidIvectorStatus
{
	MSVID_INPUT_IVECTOR_BEGIN		= 0,
	MSVID_INPUT_IVECTOR_END		    = 1,
	MSVID_INPUT_IVECTOR_CONTINUE	= 2,
}MsvidIvectorStatus;

typedef enum tagMsvidFeatureStatus
{
	MSVID_INPUT_FEATURE_BEGIN		= 0,
	MSVID_INPUT_FEATURE_END		    = 1,
	MSVID_INPUT_FEATURE_CONTINUE	= 2,
	//PK_INPUT_FEATURE_FULL		    = 3
}MsvidFeatureStatus;

//后处理策略
enum PostTypeItem{
	none,
	lda_wccn,
	plda,
};

//ivector技术路线
enum IvecTypeItem{
	plp_tv = 0,
	plp_dnn_tv,
	pncc_tv,
	pncc_dnn_tv,
	cnn,
};

//tracing
enum TracingLev{
    close_lev = 0,
    overall_lev,
    local_lev,
          };
//二级ivector头结构
struct IvecHead{
	IvecTypeItem ivec_type;	//ivec技术路线，plp-tv、plp-dnn-tv、pncc-tv、pncc-dnn-tv、cnn等
	unsigned short offset; //相对于数据域的偏移
	unsigned short len;	//模型长度
	PostTypeItem post_type;
	char fixInfo; //用于ivec定标，最高位用于判断是否为浮点，低7位存储标值信息
	unsigned char reserve[3]; //保留字段，留作备用
	IvecHead()
	{
		offset = 0;
		len = 0;
		post_type = none;
		fixInfo = 0;
		memset(reserve, 0, 3);
	}
};

//一级ivector头结构
struct TotalHead{
	unsigned short num_ivec;		//提取声纹模型，采用了多少种策略
	unsigned short len_ivec_data;	//数据域大小
	unsigned short len_head;		//头大小，方便平台使用
	char md5_code[2];	//md5码
	TotalHead()
	{
		num_ivec = 0;
		len_ivec_data = 0;	
		len_head = 0;	
		memset(md5_code, 0, 2);
	}
	//返回头的大小，用于len_head赋值，方便平台使用
	unsigned short get_head_len()
	{
		return sizeof(unsigned short) * 3 + sizeof(char) * 2 + num_ivec * sizeof(IvecHead);
	}
};

typedef struct PkIvectorHead_new{
	TotalHead p_head;
	IvecHead* p_ivec;
};

#ifdef GET_SCORE
typedef struct PkScore{
	float score_plp_simple_tv;
	float score_pncc_simple_tv;
	float score_plp_dnn_tv;
	float score_pncc_dnn_tv;
	//PostType post_type;			//post_type为枚举类型
	PostTypeItem post_type_simple_plp_tv;
	PostTypeItem post_type_simple_pncc_tv;
	PostTypeItem post_type_dnn_plp_tv;
	PostTypeItem post_type_dnn_pncc_tv;
	PkScore()
	{
		score_plp_simple_tv = 0;
		score_pncc_dnn_tv = 0;
		score_plp_dnn_tv = 0;
		score_pncc_dnn_tv = 0;
	}
};
#endif

#endif /* __IFLY_MSVID_ERRORS_H__ */

#ifndef cnn_vid_errors_h_
#define cnn_vid_errors_h_



enum CnnErrorCode{
	CNN_SUCCESS                             = 0, 
    CNN_CONFIG_ERROR                        = 910000, 	
	CNN_RES_ERROR							= 910001,
	CNN_CREATE_INST_ERROR					= 910002,
	CNN_DESTROY_INST_ERROR					= 910003,
	CNN_FEATURE_IS_NOT_FLOAT				= 910004,
	CNN_FEATURE_DIM_ERROR					= 910005,
	CNN_INPUT_NULL_ERROR					= 910006,
	CNN_INST_UNVALID_ERROR					= 910007,
	CNN_NO_FEATURE_INPUT					= 910008,
	CNN_INIT_TIMES_ERROR					= 910009,
	CNN_NO_INIT								= 910010,
	CNN_INITIALIZE_ERROR					= 910011,
	CNN_FEATURE_ADAPT_ERROR                 = 910012,
	GPU_OUT_RANGE                           = 910013,
};

enum FeatureStatus
{
	Feature_Begin,
	Feature_Continue,
	Feature_End,
};

#endif
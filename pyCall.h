#ifndef PY_CALL_H
#define PY_CALL_H
// log
#include "include/spdlog/include/spdlog/spdlog.h"
#include "include/spdlog/include/spdlog/sinks/rotating_file_sink.h"

// utils
#include "include/utils/utils.h"
#include "include/utils/json.hpp"
#include "include/aiges/type.h"
#include "pyParam.h"

const char * callWrapperError(int errNum);
int callWrapperInit(pConfig cfg);
int callWrapperFini();
int callWrapperExec(const char* usrTag, pParamList params, pDataList reqData, pDataList* respData, unsigned int psrIds[], int psrCnt);
#endif
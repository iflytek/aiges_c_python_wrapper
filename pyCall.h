#ifndef PY_CALL_H
#define PY_CALL_H
// log
#include "include/spdlog/include/spdlog/spdlog.h"
#include "include/spdlog/include/spdlog/sinks/rotating_file_sink.h"
#include "include/boost/algorithm/string.hpp"

// utils
#include "include/utils/utils.h"
#include "include/utils/json.hpp"
#include "include/aiges/type.h"
#include "pyParam.h"
#include<string.h>
std::string DATA_KEY="key";
std::string DATA_DATA="data";
std::string DATA_LEN="len";
std::string DATA_STATUS="status";
std::string DATA_TYPE="type";
std::string DATA_DESC="desc";
char* pyDictStrToChar(PyObject *obj, std::string itemKey, std::string sid,int& ret);

pDescList pyDictToDesc(PyObject* obj,std::string itemKey,std::string sid,int& ret);

int pyDictIntToInt(PyObject *obj, std::string itemKey, int &itemVal, std::string sid);

const char * callWrapperError(int errNum);
int callWrapperInit(pConfig cfg);
int callWrapperFini();
int callWrapperExec(const char* usrTag, pParamList params, pDataList reqData, pDataList* respData, unsigned int psrIds[], int psrCnt);
#endif
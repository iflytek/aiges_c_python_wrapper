#ifndef PY_CALL_H
#define PY_CALL_H
#include "include/python/Python.h"
// log
#include "include/spdlog/include/spdlog/spdlog.h"
#include "include/spdlog/include/spdlog/sinks/rotating_file_sink.h"

// utils
#include "include/utils/utils.h"
#include "include/utils/json.hpp"
int callWrapperInit();
int callWrapperFini();

int callWrapperExec();

#endif
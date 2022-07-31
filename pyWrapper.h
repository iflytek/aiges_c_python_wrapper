#ifndef __PYWRAPPER__
#define __PYWRAPPER__

#include "pybind11/embed.h"
#include "pybind11/stl.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include<map>
#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include "aiges/type.h"
#include "fswatch.h"

#define gettid() syscall(SYS_gettid)
#define PYBIND11_DETAILED_ERROR_MESSAGES

typedef struct TestDataList {
    char *key;            // 数据标识
    void *data;           // 数据实体
    unsigned int len;       // 数据长度
    struct TestDataList *next;  // 链表指针
} *pTestDataList;

namespace py = pybind11;
using namespace std::chrono_literals;

class ResponseData {
public:
    std::string key;
    std::string data;
    unsigned int len;
    int status;
    int type;
};

class Response {
public:
    std::vector <ResponseData> list;

};

class DataListNode {
public:
    std::string key;
//    std::string data;
    py::bytes data;
    unsigned int len;
    int type;

    py::bytes *get_data();
};

class DataListCls {
public:
    std::vector <DataListNode> list;
};

class Manager {
public:
    Manager();
};

class PyWrapper {
public:
    PyWrapper();

//    PyWrapper(std::map <std::string, std::string> config);

    ~PyWrapper();

    void StartMonitorWrapperClass(std::string wrapperFileAbs);

    void ReloadWrapper(); // for dynamic reload wrapper

    int wrapperInit(std::map <std::string, std::string> config);

    std::string wrapperError(int x);

    int wrapperOnceExec(std::map <std::string, std::string> params, DataListCls reqData, pDataList *respData,
                        std::string sid);

    int wrapperFini();

    int wrapperTest();

private:
    py::module_ _wrapper;
    std::string _wrapper_abs;
    py::object _obj;
    py::object _wrapperInit;
    py::object _wrapperFini;
    py::object _wrapperOnceExec;
    py::object _wrapperError;
    py::object _wrapperTest;

};


std::vector <std::map<std::string, std::string>> convertReqData(pTestDataList reqData, int dataNum);


#endif

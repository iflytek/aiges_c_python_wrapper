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
#include "spdlog/sinks/stdout_color_sinks.h"
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

// Session会话结create返回
class SessionCreateResponse {
public:
    std::string handle;
    int errCode;
};

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
    Response();

    Response(int errCode);

    std::unique_ptr <Response> responseErr(int err);

    std::vector <ResponseData> list;
    int errCode;


};

// 请求数据节点结构
class DataListNode {
public:
    std::string key;
    py::bytes data;
    unsigned int len;
    int status;
    int type;

    py::bytes get_data();
};

// 请求的数据结构 用以和pybind11 交互
class DataListCls {
public:
    std::vector <DataListNode> list;

    DataListNode *get(std::string key);
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

    std::string wrapperCreate(const char *usrTag, std::map <std::string, std::string> params, int *errNum, std::string sid);

    int wrapperWrite(char *handle, DataListCls reqData,std::string sid);
    int wrapperRead(char *handle, pDataList *respData,std::string sid);

    int wrapperTest();

private:
    py::module_ _wrapper;
    std::string _wrapper_abs;
    py::object _obj;
    py::object _wrapperInit;
    py::object _wrapperFini;
    py::object _wrapperOnceExec;
    py::object _wrapperError;
    py::object _wrapperCreate;
    py::object _wrapperWrite;
    py::object _wrapperRead;
    py::object _wrapperTest;

};

void SetHandleSid(char *handle, std::string sid);

std::string GetHandleSid(char *handle);

void DelHandleSid(char *handle);

#endif

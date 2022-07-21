#ifndef __PYWRAPPER__
#define __PYWRAPPER__

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"


typedef struct TestDataList{
    char*   key;            // 数据标识
    void*   data;           // 数据实体
    unsigned int len;       // 数据长度
    struct TestDataList* next;  // 链表指针
}*  pTestDataList;

namespace py = pybind11;
using namespace std::chrono_literals;



class PyWrapper
{
public:
  PyWrapper();
  ~PyWrapper();

  int wrapperInit(std::map<std::string, std::string> config);
  std::string wrapperError(int x);
  int wrapperOnceExec(std::map<std::string,std::string> params, std::vector<py::object> reqData, std::vector<py::object> respData, std::string sid); 
  int wrapperFini();
  int wrapperTest();

private:
  py::object _obj;
  py::object _wrapperInit; 
  py::object _wrapperFini;
  py::object _wrapperOnceExec; 
  py::object _wrapperError;
  py::object _wrapperTest;

};


std::vector<std::map<std::string, std::string>> convertReqData(pTestDataList reqData, int dataNum);


#endif

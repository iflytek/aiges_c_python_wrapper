#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include "pyWrapper.h"

namespace py = pybind11;
using namespace std::chrono_literals;


const char* WrapperFile = "main";
const char* WrapperClass = "Wrapper";

class Wrapper
{
public:
  Wrapper()
  {
    py::gil_scoped_acquire acquire;
//    py::module::import("sdk").attr("WrapperBase");
    _obj = py::module::import(WrapperFile).attr(WrapperClass)();

    _wrapperInit = _obj.attr("wrapperInit");
    _wrapperFini = _obj.attr("wrapperFini");
    _wrapperOnceExec = _obj.attr("wrapperOnceExec");
    _wrapperError = _obj.attr("wrapperError");

  }

  ~Wrapper()
  {
    _wrapperError.release();
    _wrapperInit.release();
    _wrapperFini.release();
    _wrapperOnceExec.release();
  }

  int wrapperInit(py::dict config)
  {
    py::gil_scoped_acquire acquire;
    return _wrapperInit(config).cast<int>();
  }

  std::string wrapperError(int x)
  {
    py::gil_scoped_acquire acquire;
    return _wrapperError(x).cast<std::string>();
  }

private:
  py::object _obj;
  py::object _wrapperInit; 
  py::object _wrapperFini;
  py::object _wrapperOnceExec; 
  py::object _wrapperError;

};





void thread_func(int iteration)
{
  Wrapper w;

  for (int i = 0; i < 4; i++)
  {
    w.wrapperError(i);
    w.wrapperInit();
    std::stringstream msg;
    msg << "iteration: " << iteration << " thread: " << std::this_thread::get_id() << " w.get_x(): " <<  std::endl;
    std::cout << msg.str();
  //  std::this_thread::sleep_for(100ms);
  }
}

int main() {
  py::scoped_interpreter python;
  py::gil_scoped_release release; // add this to release the GIL

  std::vector<std::thread> threads;

    threads.push_back(std::thread(thread_func, 1));

  for (auto& t : threads)
  {
    t.join();
  }

  std::cout << "olddd";

  return 0;
}

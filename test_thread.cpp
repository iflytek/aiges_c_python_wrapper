#include "pybind11/embed.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

namespace py = pybind11;
using namespace std::chrono_literals;

class Wrapper
{
public:
  Wrapper()
  {
    py::gil_scoped_acquire acquire;
    _obj = py::module::import("main2").attr("PythonClass")();
    _get_x = _obj.attr("get_x");
    _set_x = _obj.attr("set_x");

  }
  
  ~Wrapper()
  {
    _get_x.release();
    _set_x.release();
  }

  int get_x() 
  {
    py::gil_scoped_acquire acquire;
    return _get_x().cast<int>();
  }

  void set_x(int x)
  {
    py::gil_scoped_acquire acquire;
    _set_x(x);
  }

  private:
  py::object _obj;
  py::object _get_x;
  py::object _set_x;
};


void thread_func(int iteration)
{
  Wrapper w;

  for (int i = 0; i < 10; i++)
  {
    w.set_x(i);
    std::stringstream msg;
    msg << "iteration: " << iteration << " thread: " << std::this_thread::get_id() << " w.get_x(): " << w.get_x() << std::endl;
    std::cout << msg.str();
        std::this_thread::sleep_for(100ms);
  }
}

int main() {
  py::scoped_interpreter python;
  py::gil_scoped_release release; // add this to release the GIL

  std::vector<std::thread> threads;

  for (int i = 0; i < 5; ++i)
    threads.push_back(std::thread(thread_func, 1));

  for (auto& t : threads)
    t.join();

  return 0;
}


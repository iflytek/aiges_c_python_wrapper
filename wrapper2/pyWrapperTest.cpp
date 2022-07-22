
#include "pyWrapper.h"

void thread_func(int iteration) {
    PyWrapper w;

    for (int i = 0; i < 1; i++) {
        w.wrapperError(i);
        // w.wrapperTest()
        std::map <std::string, std::string> c;
        c.insert({"key", "cc"});
        w.wrapperInit(c);
        w.wrapperTest();
        std::stringstream msg;
        msg << "iteration: " << iteration << " thread: " << std::this_thread::get_id() << " w.get_x(): " << std::endl;
        std::cout << msg.str();
        //  std::this_thread::sleep_for(100ms);
    }
}


void ini() {
    std::cout << "ccccccccdddddddd" << std::endl;
//    py::scoped_interpreter python;

}
int main() {
  //  Manager *manager = new Manager();
    py::initialize_interpreter();
    py::gil_scoped_release release; // add this to release the GIL
    std::vector <std::thread> threads;

    threads.push_back(std::thread(thread_func, 1));

    for (auto &t: threads) {
        t.join();
    }

    std::cout << "olddd";

    return 0;
}

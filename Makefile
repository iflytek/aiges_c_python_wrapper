wrapper:
    g++ -fPIC -shared -std=c++11 -Wno-attributes -g -O0 -I. -I./include/spdlog/include -I./include/ -o libpyCall.so pyCall.cpp -L. -lpython
    g++ -fPIC -shared -std=c++11 -Wno-attributes -g -O0 -I. -I./include/spdlog/include -I./include/ -o libwrapper.so wrapper.cpp -L. libpyCall 
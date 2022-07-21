g++ -Wall  -fPIC  -I./include/spdlog/include/   pyWrapper.cpp  pyWrapperTest.cpp  -o main  `python3-config --cflags --ldflags --embed --libs`

#g++ -Wall  -fPIC    t.cpp  -o t  `python3-config --cflags --ldflags --libs`

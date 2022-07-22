g++ -Wall  -fPIC  -I.././include/spdlog/include/  embed_demo.cpp  -o tt  `python3-config --cflags --ldflags --embed --libs`

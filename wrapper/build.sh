g++ -Wall  -fPIC    pyWrapper.cpp  -o main  `python3-config --cflags --ldflags --libs`

g++ -Wall  -fPIC    t.cpp  -o t  `python3-config --cflags --ldflags --libs`

python_version_full := $(wordlist 2,4,$(subst ., ,$(shell python --version 2>&1)))
python_version_major := $(word 1,${python_version_full})
python_version_minor := $(word 2,${python_version_full})
python_version_patch := $(word 3,${python_version_full})


PY_GT_3_6 := $(shell [ $(python_version_major) -gt 3 -o \( $(python_version_major) -eq 3 -a $(python_version_minor) -ge 6 \) ] && echo true)

PYLIB=$(shell python3-config  --ldflags)
PYINC=$(shell python3-config --cflags)
ifeq ($(PY_GT_3_6),true)
PYLIB=$(shell python3-config  --ldflags --embed)
PYINC=$(shell python3-config --cflags --embed )
endif

wrapper:
	echo $(PYLIB)
	g++ -fPIC -shared -std=c++11 -Wno-attributes  $(PYINC)  -g -O0 -I. -I./include/spdlog/include -I./include/ -o libpyCallCommon.so pyCall_common.cpp -L. $(PYLIB)
	g++ -fPIC -shared -std=c++11 -Wno-attributes  $(PYINC) -g -O0 -I. -I./include/spdlog/include -I./include/ -o libpyCallOnce.so pyCall_once.cpp -L. -L /opt/conda/envs/loader/lib  $(PYLIB)
	g++ -fPIC -shared -std=c++11 -Wno-attributes  $(PYINC) -g -O0 -I. -I./include/spdlog/include -I./include/ -o libpyCallStream.so pyCall_stream.cpp -L. -L /opt/conda/envs/loader/lib   $(PYLIB)
	g++  -Wall -pedantic -Wextra -fPIC -shared -std=c++11 -Wno-attributes  $(PYINC) -g -O0 -I. -I./include/spdlog/include -I./include/ -o libwrapper.so wrapper.cpp -L. -L /opt/conda/envs/loader/lib -lboost_filesystem -lboost_system  -lpyCallCommon -lpyCallOnce -lpyCallStream $(PYLIB)
	mkdir -p wrapper_lib
	cp libwrapper.so libpyCallCommon.so libpyCallOnce.so libpyCallStream.so ./wrapper_lib

t:
	echo $(python_version_minor)
	echo $(PY_GT_3_6)
	rm -f test
	g++ -Wall  -fPIC  -I./include/spdlog/include/ $(PYINC) pyWrapper.cpp   pyWrapperTest.cpp -o test $(PYLIB) 

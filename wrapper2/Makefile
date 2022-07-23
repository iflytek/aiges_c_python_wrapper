python_version_full := $(wordlist 2,4,$(subst ., ,$(shell python --version 2>&1)))
python_version_major := $(word 1,${python_version_full})
python_version_minor := $(word 2,${python_version_full})
python_version_patch := $(word 3,${python_version_full})


PY_GT_3_6 := $(shell [ $(python_version_major) -gt 3 -o \( $(python_version_major) -eq 3 -a $(python_version_minor) -ge 6 \) ] && echo true)

PYLIB=$(shell python3-config  --ldflags)
PYINC=$(shell python3-config --cflags)
ifeq ($(PY_GT_3_6),true)
PYLIB=$(shell python3-config  --ldflags  --embed)
PYINC=$(shell python3-config --cflags --embed )
endif

wrapper:
	echo $(PYLIB)
	g++  -Wall -pedantic -Wextra -fPIC -shared  -std=c++1y  -fvisibility=default  -Wno-attributes  $(PYINC) -g -O0 -I. -I./include/spdlog/include/ -I./include/ -o libwrapper.so pyWrapper.cpp wrapper.cpp  -L. -L /opt/conda/envs/loader/lib -lboost_filesystem -lboost_system $(PYLIB) -rdynamic
	mkdir -p wrapper_lib
	cp libwrapper.so  ./wrapper_lib

t:
	echo $(python_version_minor)
	echo $(PY_GT_3_6)
	rm -f test
	g++ -Wall  -g -O0 -fPIC  -I./include/spdlog/include/ $(PYINC) pyWrapper.cpp   pyWrapperTest.cpp -o test $(PYLIB) 

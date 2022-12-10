python_version_full := $(wordlist 2,4,$(subst ., ,$(shell python --version 2>&1)))
python_version_major := $(word 1,${python_version_full})
python_version_minor := $(word 2,${python_version_full})
python_version_patch := $(word 3,${python_version_full})


PY_GT_3_8 := $(shell [ $(python_version_major) -gt 3 -o \( $(python_version_major) -eq 3 -a $(python_version_minor) -ge 8 \) ] && echo true)

CC=/opt/conda/envs/aiges/bin/x86_64-conda-linux-gnu-g++

PYLIB=$(shell python3-config  --ldflags)
PYINC=$(shell python3-config --cflags)
ifeq ($(PY_GT_3_8),true)
CC=g++
PYLIB=$(shell python3-config  --ldflags  --embed)
PYINC=$(shell python3-config --cflags --embed )
endif

wrapper:
	echo $(PYLIB)
	${CC}  -Wall -pedantic -Wextra -fPIC -shared  -std=c++1y  -fvisibility=default  -Wno-attributes  $(PYINC) -g -O0 -I. -I./include/spdlog/include/ -I./include/ -o libwrapper.so pyWrapper.cpp fswatch.cpp wrapper.cpp  -L. -L /opt/conda/envs/loader/lib $(PYLIB) -rdynamic
	mkdir -p wrapper_lib
	cp libwrapper.so  ./wrapper_lib

t:
	echo $(python_version_minor)
	echo $(PY_GT_3_6)
	rm -f test
	${CC}  -Wall  -g -O0 -fPIC  -I./include/spdlog/include/ $(PYINC) pyWrapper.cpp   pyWrapperTest.cpp -o test $(PYLIB)

main:
	echo $(PYLIB)
	${CC}   -Wall -pedantic -Wextra -fPIC  -std=c++1y  -fvisibility=default  -Wno-attributes  $(PYINC) -g -O0 -I. -I./include/spdlog/include/ -I./include/ -o output pyWrapper.cpp wrapper.cpp  test_data/main.cpp -L. -L /opt/conda/envs/loader/lib $(PYLIB) -rdynamic

fswatch:
	${CC}  fswatch.cpp  -l pthread -o fs_test
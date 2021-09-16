# 1、准备工作，编译方式、目标文件名、依赖库路径的定义。
CC = g++
#NVCC = /data/opt/tool/cuda-8.0.61-cudnn-6.0.21/bin/nvcc
#CFLAGS := -Wall -pthread -O2 -fPIC -fpermissive
CFLAGS := -Wall -w -pthread -O2 -fpermissive 
#-std=c++11
#NVCC_FLAGS = -std=c++11

#.o文件与.cpp文件同名
OBJS =main.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o

# 目标文件名 
EXE = ../../../bin_x64_linux/my_mspp_test

#	头文件路径
INCLUDE_PATH = -I ../../../source -I ../../include -I ./

# 依赖的静态库名称
#DEP_LIB = 

# 依赖的动态库
DEP_SO = -L ../../../bin_x64_linux -lIflyMspp -lIflyVad -lIflySpkSep -lIflyFea

all : $(EXE)

# 2. 生成.o文件
%.o : %.cpp
		$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATH)

# 3. 生成动态库文件
$(EXE) : $(OBJS)
		rm -f $@
		$(CC) $(CFLAGS) -o $@ $(OBJS) $(DEP_SO) $(DEP_LIB)
		rm -f $(OBJS)

# 4. 删除中间过程生成的文件 
clean:
		rm -f $(OBJS) $(EXE)

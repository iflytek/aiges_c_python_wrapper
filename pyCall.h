#ifndef PY_CALL_H
#define PY_CALL_H
#include "include/python/Python.h"

int callWrapperInit();
int callWrapperFini();

int callWrapperExec();

#endif
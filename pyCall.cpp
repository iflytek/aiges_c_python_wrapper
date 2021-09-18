#include "pyCall.h"


PyObject* wrapperModule;

int callWrapperInit(pConfig config){
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import wrapper");

    wrapperModule=PyImport_ImportModule("wrapper");

    PyObject* initFunc=PyObject_GetAttrString(wrapperModule,(char *)"wrapperInit");
    if (initFunc && PyCallable_Check(initFunc)) {
        PyObject* pyDict = PyDict_New();
    }else{
        
    }

    PyObject* pyRet=PyEval_CallObject(initFunc,NULL);

    int ret=0;
    PyArg_Parse(pyRet,"i",&ret);
    return ret;
}

int callWrapperExec(){
    PyObject* wrapperModule=PyImport_ImportModule("wrapper");
    PyObject* initFunc=PyObject_GetAttrString(wrapperModule,(char *)"wrapperOnceExec");
    PyObject* pyRet=PyEval_CallObject(initFunc,NULL);

    int ret=0;
    PyArg_Parse(pyRet,"i",&ret);
    return ret;
}


int callWrapperFini(){

    PyObject* wrapperModule=PyImport_ImportModule("wrapper");
    PyObject* initFunc=PyObject_GetAttrString(wrapperModule,(char *)"wrapperFini");
    PyObject* pyRet=PyEval_CallObject(initFunc,NULL);

    int ret=0;
    PyArg_Parse(pyRet,"i",&ret);
    return ret;

    Py_Finalize();
}
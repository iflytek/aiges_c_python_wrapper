#include "pyCall.h"
#include "wrapper_err.h"

PyObject* wrapperModule;

const char * _wrapperName="wrapper";

int callWrapperInit(pConfig cfg){
    Py_Initialize();
    PyRun_SimpleString("import sys");

    wrapperModule=PyImport_ImportModule(_wrapperName);
    PyObject* initFunc=PyObject_GetAttrString(wrapperModule,(char *)"wrapperInit");

    PyObject *pArgsT = PyTuple_New(1);
    PyObject* pArgsD = PyDict_New();
    for (pConfig p = cfg; p != NULL; p = p->next){
        PyDict_SetItemString(pArgsD,p->key, Py_BuildValue("s", p->value));
    }
    PyTuple_SetItem(pArgsT, 0, pArgsD);
    PyObject* pRet=PyEval_CallObject(initFunc,pArgsD);
    if (pRet==NULL){
        return WRAPPER::CError::innerError;
    }
    int ret=0;
    PyArg_Parse(pRet,"i",&ret);

    Py_DECREF(initFunc);
    Py_DECREF(pArgsT);
    Py_DECREF(pArgsD);
    Py_DECREF(pRet);

    spdlog::debug("wrapperinit ret.{}",ret);
    return ret;
}

int callWrapperExec(const char* usrTag, pParamList params, pDataList reqData, pDataList* respData, unsigned int psrIds[], int psrCnt){    
    PyObject* execFunc=PyObject_GetAttrString(wrapperModule,(char *)"wrapperOnceExec");
    //构建参数元组
    PyObject *pArgsT = PyTuple_New(6);
    
    //构建请求句柄
    PyObject* pUsrTag=PyUnicode_FromString(usrTag);
    PyTuple_SetItem(pArgsT, 0, pUsrTag);

    //构建请求参数
    PyObject* pParam = PyDict_New();
    for (pParamList p = params; p != NULL; p = p->next){
        PyDict_SetItemString(pParam,p->key, Py_BuildValue("s", p->value));
    }
    PyTuple_SetItem(pArgsT, 1, pParam);
    //构建请求数据
    PyObject* pyData = PyList_New(0);
    DataList *p = reqData;
    while (p != NULL){
        PyObject* tmp = PyDict_New();

        PyObject* pyKey=PyUnicode_FromString(p->key);
        PyDict_SetItemString(tmp,"key",pyKey);

        std::string actualData=*static_cast(p->data);
        PyObject* pyData=PyUnicode_FromString(actualData.c_str());
        PyDict_SetItemString(tmp,"data",pyData);

        PyObject*  pyStatus=Py_BuildValue("i",int(p->status));
        PyDict_SetItemString(tmp,"status",pyStatus);
        
        PyObject*  pyType=Py_BuildValue("i",int(p->type));
        PyDict_SetItemString(tmp,"type",pyType);


        PyObject* tmpDesc = PyDict_New();
        for (pParamList descP = p->desc; descP != NULL; descP= descP->next){
            PyDict_SetItemString(tmpDesc,descP->key, Py_BuildValue("s", descP->value));
        }
        PyDict_SetItemString(tmp,"desc",tmpDesc);

        PyList_Append(pyData,tmp);
        p=p->next;
    }
    PyTuple_SetItem(pArgsT, 2, pyData);


    PyObject* pRet=PyEval_CallObject(execFunc,pArgsT);
    if (pRet==NULL){
        return WRAPPER::CError::innerError;
    }
    int ret=0;
    PyArg_Parse(pRet,"i",&ret);
    spdlog::debug("wrapperinit ret.{}",ret);
    return 0;
}


int callWrapperFini(){
    PyObject* FiniFunc=PyObject_GetAttrString(wrapperModule,(char *)"wrapperFini");
    PyObject* pRet=PyEval_CallObject(FiniFunc,NULL);

    int ret=0;
    PyArg_Parse(pRet,"i",&ret);
    spdlog::debug("wrapperFini ret.{}",ret);
    
    Py_DECREF(FiniFunc);
    Py_DECREF(pRet);

    Py_Finalize();

    return ret;
}
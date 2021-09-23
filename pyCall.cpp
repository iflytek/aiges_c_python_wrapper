#include "pyCall.h"
#include "wrapper_err.h"

PyObject *wrapperModule;

const char *_wrapperName = "wrapper";

int callWrapperInit(pConfig cfg)
{
    std::cout << "start init" << std::endl;
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import wrapper");

    wrapperModule = PyImport_ImportModule(_wrapperName);
    PyObject *initFunc = PyObject_GetAttrString(wrapperModule, (char *)"wrapperInit");

    PyObject *pArgsT = PyTuple_New(1);
    PyObject *pArgsD = PyDict_New();
    for (pConfig p = cfg; p != NULL; p = p->next)
    {
        PyDict_SetItemString(pArgsD, p->key, Py_BuildValue("s", p->value));
    }
    PyTuple_SetItem(pArgsT, 0, pArgsD);
    PyObject *pRet = PyEval_CallObject(initFunc, pArgsT);
    if (pRet == NULL)
    {
        return WRAPPER::CError::innerError;
    }
    int ret = 0;
    PyArg_Parse(pRet, "i", &ret);

    Py_DECREF(initFunc);
    Py_DECREF(pArgsT);
    Py_DECREF(pArgsD);
    Py_DECREF(pRet);

    spdlog::debug("wrapperinit ret.{}", ret);
    std::cout << "finish init" << std::endl;
    return ret;
}

int callWrapperExec(const char *usrTag, pParamList params, pDataList reqData, pDataList *respData, unsigned int psrIds[], int psrCnt)
{
    PyObject *execFunc = PyObject_GetAttrString(wrapperModule, (char *)"wrapperOnceExec");
    //构建参数元组
    PyObject *pArgsT = PyTuple_New(4);

    //构建请求句柄
    PyObject *pUsrTag = PyUnicode_FromString(usrTag);
    PyTuple_SetItem(pArgsT, 0, pUsrTag);

    //构建请求参数
    PyObject *pyParam = PyDict_New();
    for (pParamList p = params; p != NULL; p = p->next)
    {
        PyDict_SetItemString(pyParam, p->key, Py_BuildValue("s", p->value));
    }
    PyTuple_SetItem(pArgsT, 1, pyParam);
    //构建请求数据
    int dataNum = 0;
    for (pDataList tmpDataPtr = reqData; tmpDataPtr != NULL; tmpDataPtr = tmpDataPtr->next)
    {
        dataNum++;
    }
    spdlog::debug("call wrapper exec ，datanum:{}",dataNum);
    dataNum=0;
    if (dataNum > 0)
    {
        PyObject *pyDataList = PyTuple_New(dataNum);
        // pDataList p = reqData;
        // for (int tmpIdx = 0; tmpIdx < dataNum; tmpIdx++)
        // {
        //     PyObject *tmp = PyDict_New();

        //     PyObject *pyKey = PyUnicode_FromString(p->key);
        //     PyDict_SetItemString(tmp, "key", pyKey);

        //     //std::string actualData=*(std::string*)(p->data);
        //     PyObject *pyData = PyUnicode_FromString("hello world");
        //     PyDict_SetItemString(tmp, "data", pyData);

        //     PyObject *pyStatus = Py_BuildValue("i", int(p->status));
        //     PyDict_SetItemString(tmp, "status", pyStatus);

        //     PyObject *pyType = Py_BuildValue("i", int(p->type));
        //     PyDict_SetItemString(tmp, "type", pyType);

        //     PyObject *tmpDesc = PyDict_New();
        //     for (pParamList descP = p->desc; descP != NULL; descP = descP->next)
        //     {
        //         PyDict_SetItemString(tmpDesc, descP->key, Py_BuildValue("s", descP->value));
        //     }
        //     PyDict_SetItemString(tmp, "desc", tmpDesc);

        //     PyTuple_SetItem(pyDataList, tmpIdx, tmp);
        //     p = p->next;
        // }
        // pyDataList= PyTuple_New(dataNum);
        PyTuple_SetItem(pArgsT, 2, pyDataList);
    }

    //构建个性化请求id
    PyObject* pyPsrIds = PyTuple_New(1);
    PyTuple_SetItem(pyPsrIds, 0, Py_BuildValue("i", 1));
    PyTuple_SetItem(pArgsT, 2, pyPsrIds);

    spdlog::debug("call wrapper exec,psrIdsNum:{}",count);
    // //构建个性化请求个数
    PyTuple_SetItem(pArgsT, 3, Py_BuildValue("i", psrCnt));

    PyObject *pRet = PyEval_CallObject(execFunc, pArgsT);
    if (pRet == NULL)
    {
        return WRAPPER::CError::innerError;
    }
    int ret = 0;
    PyArg_Parse(pRet, "i", &ret);
    spdlog::debug("wrapperinit ret.{}", ret);
    return 0;
}

int callWrapperFini()
{
    PyObject *FiniFunc = PyObject_GetAttrString(wrapperModule, (char *)"wrapperFini");
    PyObject *pRet = PyEval_CallObject(FiniFunc, NULL);

    int ret = 0;
    PyArg_Parse(pRet, "i", &ret);
    spdlog::debug("wrapperFini ret.{}", ret);

    Py_DECREF(FiniFunc);
    Py_DECREF(pRet);

    Py_Finalize();

    return ret;
}
#include "pyCall.h"
#include "wrapper_err.h"

PyObject *wrapperModule;
const char *_wrapperName = "wrapper";
std::map<int, std::string> errStrMap;

const char *implementErr = "empty error msg";

const char *callWrapperError(int ret)
{
    if (errStrMap.count(ret) != 0)
    {
        return errStrMap[ret].c_str();
    }
    PyObject *errFunc = PyObject_GetAttrString(wrapperModule, (char *)"wrapperError");
    if (!errFunc || !PyCallable_Check(errFunc))
    {
        return "wrapperError func need to implement";
    }

    PyObject *pArgsT = PyTuple_New(1);
    PyTuple_SetItem(pArgsT, 0, Py_BuildValue("i", ret));

    PyObject *pRet = PyEval_CallObject(errFunc, pArgsT);
    std::string errorStr = PyUnicode_AsUTF8(pPet);
    Py_DECREF(FiniFunc);
    Py_DECREF(pRet);

    errStrMap[ret] = errorStr;

    return errStrMap[ret].c_str();
}

void initErrorStrMap()
{
    errStrMap[WRAPPER::CError::NotImplementInit] = "wrapper init need to implement";
    errStrMap[WRAPPER::CError::NotImplementExec] = "wrapper exec need to implement";
    errStrMap[WRAPPER::CError::NotImplementFini] = "wrapper fini need to implement";
}

int callWrapperInit(pConfig cfg)
{
    int ret = 0;
    std::cout << "start init" << std::endl;

    initErrorStrMap();

    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import wrapper");

    wrapperModule = PyImport_ImportModule(_wrapperName);
    PyObject *initFunc = PyObject_GetAttrString(wrapperModule, (char *)"wrapperInit");
    if (!initFunc || !PyCallable_Check(initFunc))
    {
        return WRAPPER::CError::NotImplementInit;
    }

    PyObject *pArgsT = PyTuple_New(1);
    PyObject *pArgsD = PyDict_New();
    for (pConfig p = cfg; p != NULL; p = p->next)
    {
        PyDict_SetItemString(pArgsD, p->key, Py_BuildValue("s", p->value));
        spdlog::debug("wrapperInit config. key:{},value:{}", p->key, p->value);
    }
    try
    {
        PyTuple_SetItem(pArgsT, 0, pArgsD);
        PyObject *pRet = PyEval_CallObject(initFunc, pArgsT);
        if (pRet == NULL)
        {
            return WRAPPER::CError::innerError;
        }
        PyArg_Parse(pRet, "i", &ret);

        Py_DECREF(initFunc);
        Py_DECREF(pArgsT);
        Py_DECREF(pArgsD);
        Py_DECREF(pRet);
    }
    catch (const std::exception &e)
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperinit error:{}, ret:{}", errRlt, ret);
        }
    }
    spdlog::debug("wrapperinit ret:{}", ret);
    std::cout << "finish init" << std::endl;
    return ret;
}

int callWrapperExec(const char *usrTag, pParamList params, pDataList reqData, pDataList *respData, unsigned int psrIds[], int psrCnt)
{
    std::string sid = "";
    int ret = 0;
    for (pParamList sidP = params; sidP != NULL; sidP = sidP->next)
    {
        if (std::string("sid") == sidP->key)
        {
            sid = sidP->value;
            break;
        }
    }

    PyObject *execFunc = PyObject_GetAttrString(wrapperModule, (char *)"wrapperOnceExec");
    if (!execFunc || !PyCallable_Check(execFunc))
    {
        return WRAPPER::CError::NotImplementExec;
    }
    try
    {
        //构建参数元组
        PyObject *pArgsT = PyTuple_New(5);

        //构建请求句柄
        PyObject *pUsrTag = PyUnicode_FromString(usrTag);
        PyTuple_SetItem(pArgsT, 0, pUsrTag);

        //构建请求参数
        PyObject *pyParam = PyDict_New();
        for (pParamList p = params; p != NULL; p = p->next)
        {
            PyDict_SetItemString(pyParam, p->key, Py_BuildValue("s", p->value));
            spdlog::debug("wrapper exec param, key:{},value:{},sid:{}", p->key, p->value, sid);
        }
        PyTuple_SetItem(pArgsT, 1, pyParam);
        //构建请求数据
        int dataNum = 0;
        for (pDataList tmpDataPtr = reqData; tmpDataPtr != NULL; tmpDataPtr = tmpDataPtr->next)
        {
            dataNum++;
        }
        spdlog::debug("call wrapper exec,datanum:{}", dataNum);
        if (dataNum > 0)
        {
            PyObject *pyDataList = PyTuple_New(dataNum);
            pDataList p = reqData;
            for (int tmpIdx = 0; tmpIdx < dataNum; tmpIdx++)
            {
                PyObject *tmp = PyDict_New();

                PyObject *pyKey = PyUnicode_FromString(p->key);
                PyDict_SetItemString(tmp, "key", pyKey);

                std::string actualData=*(std::string*)(p->data);
                PyObject *pyData = PyUnicode_FromString(actualData);
                PyDict_SetItemString(tmp, "data", pyData);

                PyObject *pyStatus = Py_BuildValue("i", int(p->status));
                PyDict_SetItemString(tmp, "status", pyStatus);

                PyObject *pyType = Py_BuildValue("i", int(p->type));
                PyDict_SetItemString(tmp, "type", pyType);

                PyObject *tmpDesc = PyDict_New();
                for (pParamList descP = p->desc; descP != NULL; descP = descP->next)
                {
                    PyDict_SetItemString(tmpDesc, descP->key, Py_BuildValue("s", descP->value));
                }
                PyDict_SetItemString(tmp, "desc", tmpDesc);

                PyTuple_SetItem(pyDataList, tmpIdx, tmp);
                p = p->next;
            }
            PyTuple_SetItem(pArgsT, 2, pyDataList);
        }

        //构建个性化请求id
        int num = sizeof(psrIds) / sizeof(unsigned int);
        if (num != 0)
        {
            PyObject *pyPsrIds = PyTuple_New(num);
            for (int idx=0;idx<num;idx++)
            {
                PyTuple_SetItem(pyPsrIds, idx, Py_BuildValue("i", psrIds[idx]));
                spdlog::debug("wraperr exec psrId:{},sid:{}",psrIds[idx],sid);
            }
            PyTuple_SetItem(pArgsT, 3, pyPsrIds);
        }
        else
        {
            PyObject *pyPsrIds = PyTuple_New(0); 
            spdlog::debug("wrapper exec psrIds is empty.sid:{}",sid);
            PyTuple_SetItem(pArgsT, 3, pyPsrIds);
        }
        // //构建个性化请求个数
        PyTuple_SetItem(pArgsT, 4, Py_BuildValue("i", psrCnt));
        spdlog::debug("wrapper exec psrCnt .val :{}",psrCnt);

        PyObject *pRet = PyEval_CallObject(execFunc, pArgsT);
        if (pRet == NULL)
        {
            return WRAPPER::CError::innerError;
        }
        PyArg_Parse(pRet, "i", &ret);
    }
    catch (const std::exception &e)
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperExec error:{}, ret:{}", errRlt, ret);
        }
    }
    spdlog::debug("wrapperinit ret.{}", ret);
    return 0;
}

int callWrapperFini()
{
    int ret = 0;
    PyObject *FiniFunc = PyObject_GetAttrString(wrapperModule, (char *)"wrapperFini");
    if (!FiniFunc || !PyCallable_Check(FiniFunc))
    {
        return WRAPPER::CError::NotImplementFini;
    }
    try
    {
        PyObject *pRet = PyEval_CallObject(FiniFunc, NULL);
        PyArg_Parse(pRet, "i", &ret);
        spdlog::debug("wrapperFini ret.{}", ret);
        Py_DECREF(FiniFunc);
        Py_DECREF(pRet);
        Py_Finalize();
    }
    catch (const std::exception &e)
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperFini error:{}, ret:{}", errRlt, ret);
        }
    }

    return ret;
}

std::string log_python_exception()
{
    std::string strErrorMsg = "";
    if (!Py_IsInitialized())
    {
        strErrorMsg = "Python is not Initialized ";
        return strErrorMsg;
    }

    if (PyErr_Occurred() != NULL)
    {
        PyObject *type_obj, *value_obj, *traceback_obj;
        PyErr_Fetch(&type_obj, &value_obj, &traceback_obj);
        if (value_obj == NULL)
            return strErrorMsg;
        PyErr_NormalizeException(&type_obj, &value_obj, 0);
        if (PyString_Check(PyObject_Str(value_obj)))
        {
            strErrorMsg = PyUnicode_FromString(PyObject_Str(value_obj));
        }

        if (traceback_obj != NULL)
        {
            strErrorMsg += "Traceback:";
            PyObject *pModuleName = PyUnicode_FromString("traceback");
            PyObject *pTraceModule = PyImport_Import(pModuleName);
            Py_XDECREF(pModuleName);
            if (pTraceModule != NULL)
            {
                PyObject *pModuleDict = PyModule_GetDict(pTraceModule);
                if (pModuleDict != NULL)
                {
                    PyObject *pFunc = PyDict_GetItemString(pModuleDict, "format_exception");
                    if (pFunc != NULL)
                    {
                        PyObject *errList = PyObject_CallFunctionObjArgs(pFunc, type_obj, value_obj, traceback_obj, NULL);
                        if (errList != NULL)
                        {
                            int listSize = PyList_Size(errList);
                            for (int i = 0; i < listSize; ++i)
                            {
                                strErrorMsg += PyUnicode_FromString(PyList_GetItem(errList, i));
                            }
                        }
                    }
                }
                Py_XDECREF(pTraceModule);
            }
        }
        Py_XDECREF(type_obj);
        Py_XDECREF(value_obj);
        Py_XDECREF(traceback_obj);
    }
    return strErrorMsg;
}

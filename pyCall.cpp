#include "pyCall.h"
#include "wrapper_err.h"

const char *_wrapperName = "wrapper";
std::map<int, std::string> errStrMap;

const char *implementErr = "empty error msg";

const char *callWrapperError(int ret)
{
    if (errStrMap.count(ret) != 0)
    {
        spdlog::error("wrapper Error,ret:{}.errStr:{}", ret, errStrMap[ret]);
        return errStrMap[ret].c_str();
    }
    spdlog::error("wrapper Error,ret:{}", ret);
    std::cout<<"wrapper error"<<ret<<std::endl;
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject *wrapperModule = PyImport_ImportModule(_wrapperName);
    PyObject *errFunc = PyObject_GetAttrString(wrapperModule, "wrapperError");
    if (!errFunc || !PyCallable_Check(errFunc))
    {
        std::cout << log_python_exception << std::endl;
        PyGILState_Release(gstate);
        return "wrapperError func need to implement";
    }
    PyObject *pArgsT = PyTuple_New(1);
    try
    {
        PyTuple_SetItem(pArgsT, 0, Py_BuildValue("i", ret));

        //PyGILState_STATE gstate = PyGILState_Ensure();
        PyObject *pRet = PyEval_CallObject(errFunc, pArgsT);
        if (pRet == NULL)
        {
            std::string errRlt = "";
            errRlt = log_python_exception();
            if (errRlt != "")
            {
                spdlog::error("wrapperError error:{}", errRlt);
            }
            ret = WRAPPER::CError::innerError;
        }
        else
        {
            std::string errorStr = PyUnicode_AsUTF8(pRet);
            errStrMap[ret] = errorStr;
            spdlog::error("wrapper Error,ret:{}.errStr:{}", ret, errStrMap[ret]);
            Py_DECREF(pRet);
        }
    }
    catch (const std::exception &e)
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperError python error:{}, ret:{}", errRlt, ret);
        }
        ret = WRAPPER::CError::innerError;
        spdlog::error("wrapperError c error,ret:{}.errStr:{} what:{}",ret, errStrMap[ret],e.what());
    }
    Py_XDECREF(pArgsT);
    Py_DECREF(errFunc);
    Py_XDECREF(wrapperModule);
    PyGILState_Release(gstate);
    return errStrMap[ret].c_str();
}

void initErrorStrMap()
{
    errStrMap[WRAPPER::CError::NotImplementInit] = "wrapper init is empty";
    errStrMap[WRAPPER::CError::NotImplementExec] = "wrapper exec is empty";
    errStrMap[WRAPPER::CError::NotImplementFini] = "wrapper fini is empty";

    errStrMap[WRAPPER::CError::RltDataKeyInvalid] = "respdata need key item";
    errStrMap[WRAPPER::CError::RltDataDataInvalid] = "respdata need data item";
    errStrMap[WRAPPER::CError::RltDataLenInvalid] = "respdata need len item";
    errStrMap[WRAPPER::CError::RltDataStatusInvalid] = "respdata need status item";
    errStrMap[WRAPPER::CError::RltDataTypeInvalid] = "respdata need type item";
}

int callWrapperInit(pConfig cfg)
{
    int ret = 0;
    std::cout << "start init" << std::endl;

    initErrorStrMap();

    Py_Initialize();
    if (!Py_IsInitialized())
    {
        std::cout << "failed to init python env" << std::endl;
        return WRAPPER::CError::innerError;
    }
    else
    {
        //声明使用多线程 获取GIL锁并释放
        PyEval_InitThreads();
        int nInit = PyEval_ThreadsInitialized();
        if (nInit)
        {
            PyEval_SaveThread();
        }
    }
    std::vector<PyObject *> tmpCfgPyObj;
    //申请GIL
    PyGILState_STATE gstate = PyGILState_Ensure();

    PyRun_SimpleString("import sys");
    PyObject *wrapperModule = PyImport_ImportModule(_wrapperName);
    PyObject *initFunc = PyObject_GetAttrString(wrapperModule, "wrapperInit");
    if (!initFunc || !PyCallable_Check(initFunc))
    {
        std::cout << log_python_exception << std::endl;
        PyGILState_Release(gstate);
        return WRAPPER::CError::NotImplementInit;
    }

    PyObject *pArgsT = PyTuple_New(1);
    PyObject *pArgsD = PyDict_New();
    for (pConfig p = cfg; p != NULL; p = p->next)
    {
        PyObject *tmpCfg = Py_BuildValue("s", p->value);
        PyDict_SetItemString(pArgsD, p->key, tmpCfg);
        tmpCfgPyObj.push_back(tmpCfg);
        spdlog::debug("wrapperInit config. key:{},value:{}", p->key, p->value);
    }
    try
    {
        PyTuple_SetItem(pArgsT, 0, pArgsD);
        PyObject *pRet = PyEval_CallObject(initFunc, pArgsT);

        if (pRet == NULL)
        {
            std::string errRlt = "";
            errRlt = log_python_exception();
            if (errRlt != "")
            {
                spdlog::error("wrapperInit error:{}", errRlt);
            }
            ret = WRAPPER::CError::innerError;
        }
        else
        {
            PyArg_Parse(pRet, "i", &ret);
            Py_DECREF(pRet);
        }
    }
    catch (const std::exception &e)
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperinit error:{}, ret:{}", errRlt, ret);
        }
        ret = WRAPPER::CError::innerError;
    }
    Py_DECREF(initFunc);
    for (auto &i : tmpCfgPyObj)
    {
        Py_XDECREF(i);
    }
    Py_DECREF(pArgsD);
    Py_DECREF(pArgsT);
    Py_XDECREF(wrapperModule);
    PyGILState_Release(gstate);
    spdlog::debug("wrapperinit ret:{}", ret);
    return ret;
}

int callWrapperExec(const char *usrTag, pParamList params, pDataList reqData, pDataList *respData, unsigned int psrIds[], int psrCnt, std::string sid)
{
    int ret = 0;
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject *wrapperModule = PyImport_ImportModule(_wrapperName);
    PyObject *execFunc = PyObject_GetAttrString(wrapperModule, "wrapperOnceExec");
    Py_XDECREF(wrapperModule);
    if (!execFunc || !PyCallable_Check(execFunc))
    {
        PyGILState_Release(gstate);
        return WRAPPER::CError::NotImplementExec;
    }
    PyObject *pArgsT = PyTuple_New(6);
    std::vector<PyObject *> tmpPyObjectVec;
    try
    {
        //构建参数元组
        //构建请求句柄
        PyObject *pUsrTag = PyUnicode_FromString(usrTag);
        PyTuple_SetItem(pArgsT, 0, pUsrTag);

        //构建请求参数
        PyObject *pyParam = PyDict_New();
        for (pParamList p = params; p != NULL; p = p->next)
        {
            PyObject *tmpV = Py_BuildValue("s", p->value);
            tmpPyObjectVec.push_back(tmpV);
            PyDict_SetItemString(pyParam, p->key, tmpV);
            spdlog::debug("wrapper exec param, key:{},value:{},sid:{}", p->key, p->value, sid);
        }
        PyTuple_SetItem(pArgsT, 1, pyParam);
        //构建请求数据
        int dataNum = 0;
        for (pDataList tmpDataPtr = reqData; tmpDataPtr != NULL; tmpDataPtr = tmpDataPtr->next)
        {
            dataNum++;
        }
        spdlog::debug("call wrapper exec,datanum:{}，sid:{}", dataNum, sid);
        PyObject *pyDataList = PyTuple_New(dataNum);
        if (dataNum > 0)
        {
            pDataList p = reqData;
            for (int tmpIdx = 0; tmpIdx < dataNum; tmpIdx++)
            {
                PyObject *tmp = PyDict_New();

                PyObject *pyKey = Py_BuildValue("s", p->key);
                PyDict_SetItemString(tmp, "key", pyKey);
                tmpPyObjectVec.push_back(pyKey);
                //std::string datas(static_cast<char*>(p->data),p->len);
                //PyObject *pyData = Py_BuildValue("O", p->data);
                PyObject *pyData = PyBytes_FromStringAndSize((char *)(p->data), p->len);
                PyDict_SetItemString(tmp, "data", pyData);
                tmpPyObjectVec.push_back(pyData);

                PyObject *pyDataLen = Py_BuildValue("i", int(p->len));
                PyDict_SetItemString(tmp, "len", pyDataLen);
                tmpPyObjectVec.push_back(pyDataLen);

                PyObject *pyStatus = Py_BuildValue("i", int(p->status));
                PyDict_SetItemString(tmp, "status", pyStatus);
                tmpPyObjectVec.push_back(pyStatus);

                PyObject *pyType = Py_BuildValue("i", int(p->type));
                PyDict_SetItemString(tmp, "type", pyType);

                tmpPyObjectVec.push_back(pyType);

                PyObject *tmpDesc = PyDict_New();
                tmpPyObjectVec.push_back(tmpDesc);
                for (pParamList descP = p->desc; descP != NULL; descP = descP->next)
                {
                    PyObject *tmpV = Py_BuildValue("s", descP->value);
                    tmpPyObjectVec.push_back(tmpV);
                    PyDict_SetItemString(tmpDesc, descP->key, tmpV);
                }
                PyDict_SetItemString(tmp, "desc", tmpDesc);

                PyTuple_SetItem(pyDataList, tmpIdx, tmp);
                p = p->next;
            }
        }
        PyTuple_SetItem(pArgsT, 2, pyDataList);
        //构建响应数据体
        PyObject *pyRespData = PyList_New(0);
        PyTuple_SetItem(pArgsT, 3, pyRespData);
        //构建个性化请求id
        int num = psrCnt;
        PyObject *pyPsrIds = PyTuple_New(num);
        if (num != 0)
        {
            for (int idx = 0; idx < num; idx++)
            {
                PyTuple_SetItem(pyPsrIds, idx, Py_BuildValue("i", psrIds[idx]));
                spdlog::debug("wrapper exec psrId:{},sid:{}", psrIds[idx], sid);
            }
            PyTuple_SetItem(pArgsT, 4, pyPsrIds);
        }
        else
        {
            spdlog::debug("wrapper exec psrIds is empty.sid:{}", sid);
            PyTuple_SetItem(pArgsT, 4, pyPsrIds);
        }
        // //构建个性化请求个数
        PyTuple_SetItem(pArgsT, 5, Py_BuildValue("i", psrCnt));
        spdlog::debug("wrapper exec psrCnt .val :{},sid:{}", psrCnt, sid);
        PyObject *pRet = PyEval_CallObject(execFunc, pArgsT);
        if (pRet == NULL)
        {
            std::string errRlt = "";
            errRlt = log_python_exception();
            if (errRlt != "")
            {
                spdlog::error("wrapperExec error:{},sid:{}", errRlt, sid);
            }
            Py_DECREF(pArgsT);
            Py_DECREF(execFunc);
            ret = WRAPPER::CError::innerError;
        }
        else
        {
            PyArg_Parse(pRet, "i", &ret);
            Py_DECREF(pRet);
            if (ret == 0)
            {
                //读取响应
                int rltSize = PyList_Size(pyRespData);
                if (rltSize != 0)
                {
                    pDataList headPtr;
                    pDataList prePtr;
                    pDataList curPtr;
                    for (int idx = 0; idx < rltSize; idx++)
                    {
                        pDataList tmpData = new (DataList);

                        PyObject *tmpDict = PyList_GetItem(pyRespData, idx);
                        char *tmpRltKey = pyDictStrToChar(tmpDict, DATA_KEY, sid, ret);
                        if (ret != 0)
                        {
                            break;
                        }
                        else
                        {
                            tmpData->key = tmpRltKey;
                        }

                        int integerVal = 0;
                        ret = pyDictIntToInt(tmpDict, DATA_LEN, integerVal, sid);
                        if (ret != 0)
                        {
                            break;
                        }
                        else
                        {
                            tmpData->len = integerVal;
                        }

                        char *tmpRltData = pyDictStrToChar(tmpDict, DATA_DATA, sid, ret);
                        if (ret != 0)
                        {
                            break;
                        }
                        else
                        {
                            tmpData->data = (void *)tmpRltData;
                        }
                        int interValSta = 0;
                        ret = pyDictIntToInt(tmpDict, DATA_STATUS, interValSta, sid);
                        if (ret != 0)
                        {
                            break;
                        }
                        else
                        {
                            tmpData->status = DataStatus(interValSta);
                        }
                        int interValType = 0;
                        ret = pyDictIntToInt(tmpDict, DATA_TYPE, interValType, sid);
                        if (ret != 0)
                        {
                            break;
                        }
                        else
                        {
                            tmpData->type = DataType(interValType);
                        }
                        tmpData->next = NULL;
                        //检查下是否需要desc吧
                        tmpData->desc = pyDictToDesc(tmpDict, DATA_DESC, sid, ret);
                        if (ret != 0)
                        {
                            break;
                        }
                        if (idx == 0)
                        {
                            headPtr = tmpData;
                            prePtr = tmpData;
                            curPtr = tmpData;
                        }
                        else
                        {
                            curPtr = tmpData;
                            prePtr->next = curPtr;
                            prePtr = curPtr;
                        }
                        spdlog::debug("get result,key:{},len:{},type:{},status:{},sid:{}", tmpData->key, tmpData->len, tmpData->type, tmpData->status, sid);
                    }
                    *respData = headPtr;
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperExec error:{}, ret:{},sid:{}", errRlt, ret, sid);
        }
        ret = WRAPPER::CError::innerError;
    }
    spdlog::debug("wrapperExec ret.{},tmpPyObjectVec size:{},sid:{}", ret, tmpPyObjectVec.size(), sid);
    for (auto &i : tmpPyObjectVec)
    {
        Py_XDECREF(i);
    }
    //Py_XDECREF(pyData);
    Py_DECREF(pArgsT);
    Py_DECREF(execFunc);
    PyGILState_Release(gstate);
    return ret;
}

int callWrapperFini()
{
    int ret = 0;
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject *wrapperModule = PyImport_ImportModule(_wrapperName);
    PyObject *FiniFunc = PyObject_GetAttrString(wrapperModule, "wrapperFini");
    Py_XDECREF(wrapperModule);
    if (!FiniFunc || !PyCallable_Check(FiniFunc))
    {
        PyGILState_Release(gstate);
        return WRAPPER::CError::NotImplementFini;
    }
    try
    {
        PyObject *pRet = PyEval_CallObject(FiniFunc, NULL);
        if (pRet == NULL)
        {
            std::string errRlt = "";
            errRlt = log_python_exception();
            if (errRlt != "")
            {
                spdlog::error("wrapperFini error:{}", errRlt);
            }
            Py_DECREF(FiniFunc);
            PyGILState_Release(gstate);
            return WRAPPER::CError::innerError;
        }
        PyArg_Parse(pRet, "i", &ret);
        spdlog::debug("wrapperFini ret.{}", ret);
        PyGILState_Release(gstate);
        //Py_Finalize(); 理论上需要的 但是不注释会崩溃 TODO fix
    }
    catch (const std::exception &e)
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperFini error:{}, ret:{}", errRlt, ret);
        }
        
        ret= WRAPPER::CError::innerError;
    }
    std::cout<<"fini ret:"<<ret<<std::endl;
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
        if (PyUnicode_Check(PyObject_Str(value_obj)))
        {
            strErrorMsg = PyUnicode_AsUTF8(PyObject_Str(value_obj));
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
                                strErrorMsg += PyUnicode_AsUTF8(PyObject_Str(PyList_GetItem(errList, i)));
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

char *pyDictStrToChar(PyObject *obj, std::string itemKey, std::string sid, int &ret)
{
    std::string rltStr = "";
    char *rlt_ch = NULL;
    PyObject *pyValue = PyDict_GetItemString(obj, itemKey.c_str());
    if (pyValue == NULL)
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperExec error:{}, sid:{}", errRlt, sid);
        }
        if (itemKey == DATA_KEY)
        {
            ret = WRAPPER::CError::RltDataKeyInvalid;
            return NULL;
        }
        else if (itemKey == DATA_DATA)
        {
            ret = WRAPPER::CError::RltDataDataInvalid;
            return NULL;
        }
        else
        {
            ret = WRAPPER::CError::innerError;
            return NULL;
        }
    }
    PyObject *utf8string = PyUnicode_AsUTF8String(pyValue);
    if (itemKey == DATA_DATA)
    {
        //以字节为单位
        rlt_ch = strdup(PyBytes_AsString(utf8string));
    }
    else
    {
        rlt_ch = strdup(PyBytes_AsString(utf8string));
    }
    spdlog::debug("pyDictStrToChar , key: {},value:{},sid:{}", itemKey, rlt_ch, sid);

    Py_XDECREF(utf8string);
    return rlt_ch;
}

pDescList pyDictToDesc(PyObject *obj, std::string descKey, std::string sid, int &ret)
{
    std::string rltStr = "";
    pDescList headPtr = NULL;
    PyObject *tmpKey = Py_BuildValue("s", descKey.c_str());
    PyObject *pyDesc = PyDict_GetItem(obj, tmpKey);
    if (pyDesc == NULL)
    {
        Py_XDECREF(tmpKey);
        spdlog::debug("pyDictToDesc ,desc is empty,sid:{}", sid);
        return NULL;
    }
    else
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperExec pyDictToDesc error:{}, sid:{}", errRlt, sid);
            ret = WRAPPER::CError::innerError;
            return NULL;
        }
        else
        {
            int descDictSize = PyDict_Size(pyDesc);
            if (descDictSize == 0)
            {
                spdlog::info("pyDictToDesc desc dict is empty,sid:{}", sid);
                return NULL;
            }
            else
            {
                PyObject *descKeys = PyDict_Keys(pyDesc);
                if (descKeys == NULL)
                {
                    Py_XDECREF(tmpKey);
                    ret = WRAPPER::CError::innerError;
                    return NULL;
                }
                else
                {
                    pDescList prePtr;
                    pDescList curPtr;
                    for (int idx = 0; idx < descDictSize; idx++)
                    {
                        pDescList tmpDesc = new (ParamList);
                        PyObject *utf8string = PyUnicode_AsUTF8String(PyList_GetItem(descKeys, idx));

                        tmpDesc->key = strdup(PyBytes_AsString(utf8string));
                        std::string tmpKey = tmpDesc->key;
                        tmpDesc->value = pyDictStrToChar(pyDesc, tmpKey, sid, ret);
                        if (ret != 0)
                        {
                            return NULL;
                        }
                        tmpDesc->vlen = strlen(tmpDesc->value);
                        tmpDesc->next = NULL;
                        if (idx == 0)
                        {
                            headPtr = tmpDesc;
                            prePtr = tmpDesc;
                            curPtr = tmpDesc;
                        }
                        else
                        {
                            curPtr = tmpDesc;
                            prePtr->next = curPtr;
                            prePtr = curPtr;
                        }
                        Py_XDECREF(utf8string);
                    }
                    Py_DECREF(descKeys);
                    return headPtr;
                }
            }
        }
    }
}

int pyDictIntToInt(PyObject *obj, std::string itemKey, int &itemVal, std::string sid)
{

    PyObject *pyValue = PyDict_GetItemString(obj, itemKey.c_str());
    if (pyValue == NULL)
    {
        std::string errRlt = "";
        errRlt = log_python_exception();
        if (errRlt != "")
        {
            spdlog::error("wrapperExec error:{},sid:{}", errRlt, sid);
        }
        if (itemKey == DATA_LEN)
        {
            return WRAPPER::CError::RltDataLenInvalid;
        }
        else if (itemKey == DATA_STATUS)
        {
            return WRAPPER::CError::RltDataStatusInvalid;
        }
        else if (itemKey == DATA_TYPE)
        {
            return WRAPPER::CError::RltDataTypeInvalid;
        }
        else
        {
            return WRAPPER::CError::innerError;
        }
    }
    PyArg_Parse(pyValue, "i", &itemVal);
    return 0;
}

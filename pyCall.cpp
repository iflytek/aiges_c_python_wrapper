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
        spdlog::debug("wrapper Error,ret:{}.errStr:{}", ret, errStrMap[ret]);
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
    std::string errorStr = PyUnicode_AsUTF8(pRet);
    Py_DECREF(errFunc);
    Py_DECREF(pRet);

    errStrMap[ret] = errorStr;
    spdlog::debug("wrapper Error,ret:{}.errStr:{}", ret, errStrMap[ret]);
    return errStrMap[ret].c_str();
}

void initErrorStrMap()
{
    errStrMap[WRAPPER::CError::NotImplementInit] = "wrapper init need to implement";
    errStrMap[WRAPPER::CError::NotImplementExec] = "wrapper exec need to implement";
    errStrMap[WRAPPER::CError::NotImplementFini] = "wrapper fini need to implement";

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
            std::string errRlt = "";
            errRlt = log_python_exception();
            if (errRlt != "")
            {
                spdlog::error("wrapperInit error:{}", errRlt);
            }
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
        PyObject *pArgsT = PyTuple_New(6);

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

                PyObject *pyData = PyBytes_FromStringAndSize((char *)(p->data),p->len);
                PyDict_SetItemString(tmp, "data", pyData);

                PyObject *pyDataLen=Py_BuildValue("i", int(p->len));
                PyDict_SetItemString(tmp, "len", pyDataLen);  

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
        //构建响应数据体
        PyObject *pyRespData = PyList_New(0);
        PyTuple_SetItem(pArgsT, 3, pyRespData);
        //构建个性化请求id
        int num = psrCnt;
        if (num != 0)
        {
            PyObject *pyPsrIds = PyTuple_New(num);
            for (int idx = 0; idx < num; idx++)
            {
                PyTuple_SetItem(pyPsrIds, idx, Py_BuildValue("i", psrIds[idx]));
                spdlog::debug("wrapper exec psrId:{},sid:{}", psrIds[idx], sid);
            }
            PyTuple_SetItem(pArgsT, 4, pyPsrIds);
        }
        else
        {
            PyObject *pyPsrIds = PyTuple_New(0);
            spdlog::debug("wrapper exec psrIds is empty.sid:{}", sid);
            PyTuple_SetItem(pArgsT, 4, pyPsrIds);
        }
        // //构建个性化请求个数
        PyTuple_SetItem(pArgsT, 5, Py_BuildValue("i", psrCnt));
        spdlog::debug("wrapper exec psrCnt .val :{}", psrCnt);

        PyObject *pRet = PyEval_CallObject(execFunc, pArgsT);
        if (pRet == NULL)
        {
            std::string errRlt = "";
            errRlt = log_python_exception();
            if (errRlt != "")
            {
                spdlog::error("wrapperExec error:{},sid:{}", errRlt, sid);
            }
            return WRAPPER::CError::innerError;
        }
        PyArg_Parse(pRet, "i", &ret);
        if (ret == 0)
        {
            //读取响应
            int rltSize = PyList_Size(pyRespData);
            if (rltSize != 0)
            {
                for (int idx = 0; idx < rltSize; idx++)
                {
                    pDataList tmpData = new (DataList);

                    PyObject *tmpDict = PyList_GetItem(pyRespData, idx);

                    char *tmpRltKey;
                    ret = pyDictStrToChar(tmpDict, DATA_KEY, tmpRltKey, sid);
                    if (ret != 0)
                    {
                        return ret;
                    }else{
                        std::cout<<tmpRltKey<<std::endl;
                        memcpy(tmpData->key, tmpRltKey, strlen(tmpRltKey));
                        std::cout<<tmpData->key<<std::endl;
                    }
                    

                    int integerVal = 0;
                    ret = pyDictIntToInt(tmpDict, DATA_LEN, integerVal, sid);
                    if (ret != 0)
                    {
                        return ret;
                    }
                    else
                    {
                        tmpData->len = integerVal;
                    }

                    char *tmpRltData;
                    ret = pyDictStrToChar(tmpDict, DATA_DATA,tmpRltData, sid);
                    if (ret != 0)
                    {
                        return ret;
                    }else{
                        std::cout<<tmpRltData<<std::endl;
                        memcpy(tmpData->data, tmpRltData,integerVal);
                        std::cout<<(char*)tmpData->data<<std::endl;
                    }

                    ret = pyDictIntToInt(tmpDict, DATA_STATUS, integerVal, sid);
                    if (ret != 0)
                    {
                        return ret;
                    }
                    else
                    {
                        tmpData->status = DataStatus(integerVal);
                    }
                    ret = pyDictIntToInt(tmpDict, DATA_TYPE, integerVal, sid);
                    if (ret != 0)
                    {
                        return ret;
                    }
                    else
                    {
                        tmpData->type = DataType(integerVal);
                    }
                    *respData = tmpData;
                    spdlog::debug("get result,key:{},len:{},type:{},status:{},sid:{}",tmpData->key,tmpData->len,tmpData->type,tmpData->status,sid);
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
            spdlog::error("wrapperExec error:{}, ret:{}", errRlt, ret);
        }
        return WRAPPER::CError::innerError;
    }
    spdlog::debug("wrapperExec ret.{}", ret);
    return ret;
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
        if (pRet == NULL)
        {
            std::string errRlt = "";
            errRlt = log_python_exception();
            if (errRlt != "")
            {
                spdlog::error("wrapperFini error:{}", errRlt);
            }
            Py_DECREF(FiniFunc);
            Py_DECREF(pRet);
            return WRAPPER::CError::innerError;
        }
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
        return WRAPPER::CError::innerError;
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

int pyDictStrToChar(PyObject *obj, std::string itemKey, char *rlt_ch, std::string sid)
{
    std::string rltStr = "";

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
            return WRAPPER::CError::RltDataKeyInvalid;
        }else if(itemKey==DATA_DATA){
            return WRAPPER::CError::RltDataDataInvalid;
        }
        else
        {
            return WRAPPER::CError::innerError;
        }
    }
    PyArg_Parse(pyValue, "s", &rlt_ch);
    std::cout<<rlt_ch<<std::endl;
    spdlog::debug("pyDictStrToChar , key: {},value:{},sid:{}",itemKey,rlt_ch,sid);
    // char* actRlt = (char *)malloc(strlen(rltStr.c_str()));
    // memcpy(actRlt, (char *)rltStr.c_str(), strlen(rltStr.c_str()));
    // rlt_ch=actRlt;    
    return 0;
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
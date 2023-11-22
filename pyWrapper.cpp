#include "pyWrapper.h"
#include <dlfcn.h>

//namespace py = pybind11;
//using namespace std::chrono_literals;


const char *wrapperFileKey = "wrapper.file";
const char *wrapperFileClass = "wrapper.class";

const char *WrapperFile = "wrapper";
const char *WrapperClass = "Wrapper";
const char *PythonSo = "libpython3.so";

wrapperMeterCustom g_metric_cb;
wrapperTraceLog g_trace_cb;

std::mutex RECORD_MUTEX;
std::map <std::string, std::string> SID_RECORD;
std::map <std::string, wrapperCallback> SID_CB;
std::map<std::string, const char *> SID_USRTAG;

PYBIND11_EMBEDDED_MODULE(aiges_embed, module) {
    module.def("callback_metric", &callbackMetric, py::return_value_policy::automatic_reference);
    module.def("callback_trace", &callbackTrace, py::return_value_policy::automatic_reference);
    module.def("callback", &callBack, py::return_value_policy::automatic_reference);
    py::class_<ResponseData> responseData(module, "ResponseData");
    responseData.def(py::init<>())
            .def(py::init<std::string, unsigned int, int, int>())
            .def("setDataType",
                 [](ResponseData &r, int i) {
                     r.type = i;
                 })
            .def("setData", [](ResponseData &r, const py::bytes &b) {
                r.data = b;
                Py_ssize_t size = PyBytes_GET_SIZE(b.ptr());
                r.len = size;

            })
            .def_readwrite("key", &ResponseData::key, py::return_value_policy::automatic_reference)
            .def_readwrite("data", &ResponseData::data, py::return_value_policy::automatic_reference)
            .def_readwrite("status", &ResponseData::status, py::return_value_policy::automatic_reference)
            .def_readwrite("len", &ResponseData::len, py::return_value_policy::automatic_reference)
            .def_readwrite("type", &ResponseData::type, py::return_value_policy::automatic_reference);

    py::class_<Response> response(module, "Response");
    response.def(py::init<>())
            .def_readwrite("list", &Response::list, py::return_value_policy::automatic_reference)
            .def_readwrite("error_code", &Response::errCode, py::return_value_policy::automatic_reference)
            .def("response_err", &Response::responseErr, py::return_value_policy::automatic_reference);

    py::class_<DataListNode> dataListNode(module, "DataListNode");
    dataListNode.def(py::init<>())
            .def_readwrite("key", &DataListNode::key, py::return_value_policy::automatic_reference)
            .def_readwrite("data", &DataListNode::data, py::return_value_policy::automatic_reference)
            .def_readwrite("len", &DataListNode::len, py::return_value_policy::automatic_reference)
            .def_readwrite("status", &DataListNode::status, py::return_value_policy::automatic_reference)
            .def_readwrite("type", &DataListNode::type, py::return_value_policy::automatic_reference)
            .def("get_data", &DataListNode::get_data, py::return_value_policy::reference);

    py::class_<DataListCls> dataListCls(module, "DataListCls");
    dataListCls.def(py::init<>())
            .def_readwrite("list", &DataListCls::list, py::return_value_policy::automatic_reference)
            .def("get", &DataListCls::get, py::return_value_policy::reference);

    py::class_<SessionCreateResponse> sessionCreateResponse(module, "SessionCreateResponse");
    sessionCreateResponse.def(py::init<>())
            .def_readwrite("handle", &SessionCreateResponse::handle, py::return_value_policy::automatic_reference)
            .def_readwrite("error_code", &SessionCreateResponse::errCode, py::return_value_policy::reference);
}

/**
 * 默认Response构造函数
 */
Response::Response() {
    this->errCode = 0;
}

/**
 * 默认Response构造函数， 带错误码
 */
Response::Response(int err) {
    this->errCode = err;
}

std::unique_ptr <Response> Response::responseErr(int errCode) {
    Response *resp = new Response(errCode);
    resp->errCode = errCode;
    return std::unique_ptr<Response>(resp);
}

py::bytes DataListNode::get_data() {
    return data;
}

DataListNode *DataListCls::get(std::string key) {
    for (int idx = 0; idx < list.size(); idx++) {
        DataListNode *node = &list[idx];
        if (strcmp(node->key.c_str(), key.c_str()) == 0) {
            return node;
        }
    }
    return nullptr;
}

PyWrapper::PyWrapper() {
    // 仅仅为了 加载下python lib库使 其部分函数可被导出使用
    // https://stackoverflow.com/questions/67891197/ctypes-cpython-39-x86-64-linux-gnu-so-undefined-symbol-pyfloat-type-in-embedd
    dlopen(PythonSo, RTLD_GLOBAL | RTLD_NOW);

    // if (config.count(wrapperFileKey) == 0)
    py::gil_scoped_acquire acquire;
    _wrapper = py::module::import(WrapperFile);
    _obj = _wrapper.attr(WrapperClass)();
    _wrapper_abs = _wrapper.attr("__file__").cast<std::string>(); // 获取加载的wrapper.py的绝对地址

    _wrapperInit = _obj.attr("wrapperInit");
    _wrapperFini = _obj.attr("wrapperFini");
    _wrapperOnceExec = _obj.attr("wrapperOnceExec");
    _wrapperOnceExecAsync = _obj.attr("wrapperOnceExecAsync");
    _wrapperError = _obj.attr("wrapperError");

    // 个性化
    _wrapperLoadRes = _obj.attr("wrapperLoadRes");
    _wrapperUnloadRes = _obj.attr("wrapperUnloadRes");
    // stream support
    _wrapperCreate = _obj.attr("wrapperCreate");
    _wrapperWrite = _obj.attr("wrapperWrite");
    _wrapperRead = _obj.attr("wrapperRead");
    _wrapperTest = _obj.attr("wrapperTestFunc");

    py::gil_scoped_release release;
    StartMonitorWrapperClass(_wrapper_abs);

}

//PyWrapper::PyWrapper(std::map <std::string, std::string> config) {
//    // if (config.count(wrapperFileKey) == 0)
//    // 带config初始化，暂未实现 todo
//    py::gil_scoped_acquire acquire;
//    _obj = py::module::import(WrapperFile).attr(WrapperClass)();
//
//    _wrapperInit = _obj.attr("wrapperInit");
//    _wrapperFini = _obj.attr("wrapperFini");
//    _wrapperOnceExec = _obj.attr("wrapperOnceExec");
//    _wrapperError = _obj.attr("wrapperError");
//    _wrapperTest = _obj.attr("wrapperTestFunc");
//
//}

PyWrapper::~PyWrapper() {
    _wrapperError.release();
    _wrapperInit.release();
    _wrapperFini.release();
    _wrapperOnceExec.release();
    _wrapperTest.release();
    _wrapperCreate.release();
    _wrapperWrite.release();
    _wrapperRead.release();
    _wrapperUnloadRes.release();
    _wrapperLoadRes.release();
    pybind11::gil_scoped_release release;
}

void PyWrapper::ReloadWrapper() {
    // 保存原有实例
    // todo 需要吗？
    // 重新import wrapper.py
    py::gil_scoped_acquire acquire;
    _wrapper.reload();

    _obj = _wrapper.attr(WrapperClass)();
    _wrapperInit = _obj.attr("wrapperInit");
    _wrapperFini = _obj.attr("wrapperFini");
    _wrapperOnceExec = _obj.attr("wrapperOnceExec");
    _wrapperError = _obj.attr("wrapperError");
    _wrapperTest = _obj.attr("wrapperTestFunc");
    // stream support

    _wrapperCreate = _obj.attr("wrapperCreate");
    _wrapperWrite = _obj.attr("wrapperWrite");
    _wrapperRead = _obj.attr("wrapperRead");

    _wrapperLoadRes = _obj.attr("wrapperLoadRes");
    _wrapperUnloadRes = _obj.attr("wrapperUnloadRes");
    pybind11::gil_scoped_release release;
}

void reloadWrapper(const std::string event_file, void *flags) {
    if (flags == nullptr) {
        return;    //什么都不做
    }
    printf("reloading...wrapper\n");
    PyWrapper *w = (PyWrapper *) flags;
    w->ReloadWrapper();
    printf("reloaded...wrapper Done!\n");
    return;
}

void PyWrapper::StartMonitorWrapperClass(std::string wrapperFileAbs) {
    FSInotify *ino = new FSInotify();
    pthread_t _pid;
    std::vector <std::string> s;
    printf("starting monitoring %s, pid is: %d\n", wrapperFileAbs.c_str(), _pid);
    s.push_back(wrapperFileAbs);
    std::map <std::string, EventHandle> funs;

    /** 变化后，重载wrapper **/
    funs.insert({"IN_MOVE_SELF", reloadWrapper});
    ino->InitWatchFile(s, this);
    int ret = ino->StartWatchThread(funs, _pid);
    if (ret != 0) {
        printf("Error starting monitoring %s, pid is: %d\n", wrapperFileAbs.c_str(), _pid);
    }
}

int PyWrapper::wrapperInit(std::map <std::string, std::string> config) {

    try {
        py::gil_scoped_acquire acquire;
        int ret = _wrapperInit(config).cast<int>();
        py::gil_scoped_release release;

        return ret;
    }
    catch (py::cast_error &e) {
        spdlog::get("stderr_console")->error("_wrapperInit cast error: {}", e.what());
        return -1;
    }
    catch (py::error_already_set &e) {
        spdlog::get("stderr_console")->error("_wrapperInit  error_already_set error: {}", e.what());
        return -1;
    }∂
}

int PyWrapper::wrapperFini() {
    try {
        py::gil_scoped_acquire acquire;
        return _wrapperFini().cast<int>();
    }
    catch (py::cast_error &e) {
        spdlog::get("stderr_console")->error("Fini cast error: {}", e.what());
        return -1;
    }
    catch (py::error_already_set &e) {
        spdlog::get("stderr_console")->error("Fini  error_already_set error: {}", e.what());
        return -1;
    }
}

int PyWrapper::wrapperOnceExec(const char *usrTag, std::map <std::string, std::string> params, DataListCls reqData,
                               pDataList *respData, std::string sid, wrapperCallback cb, unsigned int psrId) {
    SetSidUsrTag(sid, usrTag);
    try {
        if (cb != nullptr) {
            SetSidCallBack(cb, sid);
        }
        params["sid"] = sid;
        // 执行python exec 推理
        py::object r = _wrapperOnceExec(params, reqData, usrTag, psrId);
        // 此段根据python的返回 ，回写 respData
        Response *resp;
        spdlog::debug("start cast python resp to c++ object, thread_id: {}, sid: {}", gettid(), sid);
        resp = r.cast<Response *>();
        pDataList headPtr;
        pDataList curPtr;
        char *ptr;
        // 先判断python有没有抛出错误. response中的 errorCode
        if (resp->errCode != 0) {
            spdlog::error("find error from python: {}", resp->errCode);
            DelSidUsrTag(sid);
            return resp->errCode;
        }

        int dataSize = resp->list.size();
        if (dataSize == 0) {
            spdlog::error("error, not find any data from resp");
            DelSidUsrTag(sid);
            return -1;
        }
        for (int idx = 0; idx < dataSize; idx++) {
            pDataList tmpData = new (DataList);
            tmpData->next = nullptr;
            ResponseData itemData = resp->list[idx];
            char *key = strdup(itemData.key.c_str());
            tmpData->key = key;
            tmpData->len = itemData.len;
            tmpData->type = DataType(itemData.type);
            tmpData->desc = nullptr;
            // 这里判断数据类型,todo 未来根据数据类型 决定是否拷贝，比如某些数据比较大，可以不拷贝
            void *pr;
            pr = malloc(itemData.len);
            if (pr == nullptr) {
                int ret = -1;
                spdlog::error("can't malloc memory for data,  sid:{}", sid);
                DelSidUsrTag(sid);
                return ret;
            }
            ptr = PyBytes_AsString(itemData.data.ptr());
            Py_ssize_t size = PyBytes_GET_SIZE(itemData.data.ptr());
//            printf("GetSIze, %d", size);
//            printf("item data len: %d", itemData.len);
            memcpy(pr, ptr, itemData.len);
            //char *data_ = new char[itemData.data.length()+1];
            // strdup(.c_str());
            tmpData->data = pr;
            //tmpData->status = DataStatus(itemData.status);
            tmpData->status = DataOnce;
            if (idx == 0) {
                headPtr = tmpData;
                curPtr = tmpData;
            } else {
                curPtr->next = tmpData;
                curPtr = tmpData;
            }
            spdlog::debug("get result,key:{},data:{},len:{},type:{},status:{},sid:{}",
                          tmpData->key, (char *) tmpData->data, tmpData->len, tmpData->type,
                          tmpData->status, sid);
        }
        *respData = headPtr;
    }
    catch (py::cast_error &e) {
        spdlog::error("cast error: {}", e.what());
        DelSidUsrTag(sid);
        return -1;
    }
    catch (py::error_already_set &e) {
        spdlog::error("error_already_set error: {}", e.what());
        DelSidUsrTag(sid);

        return -1;
    }
    catch (const std::exception &e) {
        spdlog::error("error_already_set error: {}", e.what());
        DelSidUsrTag(sid);

        return -1;
    }
    DelSidUsrTag(sid);
    return 0;

}


int PyWrapper::wrapperOnceExecAsync(const char *usrTag, std::map <std::string, std::string> params, DataListCls reqData,
                                    std::string sid, wrapperCallback cb, unsigned int psrId) {
    try {
        if (cb != nullptr) {
            SetSidCallBack(cb, sid);

        }
        int ret = 0;
        SetSidUsrTag(sid, usrTag);
        params["sid"] = sid;
        // 执行python exec 推理
        py::object r = _wrapperOnceExecAsync(params, reqData, sid, psrId);
        // 此段根据python的返回 ，回写 respData
        spdlog::info("start wrapperExecAsync cast python resp to c++ object, thread_id: {}, sid: {}", gettid(), sid);
        ret = r.cast<int>();
        return ret;

    }
    catch (py::cast_error &e) {
        spdlog::get("stderr_console")->error("cast error: {}", e.what());
        return -1;
    }
    catch (py::error_already_set &e) {
        spdlog::get("stderr_console")->error("error_already_set error: {}", e.what());
        return -1;
    }

    return 0;

}


std::string PyWrapper::wrapperError(int err) {
    try {
        py::gil_scoped_acquire acquire;
        return _wrapperError(err).cast<std::string>();

    }
    catch (py::cast_error &e) {
        spdlog::error("cast error: {}", e.what());
        return e.what();
    }
    catch (py::error_already_set &e) {
        spdlog::error("error_already_set error: {}", e.what());
        return e.what();
    }

}

int PyWrapper::wrapperSetMetricFunc(CtrlType type, wrapperMeterCustom mc) {
    if (type == CTMeterCustom) {
        g_metric_cb = mc;
    }
    return 0;
}

int PyWrapper::wrapperSetTraceFunc(CtrlType type, wrapperTraceLog mc) {
    if (type == CTTraceLog) {
        g_trace_cb = mc;
    }
    return 0;
}

std::string
PyWrapper::wrapperCreate(const char *usrTag, std::map <std::string, std::string> params, wrapperCallback cb,
                         int *errNum, std::string sid, unsigned int psrId) {
    SessionCreateResponse *resp;
    SetSidCallBack(cb, sid);
    SetSidUsrTag(sid, usrTag);

    try {
        py::gil_scoped_acquire acquire;
        // 此段根据python的返回 ，回写 respData
        py::object r = _wrapperCreate(params, sid, psrId);
        resp = r.cast<SessionCreateResponse *>();
        *errNum = resp->errCode;
        if (*errNum != 0) {
            spdlog::get("stderr_console")->error("errNum: {}", *errNum);
        }
        return resp->handle;

    }
    catch (py::cast_error &e) {
        spdlog::get("stderr_console")->error("cast error: {}", e.what());
        return e.what();
    }
    catch (py::error_already_set &e) {
        spdlog::get("stderr_console")->error("error_already_set error: {}", e.what());
        return e.what();
    }

}

// 上行数据
int PyWrapper::wrapperWrite(char *handle, DataListCls reqData, std::string sid) {
    try {
        int ret = 0;
        // 执行python exec 推理
        py::object r = _wrapperWrite(handle, reqData, sid);
        ret = r.cast<int>();
        return ret;
    }
    catch (py::cast_error &e) {
        spdlog::get("stderr_console")->error("cast error: {}", e.what());
        return -1;
    }
    catch (py::error_already_set &e) {
        spdlog::get("stderr_console")->error("error_already_set error: {}", e.what());
        return -1;
    }
}

// 下行数据
int PyWrapper::wrapperRead(char *handle, pDataList *respData, std::string sid) {
    try {
        Response *resp;
        // 执行python exec 推理
        py::object r = _wrapperRead(handle, sid);
        spdlog::debug("start cast python resp to c++ object, thread_id: {}, sid: {}", gettid(), sid);
        resp = r.cast<Response *>();
        pDataList headPtr;
        pDataList curPtr;
        // 先判断python有没有抛出错误. response中的 errorCode
        if (resp->errCode != 0) {
            spdlog::get("stderr_console")->error("find error from python: {}", resp->errCode);
            return resp->errCode;
        }

        int dataSize = resp->list.size();
        if (dataSize == 0) {
            spdlog::get("stderr_console")->error("error, not find any data from resp");
            return -1;
        }
        for (int idx = 0; idx < dataSize; idx++) {
            pDataList tmpData = new (DataList);
            tmpData->next = nullptr;
            ResponseData itemData = resp->list[idx];
            char *key = strdup(itemData.key.c_str());
            tmpData->key = key;
            tmpData->len = itemData.len;
            tmpData->type = DataType(itemData.type);
            tmpData->desc = nullptr;
            // 这里判断数据类型,todo 未来根据数据类型 决定是否拷贝，比如某些数据比较大，可以不拷贝
            void *pr;
            pr = malloc(itemData.len);
            if (pr == nullptr) {
                int ret = -1;
                spdlog::get("stderr_console")->error("can't malloc memory for data,  sid:{}", sid);
                return ret;
            }
            memcpy(pr, (const void *) itemData.data.ptr(), itemData.len);
            //char *data_ = new char[itemData.data.length()+1];
            // strdup(.c_str());
            tmpData->data = pr;
            tmpData->status = DataStatus(itemData.status);
            if (idx == 0) {
                headPtr = tmpData;
                curPtr = tmpData;
            } else {
                curPtr->next = tmpData;
                curPtr = tmpData;
            }
            spdlog::debug("get result,key:{},data:{},len:{},type:{},status:{},sid:{}",
                          tmpData->key, (char *) tmpData->data, tmpData->len, tmpData->type,
                          tmpData->status, sid);
        }
        *respData = headPtr;
    }
    catch (py::cast_error &e) {
        spdlog::get("stderr_console")->error("cast error: {}", e.what());
        return -1;
    }
    catch (py::error_already_set &e) {
        spdlog::get("stderr_console")->error("error_already_set error: {}", e.what());
        return -1;
    }
    return 0;
}

int PyWrapper::wrapperDestroy(std::string sid) {
    DelSidCallback(sid);
    DelSidUsrTag(sid);
    return 0;
}

int PyWrapper::wrapperExecFree(const char *usrTag) {
    std::string sid = GetSidByUsrTag(usrTag);
    if (sid != "")
        DelSidUsrTag(sid);
    return 0;
}

int PyWrapper::wrapperLoadRes(pDataList p, std::string  patch_id) {
    DataListCls perData;
    //构建请求数据
    int dataNum = 0;
    for (pDataList tmpDataPtr = p; tmpDataPtr != NULL; tmpDataPtr = tmpDataPtr->next) {
        dataNum++;
    }
    if (dataNum > 0) {
        for (int tmpIdx = 0; tmpIdx < dataNum; tmpIdx++) {
            DataListNode item;
            item.key = p->key;

            // 直接拷贝
            size_t len = static_cast<size_t>(p->len);
            item.data = py::bytes((char *) (p->data), len);

            item.len = p->len;
            char t = static_cast<int>(p->type);
            item.type = p->type;
            item.status = p->status;
            spdlog::debug("reqDatatype :{}，patch_id:{}", p->type, patch_id);
            perData.list.push_back(item);
            p = p->next;
        }
    }
    py::gil_scoped_acquire acquire;
    // 执行python exec 推理
    int ret = _wrapperLoadRes(perData, patch_id).cast<int>();;
    return ret;
}

int PyWrapper::wrapperUnloadRes(std::string patch_id) {
    py::gil_scoped_acquire acquire;
    // 执行python exec 推理
    int ret = _wrapperUnloadRes(patch_id).cast<int>();;
    return ret;
}

int PyWrapper::wrapperTest() {
    py::gil_scoped_acquire acquire;
    std::vector <py::dict> req;
    py::dict item;
    item["key"] = "33";
    item["data"] = "ccccc";
    item["len"] = 11;
    item["type"] = 22;
    req.push_back(item);
    std::vector <py::object> resp;
    resp.push_back(py::str("cfc"));
    py::object ret = _wrapperTest(req, resp);
    Response *l;
    try {
        l = ret.cast<Response *>();
        // py::object r =  _wrapperOnceExec(params, reqData);
    }
    catch (py::cast_error &e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    for (int i = 0; i < l->list.size(); ++i) {
        ResponseData d = l->list[i];
//        std::cout << "Response key: " << d.key << std::endl;
        //      std::cout << "Response len" << d.len << std::endl;
        //    std::cout << "response actual data Size " << d.data.length() << std::endl;

    }
    return 0;
    //  auto message = ret.cast<std::vector<std::string>>();
    // printf("%s,:", message[0].c_str());
    // printf("%s,:", message[1].c_str());
    //  printf("resp len%d\n",resp.size());
    //   printf("%d", ret);


}

int callbackMetric(const char *usrTag, const char *meterKey, int count) {
    printf("callback Metric: %s, %s, %d\n", usrTag, meterKey, count);
    return g_metric_cb(usrTag, meterKey, count);
}

int callbackTrace(const char *usrTag, const char *key, const char *value) {
    printf("callback Trace: %s, %s, %d\n", usrTag, key, value);
    return g_trace_cb(usrTag, key, value);
}

int callBack(Response *resp, std::string sid) {
    wrapperCallback cb_;
    cb_ = GetSidCB(sid);
    if (cb_ == NULL) {
        printf("null cb....\n");
        return -1;
    }
    const char *usrTag = GetSidUsrTag(sid);

    pDataList headPtr;
    pDataList curPtr;
    // 先判断python有没有抛出错误. response中的 errorCode
    if (resp->errCode != 0) {
        spdlog::get("stderr_console")->error("find error from python: {}", resp->errCode);
        return resp->errCode;
    }
    char *ptr;
    int dataSize = resp->list.size();
    if (dataSize == 0) {
        spdlog::get("stderr_console")->error("error, not find any data from resp");
        return -1;
    }
    for (int idx = 0; idx < dataSize; idx++) {
        pDataList tmpData = new (DataList);
        tmpData->next = nullptr;
        ResponseData itemData = resp->list[idx];
        char *key = strdup(itemData.key.c_str());
        tmpData->key = key;
        tmpData->len = itemData.len;
        tmpData->type = DataType(itemData.type);
        tmpData->desc = nullptr;
        // 这里判断数据类型,todo 未来根据数据类型 决定是否拷贝，比如某些数据比较大，可以不拷贝
        void *pr;
        pr = malloc(itemData.len);
        if (pr == nullptr) {
            int ret = -1;
            spdlog::get("stderr_console")->error("can't malloc memory for data,  sid:{}", sid);
            return ret;
        }
        ptr = PyBytes_AsString(itemData.data.ptr());
        Py_ssize_t size = PyBytes_GET_SIZE(itemData.data.ptr());
        memcpy(pr, ptr, itemData.len);
        // 还是有问题：：memcpy(pr, (const void *) itemData.data.ptr(), itemData.len);
        //char *data_ = new char[itemData.data.length()+1];
        // strdup(.c_str());
        tmpData->data = pr;
        tmpData->status = DataStatus(itemData.status);
        if (idx == 0) {
            headPtr = tmpData;
            curPtr = tmpData;
        } else {
            curPtr->next = tmpData;
            curPtr = tmpData;
        }
        spdlog::debug("callback result,key:{},data:{},len:{},type:{},status:{},sid:{}",
                      tmpData->key, (char *) tmpData->data, tmpData->len, tmpData->type,
                      tmpData->status, sid);
    }

    cb_(usrTag, headPtr, 0);
    printf("ok\n:");
    return 0;

}

void SetSidCallBack(wrapperCallback cb, std::string sid) {
    RECORD_MUTEX.lock();
    SID_CB[sid] = cb;
    RECORD_MUTEX.unlock();
}

wrapperCallback GetSidCB(std::string sid) {
    wrapperCallback cb;
    RECORD_MUTEX.lock();
    cb = SID_CB[sid];
    RECORD_MUTEX.unlock();
    return cb;
}

void SetSidUsrTag(std::string sid, const char *usrTag) {
    RECORD_MUTEX.lock();
    SID_USRTAG[sid] = usrTag;
    RECORD_MUTEX.unlock();
}

const char *GetSidUsrTag(std::string sid) {
    const char *usrTag;
    RECORD_MUTEX.lock();
    usrTag = SID_USRTAG[sid];
    RECORD_MUTEX.unlock();
    return usrTag;
}


const std::string GetSidByUsrTag(const char *usrTag) {
    RECORD_MUTEX.lock();
    //通过value找 key
    for (std::map<std::string, const char *>::iterator it = SID_USRTAG.begin(); it != SID_USRTAG.end(); it++) {
        if (it->second == usrTag) {
            RECORD_MUTEX.unlock();
            return it->first;

        }
    }
    RECORD_MUTEX.unlock();

    return "";
}

void SetHandleSid(char *handle, std::string sid) {
    RECORD_MUTEX.lock();
    SID_RECORD[std::string(handle)] = sid;
    RECORD_MUTEX.unlock();
}

std::string GetHandleSid(char *handle) {
    std::string rlt;
    RECORD_MUTEX.lock();
    rlt = SID_RECORD[std::string(handle)];
    RECORD_MUTEX.unlock();
    return rlt;
}

//暂时没用上
void DelHandleSid(char *handle) {
    RECORD_MUTEX.lock();

    RECORD_MUTEX.unlock();
}

void DelSidCallback(std::string sid) {
    RECORD_MUTEX.lock();

    SID_CB.erase(sid);
    RECORD_MUTEX.unlock();
}

void DelSidUsrTag(std::string sid) {
    RECORD_MUTEX.lock();
    SID_USRTAG.erase(sid);
    RECORD_MUTEX.unlock();
}
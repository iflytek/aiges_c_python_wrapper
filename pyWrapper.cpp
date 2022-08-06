#include "pyWrapper.h"
#include <dlfcn.h>

//namespace py = pybind11;
//using namespace std::chrono_literals;


const char *wrapperFileKey = "wrapper.file";
const char *wrapperFileClass = "wrapper.class";

const char *WrapperFile = "wrapper";
const char *WrapperClass = "Wrapper";
const char *PythonSo = "libpython3.so";

PYBIND11_EMBEDDED_MODULE(aiges_embed, module
) {

    py::class_<ResponseData> responseData(module, "ResponseData");
    responseData.def(py::init<>())
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
            .def_readwrite("type", &DataListNode::type, py::return_value_policy::automatic_reference)
            .def("get_data", &DataListNode::get_data, py::return_value_policy::reference);

    py::class_<DataListCls> dataListCls(module, "DataListCls");
    dataListCls.def(py::init<>())
            .def_readwrite("list", &DataListCls::list, py::return_value_policy::automatic_reference)
            .def("get", &DataListCls::get, py::return_value_policy::reference);
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
    _wrapperError = _obj.attr("wrapperError");
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

    // 变化后，重载wrapper
    funs.insert({"IN_MOVE_SELF", reloadWrapper});
    ino->InitWatchFile(s, this);
    int ret = ino->StartWatchThread(funs, _pid);
    if (ret != 0) {
        printf("Error starting monitoring %s, pid is: %d\n", wrapperFileAbs.c_str(), _pid);
    }
}


int PyWrapper::wrapperInit(std::map <std::string, std::string> config) {
    py::gil_scoped_acquire acquire;

    return _wrapperInit(config).cast<int>();
}

int PyWrapper::wrapperFini() {
    try {
        py::gil_scoped_acquire acquire;
        return _wrapperFini().cast<int>();
    }
    catch (py::cast_error &e) {
        spdlog::error("Fini cast error: {}", e.what());
        return -1;
    }
    catch (py::error_already_set &e) {
        spdlog::error("Fini  error_already_set error: {}", e.what());
        return -1;
    }
}

int PyWrapper::wrapperOnceExec(std::map <std::string, std::string> params, DataListCls reqData,
                               pDataList *respData, std::string sid) {
    try {
        py::gil_scoped_acquire acquire;
        // 执行python exec 推理
        py::object r = _wrapperOnceExec(params, reqData);
        // 此段根据python的返回 ，回写 respData
        Response *resp;
        spdlog::debug("start cast python resp to c++ object, thread_id: {}, sid: {}", gettid(), sid);
        resp = r.cast<Response *>();
        pDataList headPtr;
        pDataList curPtr;
        // 先判断python有没有抛出错误. response中的 errorCode
        if (resp->errCode != 0) {
            spdlog::error("find error from python: {}", resp->errCode);
            return resp->errCode;
        }

        int dataSize = resp->list.size();
        if (dataSize == 0) {
            spdlog::error("error, not find any data from resp");
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
                return ret;
            }
            memcpy(pr, itemData.data.data(), itemData.len);
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
        spdlog::error("cast error: {}", e.what());
        return -1;
    }
    catch (py::error_already_set &e) {
        spdlog::error("error_already_set error: {}", e.what());
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
        std::cout << "Response key: " << d.key << std::endl;
        std::cout << "Response len" << d.len << std::endl;
        std::cout << "response actual data Size " << d.data.length() << std::endl;

    }
    return 0;
    //  auto message = ret.cast<std::vector<std::string>>();
    // printf("%s,:", message[0].c_str());
    // printf("%s,:", message[1].c_str());
    //  printf("resp len%d\n",resp.size());
    //   printf("%d", ret);


}


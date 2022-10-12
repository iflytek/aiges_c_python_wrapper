#include <strstream>
#include <chrono>
#include <bits/stdc++.h>
//#include <boost/filesystem.hpp>
#include "aiges/wrapper.h"
#include "pyWrapper.h"
#include "fswatch.h" // 支持 wrapper.py 热加载

__attribute__ ((constructor)) static void so_init(void);

__attribute__ ((destructor)) static void so_deinit(void);


py::scoped_interpreter python;  // 全局解释器
py::gil_scoped_release release; // 主线程中先释放release锁
// 全局pywrapper类实例
PyWrapper *pyWrapper;
//

const char *logDir = "./log";
const char *wrapperLogFile = "./log/wrapper.log";


void so_init(void) {
    printf("libwrapper so init.\n");
}

void so_deinit(void) {
    printf("libwrapper so deinit.\n");
}

static void init_threads() {
    if (!PyEval_ThreadsInitialized()) {
        {
            py::gil_scoped_acquire acquire;
        }
        PyEval_SaveThread();
    }
}

void initlog() {
    // change log pattern
    spdlog::set_pattern("[%l] [%Y-%m-%d %H:%M:%S.%f] [%t] %v");

    // Compile time log levels
    // define SPDLOG_ACTIVE_LEVEL to desired level
    SPDLOG_TRACE("Some trace message with param {}", {});
    SPDLOG_DEBUG("Some debug message");

    // Creating a directory
    if (mkdir(logDir, 0777) == -1) {
        printf("log目录创建失败或已存在,无需创建\n");
    } else {
        printf("log目录已创建\n");
    }
    auto file_logger = spdlog::rotating_logger_mt("mspper", wrapperLogFile, 1048576 * 10, 50);
    // Console logger
    auto console_logger = spdlog::stdout_color_mt("stdout_console");
    auto err_logger = spdlog::stderr_color_mt("stderr_console");
    spdlog::set_default_logger(file_logger);
    spdlog::flush_on(spdlog::level::err);
    spdlog::flush_every(std::chrono::seconds(5));
}

void setLog(std::string loglvl) {
    spdlog::level::level_enum lvl = spdlog::level::info;
    if (std::string("debug") == loglvl) {
        lvl = spdlog::level::debug;
    } else if (std::string("error") == loglvl) {
        lvl = spdlog::level::err;
    } else if (std::string("off") == loglvl) {
        lvl = spdlog::level::off;
    }

    spdlog::set_level(lvl); // Set global log level to debug
}

int WrapperAPI wrapperSetCtrl(CtrlType type, void *func) {
    return 0;
}

int WrapperAPI wrapperInit(pConfig cfg) {
    int ret = 0;
    init_threads();
    pyWrapper = new PyWrapper();

    initlog();

    std::string loglvl = "debug";
    std::map <std::string, std::string> config;

    for (pConfig p = cfg; p != NULL; p = p->next) {
        if (p->key != NULL && p->value != NULL) {
            config.insert({p->key, p->value});
            if (std::string("log.level") == std::string(p->key)) {
                loglvl = p->value;
                continue;
            }
        }
    }

    setLog(loglvl);
    printf("WrapperInit: 当前线程ID: %d \n", gettid());
    ret = pyWrapper->wrapperInit(config);
    return ret;
}

int WrapperAPI wrapperFini() {
    printf("WrapperFini: 当前线程ID: %d \n", gettid());
    pyWrapper->wrapperFini();
    return 0;
}

const char *WrapperAPI wrapperError(int errNum) {
    return pyWrapper->wrapperError(errNum).c_str();
}

const char *WrapperAPI wrapperVersion() {
    return "1.0.0";
}

int WrapperAPI wrapperLoadRes(pDataList perData, unsigned int resId) {
    return 0;
}

int WrapperAPI wrapperUnloadRes(unsigned int resId) {
    return 0;
}

const void *
WrapperAPI wrapperCreate(const char *usrTag, pParamList params, wrapperCallback cb, unsigned int psrIds[], int psrCnt,
                         int *errNum) {
    std::string sid = "";
    if (cb == NULL) {
        printf("cb is null\n");
    }
    for (pParamList sidP = params; sidP != NULL; sidP = sidP->next) {
        if (NULL == sidP->key) {
            continue;
        }
        if (std::string("sid") == sidP->key) {
            sid = sidP->value;
            break;
        }
    }
    spdlog::debug("create session mode, thread id is:{}, sid:{}", gettid(), sid);
    //构建请求参数
    std::map <std::string, std::string> pyParams;

    for (pParamList p = params; p != NULL; p = p->next) {
        if (NULL == p->key) {
            continue;
        }
        pyParams.insert({p->key, p->value});
        spdlog::debug("wrapper create param, key:{},value:{},sid:{}", p->key, p->value, sid);
    }

    std::string handle = pyWrapper->wrapperCreate(usrTag, pyParams, cb, errNum, sid);
    char *handlePtr = strdup(handle.c_str());
    if (*errNum != 0) {
        spdlog::debug("wrapper exec Error, errNum:{}, sid:{}", *errNum, sid);

        return NULL;
    }
    SetHandleSid(handlePtr, sid);
    return static_cast<const void *>(handlePtr);

}

int WrapperAPI wrapperWrite(const void *handle, pDataList reqData) {
    int ret = 0;
    //构建请求数据
    int dataNum = 0;
    for (pDataList tmpDataPtr = reqData; tmpDataPtr != NULL; tmpDataPtr = tmpDataPtr->next) {
        dataNum++;
    }
    std::string sid = GetHandleSid((char *) handle);
    spdlog::debug("call wrapper wrapperWrite: building req data, data num:{}，sid:{}", dataNum, sid);

    DataListCls req;
    pDataList p = reqData;
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
            spdlog::debug("reqDatatype :{}，sid:{}", p->type, sid);
            req.list.push_back(item);
            p = p->next;
        }
    }
    // 构造响应数据
    ret = pyWrapper->wrapperWrite((char *) handle, req, sid);
    if (ret != 0) {
        spdlog::get("stderr_console")->error("wrapper write error!");
    }

    return ret;
}

int WrapperAPI wrapperRead(const void *handle, pDataList *respData) {
    int ret = 0;
    std::string sid = GetHandleSid((char *) handle);
    // 构造响应数据
    printf("start read...sid %s\n", sid.c_str());
    ret = pyWrapper->wrapperRead((char *) handle, respData, sid);
    if (ret != 0) {
        spdlog::get("stderr_console")->error("wrapper read error!");
    }
    spdlog::debug("wrapperRead ret Success: {}", ret);
    return ret;
    // python层的错误 once可能没有处理好异常数据，加载器并不能崩溃
}

int WrapperAPI wrapperDestroy(const void *handle) {
    int ret = 0;
    std::string sid = GetHandleSid((char *) handle);
    ret = pyWrapper->wrapperDestroy(sid);
    if (ret != 0) {
        spdlog::get("stderr_console")->error("wrapper destroy error!");
    }
    spdlog::debug("wrapperDestroy ret Success: {}", ret);
    return ret;
}

int WrapperAPI
wrapperExec(const char *usrTag, pParamList params, pDataList reqData, pDataList *respData, unsigned int psrIds[],
            int psrCnt) {
    int ret = 0;
    std::string sid = "";
    for (pParamList sidP = params; sidP != NULL; sidP = sidP->next) {
        if (NULL == sidP->key) {
            continue;
        }
        if (std::string("sid") == sidP->key) {
            sid = sidP->value;
            break;
        }
    }
    spdlog::debug("starting exec:  is:{},sid:{}", gettid(), sid);
    //构建请求参数
    std::map <std::string, std::string> pyParams;

    for (pParamList p = params; p != NULL; p = p->next) {
        if (NULL == p->key) {
            continue;
        }
        pyParams.insert({p->key, p->value});
        spdlog::debug("wrapper exec param, key:{},value:{},sid:{}", p->key, p->value, sid);
    }
    //构建请求数据
    int dataNum = 0;
    for (pDataList tmpDataPtr = reqData; tmpDataPtr != NULL; tmpDataPtr = tmpDataPtr->next) {
        dataNum++;
    }
    spdlog::debug("call wrapper exec: building req data, data num:{}，sid:{}", dataNum, sid);

    DataListCls req;
    pDataList p = reqData;
    if (dataNum > 0) {
        for (int tmpIdx = 0; tmpIdx < dataNum; tmpIdx++) {

            DataListNode item;
            item.key = p->key;

            // 直接拷贝
            size_t len = static_cast<size_t>(p->len);
            item.data = py::bytes((char *) (p->data), len);
            //
            // 写法2：
            //            Py_ssize_t len = static_cast<Py_ssize_t>(p->len);
            //            PyObject *b = PyBytes_FromStringAndSize((char*)(p->data),len);
            //            item.data = py::reinterpret_steal<py::bytes>(b);

            // 写法3：
            //            size_t len = static_cast<size_t>(p->len);
            //            std::string data ((const char*)p->data, len);
            //            item.data = py::bytes(data);
            // todo 有无0拷贝方法？

            item.len = p->len;
            char t = static_cast<int>(p->type);
            item.type = p->type;
            spdlog::debug("reqDatatype :{}，sid:{}", p->type, sid);
            req.list.push_back(item);
            p = p->next;
        }
    }

    // 构造响应数据
    ret = pyWrapper->wrapperOnceExec(usrTag, pyParams, req, respData, sid, nullptr);
    if (ret != 0) {
        spdlog::get("stderr_console")->error("wrapper exec error!");
        return ret;
    }
    spdlog::debug("onceExec ret Success: {}", ret);
    // python层的错误 once可能没有处理好异常数据，加载器并不能崩溃
    return ret;

}

int WrapperAPI wrapperExecFree(const char *usrTag, pDataList *respData) {
    if (NULL != *respData) {
        pDataList ptr = *respData;
        while (ptr != NULL) {
            if (ptr->len > 0) {
                free(ptr->data);
                ptr->data = NULL;
            }
            if (strlen(ptr->key) > 0) {
                free(ptr->key);
                ptr->key = NULL;
            }
            pDataList tmp = ptr->next;
            delete ptr;
            ptr = NULL;
            ptr = tmp;
        }
    }
    std::string sid = GetSidByUsrTag(usrTag);
    if (sid != "") {
        DelSidUsrTag(sid);
    }
    // 构造响应数据
    int ret = pyWrapper->wrapperExecFree(usrTag);
    if (ret != 0) {
        spdlog::get("stderr_console")->error("wrapper python exec fin error {}", ret);
    }
    spdlog::debug("wrapper exec free success");
    return 0;
}

int WrapperAPI
wrapperExecAsync(const char *usrTag, pParamList params, pDataList reqData, wrapperCallback callback, int timeout,
                 unsigned int psrIds[], int psrCnt) {
    int ret = 0;
    std::string sid = "";
    for (pParamList sidP = params; sidP != NULL; sidP = sidP->next) {
        if (NULL == sidP->key) {
            continue;
        }
        if (std::string("sid") == sidP->key) {
            sid = sidP->value;
            break;
        }
    }
    spdlog::debug("starting exec async:  is:{},sid:{}", gettid(), sid);
    //构建请求参数
    std::map <std::string, std::string> pyParams;

    for (pParamList p = params; p != NULL; p = p->next) {
        if (NULL == p->key) {
            continue;
        }
        pyParams.insert({p->key, p->value});
        spdlog::debug("wrapper exec async param, key:{},value:{},sid:{}", p->key, p->value, sid);
    }
    //构建请求数据
    int dataNum = 0;
    for (pDataList tmpDataPtr = reqData; tmpDataPtr != NULL; tmpDataPtr = tmpDataPtr->next) {
        dataNum++;
    }
    spdlog::debug("call wrapper exec: building req data, data num:{}，sid:{}", dataNum, sid);

    DataListCls req;
    pDataList p = reqData;
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
            spdlog::debug("reqDatatype :{}，sid:{}", p->type, sid);
            req.list.push_back(item);
            p = p->next;
        }
    }

    // 构造响应数据
    ret = pyWrapper->wrapperOnceExecAsync(usrTag, pyParams, req, sid, callback);
    if (ret != 0) {
        spdlog::get("stderr_console")->error("wrapper exec async error!");
        return ret;
    }
    spdlog::debug("onceExecAsync ret Success: {}", ret);
    // python层的错误 once可能没有处理好异常数据，加载器并不能崩溃
    return ret;
}

const char *WrapperAPI wrapperDebugInfo(const void *handle) { return NULL; }

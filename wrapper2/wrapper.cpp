#include <strstream>
#include <chrono>
#include <boost/filesystem.hpp>
#include "aiges/wrapper.h"
#include "pyWrapper.h"



py::scoped_interpreter python;
py::gil_scoped_release release; // add this to release the GIL
// 全局pywrapper类实例
PyWrapper pyWrapper;
// 
const char *wrapperLogFile = "./log/wrapper.log";


void initlog() {
    // change log pattern
    spdlog::set_pattern("[%l] [%Y-%m-%d %H:%M:%S.%f] [%t] %v");

    // Compile time log levels
    // define SPDLOG_ACTIVE_LEVEL to desired level
    SPDLOG_TRACE("Some trace message with param {}", {});
    SPDLOG_DEBUG("Some debug message");

    // Set the default logger to file logger
    boost::filesystem::path dir("./log");
    boost::filesystem::create_directory(dir);

    auto file_logger = spdlog::rotating_logger_mt("mspper", wrapperLogFile, 1048576 * 10, 50);
    spdlog::set_default_logger(file_logger);
    spdlog::flush_on(spdlog::level::err);
    spdlog::flush_every(std::chrono::seconds(5));
}

void setlog(std::string loglvl) {
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
    py::scoped_interpreter python;
    py::gil_scoped_release release; // add this to release the GIL

    initlog();

    std::string loglvl = "error";
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

    setlog(loglvl);
    printf("Now tid is %d \n", gettid());
    ret = pyWrapper.wrapperInit(config);
    return ret;
}

int WrapperAPI wrapperFini() {
    printf("now tid is %d \n", gettid());
    pyWrapper.wrapperFini();
    return 0;
}

const char *WrapperAPI wrapperError(int errNum) {
    return pyWrapper.wrapperError(errNum).c_str();
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
    return NULL;
}

int WrapperAPI wrapperWrite(const void *handle, pDataList reqData) {
    return 0;
}

int WrapperAPI wrapperRead(const void *handle, pDataList *respData) {
    return 0;
}

int WrapperAPI wrapperDestroy(const void *handle) {
    return 0;
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
    spdlog::debug("now tid is:{},sid:{}", gettid(), sid);
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
    spdlog::debug("call wrapper exec,datanum:{}，sid:{}", dataNum, sid);

    std::vector <py::dict> req;
    pDataList p = reqData;
    if (dataNum > 0) {
        for (int tmpIdx = 0; tmpIdx < dataNum; tmpIdx++) {

            py::dict item;
            item["key"] = p->key;
            item["data"] = p->data;
            item["len"] = p->len;
            item["type"] = p->type;
            req.push_back(item);
            p = p->next;
        }
    }

    // 构造响应数据
    ret = pyWrapper.wrapperOnceExec(pyParams, req, respData, sid);
    if (ret != 0) {
        spdlog::error("wrapper exec error!");
    }
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
    spdlog::debug("wrapper exec free success");
    return 0;
}

int WrapperAPI
wrapperExecAsync(const char *usrTag, pParamList params, pDataList reqData, wrapperCallback callback, int timeout,
                 unsigned int psrIds[], int psrCnt) {
    return 0;
}

const char *WrapperAPI wrapperDebugInfo(const void *handle) { return NULL; }



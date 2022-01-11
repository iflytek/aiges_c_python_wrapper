#include <string>
#include <iostream>
#include <strstream>
#include <sstream>
#include "include/aiges/wrapper.h"
#include <boost/filesystem.hpp>

#include "pyCall.h"

void initlog()
{
    // change log pattern
    spdlog::set_pattern("[%l] [%Y-%m-%d %H:%M:%S.%f] [%t] %v");

    // Compile time log levels
    // define SPDLOG_ACTIVE_LEVEL to desired level
    SPDLOG_TRACE("Some trace message with param {}", {});
    SPDLOG_DEBUG("Some debug message");

    boost::filesystem::path dir("./log");
    boost::filesystem::create_directory(dir);


    // Set the default logger to file logger
    // auto file_logger = spdlog::basic_logger_mt("quark", "./log/quark.txt");
    auto file_logger = spdlog::rotating_logger_mt("mspper", "./log/wrapper.log", 1048576 * 10, 50);
    spdlog::set_default_logger(file_logger);
    spdlog::flush_every(std::chrono::seconds(5));
}

void setlog(std::string loglvl)
{
    spdlog::level::level_enum lvl = spdlog::level::info;
    if (std::string("debug") == loglvl)
    {
        lvl = spdlog::level::debug;
    }
    else if (std::string("error") == loglvl)
    {
        lvl = spdlog::level::err;
    }
    else if (std::string("off") == loglvl)
    {
        lvl = spdlog::level::off;
    }

    spdlog::set_level(lvl); // Set global log level to debug
}

int WrapperAPI wrapperSetCtrl(CtrlType type, void *func)
{
    return 0;
}

int WrapperAPI wrapperInit(pConfig cfg)
{
    int ret = 0;

    initlog();

    std::string loglvl = "";

    for (pConfig p = cfg; p != NULL; p = p->next)
    {
        // std::cout << p->key << " -> " << p->value << std::endl;
        // log
        if (std::string("log.level") == p->key)
        {
            loglvl = p->value;
            continue;
        }
    }

    setlog(loglvl);
    //加下一行的原因https://stackoverflow.com/questions/49784583/numpy-import-fails-on-multiarray-extension-library-when-called-from-embedded-pyt
    dlopen("libpython3.5m.so.1.0", RTLD_LAZY | RTLD_GLOBAL);
    printf("now tid is %d \n", gettid());
    return callWrapperInit(cfg);
}

int WrapperAPI wrapperFini()
{
    printf("now tid is %d \n", gettid());
    callWrapperFini();
    return 0;
}

const char *WrapperAPI wrapperError(int errNum)
{
    return callWrapperError(errNum);
}

const char *WrapperAPI wrapperVersion()
{
    return "1.0.0";
}

int WrapperAPI wrapperLoadRes(pDataList perData, unsigned int resId)
{
    return 0;
}

int WrapperAPI wrapperUnloadRes(unsigned int resId)
{
    return 0;
}

const void *WrapperAPI wrapperCreate(const char *usrTag, pParamList params, wrapperCallback cb, unsigned int psrIds[], int psrCnt, int *errNum)
{
    return NULL;
}

int WrapperAPI wrapperWrite(const void *handle, pDataList reqData)
{
    return 0;
}

int WrapperAPI wrapperRead(const void *handle, pDataList *respData)
{
    return 0;
}

int WrapperAPI wrapperDestroy(const void *handle)
{
    return 0;
}

int WrapperAPI wrapperExec(const char *usrTag, pParamList params, pDataList reqData, pDataList *respData, unsigned int psrIds[], int psrCnt)
{
    return callWrapperExec(usrTag, params, reqData, respData, psrIds, psrCnt);
}
int WrapperAPI wrapperExecFree(const char *usrTag, pDataList *respData)
{
    if (NULL != *respData)
    {
        pDataList ptr = *respData; 
        while(ptr != NULL)
        {
            if (ptr->len > 0)
            {
                free(ptr->data);
                ptr->data = NULL;
            }
            if (strlen(ptr->key)>0){
                free(ptr->key);
                ptr->key=NULL;
            }
            pDataList tmp=ptr->next;
            delete ptr;
            ptr=NULL;
            ptr=tmp;
        }
    }
    spdlog::debug("wrapper exec free success");
    return 0;
}

int WrapperAPI wrapperExecAsync(const char *usrTag, pParamList params, pDataList reqData, wrapperCallback callback, int timeout, unsigned int psrIds[], int psrCnt)
{
    return 0;
}
const char *WrapperAPI wrapperDebugInfo(const void *handle) { return NULL; }

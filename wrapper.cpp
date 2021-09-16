#include <string>
#include <iostream>
#include <strstream>
#include <sstream>




void initlog()
{
    // change log pattern
    spdlog::set_pattern("[%l] [%Y-%m-%d %H:%M:%S.%f] [%t] %v");

    // Compile time log levels
    // define SPDLOG_ACTIVE_LEVEL to desired level
    SPDLOG_TRACE("Some trace message with param {}", {});
    SPDLOG_DEBUG("Some debug message");

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

int WrapperAPI wrapperSetCtrl(CtrlType type, void* func)
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



    std::cout<<callWrapperInit()<<std::endl;

    return 0;
}

int WrapperAPI wrapperFini()
{
    std::cout<<callWrapperFini()<<std::endl;
    return 0;
}

const char *WrapperAPI wrapperError(int errNum)
{
    return "inner success";
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

const void *WrapperAPI wrapperCreate(const void* usrTag, pParamList params, wrapperCallback cb, unsigned int psrIds[], int psrCnt, int* errNum)
{
    return NULL;
}

int WrapperAPI wrapperWrite(const void *handle, pDataList reqData){
    return 0;
}

int WrapperAPI wrapperRead(const void *handle, pDataList *respData) {
    return 0;
}

int WrapperAPI wrapperDestroy(const void *handle)
{
   return 0;
}

int WrapperAPI wrapperExec(const void* usrTag, pParamList params, pDataList reqData, pDataList* respData, unsigned int psrIds[], int psrCnt)
{
    std::cout<<callWrapperExec()<<std::endl;
    return 0;
}
int WrapperAPI wrapperExecFree(const void *usrTag, pDataList *respData)
{ 
    return 0;
}

int WrapperAPI wrapperExecAsync(const void* usrTag, pParamList params, pDataList reqData, wrapperCallback callback, int timeout, unsigned int psrIds[], int psrCnt)
{
    return 0;
}
const char *WrapperAPI wrapperDebugInfo(const void *handle) { return NULL; }

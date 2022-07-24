//
// Created by 杨彦波 on 2022/7/24.
//

#ifndef AI_CPYTHON_WRAPPER_FSWATCH_H
#define AI_CPYTHON_WRAPPER_FSWATCH_H

#include <map>
#include <vector>
#include <string>
#include <sys/inotify.h>
#include <limits.h>

//监控事件定义
#define _ACCESS_EVENT      "IN_ACCESS"         //文件/目录被访问
#define _ATTRIB_EVENT      "IN_ATTRIB"         //文件元数据被修改
#define _CLOSE_WR_EVENT    "IN_CLOSE_WRITE"    //以只写方式打开的文件被关闭
#define _CLOSE_NOWR_EVENT  "IN_CLOSE_NOWRITE"  //以只读方式打开的文件被关闭
#define _CREATE_EVNT       "IN_CREATE"         //一个文件/目录 被创建
#define _DELETE_SELF_EVENT "IN_DELETE_SELF"    //一个文件/目录 被删除
#define _MODIFY_EVENT      "IN_MODIFY"         //一个文件被修改
#define _MOVE_SELF_EVENT   "IN_MOVE_SELF"      //被监控的目录/文件本身被移动
#define _MOVED_FROM_EVENT  "IN_MOVED_FROM"     //文件移除到受监控的目录之外
#define _MOVED_TO_EVENT    "IN_MOVED_TO"       //一个文件被一入受监控目录
#define _OPEN_EVNET        "IN_OPEN"           //文件被打开
#define _IGNORED_EVENT     "IN_IGNORED"        //受监控的项目被移除
#define _ISDIR_EVENT       "IN_ISDIR"          //name 返回的是一个目录
#define _UNMOUNT_EVENT     "IN_UNMOUNT"        //受监控文件被卸载

#define A_FILE_SIZE ((sizeof(struct inotify_event) + NAME_MAX + 1))   //单个监控文件结构体的大小


typedef void (*EventHandle)(const std::string, void *);

class FSInotify {
public:
    FSInotify(void);

    ~FSInotify(void);

    //初始话监控文件列表
    //参数：watch_name: 欲监控的文件名称，mask= 监控的事件，默认全部监控
    size_t InitWatchFile(std::vector <std::string> &watch_name, void *par, const uint32_t mask = IN_ALL_EVENTS);


    //删除监控文件
    //rm_file: 欲删除的文件名称
    int RmWatchFile(const std::string &rm_file);

    //开始监控
    //func: 事件处理回调函数名称,
    //  _IGNORED_EVENT, _ATTRIB_EVENT 事件是对文件被覆盖后无法监控的处理，如果用户对该事件定义了新的回调函数，
    //          可能会导致文件被覆盖或vi编辑后不再监控，以及其它错误。
    //pid: 线程ID
    int StartWatchThread(std::map <std::string, EventHandle> &func, pthread_t &_pid);

private:
    //监控线程
    static void *watch_file(void *self);

    //添加监控文件
    int AddWatchFile(std::vector <std::string> &watch_name, const uint32_t mask = IN_ALL_EVENTS);

//识别事件
    void judgeEventStr(uint32_t mask, std::string &event);

    void judgeEventName(uint32_t mask, std::string &event);

    //通过文件名找到 wd
    int findWD(const std::string &file);

    //重新监控文件
    void Rewatch(const std::string &watch_name);

    /*事件默认处理回调函数
     * 对一些常见的事件进行处理，如果用户覆盖则使用用户提供的
     * */

    /*监控目标被卸载
     *  该函数会检查所移除的文件是否存在，如果存在则从新添加监控
     * */
    static void eventUnmount(const std::string event_file, void *flags);

private:
    int m_inotifyFD;          //句柄
    bool m_isContinue;         //监控进程是否继续执行
    void *m_pParameter;         //回调函数扩展参数
    size_t m_watchNum;           //监控文件数量
    std::map<int, std::string> m_watchFiles;         //监控文件和 wd 之间的映射
    std::map <std::string, EventHandle> m_handleMap;          //事件处理回调函数集合
};

#endif //AI_CPYTHON_WRAPPER_FSWATCH_H


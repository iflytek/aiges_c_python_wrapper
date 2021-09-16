#pragma once
#include <vector>
#include <iostream>
#include <string.h>
#include <stdint.h>
#include "fast_mutex.hpp"

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#define MAKE_SINGLETON(T)   T(){} static T& get_inst(){ static T inst;  return inst;}

class NodeExecTime
    {
    public:
        char strNodeName_[32];
        char strModuleName_[32];
        unsigned int tid_;
        int64_t nStartTime_;
        int64_t nStopTime_;
        //std::vector<int16_t> args_inst_;
        virtual void NodePrint(FILE* fp){};
    };
    class NodeExecTimeDemo:public NodeExecTime
    {
    public:
        std::vector<int32_t> args_inst_;
        virtual void NodePrint(FILE* fp)
        {
            char format_str[256] = "{\"pid\":\"%s\",\"ph\":\"X\",\"tid\":\"%x\",\"name\":\"%s\",\"ts\":%lld,\"dur\":%lld, \"args\":{\"inst\":[" ;
            fprintf(fp, format_str, this->strModuleName_,this->tid_, this->strNodeName_, this->nStartTime_, this->nStopTime_ - this->nStartTime_);
            
            for (int32_t i=0;i< (int32_t) this->args_inst_.size();i++)
            {
                if (i == (int32_t)(this->args_inst_.size()-1))
                {
                    fprintf(fp, "%d", this->args_inst_[i]);
                }
                else
                {
                    fprintf(fp, "%d,", this->args_inst_[i]);
                }               
            }
        }

    };

#define G_PROFILE_TRACER ProfileTracerSingleton::get_inst()
class ProfileTracerSingleton
{
public:
    MAKE_SINGLETON(ProfileTracerSingleton)
    void reserve_mem()
    {
        vNodeTimeBuf_.reserve(1024 * 1024);
    }
    void push_perf(NodeExecTime* pnet, unsigned int tid = 0)
    {
        
		if (tid == 0)
		{
#ifdef WIN32
			pnet->tid_ = GetCurrentThreadId();
#else
			pnet->tid_ = (unsigned int)pthread_self();
#endif
		}
		else
		{
			pnet->tid_ = tid;
		}

        {
            fast_mutex_autolock autolock(mutex_);
            vNodeTimeBuf_.push_back(pnet);
        }       
    }
    void push_perf(const char *node_name, const char *module_name, int64_t start_time, int64_t stop_time,const std::vector<int32_t> &v_inst,unsigned int tid = 0)
    {
        NodeExecTimeDemo* pnet = new NodeExecTimeDemo();
        pnet->strNodeName_[sizeof(pnet->strNodeName_) - 1] = 0;
        pnet->strModuleName_[sizeof(pnet->strModuleName_) - 1] = 0;
        strncpy(pnet->strNodeName_, node_name, sizeof(pnet->strNodeName_) - 1);
        strncpy(pnet->strModuleName_, module_name, sizeof(pnet->strModuleName_) - 1);
        pnet->nStartTime_ = start_time;
        pnet->nStopTime_ = stop_time;
        pnet->args_inst_ = v_inst;
        if (tid == 0)
        {
#ifdef WIN32
            pnet->tid_ = GetCurrentThreadId();
#else
            pnet->tid_ = (unsigned int)pthread_self();
#endif
        }
        else
        {
            pnet->tid_ = tid;
        }

        {
            fast_mutex_autolock autolock(mutex_);
            vNodeTimeBuf_.push_back(pnet);
        }  
    }
    void dump_perf()
    {
        fast_mutex_autolock autolock(mutex_);
        unsigned int p = (unsigned int)(&mutex_);
        char filename_dump[32];
        sprintf(filename_dump, "./tracing%llx.txt", p);
        FILE *fp = fopen(filename_dump, "w");
        if(!fp)
        {
             std::cerr << "fail open file : "<< filename_dump << std::endl;
        }
        fprintf(fp,"{\n\"traceEvents\": [\n");
        for (unsigned int i = 0; i < vNodeTimeBuf_.size(); i++)
        {
            NodeExecTime* info_item = vNodeTimeBuf_[i];
			
            info_item->NodePrint(fp);

			fprintf(fp, "]}}");

			
            if (i != vNodeTimeBuf_.size() - 1)
            {
                fprintf(fp, ",\n");
            }
            else
            {
                fprintf(fp, "\n");
            }
        }
        fprintf(fp, "],\n\"displayTimeUnit\": \"ns\"\n}\n" );
        fclose(fp);
		//printf("close files : %s\n" ,filename_dump);
        for (unsigned int i = 0; i < vNodeTimeBuf_.size(); i++)
        {
            delete vNodeTimeBuf_[i];
            vNodeTimeBuf_[i] = NULL;
        }
        vNodeTimeBuf_.clear();
        std::vector<NodeExecTime*>().swap(vNodeTimeBuf_);
    }
private:
    fast_mutex mutex_;
    std::vector<NodeExecTime*> vNodeTimeBuf_;
};

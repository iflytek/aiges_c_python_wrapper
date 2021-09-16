/**
* @file		myTkt.h
* @brief	本程序用于联合VSPP引擎和TFEE引擎提取特征模块代码
* @author	llgu
* @version
* @data		2014-5-21 16:08:27
*/

#ifndef MYTKT_H
#define MYTKT_H

#define MAX_LINE	2048


#define MAX_WAV_SIZE	(1024 * 1024 * 200)	 // 200MB

#define ALPHA		0.5		//得分融合系数，EC权重
#define MIN_SCORE	-1000
#define TV_NORM_MEAN	0.1774898
#define TV_NORM_VAR		0.08472881

#define PLP_13  0

#define MAX_LINE_LEN  (512)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <iostream>
#ifdef WIN32
#include <process.h>
#include <Windows.h>
#include <direct.h>
#include <io.h>
#else
#include <sys/time.h>
#include <pthread.h>
#include <sys/stat.h>
#endif

#include "../../include/ifly_mspp/ifly_mspp_api.h"
#include "tinyxml.h"

using namespace std;

#pragma warning(disable : 4996)
#pragma warning(disable : 4244)

typedef struct _vid_result
{
	string user_id;
	float tv_score[3];
	float ec_score[3];
	float fused_score[3];
	float normalized_score[3];
	float confidence[3];
	int decision[3];
	int error[3];
	_vid_result()
	{
		tv_score[0] = MIN_SCORE;
		tv_score[1] = MIN_SCORE;
		tv_score[2] = MIN_SCORE;
		ec_score[0] = MIN_SCORE;
		ec_score[1] = MIN_SCORE;
		ec_score[2] = MIN_SCORE;
		fused_score[0] = MIN_SCORE;
		fused_score[1] = MIN_SCORE;
		fused_score[2] = MIN_SCORE;
		normalized_score[0] = MIN_SCORE;
		normalized_score[1] = MIN_SCORE;
		normalized_score[2] = MIN_SCORE;
		confidence[0] = -1;
		confidence[1] = -1;
		confidence[2] = -1;
		decision[0] = -1;
		decision[1] = -1;
		decision[2] = -1;
		error[0] = -1;
		error[1] = -1;
		error[2] = -1;
		user_id = "";
	}
}VIDResultAll;

typedef struct _vspp_wav_info
{
	string wav_name;
	float len[3];
	int gender;
	_vspp_wav_info()
	{
		gender = 0;
		len[0] = 0;
		len[1] = 0;
		len[2] = 0;
	}
}VSPPWavInfo;

//vspp特征头配置信息
//使用新的头定义格式，主要是FFT特征维度为256，char数值爆表了,这个后续要涉及到LID、VID系统的特征解析
//#define USE_NEW_FEAHEADER //暂时不加这个头，主要为为了后暂时不做修改
typedef struct IFLY_FEA_HEADER_INFO
{
	unsigned short headerInfo;    /* feature header info, 4bit for engine type, 8bit for engine version, 4bit for feature type*/
	char           calibInfo;     /* feature fix info, 1bit for fix info and 7bit for calibration value */
	char           reserved1;	  /* reserved parameter 1 */
	unsigned int   featBytes;     /* feature data buffer length in bytes */             
	unsigned int   frameNum;      /* feature data frame number */   
#ifndef USE_NEW_FEAHEADER
	char           dimAligned;    /* feature dimension in SSE aligned buffer */
	char           dimReal;       /* feature real dimension */
	char           reserved2;	  /* reserved parameter 2 */
	char           reserved3;	  /* reserved parameter 3 */
#else //FFT特征维度128超标，改用short实现20170710
	short		   dimAligned;    /* feature dimension in SSE aligned buffer */
	short          dimReal;       /* feature real dimension */
#endif
}IflyFeaHeaderInfo,*PIflyFeaHeaderInfo;

class myTkt
{
public:
	myTkt()
	{
		fp_ec_rslt = NULL;
		fp_tv_rslt = NULL;
		fp_fused_rslt = NULL;
		fp_enroll_rslt = NULL;
		fp_log = NULL;
		fp_error = NULL;
		fp_processed = NULL;
		fp_fea_info = NULL;
		_elem_cnt = 0;
		_tv_norm_mean = (float) TV_NORM_MEAN;
		_tv_norm_var = (float) TV_NORM_VAR;
		_process_mode = 2;//默认为特征提取模块
	}
	~myTkt()
	{
		if (fp_log)
		{
			fclose(fp_log);
			fp_log = NULL;
		}
		if (fp_ec_rslt)
		{
			fclose(fp_ec_rslt);
			fp_ec_rslt = NULL;
		}
		if (fp_tv_rslt)
		{
			fclose(fp_tv_rslt);
			fp_tv_rslt = NULL;
		}
		if (fp_fused_rslt)
		{
			fp_fused_rslt = NULL;
		}
		if (fp_enroll_rslt)
		{
			fp_enroll_rslt = NULL;
		}
		if (fp_error)
		{
			fclose(fp_error);
			fp_error = NULL;
		}
		if (fp_processed)
		{
			fclose(fp_processed);
			fp_processed = NULL;
		}
	}
	// 配置文件解析
	int parse_cfg(const char *cfg_path,const char *wav_list_path,const char *result_dir_path,const char * thread_num, const char * loop_cnt)
	{
		FILE *fp = NULL;
		if (!(fp = fopen(cfg_path, "rt")))
		{
			return -1;
		}

		char line[MAX_LINE];
		char name[MAX_LINE];
		char val[MAX_LINE];
		char eqflag[MAX_LINE];

		_loop_count = atoi(loop_cnt)>1?atoi(loop_cnt):1;
		_loop_num_count = 1;
		_num_count = 0;
		_audio_type = 0;
		//2019-8-16 yfxie5
		_identify_path = wav_list_path;
		_rslt_path = result_dir_path;
		_enroll_path = "";
		_processed_list_path = "";
		_conf_dir = "";
		_fea_write_mode = 0;
		_xml_write = 0;
		_wav_write = 0;
		_process_mode = 0;
		_num_threads = atoi(thread_num)>1?atoi(thread_num):1;
		_out_data_write = 0;
		_out_binary_feature = 0;
		_elem_cnt = 0;
		//for(int i=0;i<1000;i++)
		//{identify_file_list[i] = "";}


		while(fgets(line, MAX_LINE, fp) != NULL)
		{
			if (sscanf(line, "%s %s %s\n", name, eqflag, val) != EOF)
			{
				if (strcmp(eqflag, "=") || !strcmp(name, "#"))
				{
					continue;
				}

				/*if (!strcmp(name, "identify_path"))
				{
					_identify_path = val;
					continue;
				}
				if (!strcmp(name, "result_path"))
				{
					_rslt_path = val;
					continue;
				}*/
				//yfxie
				if (!strcmp(name, "conf_dir"))
				{
					_conf_dir = val;
					continue;
				}
				if(!strcmp(name, "audio_type"))
				{
					_audio_type = atoi(val);
					continue;
				}
				/*if (!strcmp(name, "loop_count"))
				{
				_loop_count = atoi(val);
				continue;
				}*/
				if (!strcmp(name, "fea_write_mode"))
				{
					_fea_write_mode = atoi(val);
					continue;
				}
				if (!strcmp(name, "process_mode"))
				{
					_process_mode = atoi(val);
					continue;
				}
				/*if (!strcmp(name, "num_threads"))
				{
					_num_threads = atoi(val);
					continue;
				}*/
				if (!strcmp(name, "out_data_write"))
				{
					_out_data_write = atoi(val);
					continue;
				}
				if (!strcmp(name, "out_binary_feature"))
				{
					_out_binary_feature = atoi(val);
					continue;
				}
                if (!strcmp(name, "write_xml"))
                {
                    _xml_write  = atoi(val);
                    continue;
                }
				if (!strcmp(name, "write_wav"))
				{
					_wav_write  = atoi(val);
					continue;
				}
			}
		}
		fclose(fp);
		return 0;
	}

	int get_num_threads()
	{
		return _num_threads;
	}

	// 初始化VSPP，TFEE 及相关全局变量
	int init()
	{
		int ret = 0;
#ifdef WIN32
		mkdir("./../log");
#else
		mkdir("./../log", 777);
#endif
		// 初始化log文件
		if (!(fp_log = fopen("./../log/myMsppTest.log", "a+")))
		{
			printf("ERROR: Failed to create log file\n");
			return -1;
		}

		fprintf(fp_log, "===================================================\n");
		fprintf(fp_log, "Application: myMsppTest.exe  %s", get_cur_time().c_str());
        fprintf(fp_log, "input dir: %s\n" , _identify_path.c_str());
        fprintf(fp_log, "thread num: %d\n", _num_threads);
        fprintf(fp_log, "output dir : %s\n", _rslt_path.c_str());
        fprintf(fp_log, "===================================================\n");

		string fea_result = _rslt_path;
		#ifdef WIN32
		struct _stat buf;
		if (_stat(_rslt_path.c_str(), &buf) != 0)
		{	// 如果不存在
			_mkdir(_rslt_path.c_str());
		}
		#else
		struct stat buf;
		if (stat(_rslt_path.c_str(), &buf) != 0)
		{	// 如果不存在
			mkdir(_rslt_path.c_str(),777);
		}
		#endif

		fea_result += "/pure_feature";
#ifdef WIN32
		//struct _stat buf;
		if (_stat(fea_result.c_str(), &buf) != 0)
		{	// 如果不存在
			mkdir(fea_result.c_str());
		}
#else
		//struct stat buf;
		if (stat(fea_result.c_str(), &buf) != 0)
		{	// 如果不存在
			mkdir(fea_result.c_str(),777);
		}
#endif
		

		fea_result = _rslt_path;
		fea_result += "/xml";
#ifdef WIN32
		//struct _stat bufl;
		if (_stat(fea_result.c_str(), &buf) != 0)
		{	// 如果不存在
			mkdir(fea_result.c_str());
		}
#else
		//struct stat buf;
		if (stat(fea_result.c_str(), &buf) != 0)
		{	// 如果不存在
			mkdir(fea_result.c_str(),777);
		}
#endif
		_processed_list_path = _rslt_path;
		_processed_list_path += "/processed_list.txt";
		if (!(fp_processed = fopen(_processed_list_path.c_str(), "w+")))
		{
			printf("ERROR: Failed to create processed list. %s\n", _processed_list_path.c_str());
			return -1;
		}

		string rslt_file = _rslt_path + "/error_list.txt";

		if (!(fp_error = fopen(rslt_file.c_str(), "w+")))
		{
			printf("ERROR: Failed to create error list. %s\n", rslt_file.c_str());
			return -1;
		}

		if ( _process_mode == 2 )
		{	// 只提取特征，原始数据放在测试文件夹中
			if (get_enroll_identify_list(_identify_path.c_str(), _identify_file_list) != 0
				|| _identify_file_list.size() <= 0)
			{
					printf("Number of identify File is zero!\n");
					return -1;
			}

			//存入数组2018-11-13
			/*for(int i = 0; i < _identify_file_list.size(); i++)
			{
			identify_file_list[i] = _identify_file_list[i];
			}*/

		}
		else
		{
			return -1;
		}

		// 初始化
		ret = MsppInitialize(NULL,(char*)_conf_dir.c_str());//yfxie
		if(MSPP_SUCCESS != ret )
		{
			printf("ERROR: Initialization failed. ret = %d\n", ret);
			return -1;
		}
#ifdef WIN32
		_handle_mutx_read = CreateMutex(NULL, FALSE, NULL);
		_handle_mutex_write = CreateMutex(NULL, FALSE, NULL);
#else
		int ret1 = -1 ;
		int ret2 = -1;
		ret1 = pthread_mutex_init(&_handle_mutx_read,NULL);
		ret2 = pthread_mutex_init(&_handle_mutex_write,NULL);
#endif
		return 0;
	}

	int uninit()
	{
		//逆初始化VSPP引擎
		MsppUninitialize();

		_enroll_dir_list.clear();
		_identify_file_list.clear();
#ifdef WIN32
		CloseHandle(_handle_mutex_write);
		CloseHandle(_handle_mutx_read);
#else
		pthread_mutex_destroy(&_handle_mutx_read);
		pthread_mutex_destroy(&_handle_mutex_write);
#endif
		if(fp_processed)
		{
			fclose(fp_processed);
			fp_processed = NULL;
		}
		if(fp_log)
		{
			fclose(fp_log);
			fp_log = NULL;
		}
		if (fp_ec_rslt)
		{
			fclose(fp_ec_rslt);
			fp_ec_rslt = NULL;
		}
		if (fp_tv_rslt)
		{
			fclose(fp_tv_rslt);
			fp_tv_rslt = NULL;
		}
		if (fp_fused_rslt)
		{
			fclose(fp_fused_rslt);
			fp_fused_rslt = NULL;
		}
		if (fp_error)
		{
			fclose(fp_error);
			fp_error = NULL;
		}
		if (fp_fea_info)
		{
			fclose(fp_fea_info);
			fp_fea_info = NULL;
		}
		return 0;
	}

	int print_info()
	{
		printf("=======================================================\n\n");
		printf("Application: myVspp.exe  %s", get_cur_time().c_str());
		if (_process_mode == 0)
		{
			printf("Step: --=| Speaker Enrollment |=--.\n\n");
		} else if (_process_mode == 1)
		{
			printf("Step for: --=| Speaker Recognition |=--.\n\n");
		} else if (_process_mode == 2)
		{
			printf("Step for: --=| Feature Extraction |=--.\n\n");
		}
		printf("============ configuration =============================\n\n");

		printf("Recognition path:       %s\n", _identify_path.c_str());
		printf("Recognition elements:   %d\n", _identify_file_list.size());

		printf("Result saved path:      %s\n", _rslt_path.c_str());
		printf("Number of threads:      %d\n", _num_threads);
		printf("Feature write mode:     %d\n", _out_data_write);

		printf("\n============= configuration end =======================\n");
		printf("Still continue ... ? (y/n)\n");

		return 0;
	}

	// 去字符串尾部的换行符操作
	bool FilteString(char szString[])
	{
		//	strcpy(szString,"			\n		abc   \t   \n   \t   ");
		int nLen=-1;
		nLen=(int)strlen(szString);

		int i,j;
		if(nLen<0)
		{
			return false;
		}
		for(i=0;i<nLen;i++)
		{
			if(szString[i]!=' '&&szString[i]!='\t'&&szString[i]!=10&&szString[i]!=13)
			{
				break;
			}
		}

		for(j=nLen-1;j>=0;j--)
		{
			if(szString[j]!=' '&&szString[j]!='\t'&&szString[j]!=10&&szString[j]!=13)
			{
				break;
			}
		}
		if(j<i)
		{
			szString[0]=0;
		}
		else
		{
			char szTmp[10000];
			strcpy(szTmp,szString);
			memcpy(szString,szTmp+i,j-i+1);
			szString[j-i+1]=0;
		}
		nLen=(int)strlen(szString);
		if(szString[0]=='\"'&&szString[nLen-1]=='\"')
		{
			szString[nLen-1]=0;
			char szTmp[10000];
			strcpy(szTmp,szString);
			strcpy(szString,szTmp+1);
		}
		return true;
	}

	int start()
	{
#ifdef WIN32
		HANDLE   *threads_handle = NULL;
		unsigned *threads_id = NULL;

		threads_id = (unsigned *)malloc(_num_threads * sizeof(*threads_id));
		threads_handle = (HANDLE *)malloc(_num_threads * sizeof(*threads_handle));
		if (!threads_id || !threads_handle)
		{
			printf("Memory allocate failed.\n");
			if(threads_handle ) 
			{
				free(threads_handle);threads_handle = NULL;
			}
			if(threads_id){
				free(threads_id);threads_id = NULL;
			}
			return -1;
		}
#else
		pthread_t *threads_handle = NULL;
		int  *threads_id = NULL;
		threads_id = (int *)malloc(_num_threads * sizeof(int));
		memset(threads_id , 0 , _num_threads * sizeof(int));
		threads_handle = (pthread_t *)malloc(_num_threads * sizeof(threads_handle));
		memset(threads_handle , 0 , _num_threads * sizeof(threads_handle));
		if (!threads_id || !threads_handle)
		{
			printf("Memory allocate failed.\n");
			if(threads_handle ) 
			{
				free(threads_handle);threads_handle = NULL;
			}
			if(threads_id){
				free(threads_id);threads_id = NULL;
			}
			return -1;
		}
#endif
		for (int i = 0; i < _num_threads; i++)
		{
#ifdef WIN32
			threads_handle[i] = (HANDLE) _beginthreadex(NULL, 0, process, this, 0, &threads_id[i]);
			
			
			//每个线程绑定CPU2018-11-13
			//SetThreadAffinityMask(threads_handle[i],(unsigned long)i);sss
			Sleep(20);	// In case of resource over exploded (especially Database).

			if(threads_handle[i] == NULL)
			{
				printf("Creat Thread Error\n");
			}
#else
			threads_id[i] = pthread_create(&threads_handle[i],NULL, process,this);
			//sleep(1);
#endif
		}

#ifdef WIN32
		WaitForMultipleObjects(_num_threads, threads_handle, TRUE, INFINITE);
#else
		for(int i = 0 ; i< _num_threads ;i++)
		{
			if(threads_id[i] == 0)
			{
				pthread_join(threads_handle[i] , NULL);
			}
		}
#endif
		if(threads_handle ) 
		{
			free(threads_handle);
			threads_handle = NULL;
		}
		if(threads_id){
			free(threads_id);
			threads_id = NULL;
		}
		//free(threads_handle);
		//free(threads_id);
		return 0;
	}
#ifdef WIN32
	static unsigned int _stdcall process(void *params)
	{
		myTkt *obj = (myTkt *)params;
		obj->process_instace();
		return 0;
	}
#else
	static void* process(void *params)
	{
		myTkt *obj = NULL;
		obj = (myTkt *)params;

		obj->process_instace();

		return  NULL;
	}
#endif
protected:
	int process_instace()
	{
		Mspp_INST inst_i = {0};
		//char *wav_buf = NULL;
		int ret;
		//wav_buf = (char*) malloc(sizeof(char)*MAX_WAV_SIZE);
		//memset(wav_buf,0 ,MAX_WAV_SIZE);
		/*if (!wav_buf)
		{
		printf("ERROR: Memory allocate failed.\n");
		exit(-1);
		}*/

		// 创建实例
		ret = MsppCreateInst(&inst_i, NULL);
		if( ret!= MSPP_SUCCESS )
		{
			printf("ERROR MsppCreateInst failed. Ret = %d\n", ret);
			return -1;
		}

		//核心特征处理模块
		//feature_extract(inst_i, wav_buf);//改为一个参数，wav_buf在下面定义，解决句柄泄露问题
		feature_extract(inst_i);

		//销毁实例
		ret = MsppDestroyInst(inst_i);
		if( ret!= 0)
		{
			printf("ERROR MsppDestroyInst failed. Ret = %d\n", ret);
			return -1;
		}

		//free(wav_buf);

#ifdef WIN32
		_endthreadex(0);
#else
		//pthread_exit(0);
#endif
		return 0;
	}

	void write_data_to_file(string identify_file, string extension, char* buffer, int len)
	{
		string xml_file = _rslt_path + "\\vsppxml\\";
		xml_file += get_post_name(identify_file, _identify_path) + extension;
		FILE *fp_xml;
		fp_xml = fopen(xml_file.c_str(),"wt");
		fwrite(buffer,len,1,fp_xml);
		fclose(fp_xml);
	}

	//写文件函数yfxie
	int save_file(const char* pBuf, int nBufLen, std::string filename)
	{
		std::string tmp_wav_file;
		tmp_wav_file = filename;

		FILE* fFile = fopen(tmp_wav_file.c_str(), "wb");
		assert( fFile != NULL);
		if (fFile == NULL)
		{
			return -1;
		}
		int writelen = fwrite(pBuf,sizeof(char), nBufLen, fFile);
		fclose(fFile);
		assert( writelen == nBufLen );
		if ( writelen != nBufLen )
		{
			return -1;
		}
		return 0;
	}

    //int feature_extract(Mspp_INST vspp_inst, char *wav_buf)
	int feature_extract(Mspp_INST vspp_inst)
	{
		int ret = 0;
		char * wav_buf = NULL;//yfxie2018-10-23

		wav_buf = (char*) malloc(sizeof(char)*MAX_WAV_SIZE);

		//memset(wav_buf,0 ,MAX_WAV_SIZE);

		unsigned int wav_data_size =0;

		while (true)
		{
			// 读取数据
			string identify_file;
			#ifdef WIN32
			WaitForSingleObject(_handle_mutx_read, INFINITE);
			#else
			int ret = -1;
			ret = pthread_mutex_lock(&_handle_mutx_read);
			#endif
			if (_elem_cnt >= _identify_file_list.size())
			{
				_loop_count--;

				if(_loop_count <= 0)
				{
				    #ifdef WIN32
					ReleaseMutex(_handle_mutx_read);
					#else
					pthread_mutex_unlock(&_handle_mutx_read);
					#endif
					break;
				}
				_elem_cnt = 0;
				_loop_num_count++;
			}
			identify_file = _identify_file_list[_elem_cnt++];
			//identify_file = identify_file_list[_elem_cnt++];//11-13

			_num_count++;

			#ifdef WIN32
			ReleaseMutex(_handle_mutx_read);
            #else
            pthread_mutex_unlock(&_handle_mutx_read);
            #endif
			printf("Loop %d file %d for %s...\n", _loop_num_count, _num_count, identify_file.c_str());

			int succ_flag = 0;
			/*
			VSPPWavInfo wav_info;
			wav_info.wav_name = identify_file;
			wav_info.len[0] = 0;
			wav_info.len[1] = 0;
			wav_info.len[2] = 0;
			*/
			//wstring audio_type = char2wchar(_audio_type.c_str());
			MsppWaveType audio_type = (MsppWaveType)_audio_type;

			while (true)
			{
				int ret = 0;				
#if 1
				ret = read_wav_data(identify_file,wav_buf ,wav_data_size);
				if (ret != 0)
				{
					break;
				}
#endif
#if 1//10-22
				MsppAudioStatus vspp_as = MSPP_AUDIO_BEGIN;
				ret = MsppAudioWrite(vspp_inst, audio_type, wav_buf, wav_data_size, &vspp_as);
				if(ret != 0)
				{
					break;
				}
				vspp_as = MSPP_AUDIO_END;
				ret = MsppAudioWrite(vspp_inst, audio_type, wav_buf, 0, &vspp_as);
				if(ret != 0)
				{
					break;
				}
#endif
#if 1//10-20
                //XML提取
                const void *xml_result = NULL;
                unsigned int xml_length = 0;

                ret = MsppGetXmlResult(vspp_inst, &xml_result, &xml_length);
                if(ret != 0)
                {
                    fprintf(fp_log, "%s: GetResultfailed, ret = %d\n" ,identify_file.c_str(), ret);
                    break;
                }
                //XML结果写出来
                if (_xml_write == 1)
                {
					char xml_name[MAX_LINE_LEN];
					strcpy(xml_name,identify_file.c_str());
                    //string xml_file = _rslt_path + "\\xml\\";
					string xml_file = _rslt_path + "/xml/";
                    xml_file += get_file_main_name(xml_name) + ".xml";
                    FILE *fp_xml;
                    fp_xml = fopen(xml_file.c_str(),"wt");
                    fwrite(xml_result,xml_length,1,fp_xml);
                    fclose(fp_xml);
                }
#endif
				//***********************在此加入其它接口函数的测试yfxie*******************************//
	#if 1//10-18
				//获取说话人个数spk_num
				int spk_num = 0;
				char *str_tmp1 = NULL;
				char *str_tmp2 = NULL;
				str_tmp1 = strstr((char *)xml_result,"<speaker_num>");
				str_tmp2 = strstr((char *)xml_result,"</speaker_num>");
				str_tmp1 += 13;
				int tmp = str_tmp2 - str_tmp1;
				str_tmp1[tmp] = '\0';
				spk_num = atoi(str_tmp1);

				//string xml_path = _rslt_path + "\\xml\\";
				string xml_path = _rslt_path + "/xml/";
				xml_path += get_post_name(identify_file, _identify_path) + ".xml";
				char file_name[512]= {0};
				strcpy(file_name,xml_path.c_str());

				if(spk_num <= 0)
				{
					printf(" Error Spk Num: %s\n",file_name);
					//continue;
					break;
				}
				cout<<"spk num: "<<spk_num<<endl;
				//结束获取说话人个数

//新接口测试
	#endif //10-18
				//宏定义开关
#define HTK_FORMAT
#define	OUTPUT_WAV

				const void *pp_audio_data = NULL;
				unsigned int pp_audio_data_len = 0;

				MsppHeadInfo  *mspp_head_info = NULL;
	
				//memset(&mspp_head_info,0,sizeof(mspp_head_info));
				//int spk_num = 1;//10-18
				for(int i= 0 ; i < spk_num ; i++)
				{
	#if 1//10-18
					char suffix[64];
					sprintf(suffix,"%d##",i);//区分不同说话人

					ret = MsppGetFeature(vspp_inst,i,(void**)&mspp_head_info);
					if(0 != ret)
					{
						printf("MsppGetFeature error : %d\n",ret);
						continue;
					}
	#endif
					//cout<<"engine_version："<<mspp_head_info->engine_version<<endl;
					//cout<<"feature_num："<<mspp_head_info->feature_num<<endl;
					//cout<<"feature_head_size："<<mspp_head_info->feature_head_size<<endl;


	#if 1
					//是否保存二进制特征yfxie2019-08-15
#if 1
					if(_out_binary_feature == 1)
					{
						char identify_name_pk[MAX_LINE_LEN];
						strcpy(identify_name_pk,identify_file.c_str());
						string txt_file_name = _rslt_path + "/pure_feature/" + string(suffix) + get_file_main_name(identify_name_pk) +  string(".mspp");
						//存储格式：版本号 + 定、浮点 + 特征数目 + 特征1：名称、大小、内容 + 特征2 ...
						int is_fix_feature = 1;//0:float 1:fixs
						save_binary_feature(mspp_head_info, is_fix_feature, txt_file_name);//for shenzhen_PK 每条音频存特征结构体数据
					}
					
#endif
					if(_out_data_write==1)
					{
						
					//取特征
					for(int i=0;i<mspp_head_info->feature_num;i++)
					{
						//cout<<mspp_head_info->fea_head[i].fea_type<<endl;
						//cout<<mspp_head_info->fea_head[i].feature_size<<endl;
						//cout<<mspp_head_info->fea_head[i].feature_address<<endl;

						//保存PLP文件
						if(mspp_head_info->fea_head[i].fea_type==0)
						{
							if(mspp_head_info->fea_head[i].feature_size >0)
							{
								char identify_name[MAX_LINE_LEN];
								strcpy(identify_name,identify_file.c_str());

								string plp_file_name = _rslt_path + "/pure_feature/";
								plp_file_name = plp_file_name + string(suffix) + get_file_main_name(identify_name) + ".plp";

								#ifdef HTK_FORMAT //保存HTK格式
								write_htk_fea_file((char *)mspp_head_info->fea_head[i].feature_address,plp_file_name,1);
								#else
								save_file((char *)mspp_head_info->fea_head[i].feature_address,mspp_head_info->fea_head[i].feature_size,plp_file_name);
								#endif
								cout<<"end get plp39!"<<endl;
							}
						}

						//保存FB48文件
						if(mspp_head_info->fea_head[i].fea_type==2)
						{
							if(mspp_head_info->fea_head[i].feature_size >0)
							{
								char identify_name[MAX_LINE_LEN];
								strcpy(identify_name,identify_file.c_str());

								string fb_file_name = _rslt_path + "/pure_feature/";
								fb_file_name = fb_file_name + string(suffix) + get_file_main_name(identify_name) + ".fb";
							
								#ifdef HTK_FORMAT //保存HTK格式
								write_htk_fea_file((char *)mspp_head_info->fea_head[i].feature_address,fb_file_name,1);
								#else
								save_file((char *)mspp_head_info->fea_head[i].feature_address,mspp_head_info->fea_head[i].feature_size,fb_file_name);
								#endif
								cout<<"end get fb48!"<<endl;
							}
						}

						//保存PNCC39文件
						if(mspp_head_info->fea_head[i].fea_type==1)
						{
							if(mspp_head_info->fea_head[i].feature_size >0)
							{
								char identify_name[MAX_LINE_LEN];
								strcpy(identify_name,identify_file.c_str());

								string pncc_file_name = _rslt_path + "/pure_feature/";
								pncc_file_name = pncc_file_name + string(suffix) + get_file_main_name(identify_name) + ".pncc";

								#ifdef HTK_FORMAT //保存HTK格式
								write_htk_fea_file((char *)mspp_head_info->fea_head[i].feature_address,pncc_file_name,1);
								#else
								save_file((char *)mspp_head_info->fea_head[i].feature_address,mspp_head_info->fea_head[i].feature_size,pncc_file_name);
								#endif
								cout<<"end get pncc39!"<<endl;
							}
						}

						//保存FB64文件
						if(mspp_head_info->fea_head[i].fea_type==3)
						{
							if(mspp_head_info->fea_head[i].feature_size >0)
							{
								char identify_name[MAX_LINE_LEN];
								strcpy(identify_name,identify_file.c_str());

								string fb64_file_name = _rslt_path + "/pure_feature/";
								fb64_file_name = fb64_file_name + string(suffix) + get_file_main_name(identify_name) + ".fb64";

								#ifdef HTK_FORMAT //保存HTK格式
								write_htk_fea_file((char *)mspp_head_info->fea_head[i].feature_address,fb64_file_name,1);
								#else
								save_file((char *)mspp_head_info->fea_head[i].feature_address,mspp_head_info->fea_head[i].feature_size,fb64_file_name);
								#endif
								cout<<"end get fb64!"<<endl;
							}
						}


					}//end for feature_Num
					
					}
					
						//获取语音
					#ifdef OUTPUT_WAV
						char wav_name[MAX_LINE_LEN];
						strcpy(wav_name,identify_file.c_str());

						string wav_file_name = _rslt_path + "/pure_feature/";
						wav_file_name = wav_file_name + string(suffix) + get_file_main_name(wav_name) + ".wav";

						ret = MsppGetAudioData(vspp_inst, i ,&pp_audio_data,&pp_audio_data_len);
						if(0 != ret)
						{
							printf("MsppGetAudioData error : %d\n",ret);
							continue;
						}
						if(_wav_write == 1)
						{
							save_file((char *)pp_audio_data,pp_audio_data_len, wav_file_name);
							cout<<"end get wav!"<<endl;
						}
					#endif

					
					

	#endif//if 0
					
				}//end for spk_num
				
				//***********************结束加入其它接口函数的测试*******************************//

				// 特征提取
				succ_flag = 1;
				break;
			}
			#ifdef WIN32
			WaitForSingleObject(_handle_mutex_write, INFINITE);
			#else
			pthread_mutex_lock(&_handle_mutex_write);
			#endif
			fprintf(fp_processed, "%s\n", identify_file.c_str());
			if (!succ_flag)
			{
				fprintf(fp_error, "%s\n", identify_file.c_str());
			}
            #ifdef WIN32
			ReleaseMutex(_handle_mutex_write);
			#else
			pthread_mutex_unlock(&_handle_mutex_write);
			#endif
		}

		if(NULL != wav_buf )
		{
			free(wav_buf);
		}
		return 0;
	}
	int get_enroll_identify_list(const char *path, vector<string> &list)
	{
		int ret = 0;
#ifdef WIN32
		if (is_dir(path)) {
			if (_process_mode == 1) {
				ret = get_filtered_file_list(path, _processed_list_path.c_str(), list);
			} else if (_process_mode == 0) {
				ret = get_filtered_dir_list(path, _processed_list_path.c_str(), list);
			} else if (_process_mode == 2){
				ret = get_filtered_file_list(path, _processed_list_path.c_str(), list);
			} else {
				ret = -1;
			}
		} else
#endif
		if (is_file(path)){
			ret = get_file_filtered_content(path, _processed_list_path.c_str(), list);
		} else {
			return -1;
		}
		return ret;
	}
#ifdef WIN32
	int get_filtered_dir_list(const char *dir_path, const char *omit_list_file, vector<string> &dir_list)
	{
		char flt[MAX_PATH] = "*.*";

		intptr_t	find_handle;
		_finddata_t	find_data;
		char		find_file[MAX_PATH];

		vector<string> omit_list;
		if (omit_list_file != NULL) {
			get_file_filtered_content(omit_list_file, NULL, omit_list);
		}

		sprintf(find_file, "%s\\%s", dir_path, flt);
		find_handle = _findfirst(find_file, &find_data);
		if ( -1 == find_handle )
			return -1;

		// get all file
		do {
			// skip . & ..
			if ( !strcmp(find_data.name, ".") || !strcmp(find_data.name, ".."))
				continue;

			// recursive find ?
			if ( find_data.attrib & _A_SUBDIR ) {
				sprintf(find_file, "%s\\%s", dir_path, find_data.name);

				// 忽略omit_list中存在的文件
				vector<string>::iterator it = find(omit_list.begin(), omit_list.end(), find_file);
				if (it == omit_list.end()) {
					dir_list.push_back(find_file);
				}
			}
		}while ( -1 != _findnext(find_handle, &find_data));

		_findclose(find_handle);
		return 0;
	}

	int get_filtered_file_list(const char *dir_path, const char *omit_list_file, vector<string> &file_list, bool recursive = true)
	{
		char flt[MAX_PATH] = "*.*";

		intptr_t	find_handle;
		_finddata_t	find_data;
		char		find_file[MAX_PATH];

		int ret;
		vector<string> omit_list;
		if (omit_list_file != NULL) {
			get_file_filtered_content(omit_list_file, NULL, omit_list);
		}

		sprintf(find_file, "%s\\%s", dir_path, flt);
		find_handle = _findfirst(find_file, &find_data);
		if ( -1 == find_handle )
			return -1;

		// get all file
		do {
			// skip . & ..
			if ( !strcmp(find_data.name, ".") || !strcmp(find_data.name, ".."))
				continue;

			// recursive find ?
			if ( find_data.attrib & _A_SUBDIR )
			{
				if ( recursive )
				{
					char find_path[MAX_PATH];
					sprintf(find_path, "%s\\%s", dir_path, find_data.name);
					ret = get_filtered_file_list(find_path, omit_list_file, file_list, recursive);
					if ( ret != 0 )
						break;
				}
			}
			else
			{
				// 忽略omit_list中存在的文件
				vector<string>::iterator it = find(omit_list.begin(), omit_list.end(), find_file);
				// push to list
				sprintf(find_file, "%s\\%s", dir_path, find_data.name);
				if (it == omit_list.end()) {
					file_list.push_back(find_file);
				}
			}
		}while ( -1 != _findnext(find_handle, &find_data));

		_findclose(find_handle);
		return 0;
	}
#endif
	int get_file_filtered_content(const char *file_path, const char *omit_list_file, vector<string> &file_list)
	{
		FILE *fp = NULL;
        char buf[4096] = {0};

		vector<string> omit_list;
#ifdef WIN32
		if (omit_list_file) {
			get_file_filtered_content(omit_list_file, NULL, omit_list);
		}
#endif
		if (!(fp = fopen(file_path, "rt"))) {
			return -1;
		}
#if 0
		while (fgets(buf, 4096, fp) != NULL)
		{
			size_t len = strlen(buf);
			if (buf[len - 1] == '\n')
			{
#ifdef WIN32
				buf[len - 1] = '\0';
#else
				buf[len - 2] = '\0';
#endif
			}
#ifdef WIN32
			// 忽略omit_list中存在的文件
			vector<string>::iterator it = find(omit_list.begin(), omit_list.end(), buf);
			if (it == omit_list.end())
			{
				file_list.push_back(buf);
			}
#else
			file_list.push_back(buf);
#endif
		}
#endif
		while (fscanf(fp,"%s", buf) != EOF)
		{
#ifdef WIN32
			// 忽略omit_list中存在的文件
			vector<string>::iterator it = find(omit_list.begin(), omit_list.end(), buf);
			if (it == omit_list.end())
			{
				file_list.push_back(buf);
			}
#else
			file_list.push_back(buf);
#endif
		}
		fclose(fp);//yfxie20190506
		return 0;
	}

	int is_dir(const char *path)
	{
		struct stat buf;
		int ret = stat(path, &buf);
		if (ret != 0)
		{
			fprintf(fp_log, "ERROR: Failed to check file state. %s\n", path);
			return -1;
		}
		else
		{
			ret = (buf.st_mode & S_IFDIR);
		}
		if (ret > 0)
		{
			ret = 1;
		}
		return ret;
	}

	int is_file(const char *path)
	{
		struct stat buf;
		int ret = stat(path, &buf);
		if (ret != 0)
		{
			fprintf(fp_log, "ERROR: Failed to check file state. %s\n", path);
			return -1;
		}
		else
		{
			ret = (buf.st_mode & S_IFREG);
		}
		if (ret > 0)
		{
			ret = 1;
		}
		return ret;
	}


	int read_wav_data (string file, char * wav_data, unsigned int &data_size)
	{
		FILE *fp = NULL;
		if (!(fp = fopen(file.c_str(), "rb")))
		{
			return -1;
		}

		fseek(fp, 0L, SEEK_END);
		data_size = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		if (data_size <= 0)
		{
			fclose(fp);
			return -1;
		}
		else if (data_size > MAX_WAV_SIZE)
		{
			data_size = MAX_WAV_SIZE;
		}

		fread(wav_data, 1, data_size, fp);
		fclose(fp);
		return 0;
	}

	//int vspp_audio_write(VAD_INST inst, string wav_path, char *wav_buf)
	//{
	//	unsigned int wav_size = 0;
	//	VADAudioStatus inst_status = (VADAudioStatus)VAD_AUDIO_BEGIN;
	//	//wstring audio_type = char2wchar(_audio_type.c_str());
	//	VADWaveType sdec_type = (VADWaveType)_audio_type;
	//
	//	vector<string> file_list;
	//	int num_wav_files = 0;
	//	int ret = 0;
	//	char *buffer_tmp = wav_buf;

	//	if (is_dir(wav_path.c_str()) == 1)
	//	{			// 注册语音文件夹
	//		ret = get_filtered_file_list(wav_path.c_str(), NULL, file_list);
	//	}
	//	else if (is_file(wav_path.c_str()) == 1)
	//	{	// 测试语音文件
	//		file_list.push_back(wav_path.c_str());
	//	}
	//	else
	//	{
	//		return -1;
	//	}

	//	// 写入语音文件
	//	printf("文件个数为：%d\n",file_list.size());
	//	for (unsigned int i = 0; i < file_list.size(); i++)
	//	{
	//		ret = read_wav_data(file_list[i], wav_buf, wav_size);
	//		if (ret == 0)
	//		{
	//			ret = VADAudioWrite(inst, sdec_type, buffer_tmp, wav_size, &inst_status);
	//			if (ret != VAD_SUCCESS)
	//			{
	//				if (num_wav_files == 0)
	//				{
	//					inst_status = (VADAudioStatus)VAD_AUDIO_BEGIN;
	//				}
	//			}
	//			else
	//			{
	//				num_wav_files++;
	//				if (inst_status == (VADAudioStatus)VAD_AUDIO_FULL) break;
	//				inst_status = (VADAudioStatus)VAD_AUDIO_CONTINUE;
	//			}
	// 		}
	//	}

	//	if (num_wav_files != 0)
	//	{
	//		inst_status = (VADAudioStatus)VAD_AUDIO_END;
	//		ret = VADAudioWrite(inst, sdec_type, wav_buf, 0, &inst_status);
	//	}
	//	else
	//	{
	//		ret = -1;		// 没有写入过语音
	//	}
	//	return ret;
	//}
#if 0
	int parse_vspp_xml(const char *vspp_xml, int *num_spk)
	{
		TiXmlDocument doc;
		TiXmlElement *pElem;
		TiXmlHandle hRoot(0);
		int ret = -1;

		doc.Parse(vspp_xml);
		if (doc.Error()) {
			printf("ERROR in %s: %s\n", doc.Value(), doc.ErrorDesc());
			return -1;
		}

		TiXmlHandle hDoc(&doc);
		pElem = hDoc.FirstChildElement().Element();

		hRoot = TiXmlHandle(pElem);
		pElem = hRoot.FirstChild("return").Element();
		if (pElem) {
			const char *pText = pElem->GetText();
			ret = atoi (pText);
		}

		pElem = hRoot.FirstChild("speaker_num").Element();
		if (pElem) {
			*num_spk = atoi(pElem->GetText());
		}
		return ret;
	}
#endif
	int find_VID_result(string user_id, vector<VIDResultAll> VID_result)
	{
		int ret = -1;
		for (unsigned int i = 0; i < VID_result.size(); i++) {
			if (VID_result[i].user_id == user_id) {
				ret = i;
				break;
			}
		}
		return ret;
	}
#if 0
	int parse_identify_xml(const wchar_t *VID_xml, int indx, vector<VIDResultAll> &VID_result)
	{
		unsigned int user_cnt = 0;
		TiXmlDocument xml_doc;
		std::string xml = wchar2char(VID_xml);
		xml.append("\n");

		if( xml_doc.Parse(xml.c_str(), 0, TIXML_DEFAULT_ENCODING) == NULL )
		{
			printf("ERROR: Parse identify xml failed, error = %s\n", xml_doc.ErrorDesc());
			return -1;
		}

		TiXmlElement* root = xml_doc.RootElement(); //获取根节点,即<task_result>
		unsigned int result_count_ = 0;
		//循环获取该根节点下面的节点,参数2代表child为真,即有值时才会运行,否则退出
		for(TiXmlNode* task_result=root->FirstChild();task_result;task_result=task_result->NextSibling())
		{
			// 解析task_id
			if((task_result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(task_result->Value(),"task_id")))
			{
				const TiXmlElement *sub_element = (const TiXmlElement*)task_result;
				if( strcmp(sub_element->GetText(), "IdentifyUsers") )
				{
					return atoi(sub_element->GetText());
				}
			}

			// 解析return
			if((task_result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(task_result->Value(),"return")))
			{
				const TiXmlElement *sub_element = (const TiXmlElement*)task_result;
				if( strcmp(sub_element->GetText(), "0") )
				{
					return -1;
				}
			}

			// 解析result_count
			if((task_result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(task_result->Value(),"result_count")))
			{
				const TiXmlElement *sub_element = (const TiXmlElement*)task_result;
				result_count_ = atoi(sub_element->GetText());
			}

			// 解析result
			if((task_result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(task_result->Value(),"result")))
			{
				VIDResultAll tmp_result;
				VIDResultAll *presult = NULL;
				int existed_indx = -1;
				for(TiXmlNode* result =task_result->FirstChild();result;result=result->NextSibling())
				{
					if((result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(result->Value(),"user_id")))
					{
						const TiXmlElement *sub_element = (const TiXmlElement*)result;
						string user_id = sub_element->GetText();
						existed_indx = find_VID_result(user_id, VID_result);
						if (existed_indx < 0) {
							presult = &tmp_result;
							presult->user_id = user_id;
						} else {
							presult = &VID_result[existed_indx];
						}
					}

					if((result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(result->Value(),"raw_score")))
					{
						const TiXmlElement *sub_element = (const TiXmlElement*)result;
						presult->ec_score[indx] = atof(sub_element->GetText());
					}

					if((result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(result->Value(),"normalized_score")))
					{
						const TiXmlElement *sub_element = (const TiXmlElement*)result;
						presult->normalized_score[indx] = atof(sub_element->GetText() );
					}

					if((result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(result->Value(),"confidence")))
					{
						const TiXmlElement *sub_element = (const TiXmlElement*)result;
						presult->confidence[indx] = atof(sub_element->GetText());
					}

					if((result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(result->Value(),"decision")))
					{
						const TiXmlElement *sub_element = (const TiXmlElement*)result;
						presult->decision[indx] = atoi(sub_element->GetText());
					}

					if((result->Type() == TiXmlNode::TINYXML_ELEMENT) && (!strcmp(result->Value(),"error")))
					{
						const TiXmlElement *sub_element = (const TiXmlElement*)result;
						presult->error[indx] = atoi(sub_element->GetText());
					}
				}
				if (existed_indx < 0) {
					VID_result.push_back(tmp_result);
				}
			}
		}
		return 0;
	}

	int parse_enroll_xml(const wchar_t *enroll_xml)
	{
		TiXmlDocument doc;
		TiXmlElement *pElem;
		TiXmlHandle hRoot(0);
		string xml = wchar2char(enroll_xml);
		int ret = 0;

		xml.append("\n");
		if( doc.Parse(xml.c_str(), 0, TIXML_DEFAULT_ENCODING) == NULL ) {
			printf("ERROR: Parse enroll xml failed, error = %s\n", doc.ErrorDesc());
			return -1;
		}

		TiXmlHandle hDoc(&doc);
		pElem = hDoc.FirstChildElement().Element();
		if (!pElem) {
			return -1;
		}

		hRoot = TiXmlHandle(pElem);
		pElem = hRoot.FirstChild("return").Element();
		if (pElem != NULL) {
			const char *pText = pElem->GetText();
			ret = atoi (pText);
		} else {
			return -1;
		}

		return ret;
	}
#endif

	/*将注册的模型写入文件*/
	int write_model_to_file(const char* file_path,void* model,int prmi_model_size,int extern_model_size,bool is_append, string filename)
	{
		fstream  target_file(file_path,ios::app);
		// target_file<<"<FilePath>  "<<file_path<<"</FilePath>";
		target_file<<filename<<"\n";
		float* fin_model=(float*)model;
		//target_file<<"<model>\n";
		int nLen = 300;
		//int nLen = prmi_model_size/sizeof(float);
		for( int i = 0; i < 200; i++){
			target_file<< fin_model[i] <<"\n";
		}
		//target_file<<"</model>";
		target_file.close();
		return 0;
	}

	float **CreatMatFlt1(int nRow, int nCol)
	{
		float **ppMat = NULL;
		int i = 0;

		ppMat = (float* *)calloc(nRow * sizeof(float*) + nRow * nCol * sizeof(float),1);
		float* p = (float*)(ppMat + nRow);
		for (i = 0; i < nRow; i++)
		{
			ppMat[i] = (float*)(p + i * nCol);
		}
		return ppMat;
	}

	int get_compare_mode_length(const char* file_path, unsigned int * models_len)
	{
		int i = 0;
		int j = 0;
		int len = 200; // 经过lda、wccn处理后变为1*200
		float fTmp = 0.0; // 用于存放读取的值
		char s_line[2048];
		FILE *fp = NULL;
		fp = fopen(file_path, "rb");

		if (NULL == fp)
		{
			printf("Open user model file failed: %s\n", file_path);
			return -1;
		}

		while (fgets(s_line, 2048, fp) != NULL)
		{
			string s=s_line;

			if(s.find("model",0)!=-1){
				//printf("file name %s \n",s_line);
				j++;
				continue;
			}

		}

		*models_len =j;

		fclose(fp);

		return 0;
	}

	/*装载对比模型库文件*/
	int load_compare_mode_file(const char* file_path,float** user_models, unsigned int * models_len,char ***sz_model_id)
	{
		int i = 0;
		int j = 0;
		int len = 200; // 经过lda、wccn处理后变为1*200
		float fTmp = 0.0; // 用于存放读取的值
		char s_line[128];
		FILE *fp = NULL;
		fp = fopen(file_path, "rb");

		if (NULL == fp)
		{
			printf("Open file failed!\n");
			return -1;
		}

		while (fgets(s_line, 128, fp) != NULL)
		{
			FilteString(s_line);
			string s = s_line;

			if(s.find("model",0)!=-1){
				//printf("file name %s \n",s_line);
				int p = s.find_first_of("m", 0);
				s.replace(p, strlen("model_"), 0, 'm');
				strcpy(sz_model_id[0][j],s.c_str());
				j++;
				i = 0;
				continue;
			}

			if(j>0)
			{
				user_models[j-1][i]= atof(s_line);
				i++;
			}
		}

		*models_len =j;

		fclose(fp);

		return 0;
	}

	float tv_norm(float x)
	{
		return (x - _tv_norm_mean) / _tv_norm_var;
	}

	int write_identify_result(VSPPWavInfo wav_info, vector<VIDResultAll> identify_result)
	{
		// TV规整
		if (_tv_norm_type == 1 && fp_tv_rslt) {
			for (unsigned int i = 0; i < identify_result.size(); i++) {
				if (identify_result[i].normalized_score[0] > MIN_SCORE + 0.01) {
					identify_result[i].normalized_score[0] = tv_norm(identify_result[i].normalized_score[0]);
				}
				if (identify_result[i].normalized_score[1] > MIN_SCORE + 0.01) {
					identify_result[i].normalized_score[1] = tv_norm(identify_result[i].normalized_score[1]);
				}
			}
		}

		if (fp_ec_rslt)
		{
			for (unsigned int i = 0; i < identify_result.size(); i++)
			{
				float max_score = max (identify_result[i].ec_score[0], identify_result[i].ec_score[1]);
				fprintf(fp_ec_rslt, "%s ; %s ; %f ; %f ; %f ; %f ; %f ; %d\n",
					wav_info.wav_name.c_str(),
					identify_result[i].user_id.c_str(),
					max_score,
					identify_result[i].ec_score[0],
					identify_result[i].ec_score[1],
					wav_info.len[0],
					wav_info.len[1],
					wav_info.gender);
			}
			fflush(fp_ec_rslt);
		}

		if (fp_tv_rslt)
		{
			for (unsigned int i = 0; i < identify_result.size(); i++)
			{
				float max_score = max (identify_result[i].tv_score[0], identify_result[i].tv_score[1]);
				max_score = max (max_score, identify_result[i].tv_score[2]);
				fprintf(fp_tv_rslt, "%s ; %s ; %f ; %f ; %f ; %f ; %f ; %f ; %f ; %d\n",
					wav_info.wav_name.c_str(),
					identify_result[i].user_id.c_str(),
					max_score,
					identify_result[i].tv_score[0],
					identify_result[i].tv_score[1],
					identify_result[i].tv_score[2],
					wav_info.len[0],
					wav_info.len[1],
					wav_info.len[2],
					wav_info.gender);
			}
			fflush(fp_tv_rslt);
		}

		if (fp_fused_rslt) {
			for (unsigned int i = 0; i < identify_result.size(); i++) {
				float score_0 = ALPHA * identify_result[i].ec_score[0] + (1 - ALPHA) * identify_result[i].normalized_score[0];
				float score_1 = ALPHA * identify_result[i].ec_score[1] + (1 - ALPHA) * identify_result[i].normalized_score[1];
				float max_score = max (score_0, score_1);

				fprintf(fp_fused_rslt, "%s ; %s ; %f ; %f ; %f ; %f ; %f ; %d\n",
					wav_info.wav_name.c_str(),
					identify_result[i].user_id.c_str(),
					max_score,
					score_0,
					score_1,
					wav_info.len[0],
					wav_info.len[1],
					wav_info.gender);
			}
			fflush(fp_fused_rslt);
		}

		return 0;
	}

	/*int write_htk_fea_file(string file_path, char *fea_data, unsigned int fea_len)
	{
		FILE *fp = NULL;
		int ftr_header_size = 0;
		unsigned fea_float_size = (fea_len - ftr_header_size) / 4;

		fp = fopen(file_path.c_str(), "wb+");

	    if(NULL ==fp )
		{
			fprintf(fp_log, "ERROR: Create file failed: %s!\n", file_path);
			return -1;
		}

		int step_size = 10;
		int delta = 3;
		int num_cepstrum = 13;
		int num_frames = fea_float_size / (delta * num_cepstrum);

		write_htk_header(num_frames, step_size, num_cepstrum * delta, delta, fp);
		float * tmp_fea_data = (float *)malloc(fea_float_size * sizeof(float));
		if (tmp_fea_data == NULL) {
			fprintf(fp_log, "ERROR: Memory allocate failed [write_fea_file].\n");
			fclose(fp);
			return -1;
		}

		memcpy(tmp_fea_data, fea_data + ftr_header_size, fea_float_size * sizeof(float));
		swap_float_ptr((void *)tmp_fea_data, fea_float_size);
		fwrite(tmp_fea_data, sizeof (float), fea_float_size, fp);

		free(tmp_fea_data);
		tmp_fea_data = NULL;

		fclose(fp);

		return 0;
	}*/


    int write_bin_file( char *fea_data, unsigned int fea_len ,std::string file_path )
    {
        FILE *fp = NULL;

        if (!(fp = fopen(file_path.c_str(), "wb")))
        {
            printf("Can not create fea file %s.\n", file_path.c_str());
            return -1;
        }

        fwrite(fea_data, sizeof (char), fea_len, fp);

        fclose(fp);
        return 0;

    }

/*
写htk格式文件
参数1：fea_data 特征地址
参数2: fea_len ，特征长度，以byte为单位
参数3: file_path 保存文件地址
参数4：dim_num   特征维度
参数5：is_swap  是否做大小端交换，htk格式 1表示做交换，0表示不交换

*/
	/*
	int write_htk_fea_file( char *fea_data, unsigned int fea_len ,std::string file_path ,int dim_num ,int is_swap)
	{
		FILE *fp = NULL;

		unsigned int fea_float_size =  fea_len / 4;

		if (!(fp = fopen(file_path.c_str(), "wb")))
		{
			printf("Can not create fea file %s.\n", file_path.c_str());
			return -1;
		}

		int step_size = 10;

		int num_frames = fea_float_size / dim_num;

		write_htk_header(num_frames, step_size, dim_num ,is_swap , fp);

		if(1 == is_swap )
		{
			swap_float_ptr((void *)fea_data, fea_float_size);
		}

		fwrite(fea_data, sizeof (float), fea_float_size, fp);

		fclose(fp);
		return 0;
	}
	*/
	int write_htk_fea_file( char *fea_data, std::string file_path, int is_swap)
	{
		//先解析VSPP特征头yfxie2019-02-20
		IflyFeaHeaderInfo *vspp_head;
		vspp_head = (IflyFeaHeaderInfo*)fea_data;
		//memcpy(&vspp_head, fea_data, sizeof vspp_head);
		char fixed_fea = vspp_head->calibInfo;
		unsigned int fea_length = vspp_head->featBytes;
		unsigned int fea_frame = vspp_head->frameNum;
		char dim_real = vspp_head->dimReal;
		char dim_sse = vspp_head->dimAligned;

		//特征定浮点判断yfxie2019-02-20
		if ( fixed_fea == 0 )//浮点
		{
			FILE *fp = NULL;

			unsigned int fea_float_size =  fea_length / 4;

			if (!(fp = fopen(file_path.c_str(), "wb")))
			{
				printf("Can not create fea file %s.\n", file_path.c_str());
				return -1;
			}

			int step_size = 10;
			int num_frames = fea_frame;
			int dim_num = dim_real;

			write_htk_header(num_frames, step_size, dim_num, is_swap, fp);

			if(1 == is_swap )
			{
				swap_float_ptr(fea_data + sizeof(IflyFeaHeaderInfo), fea_float_size);
			}

			fwrite(fea_data + sizeof(IflyFeaHeaderInfo), sizeof(float), fea_float_size, fp);

			fclose(fp);
		}
		else//定点特征
		{
			FILE *fp = NULL;

			if (!(fp = fopen(file_path.c_str(), "wb")))
			{
				printf("Can not create fea file %s.\n", file_path.c_str());
				return -1;
			}

			int step_size = 10;
			int num_frames = fea_frame;
			int dim_num = dim_sse;
			int dim_float = dim_real;

			write_htk_header(num_frames, step_size, dim_float, is_swap, fp);

			float *float_fea_data = new float[dim_float * num_frames];
			float *tmp_float_data = float_fea_data;
			float temp;
			short *short_fea = (short*)fea_data+ sizeof(IflyFeaHeaderInfo)/2;

			for(int i=0; i<num_frames; i++)
			{
				for(int j = 0; j< dim_float; j++)
				{
					temp = short_fea[j];
					tmp_float_data[j] = (float)temp / 4096.0;
				}
				short_fea += dim_num;
				tmp_float_data += dim_float;
			}

			if(1 == is_swap )
			{
				swap_float_ptr(float_fea_data, dim_float * num_frames);
			}

			fwrite(float_fea_data, sizeof(float), dim_float * num_frames, fp);

			if(float_fea_data!=NULL)
				delete []float_fea_data;

			fclose(fp);

		}
		return 0;
	}

	int write_htk_13plp_fea_file(string file_path, char *fea_data, unsigned int fea_len)
	{
		FILE *fp = NULL;
		int ftr_header_size = 0;
		unsigned fea_float_size = (fea_len - ftr_header_size) / 4;

		if (!(fp = fopen(file_path.c_str(), "wb"))) {
			fprintf(fp_log, "ERROR: Create file failed: %s!\n", file_path.c_str());
			return -1;
		}

		int step_size = 10;
		int delta = 1;
		int num_cepstrum = 13;
		int num_frames = fea_float_size / (delta * num_cepstrum);

		write_htk_header(num_frames, step_size, num_cepstrum * delta, 3, fp);
		float * tmp_fea_data = (float *)malloc(fea_float_size * sizeof(float));
		if (tmp_fea_data == NULL) {
			fprintf(fp_log, "ERROR: Memory allocate failed [write_fea_file].\n");
			fclose(fp);
			return -1;
		}

		memcpy(tmp_fea_data, fea_data + ftr_header_size, fea_float_size * sizeof(float));
		swap_float_ptr((void *)tmp_fea_data, fea_float_size);
		fwrite(tmp_fea_data, sizeof (float), fea_float_size, fp);

		free(tmp_fea_data);
		tmp_fea_data = NULL;

		fclose(fp);
		return 	0;
	}

	//特征保存为二进制文件yfxie2018-11-20
	//save_binary_feature()参数说明
	//MsppHeadInfo *mspp_head_info 一级特征头结构体
	//int is_fix_feat 是否是浮点特征，0：浮点特征  1:定点特征 默认为0
	//std::string filename 保存文件名称
	int save_binary_feature(MsppHeadInfo *mspp_head_info, int is_fix_feature, std::string filename)
	{
		//for yfzhang8 11-20
		//string txt_file_name = string(save_path) + string("/") + string(suffix) + file_main_name +  string(".txt");
		FILE* fFile = fopen(filename.c_str(), "wb");
		if (fFile == NULL)
		{
			return -1;
		}

		int writelen = fwrite(&mspp_head_info->engine_version, 16, 1, fFile);
		writelen = fwrite(&is_fix_feature, sizeof(int), 1, fFile);
		writelen = fwrite(&mspp_head_info->feature_num, sizeof(int), 1, fFile);
		for(int i=0;i<mspp_head_info->feature_num;i++)
		{
			writelen = fwrite(&mspp_head_info->fea_head[i].fea_type,sizeof(int), 1, fFile);
			writelen = fwrite(&mspp_head_info->fea_head[i].feature_size,sizeof(int), 1, fFile);
			writelen = fwrite((char *)mspp_head_info->fea_head[i].feature_address, sizeof(char), mspp_head_info->fea_head[i].feature_size, fFile);
		}
		fclose(fFile);
		return 0;//yfxie20190506
	}

	// HTK格式操作
	short swap_16(short a)
	{
		short s;

		s=((a&0xff)<<8)|((a&0xff00)>>8);

		return s;

	}

	int swap_32(int a)
	{
		int s;

		s=((a&0xff)<<24)|((a&0xff00)<<8)|((a&0xff0000)>>8)|((a&0xff000000)>>24);

		return s;

	}

	void swap_float_ptr(void*mfcc_ptr,int size)
	{
		char *ptr;
		char temp;
		int i;

		ptr=(char*)mfcc_ptr;

		for(i=0;i<size;i++)
		{
			temp=ptr[0];
			ptr[0]=ptr[3];
			ptr[3]=temp;
			temp=ptr[1];
			ptr[1]=ptr[2];
			ptr[2]=temp;
			ptr+=4;
		}
		return;
	}
	void write_htk_header(int vad_frames,int step_size,short dim,int is_swap,FILE *f_mfcc)
	{
		int s_vad_frame;
		int step_in_ns;
		short frame_size_in_byte;
		short mfcc_fomat;

		s_vad_frame = vad_frames ;
		step_in_ns = step_size*10000;
		frame_size_in_byte = dim*4;
		mfcc_fomat = 0x09;

		if(1 == is_swap) //做交换
		{
			s_vad_frame=swap_32(vad_frames);
			step_in_ns=swap_32(step_size*10000);//change 10 ms to 100 ns in unit
			frame_size_in_byte=swap_16(dim*4);//float occupy four bytes
			mfcc_fomat=swap_16(0x09); //USER
		}

		fwrite(&s_vad_frame,sizeof(int),1,f_mfcc);
		fwrite(&step_in_ns,sizeof(int),1,f_mfcc);
		fwrite(&frame_size_in_byte,sizeof(short),1,f_mfcc);
		fwrite(&mfcc_fomat,sizeof(short),1,f_mfcc);

		return;
	}



	//read htk header and skip 12 bytes
	int read_htk_header(FILE *f_mfcc)
	{
		int s_vad_frame;
		int step_in_ns;
		short frame_size_in_byte;
		short mfcc_fomat;

		fread(&s_vad_frame,sizeof(int),1,f_mfcc);
		s_vad_frame=swap_32(s_vad_frame);

		fread(&step_in_ns,sizeof(int),1,f_mfcc);

		//	frame_size_in_byte=swap_16(j*4);//float occupy four bytes
		fread(&frame_size_in_byte,sizeof(short),1,f_mfcc);

		/*	if(delta==2)
		mfcc_fomat=swap_16(0x2906);
		else
		mfcc_fomat=swap_16(0x2b06);*/

		//	mfcc_fomat=swap_16(0x2b06);
		fread(&mfcc_fomat,sizeof(short),1,f_mfcc);

		return s_vad_frame;
	}

	// 字符集转换
	wstring char2wchar(const char * str, size_t * cvt_chrs = 0)
	{
		wchar_t dest_buf[20 * 1024];
		wchar_t * dest_ptr = dest_buf;

		size_t count = strlen(str) + 1;
		if ( count >= sizeof dest_buf / sizeof dest_buf[0] )
			dest_ptr = new wchar_t [count];
		dest_ptr[0] = 0;

#if _MSC_VER >= 1400
		size_t siw = std::max<>(sizeof dest_buf / sizeof dest_buf[0], count);
		mbstowcs_s(cvt_chrs, dest_ptr, siw, str, count);
#else	// not vc 8
		size_t cvts = mbstowcs(dest_ptr, str, count);
		if ( cvt_chrs )
			*cvt_chrs = cvts;
#endif	// not vc 8

		if ( dest_ptr == dest_buf )
			return dest_ptr;

		std::wstring out_str = dest_ptr;
		delete [] dest_ptr;
		return out_str;
	}

	string wchar2char(const wchar_t *str, size_t * cvt_chrs = 0)
	{
		size_t rslt_size = wcslen(str) * 2 + 1;
		char *buf = NULL;

		if (!(buf = (char *)malloc(rslt_size)))
		{
			printf("ERROR: Memory allocate failed [parse_return_rslt].\n");
			exit (-1);
		}

		wcstombs(buf, str, rslt_size);

		string out_str = buf;

		free(buf);
		buf = NULL;

		return out_str;
	}
	string get_base_name(const string spk_dir)
	{
		string speaker_id;
		size_t pos = 0;
		pos = spk_dir.find_last_of("/\\");
		if (pos != spk_dir.npos)
		{
			speaker_id = spk_dir.substr(pos + 1);
		}
		else
		{
			speaker_id = spk_dir;
		}
		return speaker_id;
	}

	string get_post_name(const string path, const string key_str)
	{
		string tmp = path;
		size_t pos = 0;
		pos = path.find(key_str);
		if (pos != string::npos) {
			pos += key_str.length();
			tmp = path.substr(pos + 1);
		} else {
			pos = path.find_last_of("/\\");
			if (pos != string::npos) {
				tmp = path.substr(pos + 1);
			} else {
				tmp = path;
			}
		}
		while (true) {
			pos = tmp.find_last_of("/\\");
			if (pos != string::npos) {
				tmp.replace(pos, 1, "%%");
			} else {
				break;
			}
		}
		return tmp;
	}

    string get_file_main_name(char *fileFullName)
    {
        string temp(fileFullName);
#ifdef WIN32
        int npos= temp.rfind("\\");
#else
		int npos= temp.rfind("/");
#endif
        if(npos == string::npos)
        {
            npos=0;
        }
        int dotposition = temp.rfind(".");
        if(dotposition == string::npos)
        {
            dotposition = temp.size() - 1;
        }
        string mainFileName(temp, npos+1, dotposition-npos-1);
        return mainFileName;
    }

	string get_cur_time()
	{
		string str_time = "";
		time_t cur_time;
		struct tm *loc_time = NULL;

		if (time(&cur_time) != -1) {
			str_time = ctime(&cur_time);
		}

		return str_time;
	}

	public:
		string sbinpath;

private:
	string _enroll_path;
	string _identify_path;
	string _rslt_path;
	string _user_model;
	string _processed_list_path;
	int    _loop_count;
	int    _loop_num_count;
	int    _num_count;
	string _conf_dir;//yfxie

	int    _audio_type;

	int _fea_write_mode;
	int _xml_write;//是否将xml结果写入到xml文件
	int _wav_write;//是否输出处理后音频
	int _process_mode;
	int _num_threads;
	int _out_score_type;

	int _out_data_write;//是否保存四套特征
	int _out_binary_feature;//是否保存二进制特征


	int _tv_norm_type;
	float _tv_norm_mean;
	float _tv_norm_var;

	unsigned int _elem_cnt;

	vector<string> _enroll_dir_list;

	vector<string> _identify_file_list;
	//string identify_file_list[1000];

#ifdef WIN32
	HANDLE _handle_mutex_write;
	HANDLE _handle_mutx_read;
#else
	pthread_mutex_t  _handle_mutex_write;
	pthread_mutex_t _handle_mutx_read;
#endif
	FILE *fp_ec_rslt;
	FILE *fp_tv_rslt;
	FILE *fp_fused_rslt;
	FILE *fp_enroll_rslt;
	FILE *fp_log;
	FILE *fp_error;
	FILE *fp_processed;
	FILE *fp_fea_info;

};

#endif

#include "myTkt.h"

#define TEST_CLOCK

#ifdef TEST_CLOCK

double clock_it(void)
{
#ifdef _WIN32
  clock_t start;
  double  duration;

  start = clock();
  duration = (double)(start) / CLOCKS_PER_SEC;
  return duration;
#else
  double duration = 0.0;
  struct timeval start;

  gettimeofday(&start, NULL);
  duration = (double)(start.tv_sec + start.tv_usec/1000000.0);
  return duration;
#endif // _WIN32
}
#endif

int main(int argc, char **argv)
{
	double start_time = clock_it();

	setlocale(LC_ALL, "Chinese-simplified");

	if (argc != 6)	
	{
		printf("%s configure\n",argv[0]);
		return -1;
	}

	int ret = 0;
	myTkt obj;
	obj.sbinpath = argv[0];

	//ret = obj.parse_cfg(argv[1]);
	ret = obj.parse_cfg(argv[1],argv[2],argv[3],argv[4],argv[5]);
	if (ret != 0) 
	{
		printf("Parse configure file failed. %s\n", argv[1]);
		return -1;
	}
	ret = obj.init();
	if ( ret != 0 ) 
	{
		printf("myTkt init failed.\n");
		return -1;
	}

	// 根据用户选择决定是否进行操作
	if ((ret = obj.print_info()) == 0) 
	{		
		obj.start();
	}
	
	obj.uninit();
   printf("endendend!!!\n");

   double end_time = clock_it();
   cout<<"TotalTime:"<<(double)(end_time - start_time)<<"s"<<endl;
	return 0;

}

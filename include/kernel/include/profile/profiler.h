#ifndef __PROFILER_HH__
#define __PROFILER_HH__

#include <iostream>
#ifndef WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif


///Timer for count performance
	class Timer
    {
	public:
		Timer(const char* func_name) 
		{ 
			Reset();
			func_name_ = func_name;
		}
		double Elapsed()
		{
#ifdef WIN32
			LARGE_INTEGER time_end;
			LARGE_INTEGER freq;
			QueryPerformanceCounter(&time_end);
			if (QueryPerformanceFrequency(&freq) == 0) return 0.0;  // Hardware does not support this.
			double ret = (double)time_end.QuadPart - (double)time_start_.QuadPart) /
				((double)freq.QuadPart;
			ret *= 1000;
#else
            struct timeval time_end;
            gettimeofday(&time_end, &time_zone_);
            double t1, t2;
            t1 = (double)time_start_.tv_sec +
                (double)time_start_.tv_usec / (1000 * 1000);
            t2 = (double)time_end.tv_sec + (double)time_end.tv_usec / (1000 * 1000);
            double ret = (t2 - t1) * 1000;
#endif
            return ret;
		}
		~Timer()
        {
#ifdef WIN32
			LARGE_INTEGER time_end;
			LARGE_INTEGER freq;
			QueryPerformanceCounter(&time_end);
			if (QueryPerformanceFrequency(&freq) == 0) return 0.0;  // Hardware does not support this.
			double ret = (double)time_end.QuadPart - (double)time_start_.QuadPart) /
				((double)freq.QuadPart;
					ret *= 1000;
#else
            struct timeval time_end;
            gettimeofday(&time_end, &time_zone_);
            double t1, t2;
            t1 = (double)time_start_.tv_sec +
                (double)time_start_.tv_usec / (1000 * 1000);
            t2 = (double)time_end.tv_sec + (double)time_end.tv_usec / (1000 * 1000);
            double ret = (t2 - t1) * 1000;
#endif
            std::cout << func_name_ << " cost " << ret << " ms" << std::endl;
            // return ret;
		}
	private:
		void Reset()
        {
#ifdef WIN32
			QueryPerformanceCounter(&time_start_);
#else
            gettimeofday(&this->time_start_, &time_zone_);
#endif
		}
		const char* func_name_;
#ifdef WIN32
		LARGE_INTEGER time_start_;
#else
        struct timeval time_start_;
        struct timezone time_zone_;
#endif
	};

	#define PROFILER() Timer timer(__FUNCTION__)

#endif

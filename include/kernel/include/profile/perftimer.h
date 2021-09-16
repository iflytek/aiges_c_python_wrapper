#ifndef _PERF_TIMER_H_
#define _PERF_TIMER_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <stdint.h>

class PerfTimer
{
public:
	PerfTimer()
	{
#ifdef _WIN32
		QueryPerformanceFrequency((LARGE_INTEGER *)&m_freq);
#endif
		reset();
	}
	~PerfTimer() {};

	/// <summary>
	/// resets the timer to ins initial state
	/// </summary>
	void reset()
	{
#ifdef _WIN32
		m_start_time = 0;
		m_stop_time = 0;
#endif
	}

	/// <summary>
	/// sets the start time of the timer
	/// </summary>
	int64_t start()
	{
#ifdef _WIN32
		QueryPerformanceCounter((LARGE_INTEGER *)&m_start_time);
        return (int64_t)(((double)( m_start_time) / (double)m_freq) * 1000 * 1000);
#else
		gettimeofday(&m_start_time, 0);
        return (m_start_time.tv_sec) * 1000000 + m_start_time.tv_usec;
#endif
	}

	/// <summary>
	/// Sets the current end time for the timer.  Repeated calls will reset the end time
	/// of the timer.
	/// </summary>
	int64_t stop()
	{
#ifdef _WIN32
		QueryPerformanceCounter((LARGE_INTEGER *)&m_stop_time);
        return (int64_t)(((double)(m_stop_time) / (double)m_freq) * 1000 * 1000);
#else
		gettimeofday(&m_stop_time, 0);
        return (m_stop_time.tv_sec) * 1000000 + m_stop_time.tv_usec;
#endif
	}

	// Returns the duration of the timer (in seconds)

	const double duration()const
	{
#ifdef _WIN32
		return (double)(m_stop_time - m_start_time) / (double)m_freq;
#else
		return (double)m_stop_time.tv_sec - (double)m_start_time.tv_sec +
			(((double)m_stop_time.tv_usec - (double)m_start_time.tv_usec) / 1000000.0);
#endif
	}

	const double duration_milli_seconds()const
	{
#ifdef _WIN32
		return ((double)(m_stop_time - m_start_time) / (double)m_freq) * 1000;
#else
		return ((double)m_stop_time.tv_sec - (double)m_start_time.tv_sec) * 1000.0 +
			(((double)m_stop_time.tv_usec - (double)m_start_time.tv_usec) / 1000.0);
#endif
	}

	const double duration_micro_seconds()const
	{
#ifdef _WIN32
		return ((double)(m_stop_time - m_start_time) / (double)m_freq) * 1000 * 1000;
#else
		return ((double)m_stop_time.tv_sec - (double)m_start_time.tv_sec) * 1000000.0 +
			((double)m_stop_time.tv_usec - (double)m_start_time.tv_usec);
#endif
	}

private:
#ifdef _WIN32
	__int64 m_start_time;
	__int64 m_stop_time;
	__int64 m_freq;
#else
	struct timeval m_start_time;
	struct timeval m_stop_time;
#endif
};

#endif
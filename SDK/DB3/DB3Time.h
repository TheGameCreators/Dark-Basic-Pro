#pragma once

#include "DB3.h"

#include <time.h>
#ifndef _WIN32
# include <sched.h>
#endif

#include <string>

DB_ENTER_NS()


inline u32 Millisecs() {
#if _WIN32
	return GetTickCount();
#else
	return clock();
#endif
}
inline void ReadTimer(u32 &seconds, u32 &nanoseconds) {
#if _WIN32
	u64 f, t;

	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&f));
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&t));

	t = static_cast<u64>(static_cast<double>(t)/(static_cast<double>(f)/1000000000.0));

	seconds     = static_cast<u32>(t/1000000000ULL);
	nanoseconds = static_cast<u32>(t%1000000000ULL);
#else
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	seconds     = static_cast<u32>(ts.tv_sec);
	nanoseconds = static_cast<u32>(ts.tv_nsec);
#endif
}
inline double NanosecondsToSeconds(u32 nanoseconds)
{
	return static_cast<double>(nanoseconds)/1000000000.0;
}
inline double NanosecondsToSeconds(u32 seconds, u32 nanoseconds)
{
	return static_cast<double>(seconds) + NanosecondsToSeconds(nanoseconds);
}
inline double PerfTimer() {
	u32 seconds, nanoseconds;

	ReadTimer(seconds, nanoseconds);

	return static_cast<double>(seconds) + NanosecondsToSeconds(nanoseconds);
}

namespace policy
{
	struct null_proflog
	{
		static inline void report(const char *) {}
		static inline void report(const char *, double) {}
	};
	struct stderr_proflog
	{
		static inline void report(const char *scope)
		{
			fprintf(stderr, "BEGIN PROFILE \"%s\"...\n", scope);
		}
		static inline void report(const char *scope, double time)
		{
			fprintf(stderr, "END PROFILE \"%s\": %.8f\n", scope, time);
		}
	};
	struct msgbox_proflog
	{
		static inline void report(const char *) {}
		static inline void report(const char *scope, double time)
		{
			char buf[512];

			sprintf_s(buf, "%s: %.8f seconds\n", scope, time);
			MessageBoxA(GetActiveWindow(), buf, "Profile Report", MB_ICONINFORMATION|MB_OK);
		}
	};
	static const char *const log_filename = "D:/dark-basic-3/profile.log";
	struct logfile_proflog
	{
		static inline void report(const char *scope)
		{
			FILE *f;

			f = fopen(log_filename, "a+");
			if (!f)
				return;

			fprintf(f, "[%s] entered...\n", scope);
			fclose(f);
		}
		static inline void report(const char *scope, double time)
		{
			FILE *f;

			f = fopen(log_filename, "a+");
			if (!f)
				return;

			if (time >= 60.0)
			{
				u32 m, s;

				s = static_cast<u32>(time);
				m = s/60;
				s = s%60;

				fprintf(f, "[%s] ==%.8f (%uM:%uS)==\n", scope, time, m, s);
			}
			else
				fprintf(f, "[%s] ==%.8f==\n", scope, time);
			fclose(f);
		}
	};
}

template<class _ReportPolicy=policy::logfile_proflog>
class CProfile
{
public:
	inline CProfile(const char *scopeName)
	{
		strcpy_s(m_ScopeName, scopeName);
		_ReportPolicy::report(m_ScopeName);

		ReadTimer(m_Tick[0].seconds, m_Tick[0].nanoseconds);
	}
	inline ~CProfile()
	{
		ReadTimer(m_Tick[1].seconds, m_Tick[1].nanoseconds);

		double diff = NanosecondsToSeconds(m_Tick[1].seconds, m_Tick[1].nanoseconds);
		diff -= NanosecondsToSeconds(m_Tick[0].seconds, m_Tick[0].nanoseconds);

		_ReportPolicy::report(m_ScopeName, diff);
	}

protected:
	char m_ScopeName[256];

	struct STick
	{
		u32 seconds;
		u32 nanoseconds;
	};
	STick m_Tick[2];
};

DB_LEAVE_NS()

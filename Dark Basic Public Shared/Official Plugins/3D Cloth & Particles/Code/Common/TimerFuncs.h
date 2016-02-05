#ifndef TIMERFUNCS_H
#define TIMERFUNCS_H

#include <Mmsystem.h>

class TimerUtil
{
public:
	TimerUtil(bool startNow=false)
	{
		running=false;
		QueryPerformanceFrequency(&freq);
		useLowRes=false;
		if(freq.QuadPart==0)
		{
			//No high res counter available, use low res one instead
			useLowRes=true;
		}
		if(startNow) Start();
	}

	inline void Start()
	{
		running=true;
		if(useLowRes)
		{
			msStart = timeGetTime();
		}
		else
		{
			QueryPerformanceCounter(&start);
		}
	}

	inline void Split()
	{		
		if(useLowRes)
		{
			msSplit = timeGetTime();
		}
		else
		{
			QueryPerformanceCounter(&split);
		}
	}

	inline double StopStart()
	{
		if(useLowRes)
		{
			msFinal = timeGetTime();
			msDiff = msFinal-msStart;
			msStart=msFinal;
			return msDiff*1000;
		}
		else
		{
			QueryPerformanceCounter(&final);
			diff.QuadPart =final.QuadPart-start.QuadPart;
			double result = getTimeFromTicks(diff);
			start.QuadPart=final.QuadPart;
			return result;
		}
	}


	inline void Stop()
	{
		running=false;
		if(useLowRes)
		{
			msFinal = timeGetTime();
		}
		else
		{
			QueryPerformanceCounter(&final);
		}
	}
	
	inline double getSplit()
	{
		if(useLowRes)
		{
			return msSplit*1000;
		}
		else
		{
			diff.QuadPart = split.QuadPart-start.QuadPart;
			return getTimeFromTicks(diff);
		}
	}

	inline double getFinal()
	{
		if(useLowRes)
		{
			return msFinal*1000;
		}
		else
		{
			diff.QuadPart =final.QuadPart-start.QuadPart;
			return getTimeFromTicks(diff);
		}
	}

	bool inline isRunning() {return running;}
private:	
	union
	{
		struct
		{
			LARGE_INTEGER freq;
			LARGE_INTEGER start;
			LARGE_INTEGER split;
			LARGE_INTEGER final;

			LARGE_INTEGER diff;
		};
		struct
		{
			DWORD msStart;
			DWORD msSplit;
			DWORD msFinal;
			
			DWORD msDiff;
		};
	};

	bool running;
	bool useLowRes;

	double getTimeFromTicks(const LARGE_INTEGER &time)
	{
		__int64 rem = time.QuadPart % freq.QuadPart;
		__int64 secs = (time.QuadPart - rem)/freq.QuadPart;
		double fraction = double(rem)/double(freq.QuadPart);	
		return fraction+secs;
	}	
};

#endif
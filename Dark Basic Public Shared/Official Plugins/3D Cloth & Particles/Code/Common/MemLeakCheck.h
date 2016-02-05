#ifndef MEMLEAKCHECK_H
#define MEMLEAKCHECK_H
//The best way to detect for memory leaks

//The static declaration means that the destructor is almost
//guaranteed to be the last thing that gets called, providing you
//make sure that you instantiate it immediately when your program starts


#if _DEBUG

#define INIT_MEMORYLEAK_DETECTION(val) _DetectMemoryLeaks::DetectMemoryLeaks(val)

class _DetectMemoryLeaks
{
public:
	static _DetectMemoryLeaks& DetectMemoryLeaks(int val)
	{
		static _DetectMemoryLeaks inst;
		_crtBreakAlloc=val;
		return inst;
	}

	~_DetectMemoryLeaks()
	{
		_CrtDumpMemoryLeaks();
	}
};

#else

#define INIT_MEMORYLEAK_DETECTION(val)

#endif





#endif
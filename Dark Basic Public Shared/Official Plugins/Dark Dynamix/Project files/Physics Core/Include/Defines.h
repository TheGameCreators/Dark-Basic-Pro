#define RELEASE
#define COMPILE_DYNAMIX_FULL
//#define COMPILE_WITH_TIME_LIMIT

#define COMPILE_FOR_GDK
//#define COMPILE_FOR_IRRLICHT

#ifdef COMPILE_FOR_GDK
  #define DWORDFLOAT float
  #define RETURNCAST return value;
#else
  #define DWORDFLOAT DWORD
  #define RETURNCAST return *(DWORD*)&value;
#endif


#define DYNAMIX_VERSION 111
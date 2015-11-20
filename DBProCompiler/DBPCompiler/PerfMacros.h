#ifndef PERFMACROS_H
#define PERFMACROS_H

//
//	Comment out the lines below to see the differences between using certain
//	performance enhancements or rule out bugs being caused by the performance
//	macros.
//
//	You can define __AARON_DISABLE_ALL_PERFS__ to disable all performance macros
//

//#define __AARON_DISABLE_ALL_PERFS__

#define X__AARON_PERFIMPROVE__		1
#define X__AARON_INSTRPERF__		1
#define X__AARON_LBLTBLPERF__		1
#define X__AARON_STRUCPERF__		1
#define X__AARON_STRPERF__			1
#define X__AARON_VARTABLEPERF__		1

// __AARON_INSTRPERF__
#define __AARON_STDVECTOR__			1

// __AARON_STRPERF__
#if _DEBUG
# define IS_CHECK_LENGTH_OK			1
#endif

//
//	This part actually defines the macros
//
#ifndef __AARON_DISABLE_ALL_PERFS__
# if X__AARON_PERFIMPROVE__
#  define __AARON_PERFIMPROVE__		1
# endif
# if X__AARON_INSTRPERF__
#  define __AARON_INSTRPERF__		1
# endif
# if X__AARON_LBLTBLPERF__
#  define __AARON_LBLTBLPERF__		1
# endif
# if X__AARON_STRUCPERF__
#  define __AARON_STRUCPERF__		1
# endif
# if X__AARON_STRPERF__
#  define __AARON_STRPERF__			1
# endif
# if X__AARON_VARTABLEPERF__
#  define __AARON_VARTABLEPERF__	1
# endif
#endif

#endif

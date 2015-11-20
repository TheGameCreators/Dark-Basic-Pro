#ifndef _COMMON_H_
#define _COMMON_H_

#define CURRENT_FILE		__FILE__
#define CURRENT_LINE		__LINE__
#define VC_EXTRALEAN

#include <tchar.h>
#include <wtypes.h>
#include <math.h>

#define EPSILON         0.01f
#define DEGTORAD(x)     (x * 0.01745329251994329547f)
#define RADTODEG(x)     (x * 57.2957795130823228646f)
#define PI              3.14159265358979323846f

#define LOG_GENERAL     0
#define LOG_HSR         1
#define LOG_ESR         2
#define LOG_BSP         3
#define LOG_PRT         4
#define LOG_PVS         5
#define LOG_TJR         6

#define PROCESS_HSR         0
#define PROCESS_ESR         1
#define PROCESS_BSP         2
#define PROCESS_PRT         3
#define PROCESS_PVS         4
#define PROCESS_TJR         5

#endif

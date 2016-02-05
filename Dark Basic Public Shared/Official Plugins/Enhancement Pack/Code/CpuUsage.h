#ifndef _CPUUSAGE_H
#define _CPUUSAGE_H

#include <windows.h>

class CCpuUsage
{
public:
	CCpuUsage();
	virtual ~CCpuUsage();

// Methods
	int GetCpuUsage();
	int GetCpuUsage(LPCTSTR pProcessName);

	BOOL EnablePerformaceCounters(BOOL bEnable = TRUE);

// Attributes
private:
	bool			m_bFirstTime;
	LONGLONG		m_lnOldValue ;
	LARGE_INTEGER	m_OldPerfTime100nSec;
};


#endif
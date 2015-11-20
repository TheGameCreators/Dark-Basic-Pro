#ifndef _OCCLUSION_H_
#define _OCCLUSION_H_

// Common includes
#include "..\CommonC.h"
#include "HZBManager.h"

// Header functions
class COcclusion
{
public:

	COcclusion();
	~COcclusion();

	// Mode
	void SetOcclusionMode(int iMode);
	int GetOcclusionMode(void) { return m_OcclusionMode; }

	// Start Of Cycle
	void StartOcclusionStep ( void );

	// HOQ
	void RenderOcclusionObject ( sObject* pObject );

	// HZB
	void AddOccluder ( sObject* pObject );
	void AddOccludee ( sObject* pObject );
	void DeleteOccludite ( sObject* pObject );

	// Stats
	int GetOccludedObjects(void) { return m_iOccluded; }

public:

	int						m_OcclusionMode;

	// HOQ Methods (need to improve this to avoid reserving 70,000 elements for all this!!)
	int						iQueryBusyStage[70000]; // query per-object (1MB used on this!)
	LPDIRECT3DQUERY9		d3dQuery[70000];
	DWORD					dwQueryValue[70000];
	int						m_iOccluded;

	// HZB Method
	HZBManager*				m_pHZB;

};

#endif

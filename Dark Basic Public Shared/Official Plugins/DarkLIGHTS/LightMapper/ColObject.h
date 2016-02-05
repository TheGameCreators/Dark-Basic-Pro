#ifndef COLOBJECT_H
#define COLOBJECT_H

#include "DBOData.h"

class ColObject
{
	
public:
	
	float x, y, z;

	sObject *pObject;
	ColObject *pNextObject;

	ColObject( sObject *pNewObject ) 
	{ 
		
		if ( pObject->pInstanceOfObject )
	}

	~ColObject( ) { }

};

#endif

#ifndef _CLOTHSTRUCTURE_H_
#define _CLOTHSTRUCTURE_H_

//#include <darksdk.h>
#include "globals.h"
#include "rigidbodycreation.h"
#include "clothobject.h"

namespace PhysX
{
	struct sCloth
	{
		bool			bFromObject;
		int				iObjectID;

		int				iID;
		cClothObject*	pCloth;
		NxClothDesc		desc;

		float			fWidth;
		float			fHeight;
		float			fDepth;

		int				iGroup;

		NxMat33			matRotation;

		std::vector < NxShape* > pShapeList;

		float			fRayCastHitX;
		float			fRayCastHitY;
		float			fRayCastHitZ;
		int				iRayCastHitVertex;
	};

	struct sClothVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float tu, tv;
	};

	extern std::vector < sCloth* > pClothList;
}

sObject*		dbGetObject			( int iID );
PhysX::sCloth*	dbPhyGetCloth		( int iID, bool bDisplayError = false );
NxCloth*		dbPhyGetClothObject ( int iID, bool bDisplayError = false );
void			dbPhyClearCloth		( int iID );

#endif _CLOTHSTRUCTURE_H_
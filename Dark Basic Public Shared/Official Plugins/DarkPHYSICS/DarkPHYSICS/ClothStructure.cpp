
// cloth structure includes
#include "ClothStructure.h"
#include "error.h"

// global namespace
namespace PhysX
{
	// cloth list
	std::vector < sCloth* >	pClothList;
}

PhysX::sCloth* dbPhyGetCloth ( int iID, bool bDisplayError )
{
	// returns pointer to the specified cloth object

	// run through the list
	for ( DWORD dwCloth = 0; dwCloth < PhysX::pClothList.size ( ); dwCloth++ )
	{
		// see if we have a match on ID
		if ( PhysX::pClothList [ dwCloth ]->iID == iID )
			return PhysX::pClothList [ dwCloth ];
	}

	// display an error if needed
	if ( bDisplayError )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cloth with specified ID does not exist", "", 0 );
		return NULL;
	}

	// return nothing
	return NULL;
}

NxCloth* dbPhyGetClothObject ( int iID, bool bDisplayError )
{
	// get the actual cloth data from the simulation

	// get the cloth
	PhysX::sCloth* pCloth = dbPhyGetCloth ( iID, bDisplayError );

	// see if the cloth has been created
	if ( pCloth->pCloth )
	{
		// see if the actual cloth exists
		if ( pCloth->pCloth->getNxCloth ( ) )
		{
			// return it
			return pCloth->pCloth->getNxCloth ( );
		}
	}
	
	// return nothing
	return NULL;
}

void dbPhyClearCloth ( int iID )
{
	// clear all cloth objects in the simulation

	// run through the list
	for ( DWORD dwCloth = 0; dwCloth < PhysX::pClothList.size ( ); dwCloth++ )
	{
		// get pointer to the cloth
		PhysX::sCloth* pCloth = PhysX::pClothList [ dwCloth ];

		if ( iID != -1 )
		{
			if ( pCloth->iID != iID )
				continue;
		}

		// delete the cloth if it has been built
		if ( pCloth->pCloth )
		{
			// remove the object associated with it
			PhysX::DeleteObject ( pCloth->iID );

			// delete the data
			delete pCloth->pCloth;

			// remove from list
			//PhysX::pClothList.erase ( pCloth );
			PhysX::pClothList.erase ( PhysX::pClothList.begin ( ) + dwCloth );
		}

		// remove cloth
		delete pCloth;
		pCloth = NULL;
	}

	// clear the list (180808 - only if erasing ALL)
	if ( iID == -1 ) PhysX::pClothList.clear ( );
}

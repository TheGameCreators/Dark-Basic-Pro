
#include "globals.h"
#include "vehiclestructure.h"
#include "error.h"

namespace PhysXVehicle
{
	std::vector < sVehicle* > pVehicleList;
}

PhysXVehicle::sVehicle* dbPhyGetVehicle ( int iID, bool bDisplayError )
{
	for ( DWORD dwVehicle = 0; dwVehicle < PhysXVehicle::pVehicleList.size ( ); dwVehicle++ )
	{
		if ( PhysXVehicle::pVehicleList [ dwVehicle ]->iID == iID )
			return PhysXVehicle::pVehicleList [ dwVehicle ];
	}

	// display an error if needed
	if ( bDisplayError )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Vehicle with specified ID does not exist", "", 0 );
		return NULL;
	}

	return NULL;
}
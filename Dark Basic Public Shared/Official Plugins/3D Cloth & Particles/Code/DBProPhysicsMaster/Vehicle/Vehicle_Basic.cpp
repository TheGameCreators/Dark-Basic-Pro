#include "stdafx.h"


DECLARE_PLUGIN(Vehicle_Basic);

//-----------------------------------------------------------------------------
//--- Commands ----------------------------------------------------------------
//-----------------------------------------------------------------------------



DLLEXPORT void makeVehicle(int id)
{
	ON_FAIL_DLL_SECURITY_RETURN(void)

	ALERT_ON_PERR(physics->addVehicle(id,VEHICLE_PLUGIN(Vehicle_Basic)),PCMD_MAKE_VEHICLE);
}

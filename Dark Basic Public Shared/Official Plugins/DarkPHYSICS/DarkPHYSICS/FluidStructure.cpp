
#include "FluidStructure.h"
#include "error.h"

namespace PhysX
{
	std::vector < sFluid* >	pFluidList;
	std::vector < sDrain* > pDrainList;
}

PhysX::sFluid* dbPhyGetFluid ( int iID, bool bDisplayError )
{
	for ( DWORD dwFluid = 0; dwFluid < PhysX::pFluidList.size ( ); dwFluid++ )
	{
		if ( PhysX::pFluidList [ dwFluid ]->iID == iID )
			return PhysX::pFluidList [ dwFluid ];
	}

	if ( bDisplayError )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Fluid with specified ID does not exist", "", 0 );
		return NULL;
	}

	return NULL;
}

PhysX::sDrain* dbPhyGetDrain ( int iID )
{
	for ( DWORD dwDrain = 0; dwDrain < PhysX::pDrainList.size ( ); dwDrain++ )
	{
		if ( PhysX::pDrainList [ dwDrain ]->iID == iID )
			return PhysX::pDrainList [ dwDrain ];
	}

	return NULL;
}

NxFluid* dbPhyGetFluidData ( int iID )
{
	if ( PhysX::sFluid* pFluid = dbPhyGetFluid ( iID ) )
	{
		if ( pFluid->pFluid )
		{
			return pFluid->pFluid;
		}
		else
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Fluid not built, cannot set access", "", 0 );
			return NULL;
		}
	}

	PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Fluid with specified ID does not exist", "", 0 );
	return NULL;
}

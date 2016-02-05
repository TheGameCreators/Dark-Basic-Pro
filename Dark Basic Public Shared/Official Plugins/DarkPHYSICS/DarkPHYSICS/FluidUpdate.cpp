
#include "FluidUpdate.h"
#include "globals.h"
#include "fluidstructure.h"
#include "DxFluidRenderer.h"
#include "..\..\..\..\Dark Basic Pro SDK\Shared\Camera\CCameraDataC.h"

D3DXMATRIX dbPhyGetFluidMatrix ( void );
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }


void dbPhyUpdateFluid ( void )
{
	for ( DWORD dwFluid = 0; dwFluid < PhysX::pFluidList.size ( ); dwFluid++ )
	{
		PhysX::sFluid*			pFluid  = PhysX::pFluidList [ dwFluid ];
		sObject*				pObject = NULL;
		sMesh*					pMesh	= NULL;
		PhysX::sFluidVertex*	pVertex = NULL;
		
		if ( !( pObject = PhysX::GetObject ( pFluid->iID ) ) )
			continue;

		if ( !pObject->pFrame )
			continue;

		if ( !( pMesh = pObject->pFrame->pMesh ) )
			continue;

		if ( !( pVertex = ( PhysX::sFluidVertex* ) pObject->pFrame->pMesh->pVertexData ) )
			continue;

        IDirect3DDevice9 * pd3dDevice = PhysX::GetDirect3DDevice();
        HRESULT hr = pd3dDevice->TestCooperativeLevel();
        if(hr == D3DERR_DEVICELOST)
        {
            PhysX::fluidSurfaceRenderer->onLostDevice();
        }
        else if( (hr == D3DERR_DRIVERINTERNALERROR) || (hr == D3DERR_DEVICEHUNG) || (hr == D3DERR_DEVICEREMOVED) )
        {
            PhysX::fluidSurfaceRenderer->onLostDevice();
            PhysX::fluidSurfaceRenderer->onDestroyDevice();
        }
        else if(hr == D3DERR_DEVICENOTRESET)
        {
            PhysX::fluidSurfaceRenderer->onResetDevice(pd3dDevice);
        }
        else
        {
            LPDIRECT3DVERTEXBUFFER9	vertexBuffer = PhysX::fluidSurfaceRenderer->getVertexBuffer();
            LPDIRECT3DVERTEXBUFFER9	densityBuffer = PhysX::fluidSurfaceRenderer->getDensityBuffer();

            D3DXVECTOR3 * pVertices;
            float * pDensities;

            hr = vertexBuffer->Lock(
                0,							                                // Offset to lock
                pFluid->iParticleBufferNum * sizeof(D3DXVECTOR3),			// Size to lock
                (void**) &pVertices, 
                D3DLOCK_DISCARD);

            if(hr!=0) assert(0);

            hr = densityBuffer->Lock(
                0,							                                // Offset to lock
                pFluid->iParticleBufferNum * sizeof(float),			        // Size to lock
                (void**) &pDensities, 
                D3DLOCK_DISCARD);

            for(NxU32 i=0;i<pFluid->iParticleBufferNum;i++)
            {
                *pVertices++ = *((D3DXVECTOR3 *)&pFluid->pParticleBuffer[i]);
                *pDensities++ = pFluid->pParticleDensityBuffer[i];
            }

            vertexBuffer->Unlock();
            densityBuffer->Unlock();

            PhysX::fluidSurfaceRenderer->setParticleCount(pFluid->iParticleBufferNum);
            PhysX::fluidSurfaceRenderer->setParticleRadius(0.075f);

            D3DXMATRIX view;
            D3DXMATRIX proj;

			tagCameraData*	pData		= ( tagCameraData* ) PhysX::GetCameraInternalData ( 0 );
			view = pData->matView;
			proj = pData->matProjection;
			float fovRadians = D3DX_PI;
			fovRadians = ( fovRadians / 180.0f );
			float fDegrees = 61.5f;//pData->fFOV;//strange the data does not copy to fdegrees
			fovRadians = ( fDegrees * fovRadians );
            PhysX::fluidSurfaceRenderer->render(pd3dDevice, view, proj, fovRadians);
        }

		NxBounds3 dest;
		NxVec3 extents;
		pFluid->pFluid->getWorldBounds ( dest );
		dest.getExtents ( extents );

		pObject->collision.fScaledLargestRadius = 0.0f;
		pObject->pFrame->pMesh->Collision.fRadius = 0.0f;
		pMesh->bVBRefreshRequired = true;
	}
}
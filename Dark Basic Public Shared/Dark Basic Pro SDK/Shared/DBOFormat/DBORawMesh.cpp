
//
// DBORawMesh Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////
// DBORAWMESH HEADER ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "DBORawMesh.h"
#include "DBOMesh.h"

// Mesh Load Functions

DARKSDK_DLL bool LoadRawMesh ( LPSTR pFilename, sMesh** ppMesh )
{
	// setup vars
	bool bResult = true;
	ID3DXBuffer* pAdjacency = NULL;
	ID3DXBuffer* pMaterials = NULL;
	ID3DXBuffer* pEffectInstances = NULL;
	ID3DXMesh* pNewMesh = NULL;
	DWORD dwMaterials = 0;

	// load the X file as a mesh
	HRESULT hRes = D3DXLoadMeshFromX(	pFilename,
										D3DXMESH_MANAGED,
										m_pD3D,
										&pAdjacency,
										&pMaterials,
										&pEffectInstances,
										&dwMaterials,
										&pNewMesh );

	// if loaded okay, convert to dbo-mesh
	if ( SUCCEEDED ( hRes ) )
	{
		// create dbo-mesh
		*ppMesh = new sMesh;

		// copy xfile data to dbo-mesh
		UpdateLocalMeshWithDXMesh ( *ppMesh, pNewMesh );

		// copy data from mesh to dbo-mesh
		DWORD dwIndexCount		= (*ppMesh)->dwIndexCount;
		DWORD dwVertexCount		= (*ppMesh)->dwVertexCount;

		// setup mesh properties
		(*ppMesh)->iPrimitiveType   = D3DPT_TRIANGLELIST;
		(*ppMesh)->iDrawVertexCount = dwVertexCount;
		if ( dwIndexCount==0 )
			(*ppMesh)->iDrawPrimitives  = dwVertexCount  / 3;
		else
			(*ppMesh)->iDrawPrimitives  = dwIndexCount  / 3;
	}
	else
	{
		// leeadd - 130306 - u60b3 - failed to load DX mesh
		// leeadd - 111108 - added actual return of false if fail to load mesh
		// bResult = false;

		// 220513 - if fail, create empty mesh for FPSC grace
		bResult = true;
		*ppMesh = new sMesh;
		memset ( *ppMesh, 0, sizeof(sMesh) );
		(*ppMesh)->iPrimitiveType   = D3DPT_TRIANGLELIST;
		(*ppMesh)->iDrawVertexCount = 0;
		(*ppMesh)->iDrawPrimitives  = 0;
		(*ppMesh)->dwFVF			= 274;
		(*ppMesh)->dwFVFSize		= 32;
	}

	// free the mesh and buffers
	SAFE_RELEASE ( pAdjacency );
	SAFE_RELEASE ( pMaterials );
	SAFE_RELEASE ( pNewMesh );

	// okay
	return bResult;
}

DARKSDK_DLL bool SaveRawMesh ( LPSTR pFilename, sMesh* pMesh )
{
	// result
	bool bResult=false;

	// if mesh exists
	if ( !pMesh ) return false;

	// get dx mesh from local mesh
	LPD3DXMESH pDXMesh = LocalMeshToDXMesh ( pMesh, NULL, 0 );

	// save out dx mesh
	if ( SUCCEEDED ( D3DXSaveMeshToX (	pFilename, 
										pDXMesh,
										NULL,
										NULL,
										NULL,
										0,
										//DXFILEFORMAT_TEXT ) ) ) //U57
										D3DXF_FILEFORMAT_TEXT ) ) ) //U59(SDK05)	// mike - 020206 - addition for vs8
	{
		// success
		bResult=true;
	}

	// free dx mesh
	SAFE_RELEASE(pDXMesh);

	// okay
	return bResult;
}

DARKSDK_DLL bool DeleteRawMesh ( sMesh* pMesh )
{
	// delete the mesh
	delete pMesh;

	// okay
	return true;
}


#ifndef _CMATRIXDATA_H_
#define _CMATRIXDATA_H_

#include "cpositionc.h"


#define D3D9_MATRIXVERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

struct tagMatrixVertexDesc
{
	float x, y, z;
	float nx, ny, nz;
	DWORD diffuse;
	float tu1, tv1;
};

struct tagTileProperties
{
	float fHeight;
	int	  iTile;
	D3DXVECTOR3 vNormal;
};

struct tagMatrixData : public tagObjectPos
{
	LPDIRECT3DVERTEXBUFFER9	lpVertexBuffer;
	LPDIRECT3DINDEXBUFFER9	lpIndexBuffer;
	DWORD					dwIndexBufferArraySize;
	LPDIRECT3DINDEXBUFFER9*	lpIndexBufferArray;
	LPDIRECT3DTEXTURE9*		lpTexture;

	LPDIRECT3DDEVICE9		pD3D;

	tagTileProperties*		pTiles;
	tagMatrixVertexDesc*	pfVert;
	int*					ptmpTiles;

	void*					pBuffer;
	
	int						iID;
	int						iWidth;
	int						iDepth;
	int						iXSegmented;
	int						iZSegmented;
	int						iXSegmentedFull;
	int						iZSegmentedFull;

	int						iTextureAcross;
	int						iTextureDown;

	int						iFilter;
	int						iGhostMode;		// ghost mode
	int						iTextureMode;
	int						iMipFilter;
	bool					bVisible;		// is visible
	bool					bWireframe;		// is wireframe
	bool					bLight;			// react to light
	bool					bCull;			// cull mode
	bool					bTransparency;	// transparency
	bool					bZDepth;		// z depth
	bool					bFog;
	bool					bAmbient;
	bool					bGhost;			// ghost on / off
	bool					bCollision;
	bool					bRenderAfterObjects;

	float					fTrimX;			// used to trim texture tiles
	float					fTrimY;			// used to trim texture tiles
};

#endif _CMATRIXDATA_H_
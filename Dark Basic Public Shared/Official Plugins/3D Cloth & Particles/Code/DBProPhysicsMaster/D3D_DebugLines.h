#ifndef DBPRO_D3D_DEBUGLINES_H
#define DBPRO_D3D_DEBUGLINES_H


#include "d3d9.h"	
#include "..\Common\Vector.h"



typedef IDirect3DDevice9* ( *GETDIRECT3DDEVICEPFN )	( void );


struct FVF_lineVertex
{
	float x,y,z;
	DWORD color;
};


#define MAX_NUM_D3D_LINES 600

class D3D_DebugLines
{
public:

	D3D_DebugLines()
	{
		curLine=0;
		buffer=0;
	}
	~D3D_DebugLines()
	{
		if(g_pD3D && buffer)
		{
			buffer->Release();
		}
	}

	void Init(HINSTANCE DBProSetupDebug_Handle);

	static D3D_DebugLines * getInst()
	{
		static D3D_DebugLines inst;
		return &inst;
	}

	void DrawGraphics();
	void setLine(const Vector3& p1, const Vector3& p2, DWORD color);
	void drawCross(const Vector3& p, float size,DWORD color);

	LPDIRECT3DDEVICE9	g_pD3D;
	HINSTANCE			hSetup;

	IDirect3DVertexBuffer9 * buffer;

	FVF_lineVertex d3dLine[2*MAX_NUM_D3D_LINES];	

	GETDIRECT3DDEVICEPFN GetDirect3DDevice;

	int curLine;
};


#endif

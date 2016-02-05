#include "stdafx.h"
#include "D3D_DebugLines.h"


void D3D_DebugLines::Init(HINSTANCE DBProSetupDebug_Handle)
{	
	GetDirect3DDevice = ( GETDIRECT3DDEVICEPFN ) GetProcAddress ( DBProSetupDebug_Handle, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );	
}

void D3D_DebugLines::setLine(const Vector3& p1, const Vector3& p2, DWORD color)
{
	if(curLine==MAX_NUM_D3D_LINES) return;

	FVF_lineVertex * curVert = &d3dLine[curLine*2];

	curVert->x=p1.x;
	curVert->y=p1.y;
	curVert->z=p1.z;
	curVert->color=color;

	curVert++;

	curVert->x=p2.x;
	curVert->y=p2.y;
	curVert->z=p2.z;
	curVert->color=color;

	curLine++;
}

void D3D_DebugLines::drawCross(const Vector3& p, float size,DWORD color)
{	
	size*=0.5f;
	setLine(Vector3(p.x-size,p.y,p.z),Vector3(p.x+size,p.y,p.z),color);
	setLine(Vector3(p.x,p.y-size,p.z),Vector3(p.x,p.y+size,p.z),color);
	setLine(Vector3(p.x,p.y,p.z-size),Vector3(p.x,p.y,p.z+size),color);
}

void D3D_DebugLines::DrawGraphics()
{
	if(curLine==0) return;

	HRESULT result;
	if(!g_pD3D) 
	{
		g_pD3D = GetDirect3DDevice();
		result = g_pD3D->CreateVertexBuffer(sizeof(FVF_lineVertex)*2*MAX_NUM_D3D_LINES,
									D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
									D3DFVF_XYZ | D3DFVF_DIFFUSE,
									D3DPOOL_DEFAULT,
									&buffer,0);
		if(result!=D3D_OK) throw "Bad vertex buffer";
	}

	void * ptr;

	result = buffer->Lock(0,0,&ptr,D3DLOCK_DISCARD);
	memcpy(ptr,d3dLine,sizeof(FVF_lineVertex)*2*curLine);
	result = buffer->Unlock();

	result = g_pD3D->SetStreamSource(0,buffer,0,sizeof(FVF_lineVertex));
	result = g_pD3D->SetVertexShader(0);
	result = g_pD3D->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	

	DWORD origVal[5];
	result = g_pD3D->GetRenderState(D3DRS_LIGHTING,&origVal[0]);
	result = g_pD3D->GetRenderState(D3DRS_COLORVERTEX,&origVal[1]);
	result = g_pD3D->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,&origVal[2]);
	
	result = g_pD3D->SetRenderState(D3DRS_LIGHTING,FALSE);
	result = g_pD3D->SetRenderState(D3DRS_COLORVERTEX,TRUE);
	result = g_pD3D->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);

	result = g_pD3D->DrawPrimitive( D3DPT_LINELIST, 0, curLine);
	
	result = g_pD3D->SetRenderState(D3DRS_LIGHTING,origVal[0]);
	result = g_pD3D->SetRenderState(D3DRS_COLORVERTEX,origVal[1]);
	result = g_pD3D->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,origVal[2]);
	curLine=0;
}

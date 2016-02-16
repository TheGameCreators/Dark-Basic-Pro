#pragma once
#define NOMINMAX
#include <d3dx9.h>
#include <NxDebugRenderable.h>

class D3DUtil
{
public:
	D3DUtil(void);
	D3DUtil(IDirect3DDevice9 *d3ddev, const NxDebugRenderable **dbgRenderable);
	~D3DUtil(void);

    void debugRender();
	void drawTriangles();
	void drawPoints();
	void drawLines();

	//******* DEVICE   *******//
	IDirect3DDevice9 *d3d_;
	D3DXMATRIX matIdentity_;
    //******* RENDERER *******//
	const NxDebugRenderable **dbgRenderable_;
};

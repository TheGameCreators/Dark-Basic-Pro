#include "D3DUtil.h"
D3DUtil::D3DUtil(void){
}

D3DUtil::D3DUtil(IDirect3DDevice9 *d3ddev, const NxDebugRenderable **dbgRenderable){
    d3d_ = d3ddev;
	dbgRenderable_ = dbgRenderable;
	D3DXMatrixIdentity(&matIdentity_);
}

D3DUtil::~D3DUtil(void){
}

void D3DUtil::debugRender(){
	//Test current scene != 0 using *dbdRenderable_
	
	//disable lighting?
	if(*dbgRenderable_){
		d3d_->SetTransform(D3DTS_WORLD, &matIdentity_);
		d3d_->SetRenderState(D3DRS_LIGHTING, FALSE);
		d3d_->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		this->drawLines();
		this->drawPoints();
		this->drawTriangles();
	}
}

void D3DUtil::drawTriangles(){
	unsigned int triCount = (*dbgRenderable_)->getNbTriangles();
	//NOT TESTED
	const NxDebugTriangle* tri = (*dbgRenderable_)->getTriangles();
	while(triCount--){
		d3d_->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, &tri->p0.x, 12);
		tri++;
	}
}

void D3DUtil::drawPoints(){
	unsigned int pointCount = (*dbgRenderable_)->getNbPoints();
	//NOT TESTED
	const NxDebugPoint* point = (*dbgRenderable_)->getPoints();
	while(pointCount--){
		d3d_->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &point->p.x, 8);
		point++;
	}
}

void D3DUtil::drawLines(){
	unsigned int lineCount = (*dbgRenderable_)->getNbLines();
	const NxDebugLine* line = (*dbgRenderable_)->getLines();
	while(lineCount--){
		d3d_->DrawPrimitiveUP(D3DPT_LINELIST, 1, &line->p0.x, 12);
		line++;
	}
}
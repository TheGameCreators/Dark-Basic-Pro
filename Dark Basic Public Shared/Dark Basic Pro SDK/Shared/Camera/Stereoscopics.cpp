//
// STEREOSCOPICS
// IZ3D CODE (under license)
//

// Includes
#include "Stereoscopics.h"
#include <d3d9.h>
#include <d3dx9tex.h>

// Externals
extern LPDIRECT3DDEVICE9	m_pD3D;

// Structures
struct  D3DTVERTEX_1T         { float x,y,z,rhw; float tu, tv; };
#define D3DFVF_D3DTVERTEX_1T  (D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0)) 

// Globals
IDirect3DPixelShader9*		m_pStereoscopicShader = NULL;
D3DTVERTEX_1T				m_pStereoscopicVertex [ 4 ];

// Shader - Internal Data
char shaderiZ3DCFLText[] =		   "\
ps_2_0								\
def c0, 0.5, 1, -1.00000001e-010, 0	\
dcl t0.xy							\
dcl_2d s0							\
dcl_2d s1							\
texld r0, t0, s0					\
texld r1, t0, s1					\
add r0, r0, r1						\
dp3 r1.x, r1, c0.y					\
mul r2, r0, c0.x					\
dp3 r0.x, r0, c0.y					\
mov oC0, r2							\
rcp r0.y, r0.x						\
add r0.x, r0.x, c0.z				\
mul r0.y, r1.x, r0.y				\
cmp r0, r0.x, r0.y, c0.x			\
mov oC1, r0";

// Anaglyph Shader
char shaderAnaglyphText[] =		   "\
ps_2_0								\
dcl t0.xy							\
dcl_2d s0							\
dcl_2d s1							\
texld r0, t0, s0					\
texld r1, t0, s1					\
mov r2.x, r0.x						\
mov r2.y, r1.y						\
mov r2.z, r1.z						\
mov r2.w, r0.w						\
mov oC0, r2";

// WOW Shader (greyscale)
char shaderWOWText[] =			   "\
ps_2_0								\
dcl t0.xy							\
dcl_2d s0							\
texld r0, t0, s0					\
mov r2.x, r0.w						\
mov r2.y, r0.w						\
mov r2.z, r0.w						\
mov r2.w, r0.w						\
mov oC0, r2";

// Create BACK/FRONT from LEFT/RIGHT
void StereoscopicConvertLeftRightToBackFront ( int iMode, LPDIRECT3DTEXTURE9 leftView, LPDIRECT3DTEXTURE9 rightView, LPDIRECT3DSURFACE9 resultingSurface0, LPDIRECT3DSURFACE9 resultingSurface1 )
{
	// create stereoscopic shader if not present
	if ( m_pStereoscopicShader==NULL )
	{
		// choose stereoscopic mode
		LPSTR pTheShader = shaderAnaglyphText;
		if ( iMode==1 ) pTheShader = shaderAnaglyphText;
		if ( iMode==2 ) pTheShader = shaderiZ3DCFLText;

		// calculate pixel shader for stereoscopic effect
		HRESULT res = S_OK;
		ID3DXBuffer* pCompiledShader = NULL;
		if ( FAILED ( res = D3DXAssembleShader (	pTheShader, lstrlen(pTheShader), NULL,
													NULL, 0, &pCompiledShader, NULL )))
		{ 
			return;
		}
		m_pD3D->CreatePixelShader ( (DWORD*)pCompiledShader->GetBufferPointer(), &m_pStereoscopicShader );

		// finished with buffer
		//SAFE_RELEASE ( pCompiledShader );

		// create quad for final renders
		D3DSURFACE_DESC pSrcInfo;
		resultingSurface0->GetDesc(&pSrcInfo);
		m_pStereoscopicVertex[0].x   = 0 - .5f;
		m_pStereoscopicVertex[0].y   = 0 - .5f;
		m_pStereoscopicVertex[0].z   = 0;
		m_pStereoscopicVertex[0].rhw = 1;    
		m_pStereoscopicVertex[0].tu  = 0;    
		m_pStereoscopicVertex[0].tv  = 0;    
		m_pStereoscopicVertex[1].x   = pSrcInfo.Width - .5f;
		m_pStereoscopicVertex[1].y   = 0 - .5f;
		m_pStereoscopicVertex[1].z   = 0;
		m_pStereoscopicVertex[1].rhw = 1;    
		m_pStereoscopicVertex[1].tu  = 1;    
		m_pStereoscopicVertex[1].tv  = 0;    
		m_pStereoscopicVertex[2].x   = pSrcInfo.Width - .5f;
		m_pStereoscopicVertex[2].y   = pSrcInfo.Height - .5f;
		m_pStereoscopicVertex[2].z   = 0;
		m_pStereoscopicVertex[2].rhw = 1;    
		m_pStereoscopicVertex[2].tu  = 1;    
		m_pStereoscopicVertex[2].tv  = 1;    
		m_pStereoscopicVertex[3].x   = 0 - .5f;
		m_pStereoscopicVertex[3].y   = pSrcInfo.Height - .5f;
		m_pStereoscopicVertex[3].z   = 0;
		m_pStereoscopicVertex[3].rhw = 1;    
		m_pStereoscopicVertex[3].tu  = 0;    
		m_pStereoscopicVertex[3].tv  = 1;    
	}

	// set render states
	m_pD3D->SetTexture(0, leftView);
	m_pD3D->SetTexture(1, rightView);
	m_pD3D->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, 1);
	m_pD3D->SetSamplerState(1, D3DSAMP_SRGBTEXTURE, 1);
	m_pD3D->SetRenderState(D3DRS_SRGBWRITEENABLE, 1);
	m_pD3D->SetPixelShader(m_pStereoscopicShader);
	m_pD3D->SetFVF(D3DFVF_D3DTVERTEX_1T);
	m_pD3D->SetRenderTarget(0, resultingSurface0);
	m_pD3D->SetRenderTarget(1, resultingSurface1);
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
	m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE, false );

	// draw stereoscopic back/front surface from left/right textures
	DWORD dwBackdropColor = D3DCOLOR_XRGB(255,0,0);
	m_pD3D->Clear ( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, dwBackdropColor, 1.0f, 0 );
	m_pD3D->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pStereoscopicVertex, sizeof(D3DTVERTEX_1T));

	// restore states after render
	m_pD3D->SetTexture(0, NULL);
	m_pD3D->SetTexture(1, NULL);
	m_pD3D->SetPixelShader(NULL);
	m_pD3D->SetRenderTarget(0, NULL);
	m_pD3D->SetRenderTarget(1, NULL);
}

void StereoscopicConvertLeftToGreySideBySide ( int iMode, LPDIRECT3DTEXTURE9 pLeft, LPDIRECT3DSURFACE9 pGrey, LPDIRECT3DSURFACE9 pSideBySide )
{
	// surface dimensions
	D3DSURFACE_DESC pSrcInfo;
	pGrey->GetDesc(&pSrcInfo);

	// create stereoscopic shader if not present
	if ( m_pStereoscopicShader==NULL )
	{
		// calculate pixel shader for stereoscopic effect
		HRESULT res = S_OK;
		LPSTR pTheShader = shaderWOWText;
		ID3DXBuffer* pCompiledShader = NULL;
		if ( FAILED ( res = D3DXAssembleShader (	pTheShader, lstrlen(pTheShader), NULL,
													NULL, 0, &pCompiledShader, NULL )))
		{ 
			return;
		}
		m_pD3D->CreatePixelShader ( (DWORD*)pCompiledShader->GetBufferPointer(), &m_pStereoscopicShader );

		// create quad for final renders
		m_pStereoscopicVertex[0].x   = 0 - .5f;
		m_pStereoscopicVertex[0].y   = 0 - .5f;
		m_pStereoscopicVertex[0].z   = 0;
		m_pStereoscopicVertex[0].rhw = 1;    
		m_pStereoscopicVertex[0].tu  = 0;    
		m_pStereoscopicVertex[0].tv  = 0;    
		m_pStereoscopicVertex[1].x   = pSrcInfo.Width - .5f;
		m_pStereoscopicVertex[1].y   = 0 - .5f;
		m_pStereoscopicVertex[1].z   = 0;
		m_pStereoscopicVertex[1].rhw = 1;    
		m_pStereoscopicVertex[1].tu  = 1;    
		m_pStereoscopicVertex[1].tv  = 0;    
		m_pStereoscopicVertex[2].x   = pSrcInfo.Width - .5f;
		m_pStereoscopicVertex[2].y   = pSrcInfo.Height - .5f;
		m_pStereoscopicVertex[2].z   = 0;
		m_pStereoscopicVertex[2].rhw = 1;    
		m_pStereoscopicVertex[2].tu  = 1;    
		m_pStereoscopicVertex[2].tv  = 1;    
		m_pStereoscopicVertex[3].x   = 0 - .5f;
		m_pStereoscopicVertex[3].y   = pSrcInfo.Height - .5f;
		m_pStereoscopicVertex[3].z   = 0;
		m_pStereoscopicVertex[3].rhw = 1;    
		m_pStereoscopicVertex[3].tu  = 0;    
		m_pStereoscopicVertex[3].tv  = 1;    
	}

	// set render states
	m_pD3D->SetTexture(0, pLeft);
	m_pD3D->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, 1);
	m_pD3D->SetRenderState(D3DRS_SRGBWRITEENABLE, 1);
	m_pD3D->SetPixelShader(m_pStereoscopicShader);
	m_pD3D->SetFVF(D3DFVF_D3DTVERTEX_1T);
	m_pD3D->SetRenderTarget(0, pGrey);
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
	m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE, false );

	// draw stereoscopic back/front surface from left/right textures
	DWORD dwBackdropColor = D3DCOLOR_XRGB(0,0,255);
	m_pD3D->Clear ( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, dwBackdropColor, 1.0f, 0 );
	m_pD3D->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pStereoscopicVertex, sizeof(D3DTVERTEX_1T));

	IDirect3DSurface9* pLeftSurface = NULL;
	pLeft->GetSurfaceLevel(0, &pLeftSurface );

	// Stretchrect original and grey to side by side surface
	RECT rectDestRectL = { 0, 0, pSrcInfo.Width/2, pSrcInfo.Height };
	m_pD3D->StretchRect( pLeftSurface, NULL, pSideBySide, &rectDestRectL, D3DTEXF_NONE );
	RECT rectDestRectR = { pSrcInfo.Width/2, 0, pSrcInfo.Width, pSrcInfo.Height };
	m_pD3D->StretchRect( pGrey, NULL, pSideBySide, &rectDestRectR, D3DTEXF_NONE );

	// restore states after render
	m_pD3D->SetTexture(0, NULL);
	m_pD3D->SetPixelShader(NULL);
	m_pD3D->SetRenderTarget(0, NULL);
}

void FreeStereoscopicResources ( void )
{
	if ( m_pStereoscopicShader )
	{
		m_pStereoscopicShader->Release();
		m_pStereoscopicShader = NULL;
	}
}
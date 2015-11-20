//
// TEXTURE BACKDROP
// Draw a QUAD to the backdrop, just after it is cleared (DarkSKY)
//

// Includes
#include "TextureBackdrop.h"
#include <d3d9.h>
#include <d3dx9tex.h>

// Externals
extern LPDIRECT3DDEVICE9	m_pD3D;

// Structures
struct  D3DTVERTEX_1T2        { float x,y,z,rhw; float tu, tv; };
#define D3DFVF_D3DTVERTEX_1T2 (D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0)) 

// Globals
bool						m_bBackdropVertexCreated = false;
D3DTVERTEX_1T2				m_pBackdropVertex [ 4 ];

// Create BACK/FRONT from LEFT/RIGHT
void DrawTextureBackdrop ( int iMode, LPDIRECT3DSURFACE9 resultingSurface0, LPDIRECT3DTEXTURE9 pImage )
{
	// create stereoscopic shader if not present
	if ( m_bBackdropVertexCreated==false )
	{
		// created successfully
		m_bBackdropVertexCreated = true;

		// create quad for final renders
		D3DSURFACE_DESC pSrcInfo;
		resultingSurface0->GetDesc(&pSrcInfo);
		m_pBackdropVertex[0].x   = 0 - .5f;
		m_pBackdropVertex[0].y   = 0 - .5f;
		m_pBackdropVertex[0].z   = 0;
		m_pBackdropVertex[0].rhw = 1;    
		m_pBackdropVertex[0].tu  = 0;    
		m_pBackdropVertex[0].tv  = 0;    
		m_pBackdropVertex[1].x   = pSrcInfo.Width - .5f;
		m_pBackdropVertex[1].y   = 0 - .5f;
		m_pBackdropVertex[1].z   = 0;
		m_pBackdropVertex[1].rhw = 1;    
		m_pBackdropVertex[1].tu  = 1;    
		m_pBackdropVertex[1].tv  = 0;    
		m_pBackdropVertex[2].x   = pSrcInfo.Width - .5f;
		m_pBackdropVertex[2].y   = pSrcInfo.Height - .5f;
		m_pBackdropVertex[2].z   = 0;
		m_pBackdropVertex[2].rhw = 1;    
		m_pBackdropVertex[2].tu  = 1;    
		m_pBackdropVertex[2].tv  = 1;    
		m_pBackdropVertex[3].x   = 0 - .5f;
		m_pBackdropVertex[3].y   = pSrcInfo.Height - .5f;
		m_pBackdropVertex[3].z   = 0;
		m_pBackdropVertex[3].rhw = 1;    
		m_pBackdropVertex[3].tu  = 0;    
		m_pBackdropVertex[3].tv  = 1;    
	}

	// 20120311 IanM - Save fog state, then ensure that fog is disabled
	DWORD dwFogState = FALSE;
	m_pD3D->GetRenderState(D3DRS_FOGENABLE, &dwFogState);
	if (dwFogState)
		m_pD3D->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// set render states
	m_pD3D->SetTexture(0, pImage);
	m_pD3D->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, 1);
	m_pD3D->SetRenderState(D3DRS_SRGBWRITEENABLE, 1);
	m_pD3D->SetPixelShader(NULL);
	m_pD3D->SetFVF(D3DFVF_D3DTVERTEX_1T2);
	m_pD3D->SetRenderTarget(0, resultingSurface0);
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
	m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE, false );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, false );

	// U75 - 081009 - ensure texture filtering is standard for backdrop render
	m_pD3D->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	// draw stereoscopic back/front surface from left/right textures
	m_pD3D->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pBackdropVertex, sizeof(D3DTVERTEX_1T2));

	// restore states after render
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, true );
	m_pD3D->SetTexture(0, NULL);
	m_pD3D->SetPixelShader(NULL);
	m_pD3D->SetRenderTarget(0, NULL);

	// 20120311 IanM - Re-enable the fog if it was disabled earlier
	if (dwFogState)
		m_pD3D->SetRenderState(D3DRS_FOGENABLE, TRUE);
}

void FreeTextureBackdrop ( void )
{
}
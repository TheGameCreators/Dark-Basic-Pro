#include "CSnowC.h"

#include ".\..\core\globstruct.h"

extern LPDIRECT3DDEVICE9	m_pD3D;

typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamIntPFN )  ( int );
extern IMAGE_RetLPD3DTEX9ParamIntPFN	g_Image_GetPointer;

extern GlobStruct*				g_pGlob;

CSnowstorm::CSnowstorm ( int numParticles, D3DXVECTOR3 origin, float height, float width, float depth, bool rev )
						: m_height        ( height ),
						  m_width         ( width ),
						  m_depth         ( depth ),
						  m_bRev		  ( rev ),
						  CParticleSystem ( numParticles, origin )
{
	// reset
	m_numParticles    = 0;
	m_accumulatedTime = 0.0f;
}

void CSnowstorm::InitializeParticle ( int index )
{
	// sets the initial particle properties for the snowstorm

	// start the particle at the sky at a random location in the emission zone
	if ( m_bRev )
		m_particleList [ index ].m_pos.y = -m_height;
	else
		m_particleList [ index ].m_pos.y = m_height;

	m_particleList [ index ].m_pos.x = m_origin.x + FRAND * m_width;
	m_particleList [ index ].m_pos.z = m_origin.z + FRAND * m_depth;

	// set the size of the particle
	m_particleList[index].m_size = SNOWFLAKE_SIZE;

	// give the particle a random velocity
	m_particleList [ index ].m_velocity.x = SNOWFLAKE_VELOCITY.x + FRAND * VELOCITY_VARIATION.x;
	m_particleList [ index ].m_velocity.y = SNOWFLAKE_VELOCITY.y + FRAND * VELOCITY_VARIATION.y;
	m_particleList [ index ].m_velocity.z = SNOWFLAKE_VELOCITY.z + FRAND * VELOCITY_VARIATION.z;

	// reverse Y velocity
	if ( m_bRev ) m_particleList [ index ].m_velocity.y *= -1.0f;
}

void CSnowstorm::Update ( float elapsedTime, int iExternalNumberOfEmissions )
{
	// update the existing particles, killing them and creating new ones as needed
	for ( int i = 0; i < m_numParticles; )
	{
		// update the particle's position based on the elapsed time and velocity
		m_particleList [ i ].m_pos = m_particleList [ i ].m_pos + m_particleList [ i ].m_velocity * elapsedTime;

		// if the particle has hit the ground plane, kill it
		bool bKillIt=false;
		if ( m_bRev ) 
		{
			if ( m_particleList [ i ].m_pos.y >= m_height )
				bKillIt=true;
		}
		else
		{
			if ( m_particleList [ i ].m_pos.y <= -5 )
				bKillIt=true;
		}

		if(bKillIt)
		{
			// move the last particle to the current positon, and decrease the count
			m_particleList [ i ] = m_particleList [ --m_numParticles ];
		}
		else
		{
			++i;
		}
	}

	// store the accumulated time
	m_accumulatedTime += elapsedTime;

	// determine how many new particles are needed
	// leeadd - 070306 - u60 - can override this with fed in new emissions count
	int iExtNumPartMax = iExternalNumberOfEmissions;//SNOWFLAKES_PER_SEC
	int newParticles = (int)((float)iExtNumPartMax * (float)m_accumulatedTime);

	// save the remaining time for after the new particles are released.
	if ( iExtNumPartMax > 0 )
		m_accumulatedTime -= 1.0f / ( float ) iExtNumPartMax * newParticles;
	else
		m_accumulatedTime = 0.0f;

	Emit ( newParticles );
}

void CSnowstorm::Render ( )
{
	struct tagPrimitive
	{
		float  x,  y,  z;
		float tu, tv;
	};
	
	#define D3DFVF_PRIMITIVE ( D3DFVF_XYZ | D3DFVF_TEX1 )

	tagPrimitive	vertices [ 6 ];
	D3DXVECTOR3		partPos;
	float			size;

	m_pD3D->SetVertexShader ( NULL );
	m_pD3D->SetFVF ( D3DFVF_XYZ | D3DFVF_TEX1 );

	m_pD3D->SetTexture      ( 0, m_lpTexture );

	{
		// set the cull and fill mode
		m_pD3D->SetRenderState  ( D3DRS_FILLMODE, D3DFILL_SOLID );
		m_pD3D->SetRenderState  ( D3DRS_DITHERENABLE, FALSE );
		m_pD3D->SetRenderState  ( D3DRS_LIGHTING, FALSE);
		m_pD3D->SetRenderState  ( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pD3D->SetRenderState  ( D3DRS_FOGENABLE, FALSE );
		m_pD3D->SetRenderState  ( D3DRS_ZWRITEENABLE, FALSE );
		m_pD3D->SetRenderState  ( D3DRS_ZENABLE, FALSE );
		m_pD3D->SetRenderState  ( D3DRS_COLORVERTEX, TRUE );
		m_pD3D->SetRenderState  ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		m_pD3D->SetRenderState  ( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
		m_pD3D->SetRenderState  ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
		m_pD3D->SetRenderState  ( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
		m_pD3D->SetRenderState  ( D3DRS_SPECULARENABLE, FALSE );
		m_pD3D->SetRenderState  ( D3DRS_MULTISAMPLEANTIALIAS, FALSE );

		// turn alpha blending on
		m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
		m_pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );

		// set mipmapping and texture mode
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER,       D3DTEXF_POINT  );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER,       D3DTEXF_POINT  );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER,       D3DTEXF_POINT );//D3DTEXF_NONE  );

		// setup texture states to allow for diffuse colours
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	}

	for ( int i = 0; i < m_numParticles; ++i )
	{
	    partPos = m_particleList [ i ].m_pos;
		size    = m_particleList [ i ].m_size;

		float fWidth  = partPos.x;
		float fHeight = partPos.y;

		vertices [ 0 ].x = partPos.x;				vertices [ 0 ].y = partPos.y;			vertices [ 0 ].z = partPos.z;
		vertices [ 1 ].x = partPos.x + size;		vertices [ 1 ].y = partPos.y;			vertices [ 1 ].z = partPos.z;
		vertices [ 2 ].x = partPos.x;				vertices [ 2 ].y = partPos.y - size;	vertices [ 2 ].z = partPos.z;
		vertices [ 3 ].x = partPos.x + size;		vertices [ 3 ].y = partPos.y - size;	vertices [ 3 ].z = partPos.z;

		vertices [ 0 ].tu = 0.0;		vertices [ 0 ].tv = 1.0;
		vertices [ 1 ].tu = 0.0;		vertices [ 1 ].tv = 0.0;
		vertices [ 2 ].tu = 1.0;		vertices [ 2 ].tv = 1.0;
		vertices [ 3 ].tu = 1.0;		vertices [ 3 ].tv = 0.0;
		
		// this is really crap, we need to set this up as a vertex buffer and draw them all at once
		m_pD3D->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, &vertices, sizeof ( tagPrimitive ) );

		g_pGlob->dwNumberOfPolygonsDrawn += 2;
	}
}


void CSnowstorm::InitializeSystem ( int ImageIndex )
{
	m_lpTexture = g_Image_GetPointer ( ImageIndex );
	/*
	D3DXCreateTextureFromFileExA
								(
									m_pD3D,
									"snow.bmp",
									D3DX_DEFAULT,
									D3DX_DEFAULT,
									D3DX_DEFAULT,
									0,
									D3DFMT_UNKNOWN,
									D3DPOOL_DEFAULT,
									D3DX_DEFAULT,
									D3DX_DEFAULT,
									D3DCOLOR_ARGB ( 255, 0, 0, 0 ),
									NULL,
									NULL,
									&m_lpTexture
								);
	*/

	// let parent do remaining initialization
	CParticleSystem::InitializeSystem ( );
}


void CSnowstorm::KillSystem ( )
{
	// let parent do remaining shutdown
	CParticleSystem::KillSystem ( );
}
#include "cparticlemanagerc.h"
#include <mmsystem.h>
#include ".\..\..\utilities\DXUtil.h"
#include "CSnowC.h"

// External Ptrs
extern LPDIRECT3DDEVICE9		m_pD3D;
extern D3DMATERIAL9				gWhiteMaterial;

CParticleManager::CParticleManager ( )
{
	
}

CParticleManager::~CParticleManager ( )
{
	
}

bool CParticleManager::Add ( tagParticleData* pData, int iID )
{
	///////////////////////////////////////
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	tagParticleData* ptr = NULL;
	ptr = ( tagParticleData* ) m_List.Get ( iID );
			
	if ( ptr != NULL )
		m_List.Delete ( iID );
	///////////////////////////////////////

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	tagParticleData* test;
	test = new tagParticleData;

	memset ( test,     0, sizeof ( test            ) );
	memcpy ( test, pData, sizeof ( tagParticleData ) );

	m_List.Add ( iID, ( VOID* ) test, 0, 1 );
	///////////////////////////////////////

	return true;
}

bool CParticleManager::Delete ( int iID )
{
	m_List.Delete ( iID );

	return true;
}

tagParticleData* CParticleManager::GetData ( int iID )
{		
	return ( tagParticleData* ) m_List.Get ( iID );
}

extern CSnowstorm*				m_pSnow;

void CParticleManager::Update ( void )
{
	// Standard Polydraw Renderstates
	m_pD3D->SetRenderState ( D3DRS_FILLMODE,				D3DFILL_SOLID );
	m_pD3D->SetRenderState ( D3DRS_LIGHTING,				FALSE );
	m_pD3D->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_NONE );
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE,				TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE,					TRUE );//240203-added for particle correction
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );//240203-should not be write enabled
	
	m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_WRAP );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_WRAP );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_POINT );

	// Set material for polys
	m_pD3D->SetRenderState ( D3DRS_COLORVERTEX,					TRUE );
	m_pD3D->SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE,		D3DMCS_COLOR1 );

	// wipe out fog color so particles are not affected twice
	DWORD g_dwFogColor;
	m_pD3D->GetRenderState ( D3DRS_FOGCOLOR, &g_dwFogColor );
	m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, D3DCOLOR_RGBA ( 0, 0, 0, 0 ) );

    UpdateInner();

	m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, g_dwFogColor );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
}

void CParticleManager::UpdateInner ( void )
{
	// for all particle objects
	link* check = m_List.m_start;
	for ( int temp = 0; temp < m_List.m_count; temp++ )
	{
		tagParticleData* ptr = NULL;
		ptr = this->GetData ( check->id );
		
		if ( ptr == NULL )
			return;

		// If visible
		if ( ptr->m_bVisible==true )
		{
			// World transform
			ptr->pD3D->SetTransform ( D3DTS_WORLD, &ptr->matObject );

			// Texture stage handling
			if( ptr->m_pTexture || ptr->m_bEffect )
			{
				// Texture
				ptr->pD3D->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
				ptr->pD3D->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				ptr->pD3D->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			}
			else
			{
				// Colour
				ptr->pD3D->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
				ptr->pD3D->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
			}

			// ghost handling
			if ( ptr->m_bGhost )
			{
				m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
				if ( ptr->m_fGhostPercentage >= 0.0f )
				{
					// set alpha factor and render states here
					m_pD3D->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
					m_pD3D->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

					// set alpha factor and texture stage
					m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
					m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
					float fPercentage = ptr->m_fGhostPercentage/100.0f;
					DWORD dwAlpha = (DWORD)(fPercentage*255);
					D3DCOLOR dwAlphaValueOnly = D3DCOLOR_ARGB ( dwAlpha, 0, 0, 0 );
					m_pD3D->SetRenderState( D3DRS_TEXTUREFACTOR, dwAlphaValueOnly );
				}
				else
				{
					switch ( ptr->m_iGhostMode )
					{
						case -1: // leefix - 240303 - old particle ghost mode
						{
							m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
							m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
						}
						break;

						case 0:
						{
							m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
							m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
						}
						break;

						case 1:
						{
							m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
							m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
						}
						break;

						case 2:
						{
							m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
							m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
						}
						break;

						case 3:
						{
							m_pD3D->SetRenderState ( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
							m_pD3D->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
						}
						break;

						case 4:
						{
							m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
							m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
						}
						break;

						case 5:
						{
							m_pD3D->SetRenderState ( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
							m_pD3D->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
						}
						break;
					}
				}
			}
			else
			{
				m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
			}

			// Snow, Rain, Fire...Effects (texture inside effect)
			if ( ptr->m_bEffect )
			{
				// Render special particles
				ptr->m_pSnow->Render ( );
			}
			else
			{
				// Standard particles
				ptr->pD3D->SetTexture ( 0, ptr->m_pTexture );
			
				ptr->fTime += 0.00003f;	
				PARTICLE *pParticle, **ppParticle;
				ptr->fTime += ptr->fSecsPerFrame;

				ppParticle = &ptr->m_pParticles;
				while ( *ppParticle )
				{
					pParticle = *ppParticle;

					float fT = ptr->fTime - pParticle->m_fTime0;
					float fGravity;

					if ( pParticle->m_bSpark )
					{
						fGravity = -ptr->m_fGravity;
						pParticle->m_fFade -= ptr->fSecsPerFrame * 2.25f;
					}
					else
					{
						fGravity = -(ptr->m_fGravity*2);
						pParticle->m_fFade -= ptr->fSecsPerFrame * 0.25f;
					}

					// position particle
					pParticle->m_vPos    = pParticle->m_vVel0 * fT + pParticle->m_vPos0;
					pParticle->m_vPos.y += ( 0.5f * fGravity ) * ( fT * fT );
					pParticle->m_vVel.y  = pParticle->m_vVel0.y + fGravity * fT;

					// apply some chaos to particle position
					if(ptr->m_fChaos!=0.0f)
					{
						pParticle->m_vPos.x += (((rand()%2)*2)-1) * ptr->m_fChaos;
						pParticle->m_vPos.y += (((rand()%2)*2)-1) * ptr->m_fChaos;
						pParticle->m_vPos.z += (((rand()%2)*2)-1) * ptr->m_fChaos;
					}

					// fade particle life
					if ( pParticle->m_fFade < 0.0f )
						pParticle->m_fFade = 0.0f;
						
					// kill old particles
					bool bOkToKill=false;
					if ( ptr->m_iFloorActive )
					{
						if ( pParticle->m_vPos.y < 0.0f || pParticle->m_fFade <= 0.0f )
							bOkToKill=true;
					}
					else
					{
						if ( pParticle->m_fFade <= 0.0f )
							bOkToKill=true;
					}

					if ( bOkToKill==true )
					{
						// emit sparks
						if ( !pParticle->m_bSpark && ptr->m_iFloorActive && pParticle->m_vPos.y < 0.0f )
						{
							for( int i=0; i<4; i++ )
							{
								PARTICLE *pSpark;

								if ( ptr->m_pParticlesFree )
								{
									pSpark = ptr->m_pParticlesFree;
									ptr->m_pParticlesFree = pSpark->m_pNext;
								}
								else
								{
									if ( NULL == ( pSpark = new PARTICLE ) )
										return;
								}

								pSpark->m_pNext    = pParticle->m_pNext;
								pParticle->m_pNext = pSpark;

								pSpark->m_bSpark  = true;
								pSpark->m_vPos0   = pParticle->m_vPos;
								pSpark->m_vPos0.y = 0.0f;//ptr->vPosition.y;//ptr->m_fRadius;

								FLOAT fRand1 = ( ( FLOAT ) rand() / ( FLOAT ) RAND_MAX ) * D3DX_PI * 2.00f;
								FLOAT fRand2 = ( ( FLOAT ) rand() / ( FLOAT ) RAND_MAX ) * D3DX_PI * 0.25f;

								pSpark->m_vVel0.x  = pParticle->m_vVel.x * 0.25f + cosf ( fRand1 ) * sinf ( fRand2 );
								pSpark->m_vVel0.z  = pParticle->m_vVel.z * 0.25f + sinf ( fRand1 ) * sinf ( fRand2 );
								pSpark->m_vVel0.y  = cosf ( fRand2 );
								pSpark->m_vVel0.y *= ( ( FLOAT ) rand() / ( FLOAT ) RAND_MAX ) * 1.5f;

								pSpark->m_vPos = pSpark->m_vPos0;
								pSpark->m_vVel = pSpark->m_vVel0;

								D3DXColorLerp( &pSpark->m_clrDiffuse, &pParticle->m_clrFade, &pParticle->m_clrDiffuse, pParticle->m_fFade );

								pSpark->m_clrFade = D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 1.0f );

								// mike - 250604 - change fade to colour
								pSpark->m_clrFade = ptr->EmitColor;

								pSpark->m_fFade   = ptr->m_iLife/100.0f;
								pSpark->m_fTime0  = ptr->fTime;
							}
						}

						*ppParticle = pParticle->m_pNext;
						pParticle->m_pNext = ptr->m_pParticlesFree;
						ptr->m_pParticlesFree = pParticle;

						if ( !pParticle->m_bSpark )
							ptr->m_dwParticles--;
					}
					else
					{
						ppParticle = &pParticle->m_pNext;
					}
				}

				 // Emit new particles
				DWORD dwParticlesEmit = ptr->m_dwParticles + ptr->dwNumParticlesToEmit;
				
				while ( ptr->m_dwParticles < ptr->m_dwParticlesLim && ptr->m_dwParticles < dwParticlesEmit )
				{
					if ( ptr->m_pParticlesFree )
					{
						pParticle = ptr->m_pParticlesFree;
						ptr->m_pParticlesFree = pParticle->m_pNext;
					}
					else
					{
						if ( NULL == ( pParticle = new PARTICLE ) )
							return;
					}

					pParticle->m_pNext = ptr->m_pParticles;
					ptr->m_pParticles = pParticle;
					ptr->m_dwParticles++;

					// Emit new particle
					FLOAT fRand1 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 2.0f;
					FLOAT fRand2 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 0.25f;

					pParticle->m_bSpark = FALSE;

					pParticle->m_vPos0 = ptr->vPosition;// + D3DXVECTOR3( 0.0f, ptr->m_fRadius, 0.0f );

					pParticle->m_vVel0.x  = cosf(fRand1) * sinf(fRand2) * 2.5f;
					pParticle->m_vVel0.z  = sinf(fRand1) * sinf(fRand2) * 2.5f;
					pParticle->m_vVel0.y  = cosf(fRand2);
					pParticle->m_vVel0.y *= ((FLOAT)rand()/(FLOAT)RAND_MAX) * ptr->fEmitVel;

					pParticle->m_vPos = pParticle->m_vPos0;
					pParticle->m_vVel = pParticle->m_vVel0;

					pParticle->m_clrDiffuse = ptr->EmitColor;
					pParticle->m_clrFade    = D3DXCOLOR ( 0.25f,   0.25f,   0.25f,   1.0f );
					
					// mike - 250604 - change fade to colour
					pParticle->m_clrFade = ptr->EmitColor;

					pParticle->m_fFade      = ptr->m_iLife/100.0f;
					pParticle->m_fTime0     = ptr->fTime;
				}

				//////////////////
				// start render //
				//////////////////
				{
					HRESULT hr;

					ptr->pD3D->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
					ptr->pD3D->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );

// old particles did not have distance attenuation
//					ptr->pD3D->SetRenderState( D3DRS_POINTSIZE,     FtoDW(ptr->m_fRadius/10.0f) );
//					ptr->pD3D->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.0f) );
//					ptr->pD3D->SetRenderState( D3DRS_POINTSIZE_MAX, FtoDW(ptr->m_fRadius/10.0f) );
//					ptr->pD3D->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.2f) );
//					ptr->pD3D->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.0f) );
//					ptr->pD3D->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(0.8f) );
// leechange - 030306 - u60 - now take distance into account
					ptr->pD3D->SetRenderState( D3DRS_POINTSIZE,     FtoDW(ptr->m_fRadius/10.0f) );
					ptr->pD3D->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.0f) );
					ptr->pD3D->SetRenderState( D3DRS_POINTSIZE_MAX, FtoDW(ptr->m_fRadius) );
					ptr->pD3D->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.0f) );
					ptr->pD3D->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.0f) );
					ptr->pD3D->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.0f) );
					
					ptr->pD3D->SetStreamSource( 0, ptr->m_pVB, 0, sizeof(POINTVERTEX) );
					ptr->pD3D->SetVertexShader( NULL );
					ptr->pD3D->SetFVF( D3DFVF_POINTVERTEX );

					PARTICLE*    pParticle = ptr->m_pParticles;
					POINTVERTEX* pVertices;
					DWORD        dwNumParticlesToRender = 0;

					ptr->m_dwBase += ptr->m_dwFlush;

					if ( ptr->m_dwBase >= ptr->m_dwDiscard )
						ptr->m_dwBase = 0;

					if ( FAILED ( hr = ptr->m_pVB->Lock ( ptr->m_dwBase * sizeof ( POINTVERTEX ), ptr->m_dwFlush * sizeof(POINTVERTEX),
						( VOID **) &pVertices, ptr->m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
					{
						return;
					}

					while ( pParticle )
					{
						D3DXVECTOR3 vPos(pParticle->m_vPos);
						D3DXVECTOR3 vVel(pParticle->m_vVel);
						FLOAT       fLengthSq = D3DXVec3LengthSq(&vVel);
						UINT        dwSteps;

						if( fLengthSq < 1.0f )        dwSteps = 2;
						else if( fLengthSq <  4.00f ) dwSteps = 3;
						else if( fLengthSq <  9.00f ) dwSteps = 4;
						else if( fLengthSq < 12.25f ) dwSteps = 5;
						else if( fLengthSq < 16.00f ) dwSteps = 6;
						else if( fLengthSq < 20.25f ) dwSteps = 7;
						else                          dwSteps = 8;

						vVel *= -0.04f / (FLOAT)dwSteps;

						D3DXCOLOR clrDiffuse;
						D3DXColorLerp(&clrDiffuse, &pParticle->m_clrFade, &pParticle->m_clrDiffuse, pParticle->m_fFade);
						
						
						DWORD dwDiffuse = (DWORD) clrDiffuse;
						
						// Render each particle a bunch of times to get a blurring effect
						for( DWORD i = 0; i < dwSteps; i++ )
						{
							pVertices->v     = vPos;
							pVertices->color = dwDiffuse;
							pVertices++;

							if( ++dwNumParticlesToRender == ptr->m_dwFlush )
							{
								// Done filling this chunk of the vertex buffer.  Lets unlock and
								// draw this portion so we can begin filling the next chunk.

								ptr->m_pVB->Unlock();

								
								if(FAILED(hr = ptr->pD3D->DrawPrimitive( D3DPT_POINTLIST, ptr->m_dwBase, dwNumParticlesToRender)))
									return;

								// Lock the next chunk of the vertex buffer.  If we are at the 
								// end of the vertex buffer, DISCARD the vertex buffer and start
								// at the beginning.  Otherwise, specify NOOVERWRITE, so we can
								// continue filling the VB while the previous chunk is drawing.
								ptr->m_dwBase += ptr->m_dwFlush;

								if( ptr->m_dwBase >= ptr->m_dwDiscard)
									ptr->m_dwBase = 0;

								if(FAILED(hr = ptr->m_pVB->Lock ( ptr->m_dwBase * sizeof(POINTVERTEX), ptr->m_dwFlush * sizeof(POINTVERTEX),
									(VOID **) &pVertices, ptr->m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
								{
									return;
								}

								dwNumParticlesToRender = 0;
							}

							vPos += vVel;
						}

						pParticle = pParticle->m_pNext;
					}

					ptr->m_pVB->Unlock();

					// Render any remaining particles
					if( dwNumParticlesToRender )
					{
						//ptr->pD3D->SetTransform ( D3DTS_WORLD, &ptr->matObject );
						if(FAILED(hr = ptr->pD3D->DrawPrimitive( D3DPT_POINTLIST, ptr->m_dwBase, dwNumParticlesToRender )))
							return;
					}

					// Reset render states
					ptr->pD3D->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
					ptr->pD3D->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

					//////////////////
					// end render ////
					//////////////////
				}
			}

			// Special particles active even if invisible
			if ( ptr->m_bEffect )
			{
				float fTime = 0.005f;
				ptr->m_pSnow->Update ( fTime, (int)ptr->dwNumParticlesToEmit );
			}
		}
		
		check = check->next;
	}
}

//-----------------------------------------------------------------------------
// Name: DXUtil_Timer()
// Desc: Performs timer opertations. Use the following commands:
//          TIMER_RESET           - to reset the timer
//          TIMER_START           - to start the timer
//          TIMER_STOP            - to stop (or pause) the timer
//          TIMER_ADVANCE         - to advance the timer by 0.1 seconds
//          TIMER_GETABSOLUTETIME - to get the absolute system time
//          TIMER_GETAPPTIME      - to get the current time
//          TIMER_GETELAPSEDTIME  - to get the time that elapsed between 
//                                  TIMER_GETELAPSEDTIME calls
//-----------------------------------------------------------------------------
FLOAT __stdcall DXUtil_Timer( TIMER_COMMAND command )
{
    static BOOL     m_bTimerInitialized = FALSE;
    static BOOL     m_bUsingQPF         = FALSE;
    static LONGLONG m_llQPFTicksPerSec  = 0;

    // Initialize the timer
    if( FALSE == m_bTimerInitialized )
    {
        m_bTimerInitialized = TRUE;

        // Use QueryPerformanceFrequency() to get frequency of timer.  If QPF is
        // not supported, we will timeGetTime() which returns milliseconds.
        LARGE_INTEGER qwTicksPerSec;
        m_bUsingQPF = QueryPerformanceFrequency( &qwTicksPerSec );
        if( m_bUsingQPF )
            m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
    }

    if( m_bUsingQPF )
    {
        static LONGLONG m_llStopTime        = 0;
        static LONGLONG m_llLastElapsedTime = 0;
        static LONGLONG m_llBaseTime        = 0;
        double fTime;
        double fElapsedTime;
        LARGE_INTEGER qwTime;
        
        // Get either the current time or the stop time, depending
        // on whether we're stopped and what command was sent
        if( m_llStopTime != 0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter( &qwTime );

        // Return the elapsed time
        if( command == TIMER_GETELAPSEDTIME )
        {
            fElapsedTime = (double) ( qwTime.QuadPart - m_llLastElapsedTime ) / (double) m_llQPFTicksPerSec;
            m_llLastElapsedTime = qwTime.QuadPart;
            return (FLOAT) fElapsedTime;
        }
    
        // Return the current time
        if( command == TIMER_GETAPPTIME )
        {
            double fAppTime = (double) ( qwTime.QuadPart - m_llBaseTime ) / (double) m_llQPFTicksPerSec;
            return (FLOAT) fAppTime;
        }
    
        // Reset the timer
        if( command == TIMER_RESET )
        {
            m_llBaseTime        = qwTime.QuadPart;
            m_llLastElapsedTime = qwTime.QuadPart;
            return 0.0f;
        }
    
        // Start the timer
        if( command == TIMER_START )
        {
            m_llBaseTime += qwTime.QuadPart - m_llStopTime;
            m_llStopTime = 0;
            m_llLastElapsedTime = qwTime.QuadPart;
            return 0.0f;
        }
    
        // Stop the timer
        if( command == TIMER_STOP )
        {
            m_llStopTime = qwTime.QuadPart;
            m_llLastElapsedTime = qwTime.QuadPart;
            return 0.0f;
        }
    
        // Advance the timer by 1/10th second
        if( command == TIMER_ADVANCE )
        {
            m_llStopTime += m_llQPFTicksPerSec/10;
            return 0.0f;
        }

        if( command == TIMER_GETABSOLUTETIME )
        {
            fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;
            return (FLOAT) fTime;
        }

        return -1.0f; // Invalid command specified
    }
    else
    {
        // Get the time using timeGetTime()
        static double m_fLastElapsedTime  = 0.0;
        static double m_fBaseTime         = 0.0;
        static double m_fStopTime         = 0.0;
        double fTime;
        double fElapsedTime;
        
        // Get either the current time or the stop time, depending
        // on whether we're stopped and what command was sent
        if( m_fStopTime != 0.0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
            fTime = m_fStopTime;
        else
            fTime = timeGetTime() * 0.001;
    
        // Return the elapsed time
        if( command == TIMER_GETELAPSEDTIME )
        {   
            fElapsedTime = (double) (fTime - m_fLastElapsedTime);
            m_fLastElapsedTime = fTime;
            return (FLOAT) fElapsedTime;
        }
    
        // Return the current time
        if( command == TIMER_GETAPPTIME )
        {
            return (FLOAT) (fTime - m_fBaseTime);
        }
    
        // Reset the timer
        if( command == TIMER_RESET )
        {
            m_fBaseTime         = fTime;
            m_fLastElapsedTime  = fTime;
            return 0.0f;
        }
    
        // Start the timer
        if( command == TIMER_START )
        {
            m_fBaseTime += fTime - m_fStopTime;
            m_fStopTime = 0.0f;
            m_fLastElapsedTime  = fTime;
            return 0.0f;
        }
    
        // Stop the timer
        if( command == TIMER_STOP )
        {
            m_fStopTime = fTime;
            return 0.0f;
        }
    
        // Advance the timer by 1/10th second
        if( command == TIMER_ADVANCE )
        {
            m_fStopTime += 0.1f;
            return 0.0f;
        }

        if( command == TIMER_GETABSOLUTETIME )
        {
            return (FLOAT) fTime;
        }

        return -1.0f; // Invalid command specified
    }
}
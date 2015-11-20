#ifndef _CPARTICLEDATA_H_
#define _CPARTICLEDATA_H_

#include "cpositionc.h"
#include "csnowc.h"

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>   
#include <windowsx.h>

// vertex structure for particles
struct POINTVERTEX
{
	D3DXVECTOR3		v;			// vector
	D3DCOLOR		color;		// colour
};

// flexible vertex format
#define D3DFVF_POINTVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE )

// particle data
struct PARTICLE
{
	BOOL			m_bSpark;     // use sparks?
	
	D3DXVECTOR3		m_vPos;       // current position
	D3DXVECTOR3		m_vVel;       // current velocity

	D3DXVECTOR3		m_vPos0;      // initial position
	D3DXVECTOR3		m_vVel0;      // initial velocity
	FLOAT			m_fTime0;     // time of creation

	D3DXCOLOR		m_clrDiffuse; // initial diffuse color
	D3DXCOLOR		m_clrFade;    // faded diffuse color
	FLOAT			m_fFade;      // fade progression

	PARTICLE*		m_pNext;      // next particle in list
};

// object data
struct tagParticleData : public tagObjectPos
{
	LPDIRECT3DDEVICE9		pD3D;				// pointer to d3d device

	FLOAT					m_fRadius;			// radius

	DWORD					m_dwBase;			// base
	DWORD					m_dwFlush;			// flush
	DWORD					m_dwDiscard;		// discard

	DWORD					m_dwParticles;		// num of particles
	DWORD					m_dwParticlesLim;	// max num of particles
	PARTICLE*				m_pParticles;		// pointer to particle data
	PARTICLE*				m_pParticlesFree;	// particles

	LPDIRECT3DVERTEXBUFFER9 m_pVB;				// vertex buffer

	LPDIRECT3DTEXTURE9		m_pTexture;

	float					fSecsPerFrame;
	DWORD					dwNumParticlesToEmit;
	float					fEmitVel;
	D3DXVECTOR3				vPosition;
	D3DXCOLOR				EmitColor;

	float					fTime;

	bool					m_bEffect;
	CSnowstorm*				m_pSnow;

	bool					m_bVisible;
	float					m_fGravity;
	int						m_iLife;
	int						m_iFloorActive;
	float					m_fChaos;

	bool					m_bGhost;
	int						m_iGhostMode;
	float					m_fGhostPercentage;
};


#endif _CPARTICLEDATA_H_
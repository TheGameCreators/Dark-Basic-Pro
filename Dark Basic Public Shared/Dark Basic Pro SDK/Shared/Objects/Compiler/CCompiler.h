#ifndef _CCOMPILER_H_
#define _CCOMPILER_H_

#include "Common.h"
#include "CompilerTypes.h"
#include <vector>
#include ".\..\..\DBOFormat\DBOFormat.h"

typedef std::vector < cMesh*     >  vectorMesh;
typedef std::vector < cTexture*  >  vectorTexture;
typedef std::vector < cMaterial* >  vectorMaterial;

enum COMPILESTATUS
{
    CS_IDLE         = 0,
    CS_INPROGRESS   = 1,
    CS_PAUSED       = 2,
    CS_CANCELLED    = 3
};


class cCompiler
{
	// geometry compiler

	public:
		// vertex structure
		struct sVertex
		{
			float x, y, z;
			float nx, ny, nz;
			float tu, tv;
		};

		// member variables
		vectorMesh      m_vpMeshList;			// mesh list
		vectorTexture   m_vpTextureList;		// texture list
		vectorMaterial  m_vpMaterialList;		// material list
		sHSR			m_OptionsHSR;			// hidden surface removal options
		sESR			m_OptionsESR;			// exterior surface removal options
		sBSP			m_OptionsBSP;			// binary space parition options
		sPRT			m_OptionsPRT;			// portal options
		sPVS		    m_OptionsPVS;			// potential visibility set options
		sTJR			m_OptionsTJR;			// t junction removal options
		sGeneralStats	m_StatsGeneral;			// general stats
		sHSRStats       m_StatsHSR;				// hsr stats
		sESRStats       m_StatsESR;				// esr stats
		sBSPStats       m_StatsBSP;				// bsp stats
		sPRTStats       m_StatsPRT;				// portal stats
		sPVSStats       m_StatsPVS;				// pvs stats
		sTJRStats       m_StatsTJR;				// tjr stats
		COMPILESTATUS	m_Status;				// current status
		ULONG           m_CurrentLog;			// current position of compile
		cBSPTree*		m_pBSPTree;				// the bsp tree
		sVertex*		m_pVertices;			// final vertex list
		int				m_iVertexCount;			// number of vertices
	
	public:
				 cCompiler ( );
		virtual ~cCompiler ( );

		bool CompileScene     ( sMesh* pMeshA, D3DXMATRIX* matA, sMesh* pMeshB, D3DXMATRIX* matB );	// compile 2 objects
		void Release          ( void );																// clean up
		void SetOptions       ( int iProcess, void* pOptions );										// set compile options
		void GetOptions       ( int iProcess, void* pvOptions );									// get options
		void GetStatistics    ( int iProcess, void* pvStats );										// get stats
		bool PerformHSR		  ( void );																// hidden surface removal
		bool PerformESR		  ( void );																// exterior surface removal
		bool PerformBSP		  ( void );																// binary space partition compilation
		bool PerformPRT		  ( void );																// portal compilation
		bool PerformPVS		  ( void );																// potential visibility set
		bool PerformTJR		  ( void );																// t junction repair
		bool Clip             ( cBSPTree* pTree, bool bFlag );										// clip a tree
		void BuildPolygonList ( bool bMode );														// build final polygon list
};

#endif


#ifndef _PROCESSHSR_H_
#define _PROCESSHSR_H_

#include <vector>
#include "CompilerTypes.h"
#include "Common.h"

class cBSPTree;
class cMesh;
class cCompiler;

#define HSR_ARRAY_THRESHOLD     100

typedef std::vector < cMesh*    >  vectorMesh;
typedef std::vector < cBSPTree* >  vectorBSPTree;

class cProcessHSR
{
	public:
				 cProcessHSR ( );
		virtual ~cProcessHSR ( );

		void            SetOptions ( sHSR& Options );
		void            SetParent  ( cCompiler* pParent );
		void            SetStats   ( sHSRStats* pStats  );

		bool            AddMesh ( cMesh* pMesh );
		cMesh*			GetResultMesh ( void );
		HRESULT         Process ( void );
		void            Release ( void );

	private:
		vectorMesh      m_vpMeshList;       // a list of all meshes for HSR Processing
		cMesh*			m_pResultMesh;      // the single mesh result after HSR processing

		sHSR		    m_OptionSet;        // the option set for HSR Processing
		cCompiler*		m_pParent;          // parent compiler pointer
		sHSRStats*		m_pStats;           // statistics information to fill out
};

#endif

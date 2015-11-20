// Includes
#include "windows.h"
#include "D3DX9.h"

class CColBox
{
	public:
		CColBox(float x1,float y1,float z1,float x2,float y2,float z2);
		~CColBox();

		void				Add(CColBox* pColBox);
		void				SetNext(CColBox* pNext);
		void				SetPrev(CColBox* pPrev);
		CColBox*			GetNext(void);
		CColBox*			GetPrev(void);

	private:
		CColBox*			m_pNext;
		CColBox*			m_pPrev;

	public:
		float				m_X1;
		float				m_Y1;
		float				m_Z1;
		float				m_X2;
		float				m_Y2;
		float				m_Z2;
};

class CPolyPool
{
	public:
		CPolyPool();
		~CPolyPool();
		void ReleasePool(void);

	public:
		int AddColBox(float x1, float y1, float z1, float x2, float y2, float z2);
		int CheckColBox(float ax1, float ay1, float az1, float ax2, float ay2, float az2, float x1, float y1, float z1, float x2, float y2, float z2, D3DXVECTOR3* vResult);
		int CheckColLine(float sx, float sy, float sz, float dx, float dy, float dz, float width, float acc, D3DXVECTOR3* vResult);

	private:
		CColBox*						m_pColBoxes;
};

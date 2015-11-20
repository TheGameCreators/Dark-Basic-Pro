// Includes
#include "PolyPool.h"

//
// Implementations for POLYPOOL
//

CPolyPool::CPolyPool()
{
	m_pColBoxes = NULL;
}

CPolyPool::~CPolyPool()
{
	// Delete clumps and releae buffers
	ReleasePool();
}

void CPolyPool::ReleasePool(void)
{
	// Delete any collision boxes
	if(m_pColBoxes)
	{
		delete m_pColBoxes;
		m_pColBoxes = NULL;
	}
}

//
// Implementations for COLLISION part of POLYPOOL
//

int CPolyPool::AddColBox(float x1, float y1, float z1, float x2, float y2, float z2)
{
	CColBox* pColBox = new CColBox(x1,y1,z1,x2,y2,z2);
	if(m_pColBoxes)
		m_pColBoxes->Add(pColBox);
	else
		m_pColBoxes=pColBox;

	return 1;
}

int CPolyPool::CheckColBox(	float ax1, float ay1, float az1, float ax2, float ay2, float az2, 
							float x1, float y1, float z1, float x2, float y2, float z2, D3DXVECTOR3* vResult)
{
	// Check against boxes in colpool
	vResult->x=0;
	vResult->y=0;
	vResult->z=0;
	int collision=0;
	CColBox* pColBox = m_pColBoxes;
	while(pColBox)
	{
		if(x1<=pColBox->m_X2)
			if(z1<=pColBox->m_Z2)
				if(x2>=pColBox->m_X1)
					if(z2>=pColBox->m_Z1)
						if(y1<=pColBox->m_Y2)
							if(y2>=pColBox->m_Y1)
							{
								short side=0;
								float distx1 = (float)fabs(ax2-pColBox->m_X1);
								float distx2 = (float)fabs(pColBox->m_X2-ax1);
								float disty1 = (float)fabs(ay2-pColBox->m_Y1);
								float disty2 = (float)fabs(pColBox->m_Y2-ay1);
								float distz1 = (float)fabs(az2-pColBox->m_Z1);
								float distz2 = (float)fabs(pColBox->m_Z2-az1);
								float dist = distx1;
								if(distx2<dist) { side=1; dist=distx2; }
								if(disty1<dist) { side=2; dist=disty1; }
								if(disty2<dist) { side=3; dist=disty2; }
								if(distz1<dist) { side=4; dist=distz1; }
								if(distz2<dist) { side=5; dist=distz2; }
								switch(side)
								{
									case 0 : vResult->x = x2-pColBox->m_X1; break;
									case 1 : vResult->x = x1-pColBox->m_X2; break;
									case 2 : vResult->y = y2-pColBox->m_Y1; break;
									case 3 : vResult->y = y1-pColBox->m_Y2; break;
									case 4 : vResult->z = z2-pColBox->m_Z1; break;
									case 5 : vResult->z = z1-pColBox->m_Z2; break;
								}
								collision=1;
							}
		pColBox = pColBox->GetNext();
	}
	return collision;
}

int CPolyPool::CheckColLine(float sx, float sy, float sz, float dx, float dy, float dz, float width, float acc, D3DXVECTOR3* vResult)
{
	// Create traveller for line
	float mx = sx;
	float my = sy;
	float mz = sz;
	float distx = (float)fabs(dx-sx);
	float distz = (float)fabs(dz-sz);
	float ix, iy, iz;
	int mdistance;
	if(distx>distz)
	{
		ix = (dx-sx) / distx;
		iy = (dy-sy) / distx;
		iz = (dz-sz) / distx;
		mdistance=(int)distx;
	}
	else
	{
		ix = (dx-sx) / distz;
		iy = (dy-sy) / distz;
		iz = (dz-sz) / distz;
		mdistance=(int)distz;
	}

	// Check LINE against boxes in colpool
	vResult->x=0;
	vResult->y=0;
	vResult->z=0;
	int collision=0;
	int collisiondistance[2];
	float collisionhitmx[2];
	float collisionhitmy[2];
	float collisionhitmz[2];
	collisiondistance[0]=0;
	collisiondistance[1]=-1;
	collisionhitmx[1]=sx;
	collisionhitmy[1]=sy;
	collisionhitmz[1]=sz;
	CColBox* pColBox = m_pColBoxes;
	while(pColBox)
	{
		mx = sx;
		my = sy;
		mz = sz;
		if(acc<=0.0f) acc=1.0f;
		int md = (int)(mdistance/acc);
		bool bFirstInsideIsOk=true;
		while(md>0)
		{
			// Travel checker
			mx += ix;
			my += iy;
			mz += iz;
			md--;

			// Check against box (ony boxes below destination 'camera' point)
			if(mx-width<=pColBox->m_X2)
			{
				if(my-width<=pColBox->m_Y2)
					if(mz-width<=pColBox->m_Z2)
						if(mx+width>=pColBox->m_X1)
							if(my+width>=pColBox->m_Y1)
								if(mz+width>=pColBox->m_Z1)
								{
									if(bFirstInsideIsOk==true)
									{
										collisiondistance[0]=md;
										collisionhitmx[0]=mx-ix;
										collisionhitmy[0]=my-iy;
										collisionhitmz[0]=mz-iz;
									}
									else
									{
										if(md>collisiondistance[1])
										{
											collisiondistance[1]=md;
											collisionhitmx[1]=mx-ix;
											collisionhitmy[1]=my-iy;
											collisionhitmz[1]=mz-iz;
											collision=1;
										}
									}
									break;
								}
			}
			if(bFirstInsideIsOk==true) bFirstInsideIsOk=false;
		}

		// Next box..
		pColBox = pColBox->GetNext();
	}

	if(collisiondistance[1]!=-1 && collisiondistance[0]>collisiondistance[1])
	{
		// If first insider-hit closer than further away one, just closer one
		vResult->x = collisionhitmx[0];
		vResult->y = collisionhitmy[0];
		vResult->z = collisionhitmz[0];
	}
	else
	{
		vResult->x = collisionhitmx[1];
		vResult->y = collisionhitmy[1];
		vResult->z = collisionhitmz[1];
	}
	return collision;
}

//
// Implementations for COLBOX
//

CColBox::CColBox(float x1, float y1, float z1, float x2, float y2, float z2)
{
	m_pNext = NULL;
	m_pPrev = NULL;
	m_X1 = x1;
	m_Y1 = y1;
	m_Z1 = z1;
	m_X2 = x2;
	m_Y2 = y2;
	m_Z2 = z2;
}

CColBox::~CColBox()
{
	if(GetNext())
	{
		delete GetNext();
		SetNext(NULL);
	}
}

void CColBox::Add(CColBox* pColBox)
{
	if(GetNext()==NULL)
		SetNext(pColBox);
	else
		GetNext()->Add(pColBox);
}

void CColBox::SetNext(CColBox* pNext) { m_pNext = pNext; }
void CColBox::SetPrev(CColBox* pPrev) { m_pPrev = pPrev; }
CColBox* CColBox::GetNext(void) { return m_pNext; }
CColBox* CColBox::GetPrev(void) { return m_pPrev; }

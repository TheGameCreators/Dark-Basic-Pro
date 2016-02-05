#ifndef COLTREE_H
#define COLTREE_H

//#include "dllmain.h"
#include "Point.h"
#include "Vector.h"
//#include "TreeFace.h"
//#include "hMatrix.h"

#include "LMGlobal.h"
#include "Lumel.h"

class TreeFace;
class TreeNode;

class CollisionTree
{

    public:

		//static Vector vec, vecI;
		//static Point p;
        
        CollisionTree();
        ~CollisionTree();

		void Reset( );
        
        void setFacesPerNode(int num) { if (num<2) facesPerNode = 2; else facesPerNode = num; }        
        void makeCollisionObject(unsigned vnum, float* vertices);
		void makeCollisionObject(int fnum, TreeFace *pFaceList);

		//void SetVector( float x, float y, float z );
		//void SetVector( Vector *v );
		//void SetPoint( float x, float y, float z );
        
        bool intersects( const Point* p, const Vector* vec, const Vector* vecI, Lumel *pColour, float* pShadow, TreeFace** ppLastHit ) const;
        //TreeNode* getRoot() { return tree; }
              
    private:
        
        int facesPerNode;
        TreeNode* tree;
};

#endif

#include "CollisionTree.h"
#include "TreeFace.h"

#include <math.h>

//Vector CollisionTree::vec, CollisionTree::vecI;
//Point CollisionTree::p;

CollisionTree::CollisionTree( )
{
	facesPerNode = 2;
	tree = 0;
}

CollisionTree::~CollisionTree() 
{ 
	delete tree; 
}

void CollisionTree::Reset( )
{
	delete tree;
	tree = 0;
}

void CollisionTree::makeCollisionObject(unsigned vnum, float* vertices)
{
    //build tree 
    
    int j;
    unsigned fnum = vnum/3;
    Point p1,p2,p3;
    float length;
    TreeFace* faces = 0;
            
    for(int i=0; i<(int)fnum; i++) 
    {
        j=i*9;
        
        //construct a linked list of faces
        TreeFace* aFace = new TreeFace();
        
        p1.set(*(vertices+j),*(vertices+j+1),*(vertices+j+2));
        p2.set(*(vertices+j+3),*(vertices+j+4),*(vertices+j+5));
        p3.set(*(vertices+j+6),*(vertices+j+7),*(vertices+j+8));
        
        aFace->vert1 = p1;
        aFace->vert2 = p2;
        aFace->vert3 = p3;
        
        Vector v1(&p1,&p3);
        Vector v2(&p1,&p2);
        
        aFace->normal = v1.crossProduct(&v2);
        length = aFace->normal.size();
        
		//zero area polygons will have an undefined normal, which will cause problems later
        //so if any exist remove them
        if (length>0.00001f) { aFace->normal.mult(1.0f/length); }
		else { delete aFace; continue; }
        //else { aFace->normal.set(0,1,0); aFace->collisionon=false; }
        
        v1 = aFace->normal;
        v2.set(p2.x,p2.y,p2.z);
        
        aFace->d = -1.0f*(v1.dotProduct(&v2));

		aFace->nextFace = faces;
        faces = aFace;
    }
    
    delete [] vertices;
    
    if (facesPerNode<2) facesPerNode = 2;
    
	if ( tree ) Reset( );

    tree = new TreeNode(); 
    tree->buildTree(faces,fnum,facesPerNode);
}

void CollisionTree::makeCollisionObject( int fnum, TreeFace *pFaceList )
{
	if (facesPerNode<2) facesPerNode = 2;
    
	if ( tree ) Reset( );

    tree = new TreeNode(); 
    tree->buildTree(pFaceList,fnum,facesPerNode);
}

/*
void CollisionTree::SetVector( float x, float y, float z )
{
	vec.set( x,y,z );
	vecI.set( 1/x, 1/y, 1/z );
}

void CollisionTree::SetVector( Vector *v )
{
	vec.set( v->x, v->y, v->z );
	vecI.set( 1/v->x, 1/v->y, 1/v->z );
}

void CollisionTree::SetPoint( float x, float y, float z )
{
	p.set( x,y,z );
}
*/

bool CollisionTree::intersects( const Point* p, const Vector* vec, const Vector* vecI, Lumel* pColour, float* pShadow, TreeFace** ppLastHit ) const
{    
	if ( !tree ) return false;
    return (tree->intersects(p,vec,vecI,pColour,pShadow,ppLastHit));
}
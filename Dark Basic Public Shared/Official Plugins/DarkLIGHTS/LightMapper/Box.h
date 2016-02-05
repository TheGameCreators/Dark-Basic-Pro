#ifndef BOX_H
#define BOX_H

#define L 1000000000
#define S -1000000000

#include "Point.h"
#include "Vector.h"

class Box
{

    public:
    
        //Point lastNormal;
        
        Box(Point* min, Point* max);
        Box(float x=L,float y=L,float z=L,float x2=S,float y2=S,float z2=S);
        ~Box() {}
        
        void resetBox();        
        void set(Point* min, Point* max);
        void set(float x,float y,float z,float x2,float y2,float z2);
        
        void correctBox();        
        
        bool inBox(Point* p) const;        
        int intersectBox(const Point* p, const Vector* vi) const;
                
        float maxbx() const { return maxx; }
        float maxby() const { return maxy; }
        float maxbz() const { return maxz; }
        float minbx() const { return minx; }
        float minby() const { return miny; }
        float minbz() const { return minz; }
        
        void maxbx(float x) { maxx = x; }
        void maxby(float y) { maxy = y; }
        void maxbz(float z) { maxz = z; }
        void minbx(float x) { minx = x; }
        void minby(float y) { miny = y; }
        void minbz(float z) { minz = z; }
    
    private:
    
        float maxx,maxy,maxz,minx,miny,minz;
};



inline Box::Box(Point* min, Point* max)
{
    minx = min->x; miny = min->y; minz = min->z;
    maxx = max->x; maxy = max->y; maxz = max->z;
    correctBox();
}

inline Box::Box(float x,float y,float z,float x2,float y2,float z2)
{
    minx = x; miny = y; minz = z;
    maxx = x2; maxy = y2; maxz = z2;
    correctBox();
}

inline void Box::resetBox()
{
    minx = L; miny = L; minz = L;
    maxx = S; maxy = S; maxz = S;
}

inline void Box::set(Point* min, Point* max)
{
    minx = min->x; miny = min->y; minz = min->z;
    maxx = max->x; maxy = max->y; maxz = max->z;
}            

inline void Box::set(float x,float y,float z,float x2,float y2,float z2)
{
    minx = x; miny = y; minz = z;
    maxx = x2; maxy = y2; maxz = z2;
}

inline void Box::correctBox() 
{ 
    minx=minx-0.0001f; miny=miny-0.0001f; minz=minz-0.0001f; 
    maxx=maxx+0.0001f; maxy=maxy+0.0001f; maxz=maxz+0.0001f;
}

inline bool Box::inBox(Point* p) const { 
    if (p->x<minx || p->x>maxx || p->y<miny || p->y>maxy || p->z<minz || p->z>maxz) return false;         
    return true;
}

inline int Box::intersectBox(const Point* p, const Vector* vi) const
{        
    float tminx,tmaxx,tminy,tmaxy;
    int side=0;           

    if (vi->x>=0) {
        tminx = (minx - p->x) *vi->x;
        tmaxx = (maxx - p->x) *vi->x;
    }
    else {
        tmaxx = (minx - p->x) *vi->x;
        tminx = (maxx - p->x) *vi->x;
    }
    
    if (vi->y>=0) {
        tminy = (miny - p->y) *vi->y;
        tmaxy = (maxy - p->y) *vi->y;
    }
    else {
        tmaxy = (miny - p->y) *vi->y;
        tminy = (maxy - p->y) *vi->y;
    }
               
    if ((tminx > tmaxy) || (tminy > tmaxx)) return 0;
    
    if (tminy>tminx) tminx = tminy;
    if (tmaxy<tmaxx) tmaxx = tmaxy;
    
    if (vi->z>=0) {
        tminy = (minz - p->z) *vi->z;
        tmaxy = (maxz - p->z) *vi->z;
    }
    else {
        tmaxy = (minz - p->z) *vi->z;
        tminy = (maxz - p->z) *vi->z;
    }
    
    if ((tminx > tmaxy) || (tminy > tmaxx)) return 0;
                                      
    if (tminy>tminx) tminx = tminy;
    if (tmaxy<tmaxx) tmaxx = tmaxy;
    
    if (tmaxx<0.0) return 0;
    if (tminx>1.0) return 0;
    
    return 1;
}

#endif

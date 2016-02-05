#ifndef VECTOR_H
#define VECTOR_H

#include "Point.h"
#include <math.h>

class Vector 
{
    public:
        
        Vector(float nx=0.0, float ny=0.0, float nz=0.0) { x=nx; y=ny; z=nz; }        
        Vector(const Point* p1, const Point* p2) { x = p2->x-p1->x; y = p2->y-p1->y; z = p2->z-p1->z; }
        
        void set(float nx, float ny, float nz) { x=nx; y=ny; z=nz; }        
        void set(const Point* p1, const Point* p2) { x = p2->x-p1->x; y = p2->y-p1->y; z = p2->z-p1->z; }
        
        float size() const { return sqrt(x*x + y*y + z*z); }
        float sqrSize() const { return x*x + y*y + z*z; }
        void normalise();

        void mult(float s) { x=x*s; y=y*s; z=z*s; }        
        void scale(float sx, float sy, float sz) { x=x*sx; y=y*sy; z=z*sz; }

        float dotProduct(const Vector* v2) const { return (x*v2->x + y*v2->y + z*v2->z); }
        float dotProduct(float x2, float y2, float z2) const { return (x*x2 + y*y2 + z*z2); }        
        Vector crossProduct(const Vector* v2) const;
        Vector crossProduct(float x2, float y2, float z2) const;
        
        float findAngle(const Vector* v2) const;        
        float findNormalisedAngle(const Vector* v2) const;
        
        void flattenToCrossVector(const Vector* v1, const Vector* v2);
        
        Point closestPoint(const Point* start, const Point* object) const;        
        Point closestNormalisedPoint(const Point* start, const Point* finish, const Point* object) const;

        float x,y,z,length;
        
    private:
};

inline void Vector::normalise()
{
    float temp = size();
    if (temp!=0) {
        x = x/temp;
        y = y/temp;
        z = z/temp;
    }
}

inline Vector Vector::crossProduct(const Vector* v2) const
{
    float vx,vy,vz;
   
    vx = v2->y*z - v2->z*y;
    vy = v2->z*x - v2->x*z;
    vz = v2->x*y - v2->y*x;
    
    return Vector(vx,vy,vz);
}

inline Vector Vector::crossProduct(float x2, float y2, float z2) const
{
    float vx,vy,vz;
   
    vx = y2*z - z2*y;
    vy = z2*x - x2*z;
    vz = x2*y - y2*x;
    
    return Vector(vx,vy,vz);
}

inline float Vector::findAngle(const Vector* v2) const
{
    return acos(dotProduct(v2)/(size()*v2->size()));
}

inline float Vector::findNormalisedAngle(const Vector* v2) const
{
    float temp = (x*v2->x + y*v2->y + z*v2->z);
    if (temp>1.0) temp=1.0;
    if (temp<-1.0) temp=-1.0;
    return acos(temp);
}

inline void Vector::flattenToCrossVector(const Vector* v1, const Vector* v2)
{
    Vector cross = v1->crossProduct(v2);
    float dist = cross.x*cross.x + cross.y*cross.y + cross.z*cross.z;
    if (dist>0.00001) dist = cross.dotProduct(x,y,z)/dist;
    else dist = 0;
    x = cross.x*dist;
    y = cross.y*dist;
    z = cross.z*dist;
}

inline Point Vector::closestPoint(const Point* start, const Point* object) const
{
    Vector v2(object->x-start->x,object->y-start->y,object->z-start->z);
    //Vector v3(x,y,z);
    //v3.normalise();
    float sqrLen = x*x + y*y + z*z;
    
    float dist;
    if (sqrLen>0.00001) dist = (x*v2.x + y*v2.y + z*v2.z)/sqrLen;
    else dist = 0;
    
    Point result;
    
    if (dist<=0) {
        result = *start;
        return result;
    }
    if (dist>=1) {
        result.x = start->x + x;
        result.y = start->y + y;
        result.z = start->z + z;
        return result;
    }
    
    result.x = start->x + x*dist;
    result.y = start->y + y*dist;
    result.z = start->z + z*dist;
    
    return result;
}

inline Point Vector::closestNormalisedPoint(const Point* start, const Point* finish, const Point* object) const
{
    Vector v2(object->x-start->x,object->y-start->y,object->z-start->z);
    float dist = x*v2.x + y*v2.y + z*v2.z;
    Point result;
    
    if (dist<=0) {
        result = *start;
        return result;
    }
    if ((dist*dist)>=start->getDist(finish,0)) {
        result = *finish;
        return result;
    }
    
    result.x = start->x + x*dist;
    result.y = start->y + y*dist;
    result.z = start->z + z*dist;
    
    return result;
}

#endif

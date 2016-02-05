#ifndef POINT_H
#define POINT_H

#include <math.h>

class Point 
{
    public:
        
        float x,y,z;
        
        Point(float x2=0.0, float y2=0.0, float z2=0.0) { x=x2; y=y2; z=z2; }

        void set(float x2, float y2, float z2) { x=x2; y=y2; z=z2; } 
        void set(const Point* p2) { x=p2->x; y=p2->y; z=p2->z; }
        void scale(float sx, float sy, float sz) { x=x*sx; y=y*sy; z=z*sz; }
        
        float getDist(float x, float y, float z, int accuracy) const;
        float getDist(const Point* p2, int accuracy) const;
        
        void addVec(float vx, float vy, float vz) { x+=vx; y+=vy; z+=vz; }

    private:       
};

inline float Point::getDist(const Point* p2, int accuracy) const
{
    float temp = (x-p2->x)*(x-p2->x) + (y-p2->y)*(y-p2->y) + (z-p2->z)*(z-p2->z);
            
    if (accuracy==1) temp = sqrt(temp);
            
    return temp;
}

inline float Point::getDist(float x2, float y2, float z2, int accuracy) const
{
    float temp = (x-x2)*(x-x2) + (y-y2)*(y-y2) + (z-z2)*(z-z2);
            
    if (accuracy==1) temp = sqrt(temp);
            
    return temp;
}


#endif

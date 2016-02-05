#ifndef DBPROPHYSICSMASTER_CLOTH_BASIC_H
#define DBPROPHYSICSMASTER_CLOTH_BASIC_H

class ClothBitmapElement
{
public:
	ClothBitmapElement():numParticles(4),seg(0){}
	int numParticles;
	ClothSegment * seg;
};

class Cloth_Basic : public Cloth
{
public:
	Cloth_Basic(int id) : Cloth(id){}

	ePhysError generateRectangularCloth(float sx, float sz, int resx, int resz, bool doubleSided);
	ePhysError generateClothFromBitmap(int bitmapID, float sx, float sz, bool autoTriangles, bool doubleSided);

	virtual int childClassID(){return classID;}
	//static const int classID='CBSC';
	 //int classID;
	static const int classID;
};

CLOTH_PLUGIN_INTERFACE(Cloth_Basic);


#endif
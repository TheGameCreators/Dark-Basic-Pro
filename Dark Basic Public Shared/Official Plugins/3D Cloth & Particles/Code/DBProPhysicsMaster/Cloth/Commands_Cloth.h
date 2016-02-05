#ifndef DBPROEMITTER_COMMANDS_CLOTH_H
#define DBPROEMITTER_COMMANDS_CLOTH_H

//Base class
DLLEXPORT void deleteCloth(int id);
DLLEXPORT void freezeCloth(int id, DWORD state);
DLLEXPORT void setClothElasticity(int id, float elast);
DLLEXPORT void setClothMass(int id, float mass);
DLLEXPORT void fixClothPoint(int id, int pntNo);
DLLEXPORT void fixClothPoints(int id, float px, float py, float pz, float radius);
DLLEXPORT void fixClothPoints(int id, float px, float py, float pz, float sx, float sy, float sz);
DLLEXPORT void fixClothNearestPoint(int id, float px, float py, float pz);
DLLEXPORT void freeClothPoint(int id, int pntNo);
DLLEXPORT void freeClothPoints(int id, float px, float py, float pz, float radius);
DLLEXPORT void freeClothPoints(int id, float px, float py, float pz, float sx, float sy, float sz);
DLLEXPORT void freeClothNearestPoint(int id, float px, float py, float pz);
DLLEXPORT void saveClothState(int id, LPSTR filename);
DLLEXPORT void loadClothState(int id, LPSTR filename, DWORD useVelocities);
DLLEXPORT void fixClothPointToObject(int id, int pntNo, int objectID);
DLLEXPORT void fixClothPointToObject(int id, int pntNo, int objectID, float px, float py, float pz);
DLLEXPORT void freeAllClothPoints(int id);
DLLEXPORT void freeAllClothPointsOnObject(int id, int objectID);
DLLEXPORT void freeAllClothPointsOnAllObjects(int id);
DLLEXPORT int getNumberOfClothPoints(int id);
DLLEXPORT void relaxCloth(int id);
DLLEXPORT void TransformClothUVs(int id, float scaleu, float scalev, float moveu, float movev, int mode);
DLLEXPORT void ResetClothUVs(int id, int mode);

//Basic
DLLEXPORT void makeCloth(int id);
DLLEXPORT void generateRectangularCloth(int id, float sx, float sy, int resx, int resy, DWORD useDoubleSided);
DLLEXPORT void generateClothFromBitmap(int id, int bitmapID, float sx, float sy, DWORD autoTriangles, DWORD useDoubleSided);

#endif
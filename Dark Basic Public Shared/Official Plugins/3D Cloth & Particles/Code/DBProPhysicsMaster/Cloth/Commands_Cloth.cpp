#include "stdafx.h"



DLLEXPORT void deleteCloth(int id)
{
	ALERT_ON_PERR(physics->deleteCloth(id),PCMD_DELETE_CLOTH);
}

DLLEXPORT void freezeCloth(int id, DWORD state)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FREEZE_CLOTH);

	if(state) tmp->Freeze();
	else tmp->unFreeze();
}

DLLEXPORT void setClothElasticity(int id, float elast)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_SET_CLOTH_ELASTICITY);

	RETURN_ON_PERR(tmp->setClothElasticity(elast),PCMD_SET_CLOTH_ELASTICITY);
}

DLLEXPORT void setClothMass(int id, float mass)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_SET_CLOTH_MASS);

	RETURN_ON_PERR(tmp->setClothMass(mass),PCMD_SET_CLOTH_MASS);
}

DLLEXPORT void fixClothPoint(int id, int pntNo)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FIX_CLOTH_POINT);

	RETURN_ON_PERR(tmp->fixPoint(pntNo,true),PCMD_FIX_CLOTH_POINT);
}

DLLEXPORT void fixClothPoints(int id, float px, float py, float pz, float radius)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FIX_CLOTH_POINTS);

	RETURN_ON_PERR(tmp->fixPoints(px,py,pz,radius,true),PCMD_FIX_CLOTH_POINTS);
}

DLLEXPORT void fixClothPoints(int id, float px, float py, float pz, float sx, float sy, float sz)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FIX_CLOTH_POINTS);

	RETURN_ON_PERR(tmp->fixPoints(px,py,pz,sx,sy,sz,true),PCMD_FIX_CLOTH_POINTS);
}


DLLEXPORT void fixClothNearestPoint(int id, float px, float py, float pz)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FIX_CLOTH_NEAREST_POINT);

	RETURN_ON_PERR(tmp->fixNearestPoint(px,py,pz,true),PCMD_FIX_CLOTH_NEAREST_POINT);
}

DLLEXPORT void freeClothPoint(int id, int pntNo)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FREE_CLOTH_POINT);

	RETURN_ON_PERR(tmp->fixPoint(pntNo,false),PCMD_FREE_CLOTH_POINT);
}

DLLEXPORT void freeClothPoints(int id, float px, float py, float pz, float radius)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FREE_CLOTH_POINTS);

	RETURN_ON_PERR(tmp->fixPoints(px,py,pz,radius,false),PCMD_FREE_CLOTH_POINTS);
}

DLLEXPORT void freeClothPoints(int id, float px, float py, float pz, float sx, float sy, float sz)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FREE_CLOTH_POINTS);

	RETURN_ON_PERR(tmp->fixPoints(px,py,pz,sx,sy,sz,false),PCMD_FREE_CLOTH_POINTS);
}


DLLEXPORT void freeClothNearestPoint(int id, float px, float py, float pz)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FREE_CLOTH_NEAREST_POINT);

	RETURN_ON_PERR(tmp->fixNearestPoint(px,py,pz,false),PCMD_FREE_CLOTH_NEAREST_POINT);
}


DLLEXPORT void saveClothState(int id, LPSTR filename)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_SAVE_CLOTH_STATE);

	RETURN_ON_PERR(tmp->saveClothState(filename),PCMD_SAVE_CLOTH_STATE);
}

DLLEXPORT void loadClothState(int id, LPSTR filename, DWORD useVelocities)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_LOAD_CLOTH_STATE);

	RETURN_ON_PERR(tmp->loadClothState(filename,(useVelocities>0?true:false)),PCMD_LOAD_CLOTH_STATE);
}


DLLEXPORT void fixClothPointToObject(int id, int pntNo, int objectID)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FIX_CLOTH_POINT_TO_OBJECT);

	RETURN_ON_PERR(tmp->fixPointToObject(pntNo,objectID,0),PCMD_FIX_CLOTH_POINT_TO_OBJECT);
}

DLLEXPORT void fixClothPointToObject(int id, int pntNo, int objectID, float px, float py, float pz)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FIX_CLOTH_POINT_TO_OBJECT);

	Vector3 pos(px,py,pz);
	RETURN_ON_PERR(tmp->fixPointToObject(pntNo,objectID,&pos),PCMD_FIX_CLOTH_POINT_TO_OBJECT);
}


DLLEXPORT void freeAllClothPoints(int id)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FREE_ALL_CLOTH_POINTS);

	tmp->freeAllPoints();
}

DLLEXPORT void freeAllClothPointsOnObject(int id, int objectID)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FREE_ALL_CLOTH_POINTS_ON_OBJECT);

	RETURN_ON_PERR(tmp->freePointsOnObject(objectID),PCMD_FREE_ALL_CLOTH_POINTS_ON_OBJECT);
}

DLLEXPORT void freeAllClothPointsOnAllObjects(int id)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_FREE_ALL_CLOTH_POINTS_ON_ALL_OBJECTS);

	tmp->freePointsOnAllObjects();
}

DLLEXPORT int getNumberOfClothPoints(int id)
{
	ClothPtr tmp;
	RETURNVALUE_ON_PERR(physics->getCloth(id,tmp),PCMD_GET_NUMBER_OF_CLOTH_POINTS,0);

	return tmp->getNumParticles();
}

DLLEXPORT void relaxCloth(int id)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_RELAX_CLOTH);
	tmp->Relax();
}

DLLEXPORT void TransformClothUVs(int id, float scaleu, float scalev, float moveu, float movev, int mode)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_TRANSFORM_CLOTH_UVS);
	RETURN_ON_PERR(tmp->TransformUVs(scaleu,scalev,moveu,movev,mode),PCMD_TRANSFORM_CLOTH_UVS);
}

DLLEXPORT void ResetClothUVs(int id, int mode)
{
	ClothPtr tmp;
	RETURN_ON_PERR(physics->getCloth(id,tmp),PCMD_RESET_CLOTH_UVS);
	RETURN_ON_PERR(tmp->ResetUVs(mode),PCMD_RESET_CLOTH_UVS);
}


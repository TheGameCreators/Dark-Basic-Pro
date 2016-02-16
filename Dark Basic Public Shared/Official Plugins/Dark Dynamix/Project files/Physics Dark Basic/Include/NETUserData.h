#include "UserData.h"
#include "dbodata.h"

class NETUserData :
	public UserData 
{
public:
	NETUserData(void){}
	NETUserData(int id, sObject *obj, D3DXMATRIX &mat){
		object_     =  id;
    	position_   = (NxVec3)obj->position.vecPosition;
		rotation_   = (NxVec3)obj->position.vecRotate;
		scale_      = (NxVec3)obj->position.vecScale;
		dim_        = (NxVec3)(obj->collision.vecMax - obj->collision.vecMin);
		dim_.arrayMultiply(dim_, scale_);
		mat_.setRowMajor44(mat.m);
	}
	~NETUserData(void){}

	void update(NxMat34 &mat){
		//rotate and position object at mat
	}

	void update(NxQuat &quat, NxVec3 &pos){
		D3DXVECTOR3 vec;
		//MathUtil::quaternionToEuler(quat, vec);
		//dbRotateObject(object_, vec.x, vec.y, vec.z);
		//dbPositionObject(object_, pos.x, pos.y, pos.z);
	}
	int object_;
};
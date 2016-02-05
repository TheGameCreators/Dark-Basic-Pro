#ifndef DBPROPHYSICSMASTER_PHYSICSOBJECT_H
#define DBPROPHYSICSMASTER_PHYSICSOBJECT_H

//The base physics object class
//Stores a matrix position
class PhysicsObject
{
	friend class PhysicsManager;

public:
	PhysicsObject(int id);

	virtual ~PhysicsObject();
	
	void Update();
	virtual void onUpdate()=0;	//This gets called every physics iteration

	void UpdateGraphics();
	virtual void onUpdateGraphics()=0;	//This gets called before every render

	//Used for updating parameters that depend on frame rate
	virtual void notifyFrameRateChange(float oldFps, float newFps){} 

	void objectHasGarbage();
	void garbageCollect()
	{
		if(hasGarbage)
		{
			onGarbageCollect();
			hasGarbage=false;
		}
	}
	virtual void onGarbageCollect(){}

	virtual void onRemoveFromUse(){}
	bool isValid(){return inUse;}
	void useExternalMatrix(bool state){useExternMat=state;}

	int getID(){return objId;}

	const Matrix& getMatrix(){return m;}

	ePhysError checkType(int baseClass, int childClass)
	{
		if(baseClass!=baseClassID() || childClass!=childClassID()) return PERR_WRONG_TYPE;
		return PERR_AOK;
	}
	virtual int baseClassID()=0;
	virtual int childClassID()=0;

//protected:
	// mike
	public:
	void getEngineMatrix();
	void setEngineMatrix();

	bool inUse;

	bool hasGarbage;

	int objId;
	Matrix m;

	bool isFirstGFXUpdate;	//Whether to call DLL_SetNewObjectFinalProperties

private:
	void removeFromUse()	//Only want the physicsManager to be able to call this
	{
		if(inUse)
		{
			//Call this once only
			onRemoveFromUse();
		}
		inUse=false;
	}


	bool useExternMat;	//Whether to query DBPro to obtain the matrix or not	
	struct sPositionData * DBProPosition;
};


#endif
#ifndef DBPROPHYSICSMASTER_RAGDOLL_BASIC_H
#define DBPROPHYSICSMASTER_RAGDOLL_BASIC_H

#define RAGBASIC_HEADLEFT 0
#define RAGBASIC_HEADRIGHT 1
#define RAGBASIC_NECK 2
#define RAGBASIC_CHEST 3
#define RAGBASIC_WAISTLEFT 4
#define RAGBASIC_WAISTRIGHT 5
#define RAGBASIC_HANDLEFT 6
#define RAGBASIC_UPELBOWLEFT 7
#define RAGBASIC_LOWELBOWLEFT 8
#define RAGBASIC_BICEPLEFT 9
#define RAGBASIC_TRICEPLEFT 10
#define RAGBASIC_SHOULDERLEFT 11
#define RAGBASIC_HANDRIGHT 12
#define RAGBASIC_UPELBOWRIGHT 13
#define RAGBASIC_LOWELBOWRIGHT 14
#define RAGBASIC_BICEPRIGHT 15
#define RAGBASIC_TRICEPRIGHT 16
#define RAGBASIC_SHOULDERRIGHT 17
#define RAGBASIC_LEGTOPLEFT 18
#define RAGBASIC_QUADLEFT 19
#define RAGBASIC_HAMLEFT 20
#define RAGBASIC_OUTKNEELEFT 21
#define RAGBASIC_INKNEELEFT 22
#define RAGBASIC_FOOTLEFT 23
#define RAGBASIC_LEGTOPRIGHT 24
#define RAGBASIC_QUADRIGHT 25
#define RAGBASIC_HAMRIGHT 26
#define RAGBASIC_OUTKNEERIGHT 27
#define RAGBASIC_INKNEERIGHT 28
#define RAGBASIC_FOOTRIGHT 29

#define RAGBASICPART_HEAD 0
#define RAGBASICPART_UPPERARMLEFT 1
#define RAGBASICPART_LOWERARMLEFT 2
#define RAGBASICPART_UPPERARMRIGHT 3
#define RAGBASICPART_LOWERARMRIGHT 4
#define RAGBASICPART_UPPERBODY 5
#define RAGBASICPART_LOWERBODY 6
#define RAGBASICPART_THIGHLEFT 7
#define RAGBASICPART_SHINLEFT 8
#define RAGBASICPART_THIGHRIGHT 9
#define RAGBASICPART_SHINRIGHT 10

struct objectPartLink
{
	bool flip;
	int pBase;
	int p1,p2;
	bool useTwoBases;
	int pBase2;
	int p3,p4;	
};


struct objPart
{
	objPart():active(false),objectId(-1){offset.Init();}
	bool active;
	int objectId;
	Vector3 offset;
};

class Ragdoll_Basic : public Ragdoll
{
public:
	Ragdoll_Basic(int id) : Ragdoll(id){constructBasicDoll();}
	virtual ~Ragdoll_Basic();

	virtual void onUpdate();	
	virtual void onUpdateGraphics();	
	
	virtual ePhysError fixObjectToRagdoll(int objectId, int partNo);
	virtual ePhysError removeRagdollObjectPart(int id);
	virtual ePhysError setRagdollObjectOffset(int partNo, float ox, float oy, float oz);

	ePhysError setRagdollScales(float globalScale, float upperArmScale,float lowerArmScale,
		float upperLegScale,float lowerLegScale,float chestHeightScale, float waistHeightScale);

	virtual int childClassID(){return classID;}
	//static const int classID='RBSC';
	static int classID;

	static objectPartLink partLinks[11];

	objPart bodyPart[11];

private:
	void constructBasicDoll();

	void scaleLimb(float upperScale, float lowerScale,int top, int jointA, int jointB, int muscleA, int muscleB, int bottom);
	void scaleBasicDoll(float globalScale, float upperArmScale,float lowerArmScale,
						float upperLegScale,float lowerLegScale, float chestHeightScale, float waistHeightScale);

};

RAGDOLL_PLUGIN_INTERFACE(Ragdoll_Basic);


/*

                    Head *-----*
                          \   /
Hand   UpperElbow        Ne\ /ck   Shoulder
*---------*---------*-------*-------*---------*---------*
     \    |    /     \   \  |  /   /     \    |    /     
	  ----*----       \     *Chest/       ----*----
      Lower Elbow      \   /  \  /
					    \       /
						 *-----*Waist
						/ ==x== \
					   *---------*Thigh
					   |         |
					  /|         |\
					 | |         | |
					 *-*Inside   *-*Outside Knee
					 | |Knee     | |
					  \|         |/
					   |         |
					   *         *Foot


*/


#endif
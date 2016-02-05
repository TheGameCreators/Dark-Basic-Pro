#include "stdafx.h"


void Ragdoll_Basic::constructBasicDoll()
{
	numParticles=30;
	numLinks=108;
	numRotConstraints=4;

	particle = new RagdollParticle[numParticles];

	//Head, neck, and chest
	particle[RAGBASIC_HEADLEFT].Set(-0.055f,2.0f,0.0f,0.1f,RAGBASIC_HEADLEFT);
	particle[RAGBASIC_HEADRIGHT].Set(0.055f,2.0f,0.0f,0.1f,RAGBASIC_HEADRIGHT);
	particle[RAGBASIC_NECK].Set(0.000000f,1.75f,0.0f,0.1f,RAGBASIC_NECK);
	particle[RAGBASIC_CHEST].Set(0.000000f,1.44f,0.0f,0.1f,RAGBASIC_CHEST);
	
	//Waist
	particle[RAGBASIC_WAISTLEFT].Set(-0.19f,1.2f,0.0f,0.1f,RAGBASIC_WAISTLEFT);
	particle[RAGBASIC_WAISTRIGHT].Set(0.19f,1.2f,0.0f,0.1f,RAGBASIC_WAISTRIGHT);

	//Left arm
	particle[RAGBASIC_HANDLEFT].Set(-1.1f,1.6f,0.0f,0.1f,RAGBASIC_HANDLEFT);
	particle[RAGBASIC_BICEPLEFT].Set(-0.6f,1.66f,0.071f,0.1f,RAGBASIC_BICEPLEFT);
	particle[RAGBASIC_TRICEPLEFT].Set(-0.6f,1.66f,-0.071f,0.1f,RAGBASIC_TRICEPLEFT);
	particle[RAGBASIC_UPELBOWLEFT].Set(-0.68f,1.72f,0.0f,0.1f,RAGBASIC_UPELBOWLEFT);
	particle[RAGBASIC_LOWELBOWLEFT].Set(-0.68f,1.6f,0.0f,0.1f,RAGBASIC_LOWELBOWLEFT);
	particle[RAGBASIC_SHOULDERLEFT].Set(-0.25f,1.67f,0.0f,0.1f,RAGBASIC_SHOULDERLEFT);

	//Right arm
	particle[RAGBASIC_HANDRIGHT].Set(1.1f,1.6f,0.0f,0.1f,RAGBASIC_HANDRIGHT);
	particle[RAGBASIC_BICEPRIGHT].Set(0.6f,1.66f,0.071f,0.1f,RAGBASIC_BICEPRIGHT);
	particle[RAGBASIC_TRICEPRIGHT].Set(0.6f,1.66f,-0.071f,0.1f,RAGBASIC_TRICEPRIGHT);
	particle[RAGBASIC_UPELBOWRIGHT].Set(0.68f,1.72f,0.0f,0.1f,RAGBASIC_UPELBOWRIGHT);
	particle[RAGBASIC_LOWELBOWRIGHT].Set(0.68f,1.6f,0.0f,0.1f,RAGBASIC_LOWELBOWRIGHT);
	particle[RAGBASIC_SHOULDERRIGHT].Set(0.25f,1.67f,0.0f,0.1f,RAGBASIC_SHOULDERRIGHT);
	
	//Left leg
	particle[RAGBASIC_LEGTOPLEFT].Set(-0.13f,0.95f,0.0f,0.1f,RAGBASIC_LEGTOPLEFT);
	particle[RAGBASIC_QUADLEFT].Set(-0.21f,0.55f,0.07f,0.1f,RAGBASIC_QUADLEFT);
	particle[RAGBASIC_HAMLEFT].Set(-0.21f,0.55f,-0.07f,0.1f,RAGBASIC_HAMLEFT);
	particle[RAGBASIC_OUTKNEELEFT].Set(-0.27f,0.49f,0.0f,0.1f,RAGBASIC_OUTKNEELEFT);
	particle[RAGBASIC_INKNEELEFT].Set(-0.18f,0.49f,0.0f,0.1f,RAGBASIC_INKNEELEFT);
	particle[RAGBASIC_FOOTLEFT].Set(-0.27f,0.0f,0.0f,0.05f,RAGBASIC_FOOTLEFT);

	//Right leg
	particle[RAGBASIC_LEGTOPRIGHT].Set(0.13f,0.95f,0.0f,0.1f,RAGBASIC_LEGTOPRIGHT);
	particle[RAGBASIC_QUADRIGHT].Set(0.21f,0.55f,0.07f,0.1f,RAGBASIC_QUADRIGHT);
	particle[RAGBASIC_HAMRIGHT].Set(0.21f,0.55f,-0.07f,0.1f,RAGBASIC_HAMRIGHT);
	particle[RAGBASIC_OUTKNEERIGHT].Set(0.27f,0.49f,0.0f,0.1f,RAGBASIC_OUTKNEERIGHT);
	particle[RAGBASIC_INKNEERIGHT].Set(0.18f,0.49f,0.0f,0.1f,RAGBASIC_INKNEERIGHT);
	particle[RAGBASIC_FOOTRIGHT].Set(0.27f,0.0f,0.0f,0.05f,RAGBASIC_FOOTRIGHT);

	//Make normal links
	link = new RagdollLink[numLinks];

	//Head
	link[0].Set(&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_HEADRIGHT]);
	link[1].Set(&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_NECK]);
	link[2].Set(&particle[RAGBASIC_HEADRIGHT],&particle[RAGBASIC_NECK]);

	//Upper torso
	link[3].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_NECK]);
	link[4].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_NECK]);
	link[5].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_CHEST]);
	link[6].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_CHEST]);
	link[7].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_WAISTLEFT]);
	link[8].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_WAISTRIGHT]);
	link[9].Set(&particle[RAGBASIC_NECK],&particle[RAGBASIC_CHEST]);
	link[10].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_CHEST]);
	link[11].Set(&particle[RAGBASIC_WAISTRIGHT],&particle[RAGBASIC_CHEST]);
	link[12].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_WAISTRIGHT]);
	link[13].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_WAISTRIGHT]);
	link[14].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_WAISTLEFT]);
	link[15].Set(&particle[RAGBASIC_NECK],&particle[RAGBASIC_WAISTRIGHT]);
	link[16].Set(&particle[RAGBASIC_NECK],&particle[RAGBASIC_WAISTLEFT]);
	link[17].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_SHOULDERLEFT]);

	//Lower torso
	link[18].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_LEGTOPLEFT]);
	link[19].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_LEGTOPRIGHT]);
	link[20].Set(&particle[RAGBASIC_WAISTRIGHT],&particle[RAGBASIC_LEGTOPLEFT]);
	link[21].Set(&particle[RAGBASIC_WAISTRIGHT],&particle[RAGBASIC_LEGTOPRIGHT]);
	link[22].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_LEGTOPRIGHT]);

	//Left arm
	link[23].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_UPELBOWLEFT]);
	link[24].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_LOWELBOWLEFT]);
	link[25].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_BICEPLEFT]);
	link[26].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_TRICEPLEFT]);
	link[27].Set(&particle[RAGBASIC_BICEPLEFT],&particle[RAGBASIC_UPELBOWLEFT]);
	link[28].Set(&particle[RAGBASIC_BICEPLEFT],&particle[RAGBASIC_LOWELBOWLEFT]);
	link[29].Set(&particle[RAGBASIC_TRICEPLEFT],&particle[RAGBASIC_UPELBOWLEFT]);
	link[30].Set(&particle[RAGBASIC_TRICEPLEFT],&particle[RAGBASIC_LOWELBOWLEFT]);
	link[31].Set(&particle[RAGBASIC_BICEPLEFT],&particle[RAGBASIC_TRICEPLEFT]);
	link[32].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_LOWELBOWLEFT]);
	link[33].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_HANDLEFT]);
	link[34].Set(&particle[RAGBASIC_LOWELBOWLEFT],&particle[RAGBASIC_HANDLEFT]);

	//Right arm
	link[35].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_UPELBOWRIGHT]);
	link[36].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_LOWELBOWRIGHT]);
	link[37].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_BICEPRIGHT]);
	link[38].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_TRICEPRIGHT]);
	link[39].Set(&particle[RAGBASIC_BICEPRIGHT],&particle[RAGBASIC_UPELBOWRIGHT]);
	link[40].Set(&particle[RAGBASIC_BICEPRIGHT],&particle[RAGBASIC_LOWELBOWRIGHT]);
	link[41].Set(&particle[RAGBASIC_TRICEPRIGHT],&particle[RAGBASIC_UPELBOWRIGHT]);
	link[42].Set(&particle[RAGBASIC_TRICEPRIGHT],&particle[RAGBASIC_LOWELBOWRIGHT]);
	link[43].Set(&particle[RAGBASIC_BICEPRIGHT],&particle[RAGBASIC_TRICEPRIGHT]);
	link[44].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_LOWELBOWRIGHT]);
	link[45].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_HANDRIGHT]);
	link[46].Set(&particle[RAGBASIC_LOWELBOWRIGHT],&particle[RAGBASIC_HANDRIGHT]);
	
	//Left leg
	link[47].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_INKNEELEFT]);
	link[48].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_OUTKNEELEFT]);
	link[49].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_QUADLEFT]);
	link[50].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_HAMLEFT]);
	link[51].Set(&particle[RAGBASIC_QUADLEFT],&particle[RAGBASIC_INKNEELEFT]);
	link[52].Set(&particle[RAGBASIC_QUADLEFT],&particle[RAGBASIC_OUTKNEELEFT]);
	link[53].Set(&particle[RAGBASIC_HAMLEFT],&particle[RAGBASIC_INKNEELEFT]);
	link[54].Set(&particle[RAGBASIC_HAMLEFT],&particle[RAGBASIC_OUTKNEELEFT]);
	link[55].Set(&particle[RAGBASIC_QUADLEFT],&particle[RAGBASIC_HAMLEFT]);
	link[56].Set(&particle[RAGBASIC_INKNEELEFT],&particle[RAGBASIC_OUTKNEELEFT]);
	link[57].Set(&particle[RAGBASIC_INKNEELEFT],&particle[RAGBASIC_FOOTLEFT]);
	link[58].Set(&particle[RAGBASIC_OUTKNEELEFT],&particle[RAGBASIC_FOOTLEFT]);

	//Right leg
	link[59].Set(&particle[RAGBASIC_LEGTOPRIGHT],&particle[RAGBASIC_INKNEERIGHT]);
	link[60].Set(&particle[RAGBASIC_LEGTOPRIGHT],&particle[RAGBASIC_OUTKNEERIGHT]);
	link[61].Set(&particle[RAGBASIC_LEGTOPRIGHT],&particle[RAGBASIC_QUADRIGHT]);
	link[62].Set(&particle[RAGBASIC_LEGTOPRIGHT],&particle[RAGBASIC_HAMRIGHT]);
	link[63].Set(&particle[RAGBASIC_QUADRIGHT],&particle[RAGBASIC_INKNEERIGHT]);
	link[64].Set(&particle[RAGBASIC_QUADRIGHT],&particle[RAGBASIC_OUTKNEERIGHT]);
	link[65].Set(&particle[RAGBASIC_HAMRIGHT],&particle[RAGBASIC_INKNEERIGHT]);
	link[66].Set(&particle[RAGBASIC_HAMRIGHT],&particle[RAGBASIC_OUTKNEERIGHT]);
	link[67].Set(&particle[RAGBASIC_QUADRIGHT],&particle[RAGBASIC_HAMRIGHT]);
	link[68].Set(&particle[RAGBASIC_INKNEERIGHT],&particle[RAGBASIC_OUTKNEERIGHT]);
	link[69].Set(&particle[RAGBASIC_INKNEERIGHT],&particle[RAGBASIC_FOOTRIGHT]);
	link[70].Set(&particle[RAGBASIC_OUTKNEERIGHT],&particle[RAGBASIC_FOOTRIGHT]);


	//Don't let head get too close to chest
	link[71].Set(&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.48f*0.48f);
	link[72].Set(&particle[RAGBASIC_HEADRIGHT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.48f*0.48f);

	//Don't let head get too close or far away from shoulders 
	link[73].Set(&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_SHOULDERLEFT],RAGLINK_MINMAX,0.13f*0.13f,0.51f*0.51f);
	link[74].Set(&particle[RAGBASIC_HEADRIGHT],&particle[RAGBASIC_SHOULDERRIGHT],RAGLINK_MINMAX,0.13f*0.13f,0.51f*0.51f);

	//Rotational head restraint (passing 3 particle arguments for each link)
	link[75].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_HEADRIGHT],0.05f);
	link[76].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_HEADRIGHT],&particle[RAGBASIC_HEADLEFT],0.05f);

	//Stop arms going through chest
	link[77].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.32f*0.32f);
	link[78].Set(&particle[RAGBASIC_LOWELBOWLEFT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.32f*0.32f);
	link[79].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.32f*0.32f);
	link[80].Set(&particle[RAGBASIC_LOWELBOWRIGHT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.32f*0.32f);

	//Stop knees bending too much
	link[81].Set(&particle[RAGBASIC_FOOTLEFT],&particle[RAGBASIC_HAMLEFT],RAGLINK_MAXIMUM,0.548f*0.548f);
	link[82].Set(&particle[RAGBASIC_FOOTRIGHT],&particle[RAGBASIC_HAMRIGHT],RAGLINK_MAXIMUM,0.548f*0.548f);
	
	//Stop knees wrong way
	link[83].Set(&particle[RAGBASIC_FOOTLEFT],&particle[RAGBASIC_QUADLEFT],RAGLINK_MINIMUM,0.535f*0.535f);
	link[84].Set(&particle[RAGBASIC_FOOTRIGHT],&particle[RAGBASIC_QUADRIGHT],RAGLINK_MINIMUM,0.535f*0.535f);

	//Keep top of legs away from chest
	link[85].Set(&particle[RAGBASIC_CHEST],&particle[RAGBASIC_LEGTOPRIGHT],RAGLINK_MINIMUM,0.47f*0.47f);
	link[86].Set(&particle[RAGBASIC_CHEST],&particle[RAGBASIC_LEGTOPLEFT],RAGLINK_MINIMUM,0.47f*0.47f);

	//Keep knees away from waist
	link[87].Set(&particle[RAGBASIC_WAISTRIGHT],&particle[RAGBASIC_QUADRIGHT],RAGLINK_MINIMUM,0.59f*0.59f);
	link[88].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_QUADLEFT],RAGLINK_MINIMUM,0.59f*0.59f);

	//Keep knees away from each other
	link[89].Set(&particle[RAGBASIC_OUTKNEELEFT],&particle[RAGBASIC_OUTKNEERIGHT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep elbows away from neck and head
	link[90].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_HEADLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[91].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_HEADRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[92].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_NECK],RAGLINK_MINIMUM,0.2f*0.2f);
	link[93].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_NECK],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep elbows away from opposite shoulders
	link[94].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_SHOULDERRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[95].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_SHOULDERLEFT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from head
	link[96].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_HEADLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[97].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_HEADRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from shoulders
	link[98].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_SHOULDERLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[99].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_SHOULDERRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[100].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_SHOULDERRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[101].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_SHOULDERLEFT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from chest
	link[102].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.2f*0.2f);
	link[103].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from waist
	link[104].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_WAISTLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[105].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_WAISTRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from top of legs
	link[106].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_LEGTOPLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[107].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_LEGTOPRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);

	rotConstr = new RagdollRotConstraint[numRotConstraints];

	rotConstr[0].Set(&particle[RAGBASIC_BICEPLEFT],
					&particle[RAGBASIC_TRICEPLEFT],
					&particle[RAGBASIC_UPELBOWLEFT],
					&particle[RAGBASIC_LOWELBOWLEFT],10.0f);

	rotConstr[1].Set(&particle[RAGBASIC_BICEPRIGHT],
					&particle[RAGBASIC_TRICEPRIGHT],
					&particle[RAGBASIC_UPELBOWRIGHT],
					&particle[RAGBASIC_LOWELBOWRIGHT],10.0f);

	rotConstr[2].Set(&particle[RAGBASIC_QUADLEFT],
					&particle[RAGBASIC_HAMLEFT],
					&particle[RAGBASIC_INKNEELEFT],
					&particle[RAGBASIC_OUTKNEELEFT],20.0f);

	rotConstr[3].Set(&particle[RAGBASIC_QUADRIGHT],
					&particle[RAGBASIC_HAMRIGHT],
					&particle[RAGBASIC_INKNEERIGHT],
					&particle[RAGBASIC_OUTKNEERIGHT],20.0f);

	
}


void Ragdoll_Basic::scaleBasicDoll(float globalScale, 
								float upperArmScale,float lowerArmScale,
								float upperLegScale,float lowerLegScale,
								float chestHeightScale, float waistHeightScale)
{
	//Reset original particle positions

	//Head, neck, and chest
	particle[RAGBASIC_HEADLEFT].Set(-0.055f,2.0f,0.0f,0.1f,RAGBASIC_HEADLEFT);
	particle[RAGBASIC_HEADRIGHT].Set(0.055f,2.0f,0.0f,0.1f,RAGBASIC_HEADRIGHT);
	particle[RAGBASIC_NECK].Set(0.000000f,1.75f,0.0f,0.1f,RAGBASIC_NECK);
	particle[RAGBASIC_CHEST].Set(0.000000f,1.44f,0.0f,0.1f,RAGBASIC_CHEST);
	
	//Waist
	particle[RAGBASIC_WAISTLEFT].Set(-0.19f,1.2f,0.0f,0.1f,RAGBASIC_WAISTLEFT);
	particle[RAGBASIC_WAISTRIGHT].Set(0.19f,1.2f,0.0f,0.1f,RAGBASIC_WAISTRIGHT);

	//Left arm
	particle[RAGBASIC_HANDLEFT].Set(-1.1f,1.6f,0.0f,0.1f,RAGBASIC_HANDLEFT);
	particle[RAGBASIC_BICEPLEFT].Set(-0.6f,1.66f,0.071f,0.1f,RAGBASIC_BICEPLEFT);
	particle[RAGBASIC_TRICEPLEFT].Set(-0.6f,1.66f,-0.071f,0.1f,RAGBASIC_TRICEPLEFT);
	particle[RAGBASIC_UPELBOWLEFT].Set(-0.68f,1.72f,0.0f,0.1f,RAGBASIC_UPELBOWLEFT);
	particle[RAGBASIC_LOWELBOWLEFT].Set(-0.68f,1.6f,0.0f,0.1f,RAGBASIC_LOWELBOWLEFT);
	particle[RAGBASIC_SHOULDERLEFT].Set(-0.25f,1.67f,0.0f,0.1f,RAGBASIC_SHOULDERLEFT);

	//Right arm
	particle[RAGBASIC_HANDRIGHT].Set(1.1f,1.6f,0.0f,0.1f,RAGBASIC_HANDRIGHT);
	particle[RAGBASIC_BICEPRIGHT].Set(0.6f,1.66f,0.071f,0.1f,RAGBASIC_BICEPRIGHT);
	particle[RAGBASIC_TRICEPRIGHT].Set(0.6f,1.66f,-0.071f,0.1f,RAGBASIC_TRICEPRIGHT);
	particle[RAGBASIC_UPELBOWRIGHT].Set(0.68f,1.72f,0.0f,0.1f,RAGBASIC_UPELBOWRIGHT);
	particle[RAGBASIC_LOWELBOWRIGHT].Set(0.68f,1.6f,0.0f,0.1f,RAGBASIC_LOWELBOWRIGHT);
	particle[RAGBASIC_SHOULDERRIGHT].Set(0.25f,1.67f,0.0f,0.1f,RAGBASIC_SHOULDERRIGHT);
	
	//Left leg
	particle[RAGBASIC_LEGTOPLEFT].Set(-0.13f,0.95f,0.0f,0.1f,RAGBASIC_LEGTOPLEFT);
	particle[RAGBASIC_QUADLEFT].Set(-0.21f,0.55f,0.07f,0.1f,RAGBASIC_QUADLEFT);
	particle[RAGBASIC_HAMLEFT].Set(-0.21f,0.55f,-0.07f,0.1f,RAGBASIC_HAMLEFT);
	particle[RAGBASIC_OUTKNEELEFT].Set(-0.27f,0.49f,0.0f,0.1f,RAGBASIC_OUTKNEELEFT);
	particle[RAGBASIC_INKNEELEFT].Set(-0.18f,0.49f,0.0f,0.1f,RAGBASIC_INKNEELEFT);
	particle[RAGBASIC_FOOTLEFT].Set(-0.27f,0.0f,0.0f,0.05f,RAGBASIC_FOOTLEFT);

	//Right leg
	particle[RAGBASIC_LEGTOPRIGHT].Set(0.13f,0.95f,0.0f,0.1f,RAGBASIC_LEGTOPRIGHT);
	particle[RAGBASIC_QUADRIGHT].Set(0.21f,0.55f,0.07f,0.1f,RAGBASIC_QUADRIGHT);
	particle[RAGBASIC_HAMRIGHT].Set(0.21f,0.55f,-0.07f,0.1f,RAGBASIC_HAMRIGHT);
	particle[RAGBASIC_OUTKNEERIGHT].Set(0.27f,0.49f,0.0f,0.1f,RAGBASIC_OUTKNEERIGHT);
	particle[RAGBASIC_INKNEERIGHT].Set(0.18f,0.49f,0.0f,0.1f,RAGBASIC_INKNEERIGHT);
	particle[RAGBASIC_FOOTRIGHT].Set(0.27f,0.0f,0.0f,0.05f,RAGBASIC_FOOTRIGHT);

	int i;

	//Scale all particle positions
	for(i=0;i<numParticles;i++)
	{
		particle[i].pos*=globalScale;
	}
	//Scale limbs
	scaleLimb(upperArmScale, lowerArmScale, RAGBASIC_SHOULDERLEFT, RAGBASIC_UPELBOWLEFT, RAGBASIC_LOWELBOWLEFT, RAGBASIC_BICEPLEFT, RAGBASIC_TRICEPLEFT, RAGBASIC_HANDLEFT);
	scaleLimb(upperArmScale, lowerArmScale, RAGBASIC_SHOULDERRIGHT, RAGBASIC_UPELBOWRIGHT, RAGBASIC_LOWELBOWRIGHT, RAGBASIC_BICEPRIGHT, RAGBASIC_TRICEPRIGHT, RAGBASIC_HANDRIGHT);
	scaleLimb(upperLegScale, lowerLegScale, RAGBASIC_LEGTOPLEFT, RAGBASIC_OUTKNEELEFT, RAGBASIC_INKNEELEFT, RAGBASIC_QUADLEFT, RAGBASIC_HAMLEFT, RAGBASIC_FOOTLEFT);
	scaleLimb(upperLegScale, lowerLegScale, RAGBASIC_LEGTOPRIGHT, RAGBASIC_OUTKNEERIGHT, RAGBASIC_INKNEERIGHT, RAGBASIC_QUADRIGHT, RAGBASIC_HAMRIGHT, RAGBASIC_FOOTRIGHT);

	//Scale chest vertically, and move arms and head with relevant points
	float oldShoulderLeftDiff = particle[RAGBASIC_SHOULDERLEFT].pos.y;
	float oldShoulderRightDiff = particle[RAGBASIC_SHOULDERRIGHT].pos.y;
	float oldNeckDiff = particle[RAGBASIC_NECK].pos.y;
	particle[RAGBASIC_SHOULDERLEFT].pos.y-=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_SHOULDERLEFT].pos.y*=chestHeightScale;
	particle[RAGBASIC_SHOULDERLEFT].pos.y+=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_SHOULDERRIGHT].pos.y-=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_SHOULDERRIGHT].pos.y*=chestHeightScale;
	particle[RAGBASIC_SHOULDERRIGHT].pos.y+=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_NECK].pos.y-=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_NECK].pos.y*=chestHeightScale;
	particle[RAGBASIC_NECK].pos.y+=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_CHEST].pos.y-=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_CHEST].pos.y*=chestHeightScale;
	particle[RAGBASIC_CHEST].pos.y+=particle[RAGBASIC_WAISTLEFT].pos.y;

	oldShoulderLeftDiff -= particle[RAGBASIC_SHOULDERLEFT].pos.y;
	oldShoulderRightDiff -= particle[RAGBASIC_SHOULDERRIGHT].pos.y;
	oldNeckDiff -= particle[RAGBASIC_NECK].pos.y;
	
	particle[RAGBASIC_HANDLEFT].pos.y-=oldShoulderLeftDiff;
	particle[RAGBASIC_BICEPLEFT].pos.y-=oldShoulderLeftDiff;
	particle[RAGBASIC_TRICEPLEFT].pos.y-=oldShoulderLeftDiff;
	particle[RAGBASIC_UPELBOWLEFT].pos.y-=oldShoulderLeftDiff;
	particle[RAGBASIC_LOWELBOWLEFT].pos.y-=oldShoulderLeftDiff;
	particle[RAGBASIC_HANDRIGHT].pos.y-=oldShoulderRightDiff;
	particle[RAGBASIC_BICEPRIGHT].pos.y-=oldShoulderRightDiff;
	particle[RAGBASIC_TRICEPRIGHT].pos.y-=oldShoulderRightDiff;
	particle[RAGBASIC_UPELBOWRIGHT].pos.y-=oldShoulderRightDiff;
	particle[RAGBASIC_LOWELBOWRIGHT].pos.y-=oldShoulderRightDiff;
	particle[RAGBASIC_HEADLEFT].pos.y-=oldNeckDiff;
	particle[RAGBASIC_HEADRIGHT].pos.y-=oldNeckDiff;
	
	//Scale lower torso vertically down, and move legs with relevant points
	float oldLegLeftDiff = particle[RAGBASIC_LEGTOPLEFT].pos.y;
	float oldLegRightDiff = particle[RAGBASIC_LEGTOPRIGHT].pos.y;
	particle[RAGBASIC_LEGTOPLEFT].pos.y-=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_LEGTOPLEFT].pos.y*=waistHeightScale;
	particle[RAGBASIC_LEGTOPLEFT].pos.y+=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_LEGTOPRIGHT].pos.y-=particle[RAGBASIC_WAISTLEFT].pos.y;
	particle[RAGBASIC_LEGTOPRIGHT].pos.y*=waistHeightScale;
	particle[RAGBASIC_LEGTOPRIGHT].pos.y+=particle[RAGBASIC_WAISTLEFT].pos.y;

	oldLegLeftDiff -= particle[RAGBASIC_LEGTOPLEFT].pos.y;
	oldLegRightDiff -= particle[RAGBASIC_LEGTOPRIGHT].pos.y;
	
	particle[RAGBASIC_QUADLEFT].pos.y-=oldLegLeftDiff;
	particle[RAGBASIC_HAMLEFT].pos.y-=oldLegLeftDiff;
	particle[RAGBASIC_OUTKNEELEFT].pos.y-=oldLegLeftDiff;
	particle[RAGBASIC_INKNEELEFT].pos.y-=oldLegLeftDiff;
	particle[RAGBASIC_FOOTLEFT].pos.y-=oldLegLeftDiff;
	particle[RAGBASIC_QUADRIGHT].pos.y-=oldLegRightDiff;
	particle[RAGBASIC_HAMRIGHT].pos.y-=oldLegRightDiff;
	particle[RAGBASIC_OUTKNEERIGHT].pos.y-=oldLegRightDiff;
	particle[RAGBASIC_INKNEERIGHT].pos.y-=oldLegRightDiff;
	particle[RAGBASIC_FOOTRIGHT].pos.y-=oldLegRightDiff;

	//Move all points up so that feet are at y=0
	float vdist = -particle[RAGBASIC_FOOTLEFT].pos.y;
	
	for(i=0;i<numParticles;i++)
	{
		particle[i].pos.y+=vdist;
		//Make sure velocities are zero
		particle[i].lastPos = particle[i].pos;
	}


	//Reset link lengths

	//Head
	link[0].Set(&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_HEADRIGHT]);
	link[1].Set(&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_NECK]);
	link[2].Set(&particle[RAGBASIC_HEADRIGHT],&particle[RAGBASIC_NECK]);

	//Upper torso
	link[3].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_NECK]);
	link[4].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_NECK]);
	link[5].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_CHEST]);
	link[6].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_CHEST]);
	link[7].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_WAISTLEFT]);
	link[8].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_WAISTRIGHT]);
	link[9].Set(&particle[RAGBASIC_NECK],&particle[RAGBASIC_CHEST]);
	link[10].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_CHEST]);
	link[11].Set(&particle[RAGBASIC_WAISTRIGHT],&particle[RAGBASIC_CHEST]);
	link[12].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_WAISTRIGHT]);
	link[13].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_WAISTRIGHT]);
	link[14].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_WAISTLEFT]);
	link[15].Set(&particle[RAGBASIC_NECK],&particle[RAGBASIC_WAISTRIGHT]);
	link[16].Set(&particle[RAGBASIC_NECK],&particle[RAGBASIC_WAISTLEFT]);
	link[17].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_SHOULDERLEFT]);

	//Lower torso
	link[18].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_LEGTOPLEFT]);
	link[19].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_LEGTOPRIGHT]);
	link[20].Set(&particle[RAGBASIC_WAISTRIGHT],&particle[RAGBASIC_LEGTOPLEFT]);
	link[21].Set(&particle[RAGBASIC_WAISTRIGHT],&particle[RAGBASIC_LEGTOPRIGHT]);
	link[22].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_LEGTOPRIGHT]);

	//Left arm
	link[23].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_UPELBOWLEFT]);
	link[24].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_LOWELBOWLEFT]);
	link[25].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_BICEPLEFT]);
	link[26].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_TRICEPLEFT]);
	link[27].Set(&particle[RAGBASIC_BICEPLEFT],&particle[RAGBASIC_UPELBOWLEFT]);
	link[28].Set(&particle[RAGBASIC_BICEPLEFT],&particle[RAGBASIC_LOWELBOWLEFT]);
	link[29].Set(&particle[RAGBASIC_TRICEPLEFT],&particle[RAGBASIC_UPELBOWLEFT]);
	link[30].Set(&particle[RAGBASIC_TRICEPLEFT],&particle[RAGBASIC_LOWELBOWLEFT]);
	link[31].Set(&particle[RAGBASIC_BICEPLEFT],&particle[RAGBASIC_TRICEPLEFT]);
	link[32].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_LOWELBOWLEFT]);
	link[33].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_HANDLEFT]);
	link[34].Set(&particle[RAGBASIC_LOWELBOWLEFT],&particle[RAGBASIC_HANDLEFT]);

	//Right arm
	link[35].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_UPELBOWRIGHT]);
	link[36].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_LOWELBOWRIGHT]);
	link[37].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_BICEPRIGHT]);
	link[38].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_TRICEPRIGHT]);
	link[39].Set(&particle[RAGBASIC_BICEPRIGHT],&particle[RAGBASIC_UPELBOWRIGHT]);
	link[40].Set(&particle[RAGBASIC_BICEPRIGHT],&particle[RAGBASIC_LOWELBOWRIGHT]);
	link[41].Set(&particle[RAGBASIC_TRICEPRIGHT],&particle[RAGBASIC_UPELBOWRIGHT]);
	link[42].Set(&particle[RAGBASIC_TRICEPRIGHT],&particle[RAGBASIC_LOWELBOWRIGHT]);
	link[43].Set(&particle[RAGBASIC_BICEPRIGHT],&particle[RAGBASIC_TRICEPRIGHT]);
	link[44].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_LOWELBOWRIGHT]);
	link[45].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_HANDRIGHT]);
	link[46].Set(&particle[RAGBASIC_LOWELBOWRIGHT],&particle[RAGBASIC_HANDRIGHT]);
	
	//Left leg
	link[47].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_INKNEELEFT]);
	link[48].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_OUTKNEELEFT]);
	link[49].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_QUADLEFT]);
	link[50].Set(&particle[RAGBASIC_LEGTOPLEFT],&particle[RAGBASIC_HAMLEFT]);
	link[51].Set(&particle[RAGBASIC_QUADLEFT],&particle[RAGBASIC_INKNEELEFT]);
	link[52].Set(&particle[RAGBASIC_QUADLEFT],&particle[RAGBASIC_OUTKNEELEFT]);
	link[53].Set(&particle[RAGBASIC_HAMLEFT],&particle[RAGBASIC_INKNEELEFT]);
	link[54].Set(&particle[RAGBASIC_HAMLEFT],&particle[RAGBASIC_OUTKNEELEFT]);
	link[55].Set(&particle[RAGBASIC_QUADLEFT],&particle[RAGBASIC_HAMLEFT]);
	link[56].Set(&particle[RAGBASIC_INKNEELEFT],&particle[RAGBASIC_OUTKNEELEFT]);
	link[57].Set(&particle[RAGBASIC_INKNEELEFT],&particle[RAGBASIC_FOOTLEFT]);
	link[58].Set(&particle[RAGBASIC_OUTKNEELEFT],&particle[RAGBASIC_FOOTLEFT]);

	//Right leg
	link[59].Set(&particle[RAGBASIC_LEGTOPRIGHT],&particle[RAGBASIC_INKNEERIGHT]);
	link[60].Set(&particle[RAGBASIC_LEGTOPRIGHT],&particle[RAGBASIC_OUTKNEERIGHT]);
	link[61].Set(&particle[RAGBASIC_LEGTOPRIGHT],&particle[RAGBASIC_QUADRIGHT]);
	link[62].Set(&particle[RAGBASIC_LEGTOPRIGHT],&particle[RAGBASIC_HAMRIGHT]);
	link[63].Set(&particle[RAGBASIC_QUADRIGHT],&particle[RAGBASIC_INKNEERIGHT]);
	link[64].Set(&particle[RAGBASIC_QUADRIGHT],&particle[RAGBASIC_OUTKNEERIGHT]);
	link[65].Set(&particle[RAGBASIC_HAMRIGHT],&particle[RAGBASIC_INKNEERIGHT]);
	link[66].Set(&particle[RAGBASIC_HAMRIGHT],&particle[RAGBASIC_OUTKNEERIGHT]);
	link[67].Set(&particle[RAGBASIC_QUADRIGHT],&particle[RAGBASIC_HAMRIGHT]);
	link[68].Set(&particle[RAGBASIC_INKNEERIGHT],&particle[RAGBASIC_OUTKNEERIGHT]);
	link[69].Set(&particle[RAGBASIC_INKNEERIGHT],&particle[RAGBASIC_FOOTRIGHT]);
	link[70].Set(&particle[RAGBASIC_OUTKNEERIGHT],&particle[RAGBASIC_FOOTRIGHT]);
return;

	//Don't let head get too close to chest
	link[71].Set(&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.48f*0.48f);
	link[72].Set(&particle[RAGBASIC_HEADRIGHT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.48f*0.48f);

	//Don't let head get too close or far away from shoulders 
	link[73].Set(&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_SHOULDERLEFT],RAGLINK_MINMAX,0.13f*0.13f,0.51f*0.51f);
	link[74].Set(&particle[RAGBASIC_HEADRIGHT],&particle[RAGBASIC_SHOULDERRIGHT],RAGLINK_MINMAX,0.13f*0.13f,0.51f*0.51f);

	//Rotational head restraint (passing 3 particle arguments for each link)
	link[75].Set(&particle[RAGBASIC_SHOULDERRIGHT],&particle[RAGBASIC_HEADLEFT],&particle[RAGBASIC_HEADRIGHT],0.05f);
	link[76].Set(&particle[RAGBASIC_SHOULDERLEFT],&particle[RAGBASIC_HEADRIGHT],&particle[RAGBASIC_HEADLEFT],0.05f);

	//Stop arms going through chest
	link[77].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.32f*0.32f);
	link[78].Set(&particle[RAGBASIC_LOWELBOWLEFT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.32f*0.32f);
	link[79].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.32f*0.32f);
	link[80].Set(&particle[RAGBASIC_LOWELBOWRIGHT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.32f*0.32f);

	//Stop knees bending too much
	link[81].Set(&particle[RAGBASIC_FOOTLEFT],&particle[RAGBASIC_HAMLEFT],RAGLINK_MAXIMUM,0.548f*0.548f);
	link[82].Set(&particle[RAGBASIC_FOOTRIGHT],&particle[RAGBASIC_HAMRIGHT],RAGLINK_MAXIMUM,0.548f*0.548f);
	
	//Stop knees wrong way
	link[83].Set(&particle[RAGBASIC_FOOTLEFT],&particle[RAGBASIC_QUADLEFT],RAGLINK_MINIMUM,0.535f*0.535f);
	link[84].Set(&particle[RAGBASIC_FOOTRIGHT],&particle[RAGBASIC_QUADRIGHT],RAGLINK_MINIMUM,0.535f*0.535f);

	//Keep top of legs away from chest
	link[85].Set(&particle[RAGBASIC_CHEST],&particle[RAGBASIC_LEGTOPRIGHT],RAGLINK_MINIMUM,0.47f*0.47f);
	link[86].Set(&particle[RAGBASIC_CHEST],&particle[RAGBASIC_LEGTOPLEFT],RAGLINK_MINIMUM,0.47f*0.47f);

	//Keep knees away from waist
	link[87].Set(&particle[RAGBASIC_WAISTRIGHT],&particle[RAGBASIC_QUADRIGHT],RAGLINK_MINIMUM,0.59f*0.59f);
	link[88].Set(&particle[RAGBASIC_WAISTLEFT],&particle[RAGBASIC_QUADLEFT],RAGLINK_MINIMUM,0.59f*0.59f);

	//Keep knees away from each other
	link[89].Set(&particle[RAGBASIC_OUTKNEELEFT],&particle[RAGBASIC_OUTKNEERIGHT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep elbows away from neck and head
	link[90].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_HEADLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[91].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_HEADRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[92].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_NECK],RAGLINK_MINIMUM,0.2f*0.2f);
	link[93].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_NECK],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep elbows away from opposite shoulders
	link[94].Set(&particle[RAGBASIC_UPELBOWLEFT],&particle[RAGBASIC_SHOULDERRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[95].Set(&particle[RAGBASIC_UPELBOWRIGHT],&particle[RAGBASIC_SHOULDERLEFT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from head
	link[96].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_HEADLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[97].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_HEADRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from shoulders
	link[98].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_SHOULDERLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[99].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_SHOULDERRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[100].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_SHOULDERRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[101].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_SHOULDERLEFT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from chest
	link[102].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.2f*0.2f);
	link[103].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_CHEST],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from waist
	link[104].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_WAISTLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[105].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_WAISTRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);

	//Keep hands away from top of legs
	link[106].Set(&particle[RAGBASIC_HANDLEFT],&particle[RAGBASIC_LEGTOPLEFT],RAGLINK_MINIMUM,0.2f*0.2f);
	link[107].Set(&particle[RAGBASIC_HANDRIGHT],&particle[RAGBASIC_LEGTOPRIGHT],RAGLINK_MINIMUM,0.2f*0.2f);
	
}


void Ragdoll_Basic::scaleLimb(float upperScale, float lowerScale,int top, int jointA, int jointB, int muscleA, int muscleB, int bottom)
{
	Vector3 delta = particle[jointA].pos - particle[top].pos;
	delta += particle[jointB].pos - particle[top].pos;
	delta*=0.5f;
				
	particle[jointA].pos.scaleUsingCenter(upperScale,particle[top].pos);
	particle[jointB].pos.scaleUsingCenter(upperScale,particle[top].pos);
	particle[muscleA].pos.scaleUsingCenter(upperScale,particle[top].pos);
	particle[muscleB].pos.scaleUsingCenter(upperScale,particle[top].pos);

	Vector3 delta2 = particle[jointA].pos - particle[top].pos;
	delta2 += particle[jointB].pos - particle[top].pos;
	delta2*=0.5f;
	delta2-=delta;

	particle[bottom].pos+=delta2;
	Vector3 avg = (particle[jointA].pos + particle[jointB].pos)*0.5f;
	particle[bottom].pos.scaleUsingCenter(lowerScale,avg);
}
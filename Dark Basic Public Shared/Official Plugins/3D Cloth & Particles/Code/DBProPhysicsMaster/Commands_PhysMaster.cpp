#include "stdafx.h"


static int maxIterations=10;
static bool allowTimerBail=false;
static bool allowAutoGarbageCollect=true;
static int ticksSinceLastGC=0;
static float autoGarbageCollectInterval=30.0f;
static bool frameRateIsClamped=false;

DLLEXPORT void updatePhysics()
{
	int numLoops=1;
	if(!physics->timer.isRunning())
	{
		physics->timer.Start();		
	}
	else
	{
		physics->timer.Split();
		double timePassed = physics->timer.getSplit();
		if(timePassed<PhysicsManager::mainTimeInterval)
		{
			if(frameRateIsClamped)
			{								
				//May as well do this here while we have a bit of time to spare
				if (allowAutoGarbageCollect) physics->garbageCollect();

				//need to wait a while
				while(timePassed<PhysicsManager::mainTimeInterval)
				{
					physics->timer.Split();
					timePassed = physics->timer.getSplit();
				}
				physics->timer.Start(); //Reset the timer
			}
			else
			{
				//We haven't reached the next time of execution
				physics->UpdateGraphics();
				return;
			}
		}
		else		
		{
			physics->timer.Start(); //Reset the timer

			//possibly need to loop over more than one frame
			numLoops = (int)floor(timePassed*PhysicsManager::mainFps);
			assert(numLoops>0);
		}		
	}
	
	bool needBail=false;
	if(allowTimerBail && numLoops>maxIterations)
	{
		needBail=true;
		numLoops=maxIterations;		
	}

	for(int i=0;i<numLoops;i++)
	{
		physics->Update();
	}
	physics->UpdateGraphics();

	if(needBail) physics->timer.StopStart();
	if(allowAutoGarbageCollect)
	{
		ticksSinceLastGC++;
		if(ticksSinceLastGC>PhysicsManager::mainFps*autoGarbageCollectInterval)
		{
			physics->garbageCollect();
		}
	}
}




DLLEXPORT void setPhysicsFrameRate(float fps)
{
	if(fps<10.0f) fps=10.0f;
	physics->setFrameRate(fps);
}

DLLEXPORT void setPhysicsMaxIterations(int num)
{
	if(num<1) num=1;
	if(num>PhysicsManager::mainFps) num=(int)floor(PhysicsManager::mainFps);
	maxIterations=num;
}


DLLEXPORT void setPhysicsTimerBailout(DWORD state)
{
	if(state) allowTimerBail=true;
	else allowTimerBail=false;
}


DLLEXPORT void setPhysicsAutoGarbageCollect(DWORD state)
{
	if(state) allowAutoGarbageCollect=true;
	else allowAutoGarbageCollect=false;
}

DLLEXPORT void setPhysicsGarbageCollectInterval(float secs)
{
	if(secs<10.0f) secs=10.0f;
	autoGarbageCollectInterval=secs;
}


DLLEXPORT void physicsGarbageCollect()
{
	physics->garbageCollect();
}

DLLEXPORT void clampPhysicsFrameRate(DWORD state)
{
	if(state) frameRateIsClamped=true;
	else frameRateIsClamped=false;
}


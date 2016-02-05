#ifndef DBPROPHYSICSMASTER_COMMANDS_PHYSMASTER_H
#define DBPROPHYSICSMASTER_COMMANDS_PHYSMASTER_H

DLLEXPORT void updatePhysics();
DLLEXPORT void setPhysicsFrameRate(float fps);
DLLEXPORT void setPhysicsMaxIterations(int num);
DLLEXPORT void setPhysicsTimerBailout(DWORD state);
DLLEXPORT void setPhysicsAutoGarbageCollect(DWORD state);
DLLEXPORT void setPhysicsGarbageCollectInterval(float secs);
DLLEXPORT void physicsGarbageCollect();
DLLEXPORT void clampPhysicsFrameRate(DWORD state);

#endif
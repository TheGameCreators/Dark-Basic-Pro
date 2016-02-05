#ifndef DBPROEMITTER_COMMANDS_EMITTER_H
#define DBPROEMITTER_COMMANDS_EMITTER_H


//Base class
DLLEXPORT void deleteEmitter(int id);
DLLEXPORT void freezeEmitter(int id, DWORD state);
DLLEXPORT void setParticleZSorting(int id, DWORD state);
DLLEXPORT void setRate(int id, float particlesPerSec);
DLLEXPORT void setExplode(int id, float param);
DLLEXPORT void setParticleVelocity(int id, float velocity, float percentVar);
DLLEXPORT void setParticleMass(int id, float mass, float percentVar);
DLLEXPORT void setParticleSize(int id, float size, float percentVar);
DLLEXPORT void setParticleLife(int id, float secs, float percentVar);
DLLEXPORT void setParticleColor(int id, int R, int G, int B, int A);

//Basic
DLLEXPORT void makeEmitter(int id, int numParticles);

//Box
DLLEXPORT void makeBoxEmitter(int id, int numParticles);
DLLEXPORT void setSpawnBox(int id, float sx, float sy, float sz);
DLLEXPORT void setRadialEmission(int id, DWORD state);

//Sphere
DLLEXPORT void makeSphericalEmitter(int id, int numParticles);
DLLEXPORT void setSpawnRadius(int id, float minRadius, float maxRadius);
DLLEXPORT void setSpawnHemisphere(int id, DWORD state);



#endif
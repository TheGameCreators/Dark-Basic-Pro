#include "stdafx.h"

DLLEXPORT PluginManager * getPluginManager()
{
	return PluginManager::getPluginManager();
}

PluginManager::~PluginManager()
{
	effectorPlugins.deleteAndClear();
	emitterPlugins.deleteAndClear();
	colliderPlugins.deleteAndClear();
	clothPlugins.deleteAndClear();
	ragdollPlugins.deleteAndClear();
	vehiclePlugins.deleteAndClear();
}

void * PluginManager::RegisterEffector(EFFECTOR_FACTORYFUNC ff)
{	
	EffectorPlugin * tmp = new EffectorPlugin;
	tmp->makeInstance=ff;
	effectorPlugins.Push(tmp);
	return tmp;
}

void * PluginManager::RegisterEmitter(EMITTER_FACTORYFUNC ff)
{
	EmitterPlugin * tmp = new EmitterPlugin;
	tmp->makeInstance=ff;
	emitterPlugins.Push(tmp);
	return tmp;
}

void * PluginManager::RegisterCollider(COLLIDER_FACTORYFUNC ff)
{
	ColliderPlugin * tmp = new ColliderPlugin;
	tmp->makeInstance=ff;
	colliderPlugins.Push(tmp);
	return tmp;
}

void * PluginManager::RegisterCloth(CLOTH_FACTORYFUNC ff)
{
	ClothPlugin * tmp = new ClothPlugin;
	tmp->makeInstance=ff;
	clothPlugins.Push(tmp);
	return tmp;
}

void * PluginManager::RegisterVehicle(VEHICLE_FACTORYFUNC ff)
{
	VehiclePlugin * tmp = new VehiclePlugin;
	tmp->makeInstance=ff;
	vehiclePlugins.Push(tmp);
	return tmp;
}

void * PluginManager::RegisterRagdoll(RAGDOLL_FACTORYFUNC ff)
{
	RagdollPlugin * tmp = new RagdollPlugin;
	tmp->makeInstance=ff;
	ragdollPlugins.Push(tmp);
	return tmp;
}

#pragma once

#include "OptimizedMath.h"
#include "Transform.h"

#ifdef DARKSDK_COMPILE
void ConstructorTransforms();
void DestructorTransforms();
void SetErrorHandlerTransforms(LPVOID pErrorHandlerPtr);
void PassCoreDataTransforms(LPVOID pGlobPtr);
#endif

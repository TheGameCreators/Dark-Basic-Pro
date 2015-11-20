#ifndef RENDERLIST_H
#define RENDERLIST_H

#define _CRT_SECURE_NO_DEPRECATE

#include "..\global.h"
#include <wtypes.h>

extern DARKSDK void AddToRenderList(LPVOID pFunction, int iPriority);
extern DARKSDK void RemoveFromRenderList(LPVOID pFunction);
extern void ExecuteRenderList();
#endif

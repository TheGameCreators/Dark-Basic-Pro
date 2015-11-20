#include "TransformsAPI.h"

GlobStruct *g_pGlob = nullptr;

DB_EXPORT void Constructor() {
}
DB_EXPORT void Destructor() {
}
DB_EXPORT void SetErrorHandler(LPVOID pErrorHandlerPtr) {
	if (pErrorHandlerPtr) {} //unused
}
DB_EXPORT void PassCoreData(LPVOID pGlobPtr) {
	g_pGlob = reinterpret_cast<GlobStruct *>(pGlobPtr);
}

#ifdef DARKSDK_COMPILE
void ConstructorTransforms() {
	Constructor();
}
void DestructorTransforms() {
	Destructor();
}
void SetErrorHandlerTransforms(LPVOID pErrorHandlerPtr) {
	SetErrorHandler(pErrorHandlerPtr);
}
void PassCoreDataTransforms(LPVOID pGlobPtr) {
	PassCoreData(pGlobPtr);
}
#endif

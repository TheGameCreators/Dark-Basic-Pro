#include "Error.h"

bool DBError::objectExists(int objectID, string function){
	bool result = dbObjectExist(objectID);
	if(!result){
		Message::objectNotExist(objectID, function);
	}
	return result;
}


bool DBError::limbExists(int objectID, int limbID, string function){
	if(!objectExists(objectID, function))
		return false;
	bool result = dbLimbExist(objectID, limbID);
	if(!result){
		Message::limbNotExist(objectID, limbID, function);
	}
	return result;
}

bool DBError::meshExists(int objectID, string function){
	if(!objectExists(objectID, function)){
       return false;
	}
	sObject* obj;
	obj = dbGetObject(objectID);
    if(obj->pFrame->pMesh == 0){
		Message::meshNotExist(objectID, function);
	    return false;
	}
	else
		return true;
}

bool DBError::meshExists(int objectID, int limbID, string function){
	if(!objectExists(objectID, function) || !limbExists(objectID, limbID, function)){
       return false;
	}
	sObject* obj;
	obj = dbGetObject(objectID);
    if( obj->ppFrameList[limbID]->pMesh == 0){
		Message::meshNotExist(objectID, limbID, function);
	    return false;
	}
	else
		return true;
}

bool DBError::boneMeshExists(int objectID, int limbID, string function){
	if(!objectExists(objectID, function) || !limbExists(objectID, limbID, function)){
       return false;
	}
	sObject* obj;
	obj = dbGetObject(objectID);
    if( obj->ppMeshList[limbID] == 0){
		Message::meshNotExist(objectID, limbID, function);
	    return false;
	}
	else
		return true;
}

bool DBError::fileExists(string file, string function){
    FILE* fp = NULL;
    //will not work if you do not have read permissions
    //to the file, but if you don't have read, it
    //may as well not exist to begin with.
    fp = fopen( file.c_str(), "rb" );
    if( fp != NULL )
    {
        fclose( fp );
        return true;
    }
	else{
		Message::fileNotExist(file, function);
		return false;
	}
	
}

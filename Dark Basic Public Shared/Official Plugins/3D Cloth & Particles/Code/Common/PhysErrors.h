#ifndef DBPROPHYSICS_PHYSERRORS_H
#define DBPROPHYSICS_PHYSERRORS_H


#define Stringize( L )			#L
#define MakeString( M, L )		M(L)
#define $Line					MakeString( Stringize, __LINE__ )

#if _DEBUG
#define MakeReminder(msg) message(__FILE__ "(" $Line ") : warning: " msg)
#else
#define MakeReminder(msg) message(__FILE__ "(" $Line ") : error: " msg)
#endif


#define ON_FAIL_DLL_SECURITY_RETURN(retval) DWORD * pErrPtr = (DWORD*)physics->DBPro_globalPtr->g_pErrorHandlerRef;\
DWORD * pESPPtr = pErrPtr + 1;\
DWORD dwRecordedESP = *pESPPtr;\
DWORD dwCurrentESP;\
__asm mov dwCurrentESP, EBP;\
int iDifference = abs ( dwCurrentESP - dwRecordedESP );\
if ( iDifference > 8 ) return retval;



enum ePhysError{
PERR_AOK = 0,
PERR_OUT_OF_MEMORY,
PERR_ITEM_EXISTS,
PERR_ITEM_DOESNT_EXIST,
PERR_WRONG_TYPE,
PERR_BAD_PARAMETER,
PERR_NULL_VECTOR,
PERR_EFF_INCOMPATIBLE_PARTICLE,
PERR_EFF_INCOMPATIBLE_CLOTH,
PERR_EFF_INCOMPATIBLE_RAGDOLL,
PERR_EFF_INCOMPATIBLE_VEHICLE,
PERR_COL_INCOMPATIBLE_PARTICLE,
PERR_COL_INCOMPATIBLE_CLOTH,
PERR_COL_INCOMPATIBLE_RAGDOLL,
PERR_COL_INCOMPATIBLE_VEHICLE,
PERR_CLOTH_NOT_GENERATED,
PERR_CLOTH_ALREADY_GENERATED,
PERR_SUSPENSION_DOESNT_EXIST,
PERR_IMAGE_DOESNT_EXIST,
PERR_NO_VALID_PIXELS_IN_IMAGE,
PERR_CANT_CREATE_SAVE_FILE,
PERR_CANT_LOAD_FILE,
PERR_IO_FILE_ERROR,
PERR_INCORRECT_FILE_FORMAT,
PERR_NO_DATA_IN_OBJECT,
PERR_KEY_DOESNT_EXIST,
PERR_OBJECT_NOT_LINKED,
PERR_BAD_MODULE
};

#define MAX_PERR_VALUE PERR_BAD_MODULE

extern ePhysError lastError;
extern void reportError(ePhysError err, enum ePhysCommand cmd);

//Two #defines used to report errors
#define RETURN_ON_PERR(routine,usercmd) if((lastError = routine)!=PERR_AOK)\
{reportError(lastError,usercmd);return;}

#define RETURNVALUE_ON_PERR(routine,usercmd,returnVal) if((lastError = routine)!=PERR_AOK)\
{reportError(lastError,usercmd);return returnVal;}

#define ALERT_ON_PERR(routine,usercmd) if((lastError = routine)!=PERR_AOK)\
{reportError(lastError,usercmd);}


#endif
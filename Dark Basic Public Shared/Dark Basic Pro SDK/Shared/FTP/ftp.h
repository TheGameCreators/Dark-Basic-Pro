//
// DARKBASIC FTP HEADER
//

// Includes
#include "wininet.h"

// External Functions Defined

// Initialisation
extern void		FTP_Reset(void);
extern bool		FTP_ConnectEx(char* url, char* username, char* password, int iUseWindows);
extern bool		FTP_Connect(char* url, char* username, char* password);
extern bool		FTP_Disconnect(int dialupdisconnect);
extern void		FTP_CloseCompletely(void);

// Handling
extern bool		FTP_SetDir(char* ftpdir);
extern bool		FTP_FindFirst(void);
extern bool		FTP_FindNext(void);
extern void		FTP_ControlDownload(int size);
extern void		FTP_TerminateDownload(void);
extern bool		FTP_GetFile(char* ftpfile, char* localfile, DWORD optionalfilesize, DWORD useprogress);
extern bool		FTP_PutFile(char* localfile);
extern bool		FTP_DeleteFile(char* ftpfile);

// Expressions
extern int		FTP_GetStatus(void);
extern int		FTP_GetFailureState(void);
extern void		FTP_GetError(char* errorstring);
extern bool		FTP_GetDir(char* ftpdir);
extern int		FTP_GetFileType(void);
extern bool		FTP_GetFileName(char* ftpfile);
extern int		FTP_GetFileSize(void);
extern int		FTP_GetProgress(void);


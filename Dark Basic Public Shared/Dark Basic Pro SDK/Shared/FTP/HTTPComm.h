//
// HTTP Comm Header
//

// Functions to access an HTTP Secure Server
bool	HTTP_Connect		( char* url );
// 20120416 IanM - Allow higher range of port numbers, to allow flags to be sent in the high bits
bool	HTTP_Connect		( char* url, DWORD port );
bool	HTTP_Connect		( char* url, DWORD port, int secure );
LPSTR	HTTP_RequestData	( LPSTR pVerb, LPSTR pObjectName, LPSTR pHeaderString, DWORD dwHeaderSize, LPSTR pPostData, DWORD dwPostDataSize );
LPSTR	HTTP_RequestData	( LPSTR pVerb, LPSTR pObjectName, LPSTR pHeaderString, DWORD dwHeaderSize, LPSTR pPostData, DWORD dwPostDataSize, DWORD dwAccessFlag );
bool	HTTP_Disconnect		( void );

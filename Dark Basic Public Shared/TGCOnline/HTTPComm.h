//
// HTTP Comm Header
//

// Functions to access an HTTP Secure Server
bool	HTTP_Connect		( char* url );
LPSTR	HTTP_RequestData	( LPSTR pVerb, LPSTR pObjectName, LPSTR pHeaderString, DWORD dwHeaderSize, LPSTR pPostData, DWORD dwPostDataSize );
bool	HTTP_Disconnect		( void );

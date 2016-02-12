//
// External Text Reader
//

// Defines
#define EXTWORDSMAX 160

// Externals
extern char g_pTGCWord [ EXTWORDSMAX ][ 1024 ];

// Prototypes
void GatherAllExternalWords(LPSTR pWordsFile);
bool FindExternalWords(void);

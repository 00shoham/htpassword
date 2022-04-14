#ifndef _INCLUDE_PASSWORD_HISTORY
#define _INCLUDE_PASSWORD_HISTORY

int AddPasswordToHistory( char* lockPath, char* histFile, char* userID, char* password );
int IsPasswordInHistory( char* histFile, char* userID, char* password );
int RemoveUserFromHistory( char* lockPath, char* histFile, char* userID );

#endif

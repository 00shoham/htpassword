#ifndef _INCLUDE_PASSWORD_FILE
#define _INCLUDE_PASSWORD_FILE

typedef struct _adminUser
  {
  char* id;
  struct _adminUser* next;
  } _ADMIN_USER;

typedef struct _dictionaryFile
  {
  char* path;
  struct _dictionaryFile* next;
  } _DICTIONARY_FILE;

typedef struct _passwordPolicy
  {
  int minLength;
  int maxLength;
  int minLetters;
  int maxLetters;
  int minDigits;
  int maxDigits;
  int minPunct;
  int maxPunct;
  int requireMixedCase;
  char* hibpAPIKey;
  _DICTIONARY_FILE* dictionaries;
  } _PASSWORD_POLICY;

#define DEFAULT_POLICY_MIN_LENGTH              7
#define DEFAULT_POLICY_MAX_LENGTH              50
#define DEFAULT_POLICY_MIN_LETTERS             1
#define DEFAULT_POLICY_MAX_LETTERS             -1
#define DEFAULT_POLICY_MIN_DIGITS              1
#define DEFAULT_POLICY_MAX_DIGITS              -1
#define DEFAULT_POLICY_MIN_PUNCT               0
#define DEFAULT_POLICY_MAX_PUNCT               -1
#define DEFAULT_POLICY_REQUIRE_MIXED_CASE      1

typedef struct _passwordFile
  {
  char* id;
  char* description;
  char* path;
  _ADMIN_USER* admins;
  _PASSWORD_POLICY policy;
  char* historyFile;
  char* lockFile;
  struct _passwordFile* next;
  } _PASSWORD_FILE;

_ADMIN_USER* AddAdminUser( char* id, _ADMIN_USER* list );
int DeleteAdminUser( _ADMIN_USER** list, char* id ); 

_DICTIONARY_FILE* AddDictionaryFile( char* path, _DICTIONARY_FILE* list );
int DeleteDictionaryFile( _DICTIONARY_FILE** list, char* path ); 

int SetDefaultPolicy( _PASSWORD_POLICY* policy );

_PASSWORD_FILE* AddPasswordFile( char* id, _PASSWORD_FILE* list );
_PASSWORD_FILE* HTPasswdFileFromID( _CONFIG* conf, char* id );
int HTPasswdIsUserAdmin( _PASSWORD_FILE* pf, char* userID );

_PASSWORD_FILE* AddPasswordFile( char* id, _PASSWORD_FILE* list );
_PASSWORD_FILE* HTPasswdFileFromID( _CONFIG* conf, char* id );

#endif

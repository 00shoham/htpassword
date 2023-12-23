#ifndef _INCLUDE_CONFIG
#define _INCLUDE_CONFIG

#define CONFIGDIR "/usr/local/etc/"
#define CONFIGFILE "htpassword.ini"

#define DEFAULT_USER_ENV_VAR "REMOTE_USER"

typedef struct _config
  {
  int currentlyParsing;
  _TAG_VALUE* parserLocation;

  /* to avoid duplicate includes */
  _TAG_VALUE *includes;

  /* macro expansion */
  _TAG_VALUE *list;

  /* used in installers and diagnostics */
  int listIncludes;
  int includeCounter;

  /* the meat of it */
  _PASSWORD_FILE* passwordFiles;

  char* myCSS;

  char* userEnvVar;
  uint8_t key[AES_KEYLEN];
  char* sessionCookieName;
  char* authServiceUrl;
  char* urlEnvVar;
  } _CONFIG;

void SetDefaults( _CONFIG* config );
void ReadConfig( _CONFIG* config, char* filePath );
void PrintConfig( FILE* f, _CONFIG* config );
void FreeConfig( _CONFIG* config );
void ValidateConfig( _CONFIG* config );

#endif

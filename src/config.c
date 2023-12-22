#include "base.h"

char defaultKey[] =
  {
  0xdb, 0xd2, 0x5a, 0x74, 0xc7, 0x44, 0x3b, 0x43,
  0xd2, 0x24, 0xcb, 0xce, 0x63, 0x4a, 0xe5, 0x4d,
  0x5d, 0x64, 0xd6, 0xcc, 0xcb, 0x97, 0x75, 0x2b,
  0x95, 0xa8, 0x52, 0x85, 0xe6, 0xed, 0x32, 0xc1
  };

void SetDefaults( _CONFIG* config )
  {
  memset( config, 0, sizeof(_CONFIG) );

  memcpy( config->key, defaultKey, AES_KEYLEN );
  config->userEnvVar = strdup( DEFAULT_USER_ENV_VAR );
  config->sessionCookieName = strdup( COOKIE_ID );
  config->authServiceUrl = strdup( DEFAULT_AUTH_URL );
  config->urlEnvVar = strdup( DEFAULT_REQUEST_URI_ENV_VAR );
  }

void FreeConfig( _CONFIG* config )
  {
  if( config==NULL )
    return;

  for( _PASSWORD_FILE* pf = config->passwordFiles; pf!=NULL; )
    {
    FreeIfAllocated( &(pf->id) );
    FreeIfAllocated( &(pf->description) );
    FreeIfAllocated( &(pf->path) );
    FreeIfAllocated( &(pf->historyFile) );
    FreeIfAllocated( &(pf->lockFile) );

    for( _ADMIN_USER* au = pf->admins; au!=NULL; )
      {  
      FreeIfAllocated( &(au->id) );
      _ADMIN_USER* nau = au->next;
      free( au );
      au = nau;
      }

    for( _DICTIONARY_FILE* df = pf->policy.dictionaries; df!=NULL; )
      {
      FreeIfAllocated( &(df->path) );
      _DICTIONARY_FILE* ndf = df->next;
      free( df );
      df = ndf;
      }
     
    _PASSWORD_FILE* npf = pf->next;
    free( pf );
    pf = npf;
    }

  FreeTagValue( config->includes );
  FreeTagValue( config->list );

  FreeIfAllocated( &(config->userEnvVar) );
  FreeIfAllocated( &(config->sessionCookieName) );
  FreeIfAllocated( &(config->authServiceUrl) );
  FreeIfAllocated( &(config->urlEnvVar) );

  free( config );
  }

void ProcessConfigLine( char* ptr, char* equalsChar, _CONFIG* config )
  {
  *equalsChar = 0;

  char* variable = TrimHead( ptr );
  TrimTail( variable );
  char* value = TrimHead( equalsChar+1 );
  char* originalValue = value;
  TrimTail( value );

  if( NOTEMPTY( variable ) && NOTEMPTY( value ) )
    {
    char valueBuf[BUFLEN];

    /* expand any macros in the value */
    if( strchr( value, '$' )!=NULL )
      {
      int loopMax = 10;
      while( loopMax>0 )
        {
        int n = ExpandMacros( value, valueBuf, sizeof( valueBuf ), config->list );
        if( n>0 )
          {
          /* don't free the original value - that's done elsewhere */
          if( value!=NULL && value!=originalValue )
            {
            free( value );
            }
          value = strdup( valueBuf );
          }
        else
          {
          break;
          }
        --loopMax;
        }
      }

    config->list = NewTagValue( variable, value, config->list, 1 );

    if( strcasecmp( variable, "PASSWORD_FILE" )==0 )
      {
      config->passwordFiles = AddPasswordFile( value, config->passwordFiles );
      }
    else if( strcasecmp( variable, "PATH" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      if( FileExists( value )!=0 )
        Error( "Path [%s] does not exist", value );
      FreeIfAllocated( &(config->passwordFiles->path) );
      config->passwordFiles->path = strdup( value );
      }
    else if( strcasecmp( variable, "DESCRIPTION" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      FreeIfAllocated( &(config->passwordFiles->description) );
      config->passwordFiles->description = strdup( value );
      }
    else if( strcasecmp( variable, "ADMIN_USER" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      config->passwordFiles->admins = AddAdminUser( value, config->passwordFiles->admins );
      }
    else if( strcasecmp( variable, "MIN_LENGTH" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      int n = atoi( value );
      if( n < 6 )
        Error( "%s must be at least 6", variable );
      config->passwordFiles->policy.minLength = n;
      }
    else if( strcasecmp( variable, "MAX_LENGTH" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      int n = atoi( value );
      if( n == -1 || n>10 ) { /* ok */ }
      else
        Error( "%s must be either -1 (none) or more than 10", variable );
      config->passwordFiles->policy.maxLength = n;
      }
    else if( strcasecmp( variable, "MIN_LETTERS" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      int n = atoi( value );
      if( n < -1 || n==0 || n>50 )
        Error( "%s must be -1 (none) or >0 (min) but no more than 50 (crazy)", variable );
      config->passwordFiles->policy.minLetters = n;
      }
    else if( strcasecmp( variable, "MAX_LETTERS" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      int n = atoi( value );
      if( n == -1 || n>10 ) { /* ok */ }
      else
        Error( "%s must be either -1 (none) or more than 10", variable );
      config->passwordFiles->policy.maxLetters = n;
      }
    else if( strcasecmp( variable, "MIN_DIGITS" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      int n = atoi( value );
      if( n < -1 || n>5 )
        Error( "%s must be -1|0 (none) or up to 5", variable );
      config->passwordFiles->policy.minDigits = n;
      }
    else if( strcasecmp( variable, "MAX_DIGITS" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      int n = atoi( value );
      if( n == -1 || n>1 ) { /* ok */ }
      else
        Error( "%s must be either -1 (none) or more than 1", variable );
      config->passwordFiles->policy.maxDigits = n;
      }
    else if( strcasecmp( variable, "MIN_PUNCT" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      int n = atoi( value );
      if( n < -1 || n>5 )
        Error( "%s must be -1|0 (none) or up to 5", variable );
      config->passwordFiles->policy.minPunct = n;
      }
    else if( strcasecmp( variable, "MAX_PUNCT" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      int n = atoi( value );
      if( n == -1 || n>10 ) { /* ok */ }
      else
        Error( "%s must be either -1 (none) or more than 10", variable );
      config->passwordFiles->policy.maxPunct = n;
      }
    else if( strcasecmp( variable, "MIXED_CASE" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      int n = -1;
      if( strcasecmp( value, "true" )==0 )
        n = 1;
      else if( strcasecmp( value, "false" )==0 )
        n = 0;
      else
        n = atoi( value );
      if( n != 0 && n!=1 )
        Error( "%s must be either 0|false or 1|true", variable );
      config->passwordFiles->policy.requireMixedCase = n;
      }
    else if( strcasecmp( variable, "DICTIONARY" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      config->passwordFiles->policy.dictionaries =
        AddDictionaryFile( value, config->passwordFiles->policy.dictionaries );
      }
    else if( strcasecmp( variable, "HIBP_KEY" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      FreeIfAllocated( &(config->passwordFiles->policy.hibpAPIKey) );
      config->passwordFiles->policy.hibpAPIKey = strdup( variable );
      }
    else if( strcasecmp( variable, "HISTORY_FILE" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      if( FileExists( value ) !=0 )
        {
        if( Touch( value )!=0 )
          Error( "Cannot open history file %s", value );
        }
      FreeIfAllocated( &(config->passwordFiles->historyFile) );
      config->passwordFiles->historyFile = strdup( value );
      }
    else if( strcasecmp( variable, "LOCK_FILE" )==0 )
      {
      if( config->passwordFiles==NULL )
        Error( "%s must follow PASSWORD_FILE", variable );
      if( FileExists( value ) !=0 )
        {
        if( Touch( value )!=0 )
          Error( "Lock file %s does not exist", value );
        }
      FreeIfAllocated( &(config->passwordFiles->lockFile) );
      config->passwordFiles->lockFile = strdup( value );
      }
    else if( strcasecmp( variable, "USER_ENV_VARIABLE" )==0 )
      {
      FreeIfAllocated( &(config->userEnvVar) );
      config->userEnvVar = strdup( value );
      }
    else if( strcasecmp( variable, "SESSION_COOKIE_NAME" )==0 )
      {
      FreeIfAllocated( &(config->sessionCookieName) );
      config->sessionCookieName = strdup( value );
      }
    else if( strcasecmp( variable, "AUTHENTICATION_SERVICE_URL" )==0 )
      {
      FreeIfAllocated( &(config->authServiceUrl) );
      config->authServiceUrl = strdup( value );
      }
    else if( strcasecmp( variable, "URL_ENV_VARIABLE" )==0 )
      {
      FreeIfAllocated( &(config->urlEnvVar) );
      config->urlEnvVar = strdup( value );
      }
    else if( strcasecmp( variable, "SESSION_COOKIE_ENCRYPTION_KEY" )==0 )
      {
      uint8_t binaryKey[100];
      memset( binaryKey, 0, sizeof(binaryKey) );
      UnescapeString( value, binaryKey, sizeof(binaryKey)-1 );
      memset( config->key, 0, AES_KEYLEN );
      memcpy( config->key, binaryKey, AES_KEYLEN );
      }
    else
      {
      /* append this variable to our linked list, for future expansion */
      /* do this always, so not here for just
         invalid commands:
         config->list = NewTagValue( variable, value, config->list, 1 );
      */
      }
    }
  }

void PrintPasswordPolicy( FILE* f, _PASSWORD_POLICY* policy )
  {
  if( f==NULL )
    Error("Cannot print configuration to NULL file");

  if( policy==NULL )
    Error("Cannot print NULL password policy");

  fprintf( f, "MIN_LENGTH=%d\n", policy->minLength );
  fprintf( f, "MAX_LENGTH=%d\n", policy->maxLength );
  fprintf( f, "MIN_LETTERS=%d\n", policy->minLetters );
  fprintf( f, "MAX_LETTERS=%d\n", policy->maxLetters );
  fprintf( f, "MIN_DIGITS=%d\n", policy->minDigits );
  fprintf( f, "MAX_DIGITS=%d\n", policy->maxDigits );
  fprintf( f, "MIN_PUNCT=%d\n", policy->minPunct );
  fprintf( f, "MAX_PUNCT=%d\n", policy->maxPunct );
  fprintf( f, "MIXED_CASE=%d\n", policy->requireMixedCase );
  for( _DICTIONARY_FILE* df = policy->dictionaries; df!=NULL; df=df->next )
    {
    fprintf( f, "DICTIONARY=%s\n", NULLPROTECT( df->path ) );
    }
  }

void PrintConfig( FILE* f, _CONFIG* config )
  {
  if( f==NULL )
    Error("Cannot print configuration to NULL file");

  if( config==NULL )
    Error("Cannot print NULL configuration");

  if( memcmp( config->key, defaultKey, AES_KEYLEN )!=0 )
    {
    char key_ascii[100];
    fprintf( f, "SESSION_COOKIE_ENCRYPTION_KEY=%s\n", EscapeString( config->key, AES_KEYLEN, key_ascii, sizeof( key_ascii ) ) );
    }

  for( _PASSWORD_FILE* pf = config->passwordFiles; pf!=NULL; pf=pf->next )
    {
    fprintf( f, "PASSWORD_FILE=%s\n", NULLPROTECT( pf->id ) );
    fprintf( f, "DESCRIPTION=%s\n", NULLPROTECT( pf->description ) );
    fprintf( f, "PATH=%s\n", NULLPROTECT( pf->path ) );
    for( _ADMIN_USER* au = pf->admins; au!=NULL; au=au->next )
      fprintf( f, "ADMIN_USER=%s\n", NULLPROTECT( au->id ) );
    PrintPasswordPolicy( f, &(pf->policy) );
    if( NOTEMPTY( pf->historyFile ) )
      fprintf( f, "HISTORY_FILE=%s\n", pf->historyFile );
    if( NOTEMPTY( pf->lockFile ) )
      fprintf( f, "LOCK_FILE=%s\n", pf->lockFile );
    }

  if( NOTEMPTY( config->userEnvVar )
      && strcmp( config->userEnvVar, DEFAULT_USER_ENV_VAR )!=0 )
    {
    fprintf( f, "USER_ENV_VARIABLE=%s\n", config->userEnvVar );
    }

  if( NOTEMPTY( config->sessionCookieName )
      && strcmp( config->sessionCookieName, COOKIE_ID )!=0 )
    {
    fprintf( f, "SESSION_COOKIE_NAME=%s\n", config->sessionCookieName );
    }

  if( NOTEMPTY( config->authServiceUrl )
      && strcmp( config->authServiceUrl, DEFAULT_AUTH_URL )!=0 )
    {
    fprintf( f, "SESSION_COOKIE_NAME=%s\n", config->authServiceUrl );
    }

  if( NOTEMPTY( config->urlEnvVar )
      && strcmp( config->urlEnvVar, DEFAULT_REQUEST_URI_ENV_VAR )!=0 )
    {
    fprintf( f, "URL_ENV_VARIABLE=%s\n", config->urlEnvVar );
    }
  }

void ReadConfig( _CONFIG* config, char* filePath )
  {
  char folder[BUFLEN];
  folder[0] = 0;
  (void)GetFolderFromPath( filePath, folder, sizeof( folder )-1 );

  if( EMPTY( filePath ) )
    {
    Error( "Cannot read configuration file with empty/NULL name");
    }

  FILE* f = fopen( filePath, "r" );
  if( f==NULL )
    {
    Error( "Failed to open configuration file %s", filePath );
    }
  config->parserLocation = NewTagValue( filePath, "", config->parserLocation, 0 );
  config->parserLocation->iValue = 0;
  ++ ( config->currentlyParsing );

  char buf[BUFLEN];
  char* endOfBuf = buf + sizeof(buf)-1;
  while( fgets(buf, sizeof(buf)-1, f )==buf )
    {
    ++(config->parserLocation->iValue);

    char* ptr = TrimHead( buf );
    TrimTail( ptr );

    while( *(ptr + strlen(ptr) - 1)=='\\' )
      {
      char* startingPoint = ptr + strlen(ptr) - 1;
      if( fgets(startingPoint, endOfBuf-startingPoint-1, f )!=startingPoint )
        {
        ++(config->parserLocation->iValue);
        break;
        }
      ++config->parserLocation->iValue;
      TrimTail( startingPoint );
      }

    if( *ptr==0 )
      {
      continue;
      }

    if( *ptr=='#' )
      {
      ++ptr;
      if( strncmp( ptr, "include", 7 )==0 )
        { /* #include */
        ptr += 7;
        while( *ptr!=0 && ( *ptr==' ' || *ptr=='\t' ) )
          {
          ++ptr;
          }
        if( *ptr!='"' )
          {
          Error("#include must be followed by a filename in \" marks.");
          }
        ++ptr;
        char* includeFileName = ptr;
        while( *ptr!=0 && *ptr!='"' )
          {
          ++ptr;
          }
        if( *ptr=='"' )
          {
          *ptr = 0;
          }
        else
          {
          Error("#include must be followed by a filename in \" marks.");
          }

        int redundantInclude = 0;
        for( _TAG_VALUE* i=config->includes; i!=NULL; i=i->next )
          {
          if( NOTEMPTY( i->tag ) && strcmp( i->tag, includeFileName )==0 )
            {
            redundantInclude = 1;
            break;
            }
          }

        if( redundantInclude==0 )
          {
          config->includes = NewTagValue( includeFileName, "included", config->includes, 1 );

          if( config->listIncludes )
            {
            if( config->includeCounter )
              {
              fputs( " ", stdout );
              }
            fputs( includeFileName, stdout );
            ++config->includeCounter;
            }

          char* confPath = SanitizeFilename( CONFIGDIR, NULL, includeFileName, 0 );
          if( FileExists( confPath )==0 )
            {
            ReadConfig( config, confPath );
            }
          else
            {
            confPath = SanitizeFilename( folder, NULL, includeFileName, 0 );
            if( FileExists( confPath )==0 )
              {
              ReadConfig( config, confPath );
              }
            else
              {
              Warning( "Cannot open #include \"%s\" -- skipping.",
                       confPath );
              }
            }
          }
        }
      else if( strncmp( ptr, "print", 5 )==0 )
        { /* #print */
        ptr += 5;
        while( *ptr!=0 && ( *ptr==' ' || *ptr=='\t' ) )
          {
          ++ptr;
          }
        if( *ptr!='"' )
          {
          Error("#include must be followed by a filename in \" marks.");
          }
        ++ptr;
        char* printFileName = ptr;
        while( *ptr!=0 && *ptr!='"' )
          {
          ++ptr;
          }
        if( *ptr=='"' )
          {
          *ptr = 0;
          }
        else
          {
          Error("#print must be followed by a filename in \" marks.");
          }

        FILE* printFile = fopen( printFileName, "w" );
        if( printFile==NULL )
          {
          Error( "Could not open/create %s to print configuration.",
                 printFileName );
          }
        PrintConfig( printFile, config );
        fclose( printFile );
        Notice( "Printed configuration to %s.", printFileName );
        }
      else if( strncmp( ptr, "exit", 4 )==0 )
        { /* #exit */
        ptr += 4;
        ValidateConfig( config );
        Notice( "Exit program due to command in config file." );
        exit(0);
        }

      /* not #include or #include completely read by now */
      continue;
      }

    /* printf("Processing [%s]\n", ptr ); */
    char* equalsChar = NULL;
    for( char* eolc = ptr; *eolc!=0; ++eolc )
      {
      if( equalsChar==NULL && *eolc == '=' )
        {
        equalsChar = eolc;
        }

      if( *eolc == '\r' || *eolc == '\n' )
        {
        *eolc = 0;
        break;
        }
      }

    if( *ptr!=0 && equalsChar!=NULL && equalsChar>ptr )
      {
      ProcessConfigLine( ptr, equalsChar, config );
      }
    }

  /* unroll the stack of config filenames after ReadConfig ended */
  _TAG_VALUE* tmp = config->parserLocation->next;
  if( config->parserLocation->tag!=NULL ) { FREE( config->parserLocation->tag ); }
  if( config->parserLocation->value!=NULL ) { FREE( config->parserLocation->value ); }
  FREE( config->parserLocation );
  config->parserLocation = tmp;

  -- ( config->currentlyParsing );

  fclose( f );
  }

void ValidateConfig( _CONFIG* config )
  {
  if( config==NULL )
    Error( "Cannot validate a NULL configuration" );

  if( config->passwordFiles==NULL )
    Error( "Must specify at least one PASSWORD_FILE in config" );

  for( _PASSWORD_FILE* pf=config->passwordFiles; pf!=NULL; pf=pf->next )
    {
    if( EMPTY( pf->id ) )
      Error( "Password file with no ID" );
    if( EMPTY( pf->path ) )
      Error( "Password file with no path" );
    if( FileExists( pf->path )!=0 )
      Error( "Password file path [%s] cannot be opened", pf->path );
    if( EMPTY( pf->description ) )
      Error( "Password file %s with no description", pf->id );
    if( pf->admins==NULL )
      Error( "Password file %s must specify at least one admin user", pf->id );
    if( NOTEMPTY( pf->historyFile ) )
      {
      if( EMPTY( pf->lockFile ) )
        Error( "When specifying a history file (%s), you must also specify a lock file",
               pf->historyFile );
      if( FileExists( pf->historyFile )!=0 )
        Error( "Cannot access history file %s", pf->historyFile );
      }

    for( _DICTIONARY_FILE* df = pf->policy.dictionaries; df!=NULL; df=df->next )
      {
      if( EMPTY( df->path ) )
        Error( "Dictionary file under password file %s empty", pf->id );
      if( FileExists( df->path )!=0 )
        Error( "Cannot access dictionary file %s", df->path );
      }

    if( pf->admins==NULL )
      Error( "Password file %s must have at least one admin user", pf->id );
    for( _ADMIN_USER* au = pf->admins; au!=NULL; au=au->next )
      {
      if( EMPTY( au->id ) || StringMatchesUserIDFormat( au->id )!=0 )
        Error( "Undefined or invalid admin user in password file %s", pf->id );
      }
    }
  }



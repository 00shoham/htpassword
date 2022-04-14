#include "base.h"

#define LOCKFILE "/tmp/.htpasswd.test.lock"

enum operation { op_invalid, op_add, op_isuser, op_testpass, op_del, op_reset, op_change };

void Usage( char* cmd )
  {
  printf( "USAGE: %s ... arguments ... \n", cmd );
  printf( "Where arguments may be:\n");
  printf( "    -c FILENAME   config file\n");
  printf( "    -d DIRECTORY  location of config file\n");
  printf( "    -u USERID     user ID\n");
  printf( "    -p PASSWORD   proposed password\n");
  printf( "    -h            print this\n");
  exit(0);
  }

void PrintPasswordWarning( int err, _TAG_VALUE* messages )
  {
  printf( "Error %d\n", err );
  for( ; messages!=NULL; messages=messages->next )
    {
    if( NOTEMPTY( messages->value ) )
      printf( " --> %s\n", messages->value );
    }
  }

int main( int argc, char** argv )
  {
  char* confDir = ".";
  char* confFile = "config.ini";
  char* password = NULL;
  char* userID = NULL;

  for( int i=1; i<argc; ++i )
    {
    if( strcmp( argv[i], "-c" )==0 && i+1<argc )
      {
      confFile = argv[++i];
      }
    else if( strcmp( argv[i], "-d" )==0 && i+1<argc )
      {
      confDir = argv[++i];
      }
    else if( strcmp( argv[i], "-u" )==0 && i+1<argc )
      {
      userID = argv[++i];
      }
    else if( strcmp( argv[i], "-p" )==0 && i+1<argc )
      {
      password = argv[++i];
      }
    else if( strcmp( argv[i], "-h" )==0 )
      {
      printf("USAGE: %s [-c configFile] [-d configDir] [-o outFile]\n", argv[0] );
      exit(0);
      }
    else
      {
      printf("ERROR: unknown argument [%s]\n", argv[i] );
      exit(1);
      }
    }

  if( EMPTY( confDir ) )
    Error( "config dir cannot be empty" );

  if( EMPTY( confFile ) )
    Error( "config file cannot be empty" );

  if( EMPTY( password ) )
    Error( "you must specify a password to test" );

  if( EMPTY( userID ) )
    Error( "you must specify a userID to test" );

  char* confPath = MakeFullPath( confDir, confFile );
  _CONFIG* conf = (_CONFIG*)calloc( 1, sizeof( _CONFIG ) );
  if( conf==NULL ) Error( "Cannot allocate CONFIG object" );

  SetDefaults( conf );
  ReadConfig( conf, confPath );
  ValidateConfig( conf );

  int err = 0;

  _TAG_VALUE* messages = NULL;
  for( _PASSWORD_FILE* pf = conf->passwordFiles; pf!=NULL; pf=pf->next )
    {
    _PASSWORD_POLICY* pol = &(pf->policy);
    err = TestPasswordQuality( userID, password,
                               pf->historyFile,
                               pol,
                               &messages );
    if( err )
      {
      PrintPasswordWarning( err, messages );
      }
    else
      {
      if( NOTEMPTY( pf->historyFile ) )
        {
        err = AddPasswordToHistory( pf->lockFile, pf->historyFile, userID, password );
        if( err )
          Warning( "Failed to add password to history" );
        }
      }

    if( messages!=NULL )
      {
      FreeTagValue( messages );
      }
    }

  return 0;
  }


#include "base.h"

void Usage( char* cmd )
  {
  printf("USAGE: %s [-h]\n", cmd );
  }

int main( int argc, char** argv )
  {
  char* confPath = MakeFullPath( CONFIGDIR, CONFIGFILE );
  _CONFIG* conf = (_CONFIG*)calloc( 1, sizeof( _CONFIG ) );
  if( conf==NULL ) Error( "Cannot allocate CONFIG object" );

  SetDefaults( conf );
  ReadConfig( conf, confPath );
  ValidateConfig( conf );

  char* q = getenv( "QUERY_STRING" );
  int i;

  if( NOTEMPTY( q ) )
    { /* URL received - CGI action */
    if( ( strncasecmp( q, "api&", 4 )==0 || strncasecmp( q, "api/", 4 )==0 )
        && q[4]!=0 )
      {
      inCGI = 2;

      /* char* whoAmI = ExtractUserIDOrDie( cm_api, conf->userEnvVar ); */
      char* whoAmI = ExtractUserIDOrDieEx( cm_api,
                                           conf->userEnvVar, conf->sessionCookieName,
                                           conf->urlEnvVar, conf->authServiceUrl,
                                           conf->key );

      fputs( "Content-Type: application/json\r\n\r\n", stdout );
      fflush(stdout);

      CallAPIFunction( conf, whoAmI, q+4 );
      fflush(stdout);
      FreeConfig( conf );
      exit(0);
      }
    } /* URL had args */
  else
    { /* no args to URL */
    printf( "Location: /htpassword/ui.html\r\n\r\n");
    exit(0);
    }

  if( argc==1 || (argc>1 && argv[1][0]!='-') ) /* q!=NULL && *q!=0 */
    { /* CGI */
    printf( "Location: /htpassword/ui.html\r\n\r\n");
    exit(0);
    }
  else
    { /* CLI */

    for( i=1; i<argc; ++i )
      {
      if( strcmp( argv[i], "-h" )==0 )
        {
        Usage( argv[0] );
        exit(0);
        }
      else
        {
        fprintf( stderr, "Invalid option: [%s]\n", argv[i] );
        Usage( argv[0] );
        exit(-1);
        }
      }
    }

  return 0;
  }

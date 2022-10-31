#include "base.h"

int HaveIBeenPwned( char* apiKey, char* password )
  {
  if( EMPTY( apiKey ) )
    Error( "Cannot call HaveIBeenPwned without an API key" );

  if( EMPTY( password ) )
    Error( "Cannot call HaveIBeenPwned without a password" );

  char hash[50];
  SHA1hex( hash, password, strlen(password) );

  char firstFive[10];
  strncpy( firstFive, hash, 5 );
  firstFive[5] = 0;

  char* suffix = hash + 5;
  int suffixLen = strlen( suffix );

  char url[500];

  snprintf( url, sizeof(url)-1, "%s/%s", HIBP_PW_URL , firstFive );
  _DATA postResult = {0};
  char* errorMsg = NULL;

  /* need to add http header hibp-api-key: [your key] */
  _TAG_VALUE* httpHeaders = NULL;
  httpHeaders = NewTagValue( "user-agent", HIBP_MY_USERAGENT, httpHeaders, 1 );
  httpHeaders = NewTagValue( "hibp-api-key", apiKey, httpHeaders, 1 );

  /* get a bunch of lines like this:
  FC9E33E08E8CBDD5F1A8E6011897B00070B:5
  The :nn is the number of compromised passwords that match.
  */

  int err = WebTransaction( url, HTTP_GET, NULL, 0, "application/json", 
                            &postResult, NULL, NULL,
                            NULL, NULL, NULL,
                            HIBP_TIMEOUT,
                            httpHeaders,
                            NULL,
                            0, 0, 
                            &errorMsg );

  if( err )
    {
    Warning( "Trying to connect to %s - got error %d:%s",
             HIBP_PW_URL, err, NULLPROTECT( errorMsg ) );
    return -1;
    }

  int nMatches = 0;
  if( postResult.data==NULL )
    {
    Warning( "No data in HTTP result from %s", HIBP_PW_URL );
    return -2;
    }
  else
    {
    char* ptr = NULL;
    for( char* line = strtok_r( (char*)postResult.data, "\r\n", &ptr );
         line != NULL;
         line = strtok_r( NULL, "\r\n", &ptr ) )
      {
      if( strncasecmp( line, suffix, suffixLen )==0 )
        {
        ++nMatches;
        break;
        }
      }
    }

  if( nMatches )
    return 0;
  else
    return -3;
  }


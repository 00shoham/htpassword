#include "base.h"

#define SUCCESS "success"
#define ERROR "error"
#define BAD_PASSWORD "Password not strong enough"
#define WRONG_PASSWORD "Incorrect password"

void SendShortResponse( int code, char* result, _TAG_VALUE* response )
  {
  char responseBuf[BUFLEN];
  response = NewTagValueInt( "code", code, response, 1 );
  response = NewTagValue( "result", result, response, 1 );
  ListToJSON( response, responseBuf, sizeof(responseBuf)-1 );
  fputs( responseBuf, stdout );
  fputs( "\r\n", stdout );
  }

void SimpleError( char* msg )
  {
  printf( "<html><body><b>ERROR: %s\n</b></body></html>\n",
          msg );
  }

void BasicValidationNoUserID( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args,
                              _PASSWORD_FILE** pfPtr )
  {
  if( pfPtr==NULL )
    APIError( methodName, -999, "Internal validation usage bug" );

  if( conf==NULL )
    APIError( methodName, -1, "No config provided" );

  if( conf->passwordFiles==NULL )
    APIError( methodName, -2, "No .htpasswd files defined" );

  if( userID==NULL )
    APIError( methodName, -3, "Use of this function requires authentication" );

  char* pfID = GetTagValue( args, "ID" );
  if( EMPTY( pfID ) )
    APIError( methodName, -4, "An .htpasswd file ID must be specified in %s", methodName );

  _PASSWORD_FILE* pf = HTPasswdFileFromID( conf, pfID );
  if( pf==NULL )
    APIError( methodName, -5, "Invalid .htpasswd file ID (%s)", pfID );

  *pfPtr = pf;
  }

void BasicValidation( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args,
                      _PASSWORD_FILE** pfPtr, char** paramUserIdPtr )
  {
  if( paramUserIdPtr==NULL )
    APIError( methodName, -999, "Internal validation usage bug" );

  BasicValidationNoUserID( conf, userID, methodName, args, pfPtr );

  char* paramUserID = GetTagValue( args, "USERID" );
  if( EMPTY( paramUserID ) )
    APIError( methodName, -6, "A user ID parameter must be specified (in addition to signing in as a valid user)" );

  *paramUserIdPtr = paramUserID;
  }

void ListPasswordFiles( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  if( conf==NULL )
    APIError( methodName, -1, "No config provided" );

  if( conf->passwordFiles==NULL )
    APIError( methodName, -2, "No .htpasswd files defined" );

  printf( "{ \"result\":\"%s\", \"code\":0,\n", SUCCESS );
  printf( "  \"FILES\":[\n" );
  int gotOne = 0;
  for( _PASSWORD_FILE* pf = conf->passwordFiles; pf!=NULL; pf=pf->next )
    {
    if( gotOne )
      printf( ", \n" );
    printf( "    { \"ID\":\"%s\", \"DESCRIPTION\":\"%s\" }",
            NULLPROTECT( pf->id ), NULLPROTECT( pf->description ) );
    gotOne = 1;
    }
  printf( "\n    ]\n}\n" );
  }

void CheckPasswordStrength( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  _PASSWORD_FILE* pf = NULL;
  char* paramUserID = NULL;

  BasicValidation( conf, userID, methodName, args, &pf, &paramUserID );

  /* can run this test on yourself or on another user if you are
     an admin */
  if( strcmp( paramUserID, userID )==0 || HTPasswdIsUserAdmin( pf, userID )==0 )
    {
    /* ok */
    }
  else
    APIError( methodName, -7, "Not permitted to perform this test on user [%s]", paramUserID );

/* what if we are adding a user? */
  if( HTPasswdValidUser( pf->lockFile, pf->path, paramUserID )!=0 )
    {
    if( paramUserID != NULL )
      *paramUserID = 0;
      /* so we don't bother with history checks later */
    }
    /* APIError( methodName, -8, "User [%s] does not appear in .htpasswd file %s", paramUserID, pf->id ); */

  char* password = GetTagValue( args, "PASSWORD" );
  if( EMPTY( password ) )
    APIError( methodName, -9, "Missing PASSWORD parameter." );

  _TAG_VALUE* messages = NULL;
  int err = TestPasswordQuality( paramUserID, 
                                 password,
                                 pf->historyFile,
                                 &(pf->policy),
                                 &messages );

  if( err==0 )
    SendShortResponse( 0, SUCCESS, NULL );
  else
    {
    printf( "{ \"result\":\"%s\", \"code\":-1,\n", BAD_PASSWORD );
    printf( "  \"CONSTRAINTS\":[\n" );
    int gotOne = 0;
    for( _TAG_VALUE* tv = messages; tv!=NULL; tv=tv->next )
      {
      if( gotOne )
        printf( ", \n" );
      printf( "    \"%s\"", NULLPROTECT( tv->value ) );
      gotOne = 1;
      }
    printf( "\n    ]\n}\n" );
    }

  if( messages!=NULL )
    FreeTagValue( messages );
  }

void CheckPasswordValue( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  _PASSWORD_FILE* pf = NULL;
  char* paramUserID = NULL;

  BasicValidation( conf, userID, methodName, args, &pf, &paramUserID );

  /* can run this test on yourself or on another user if you are
     an admin */
  if( strcmp( paramUserID, userID )==0 || HTPasswdIsUserAdmin( pf, userID )==0 )
    {
    /* ok */
    }
  else
    APIError( methodName, -7, "Not permitted to perform this test on user [%s]", paramUserID );

  if( HTPasswdValidUser( pf->lockFile, pf->path, paramUserID )!=0 )
    APIError( methodName, -8, "User [%s] does not appear in .htpasswd file %s", paramUserID, pf->id );

  char* password = GetTagValue( args, "PASSWORD" );
  if( EMPTY( password ) )
    APIError( methodName, -9, "Missing PASSWORD parameter." );

  int err = HTPasswdCheckPassword( pf->lockFile, pf->path, paramUserID, password );

  if( err==0 )
    SendShortResponse( 0, SUCCESS, NULL );
  else
    SendShortResponse( err, WRONG_PASSWORD, NULL );
  }

void ChangePasswordValue( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  _PASSWORD_FILE* pf = NULL;
  char* paramUserID = NULL;

  BasicValidation( conf, userID, methodName, args, &pf, &paramUserID );

  /* can only run this function on yourself.  no admin option. */
  if( strcmp( paramUserID, userID )!=0 )
    APIError( methodName, -7, "Specified and authenticated user IDs differ" );

  if( HTPasswdValidUser( pf->lockFile, pf->path, paramUserID )!=0 )
    APIError( methodName, -8, "User [%s] does not appear in .htpasswd file %s", paramUserID, pf->id );

  char* oldp = GetTagValue( args, "OLDP" );
  if( EMPTY( oldp ) )
    APIError( methodName, -9, "Missing OLDP parameter." );

  char* newp = GetTagValue( args, "NEWP" );
  if( EMPTY( newp ) )
    APIError( methodName, -10, "Missing NEWP parameter." );

  if( strcmp( oldp, newp )==0 )
    APIError( methodName, -11, "Old and new passwords the same." );

  int err = 0;

  _TAG_VALUE* messages = NULL;
  err = TestPasswordQuality( paramUserID, 
                             newp,
                             pf->historyFile,
                             &(pf->policy),
                             &messages );
  char* violations = AggregateMessages( messages );
  if( messages )
    FreeTagValue( messages );
  if( err )
    APIError( methodName, -12, "Password change for %s failed due to policy violation (%s)",
              userID, NULLPROTECT( violations ) );

  err = HTPasswdChangePassword( pf->lockFile, pf->path, paramUserID, oldp, newp );
  if( err )
    APIError( methodName, -13, "Password change failed.  Perhaps the wrong old password?" );

  if( err==0 && NOTEMPTY( pf->historyFile ) )
    {
    err = AddPasswordToHistory( pf->lockFile, pf->historyFile, paramUserID, newp );
    if( err )
      APIError( methodName, -14, "Password changed but adding new password to history failed." );
    }

  if( err==0 )
    SendShortResponse( 0, SUCCESS, NULL );
  else
    SendShortResponse( err, ERROR, NULL );
  }

void ResetPasswordValue( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  _PASSWORD_FILE* pf = NULL;
  char* paramUserID = NULL;

  BasicValidation( conf, userID, methodName, args, &pf, &paramUserID );

  /* must be an admin to use this */
  if( HTPasswdIsUserAdmin( pf, userID )!=0 )
    APIError( methodName, -7, "Must be an admin user to call this function" );

  if( HTPasswdValidUser( pf->lockFile, pf->path, paramUserID )!=0 )
    APIError( methodName, -8, "User [%s] does not appear in .htpasswd file %s", paramUserID, pf->id );

  char* password = GetTagValue( args, "PASSWORD" );
  if( EMPTY( password ) )
    APIError( methodName, -9, "Missing PASSWORD parameter." );

  int err = 0;

  _TAG_VALUE* messages = NULL;
  err = TestPasswordQuality( paramUserID, 
                             password,
                             NULL, /* no history for admin resets */
                             &(pf->policy),
                             &messages );
  char* violations = AggregateMessages( messages );
  if( messages )
    FreeTagValue( messages );
  if( err )
    APIError( methodName, -11, "Password reset for %s failed due to policy violation (%s)",
              userID, NULLPROTECT( violations ) );

  err = HTPasswdResetPassword( pf->lockFile, pf->path, paramUserID, password );
  if( err )
    APIError( methodName, -12, "Password reset failed (%d)", err );

  /* we didn't check history but we do append to it for a reset */
  if( err==0 && NOTEMPTY( pf->historyFile ) )
    {
    err = AddPasswordToHistory( pf->lockFile, pf->historyFile, paramUserID, password );
    if( err )
      APIError( methodName, -13, "Password reset but adding new password to history failed." );
    }

  if( err==0 )
    SendShortResponse( 0, SUCCESS, NULL );
  else
    SendShortResponse( err, ERROR, NULL );
  }

void AddUserRecord( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  _PASSWORD_FILE* pf = NULL;
  char* paramUserID = NULL;

  BasicValidation( conf, userID, methodName, args, &pf, &paramUserID );

  /* must be an admin to use this */
  if( HTPasswdIsUserAdmin( pf, userID )!=0 )
    APIError( methodName, -7, "Must be an admin user to call this function" );

  if( HTPasswdValidUser( pf->lockFile, pf->path, paramUserID )==0 )
    APIError( methodName, -8, "User [%s] already exists in .htpasswd file %s", paramUserID, pf->id );

  char* password = GetTagValue( args, "PASSWORD" );
  if( EMPTY( password ) )
    APIError( methodName, -9, "Missing PASSWORD parameter." );

  int err = 0;

  _TAG_VALUE* messages = NULL;
  err = TestPasswordQuality( paramUserID, 
                             password,
                             NULL, /* new user, no history */
                             &(pf->policy),
                             &messages );
  char* violations = AggregateMessages( messages );
  if( messages )
    FreeTagValue( messages );
  if( err )
    APIError( methodName, -11, "Adding new user %s failed due to policy violation (%s)",
              paramUserID, NULLPROTECT( violations ) );

  err = HTPasswdAddUser( pf->lockFile, pf->path, paramUserID, password );
  if( err )
    APIError( methodName, -12, "Adding a user failed (%d)", err );

  /* we didn't check history but we do append to it for a reset */
  if( err==0 && NOTEMPTY( pf->historyFile ) )
    {
    err = AddPasswordToHistory( pf->lockFile, pf->historyFile, paramUserID, password );
    if( err )
      APIError( methodName, -13, "Adding a user succeeded but adding new password to history failed." );
    }

  if( err==0 )
    SendShortResponse( 0, SUCCESS, NULL );
  else
    SendShortResponse( err, ERROR, NULL );
  }

void DelUserRecord( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  _PASSWORD_FILE* pf = NULL;
  char* paramUserID = NULL;

  BasicValidation( conf, userID, methodName, args, &pf, &paramUserID );

  /* must be an admin to use this */
  if( HTPasswdIsUserAdmin( pf, userID )!=0 )
    APIError( methodName, -7, "Must be an admin user to call this function" );

  if( HTPasswdValidUser( pf->lockFile, pf->path, paramUserID )!=0 )
    APIError( methodName, -8, "User [%s] does not exist in .htpasswd file %s", paramUserID, pf->id );

  int err = 0;

  err = HTPasswdRemoveUser( pf->lockFile, pf->path, paramUserID );
  if( err )
    APIError( methodName, -12, "Removing user [%s] user failed (%d)", paramUserID, err );

  /* we didn't check history but we do append to it for a reset */
  if( err==0 && NOTEMPTY( pf->historyFile ) )
    {
    err = RemoveUserFromHistory( pf->lockFile, pf->historyFile, paramUserID );
    if( err )
      APIError( methodName, -13, "Removing user [%s] succeeded but removing their password history failed (%d).", paramUserID, err );
    }

  if( err==0 )
    SendShortResponse( 0, SUCCESS, NULL );
  else
    SendShortResponse( err, ERROR, NULL );
  }

void ListUsers( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  _PASSWORD_FILE* pf = NULL;

  BasicValidationNoUserID( conf, userID, methodName, args, &pf );

  /* must be an admin to use this */
  if( HTPasswdIsUserAdmin( pf, userID )!=0 )
    APIError( methodName, -7, "Must be an admin user to call this function" );

  printf( "{ \"result\":\"%s\", \"code\":0,\n", SUCCESS );
  printf( "  \"USERS\":[\n" );

  FILE* f = fopen( pf->path, "r" );
  if( f==NULL )
    APIError( methodName, -8, "Cannot open .htpasswd file" );

  int gotOne = 0;
  char buf[BUFLEN];
  while( fgets( buf, sizeof(buf)-1, f )==buf )
    {
    if( EMPTY( buf ) || *buf=='*' )
      continue;
    char* ptr = NULL;
    char* userID = strtok_r( buf, ":\r\n", &ptr );
    if( NOTEMPTY( userID ) && StringMatchesUserIDFormat(userID)==0 )
      {
      printf("%s    { \"ID\":\"%s\" }", gotOne?",\n":"", userID );
      gotOne = 1;
      }
    }
  fclose( f );
  printf( "\n    ]\n}\n" );
  }

void ReturnAuthenticatedUserID( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  if( conf==NULL )
    APIError( methodName, -1, "No config provided" );

  if( userID==NULL )
    APIError( methodName, -2, "Use of this function requires authentication" );

  _TAG_VALUE* response = NewTagValue( "USERID", userID, NULL, 1 );
  SendShortResponse( 0, SUCCESS, response );
  FreeTagValue( response );
  }

void IsLoggedInUserAdmin( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args )
  {
  _PASSWORD_FILE* pf = NULL;

  BasicValidationNoUserID( conf, userID, methodName, args, &pf );
  int isAdmin = HTPasswdIsUserAdmin( pf, userID );

  _TAG_VALUE* response = NewTagValue( "ISADMIN", isAdmin==0?"true":"false", NULL, 1 );
  SendShortResponse( 0, SUCCESS, response );
  FreeTagValue( response );
  }

typedef struct fnRecord
  {
  char* topic;
  char* action;
  void (*function)( _CONFIG* conf, char* userID, char* methodName, _TAG_VALUE* args );
  } _FN_RECORD;

_FN_RECORD methods[] =
  {
    { "file", "list", ListPasswordFiles },
    { "password", "checkstrength", CheckPasswordStrength },
    { "password", "checkpassword", CheckPasswordValue },
    { "password", "changepassword", ChangePasswordValue },
    { "password", "resetpassword", ResetPasswordValue },
    { "user", "add", AddUserRecord },
    { "user", "delete", DelUserRecord },
    { "user", "list", ListUsers },
    { "user", "whoami", ReturnAuthenticatedUserID },
    { "user", "isadmin", IsLoggedInUserAdmin },
    { "NULL", "NULL", NULL }
  };

void CallAPIFunction( _CONFIG* conf, char* userID, char* method )
  {
  char* ptr = NULL;
  char* topic = strtok_r( method, "&/", &ptr );
  char* action = strtok_r( NULL, "&/", &ptr );

  if( EMPTY( topic ) || EMPTY( action ) )
    APIError( method, -4, "API calls must specify a topic and action " );

  char methodNameBuf[BUFLEN];
  snprintf( methodNameBuf, sizeof(methodNameBuf)-1, "%s/%s", topic, action );

  char inputBuf[BUFLEN];
  if( fgets( inputBuf, sizeof(inputBuf)-1, stdin ) != inputBuf )
    inputBuf[0] = 0;

  _TAG_VALUE* args = ParseJSON( inputBuf );

  int gotOne = 0;
  for( _FN_RECORD* fn=methods; fn->topic!=NULL; ++fn )
    {
    if( NOTEMPTY( fn->topic )
        && NOTEMPTY( fn->action )
        && fn->function!=NULL
        && strcasecmp( fn->topic, topic )==0
        && strcasecmp( fn->action, action )==0 )
      {
      (fn->function)( conf, userID, methodNameBuf, args );
      gotOne = 1;
      break;
      }
    }

  if( ! gotOne )
    APIError( method, -5, "Invalid function and/or topic. (%s/%s)", topic, action );

  FreeTagValue( args );
  }


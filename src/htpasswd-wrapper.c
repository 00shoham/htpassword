/* These functions call the htpasswd binary to manipulate the contents of
   a .htpasswd file.  if you want to apply password policy rules or update
   history, do that elsewhere - before/after calling these. */

#include "base.h"

#define HTPASSWDBIN "/usr/bin/htpasswd"
#define TIMEOUT_ATTEMPT 1
#define TIMEOUT_MAX 5

int HTPasswdValidUser( char* lockPath, char* passwdFile, char* userID )
  {
  int err = 0;
  int lockFD = 0;

  if( EMPTY( passwdFile ) )
    return -1;

  err = StringMatchesUserIDFormat( userID );
  if( err )
    goto end;

  if( NOTEMPTY( lockPath ) )
    {
    lockFD = LockFile( lockPath );
    if( lockFD<=0 )
      {
      Warning( "Failed to lock [%s] (%d:%s)", lockPath, errno, strerror( errno ) );
      err = -100;
      goto end;
      }
    }

  FILE* f = fopen( passwdFile, "r" );
  if( f==NULL )
    {
    err = -3;
    goto end;
    }

  char userWithColon[300];
  snprintf( userWithColon, sizeof(userWithColon)-1, "%s:", userID );
  int l = strlen( userWithColon );
  char buf[BUFLEN];
  int found = 0;
  while( fgets( buf, sizeof(buf)-1, f )==buf )
    {
    if( strncmp( userWithColon, buf, l )==0 )
      {
      found = 1;
      break;
      }
    }

  fclose( f );

  if( ! found )
    err = -10;

  end:
  if( lockFD )
    {
    int x = UnLockFile( lockFD );
    if( x )
      {
      Warning( "Failed to clear lock file [%s] - %d (%d:%s)", lockPath, x, errno, strerror( errno ) );
      err = -400 + x;
      }
    }

  return err;
  }

int HTPasswdAddUser( char* lockPath, char* passwdFile, char* userID, char* password )
  {
  int err = 0;
  int lockFD = 0;

  err = StringMatchesUserIDFormat( userID );
  if( err )
    goto end;

  if( NOTEMPTY( lockPath ) )
    {
    lockFD = LockFile( lockPath );
    if( lockFD<=0 )
      {
      Warning( "Failed to lock [%s] (%d:%s)", lockPath, errno, strerror( errno ) );
      err = -100;
      goto end;
      }
    }

  err = HTPasswdValidUser( NULL, passwdFile, userID );
  if( err==0 )
    {
    Warning( "User [%s] already exists", userID );
    goto end;
    }

  char cmd[BUFLEN];
  snprintf( cmd, sizeof(cmd)-1, "%s -iB '%s' '%s'", HTPASSWDBIN, passwdFile, userID );

  char passwdNewline[BUFLEN];
  snprintf( passwdNewline, sizeof(passwdNewline), "%s\n", password );

  Notice( "Running command [%s] with input line [%s]", cmd, passwdNewline );
  err = WriteLineToCommand( cmd, passwdNewline, 1/*sec poll*/, 5/*sec max*/ );
  Notice( "Command returned %d", err );

  end:
  if( lockFD )
    {
    int x = UnLockFile( lockFD );
    if( x )
      {
      Warning( "Failed to clear lock file [%s] - %d (%d:%s)", lockPath, x, errno, strerror( errno ) );
      err = -400 + x;
      }
    }

  return err;
  }


int HTPasswdRemoveUser( char* lockPath, char* passwdFile, char* userID )
  {
  int err = 0;
  int lockFD = 0;

  err = StringMatchesUserIDFormat( userID );
  if( err )
    goto end;

  if( NOTEMPTY( lockPath ) )
    {
    lockFD = LockFile( lockPath );
    if( lockFD<=0 )
      {
      Warning( "Failed to lock [%s] (%d:%s)", lockPath, errno, strerror( errno ) );
      err = -100;
      goto end;
      }
    }

  err = HTPasswdValidUser( NULL, passwdFile, userID );
  if( err )
    {
    Warning( "User [%s] does not exist", userID );
    goto end;
    }

  char cmd[BUFLEN];
  snprintf( cmd, sizeof(cmd)-1, "%s -D '%s' '%s'", HTPASSWDBIN, passwdFile, userID );

  err = AsyncRunCommandNoIO( cmd );

  end:
  if( lockFD )
    {
    int x = UnLockFile( lockFD );
    if( x )
      {
      Warning( "Failed to clear lock file [%s] - %d (%d:%s)", lockPath, x, errno, strerror( errno ) );
      err = -400 + x;
      }
    }

  return err;
  }

int HTPasswdResetPassword( char* lockPath, char* passwdFile, char* userID, char* password )
  {
  int err = 0;
  int lockFD = 0;

  err = StringMatchesUserIDFormat( userID );
  if( err )
    goto end;

  if( NOTEMPTY( lockPath ) )
    {
    lockFD = LockFile( lockPath );
    if( lockFD<=0 )
      {
      Warning( "Failed to lock [%s] (%d:%s)", lockPath, errno, strerror( errno ) );
      err = -100;
      goto end;
      }
    }

  err = HTPasswdValidUser( NULL, passwdFile, userID );
  if( err )
    {
    Warning( "User [%s] does not exist", userID );
    goto end;
    }

  char cmd[BUFLEN];
  snprintf( cmd, sizeof(cmd)-1, "%s -iB '%s' '%s'", HTPASSWDBIN, passwdFile, userID );

  char passwdNewline[BUFLEN];
  snprintf( passwdNewline, sizeof(passwdNewline), "%s\n", password );

  err = WriteLineToCommand( cmd, passwdNewline, 1/*sec poll*/, 5/*sec max*/ );

  end:
  if( lockFD )
    {
    int x = UnLockFile( lockFD );
    if( x )
      {
      Warning( "Failed to clear lock file [%s] - %d (%d:%s)", lockPath, x, errno, strerror( errno ) );
      err = -400 + x;
      }
    }

  return err;
  }

int HTPasswdCheckPassword( char* lockPath, char* passwdFile, char* userID, char* password )
  {
  int err = 0;
  int lockFD = 0;

  err = StringMatchesUserIDFormat( userID );
  if( err )
    goto end;

  if( NOTEMPTY( lockPath ) )
    {
    lockFD = LockFile( lockPath );
    if( lockFD<=0 )
      {
      Warning( "Failed to lock [%s] (%d:%s)", lockPath, errno, strerror( errno ) );
      err = -100;
      goto end;
      }
    }

  err = HTPasswdValidUser( NULL, passwdFile, userID );
  if( err )
    {
    Warning( "User [%s] does not exist", userID );
    goto end;
    }

  char cmd[BUFLEN];
  snprintf( cmd, sizeof(cmd)-1, "%s -iv '%s' '%s'", HTPASSWDBIN, passwdFile, userID );

  char passwdNewline[BUFLEN];
  snprintf( passwdNewline, sizeof(passwdNewline), "%s\n", password );

  char response[BUFLEN];
  err = WriteReadLineToFromCommand( cmd, passwdNewline, response, sizeof(response)-1, TIMEOUT_ATTEMPT, TIMEOUT_MAX );

  if( err )
    goto end;

  char goodResponse[BUFLEN];
  snprintf( goodResponse, sizeof(goodResponse)-1, "Password for user %s correct.\n", userID );

  if( strcmp( response, goodResponse )!=0 )
    err = -10;

  end:
  if( lockFD )
    {
    int x = UnLockFile( lockFD );
    if( x )
      {
      Warning( "Failed to clear lock file [%s] - %d (%d:%s)", lockPath, x, errno, strerror( errno ) );
      err = -400 + x;
      }
    }

  return err;
  }

int HTPasswdChangePassword( char* lockPath, char* passwdFile, char* userID, char* oldp, char* newp )
  {
  int err = 0;
  int lockFD = 0;

  err = StringMatchesUserIDFormat( userID );
  if( err )
    goto end;

  if( NOTEMPTY( lockPath ) )
    {
    lockFD = LockFile( lockPath );
    if( lockFD<=0 )
      {
      Warning( "Failed to lock [%s] (%d:%s)", lockPath, errno, strerror( errno ) );
      err = -100;
      goto end;
      }
    }

  err = HTPasswdValidUser( NULL, passwdFile, userID );
  if( err )
    {
    Warning( "User [%s] does not exist", userID );
    goto end;
    }

  err = HTPasswdCheckPassword( NULL, passwdFile, userID, oldp );
  if( err )
    {
    Warning( "Provided old password not correct." );
    goto end;
    }

  err = HTPasswdResetPassword( NULL, passwdFile, userID, newp );
  if( err )
    {
    Warning( "Failed to set new password for [%s].", userID );
    goto end;
    }

  end:
  if( lockFD )
    {
    int x = UnLockFile( lockFD );
    if( x )
      {
      Warning( "Failed to clear lock file [%s] - %d (%d:%s)", lockPath, x, errno, strerror( errno ) );
      err = -400 + x;
      }
    }

  return err;
  }


_PASSWORD_FILE* HTPasswdFileFromID( _CONFIG* conf, char* id )
  {
  if( EMPTY( id ) )
    return NULL;
  if( conf==NULL )
    return NULL;

  for( _PASSWORD_FILE* pf=conf->passwordFiles; pf!=NULL; pf=pf->next )
    {
    if( NOTEMPTY( pf->id ) && strcmp( pf->id, id )==0 )
      return pf;
    }

  return NULL;
  }

int HTPasswdIsUserAdmin( _PASSWORD_FILE* pf, char* userID )
  {
  if( pf==NULL )
    return -1;
  if( EMPTY( userID ) )
    return -2;

  for( _ADMIN_USER* au = pf->admins; au!=NULL; au=au->next )
    {
    if( NOTEMPTY( au->id ) && strcmp( au->id, userID )==0 )
      return 0;
    }

  return -3;
  }

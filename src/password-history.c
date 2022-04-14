#include "base.h"

int AddPasswordToHistory( char* lockPath, char* histFile, char* userID, char* password )
  {
  int err = 0;
  int lockFD = 0;

  if( EMPTY( histFile ) )
    return -1;

  if( EMPTY( password ) )
    return -2;

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

  char* record = NULL;
  err = GeneratePasswordHistoryRecord( userID, password, &record );
  if( err )
    {
    err += -200;
    goto end;
    }

  FILE* f = fopen( histFile, "a" );
  if( f==NULL )
    {
    err = -3;
    goto end;
    }
  fputs( record, f );
  fputs( "\n", f );
  fclose( f );

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

int IsPasswordInHistory( char* histFile, char* userID, char* password )
  {
  int err = 0;
  char* paddedUser = NULL;

  if( EMPTY( histFile ) )
    return -1;

  if( EMPTY( password ) )
    return -2;

  err = StringMatchesUserIDFormat( userID );
  if( err )
    goto end;

  int l = strlen( userID );
  paddedUser = malloc( l+2 );
  strcpy( paddedUser, userID );
  paddedUser[l] = ':';
  paddedUser[l+1] = 0;

  FILE* f = fopen( histFile, "r" );
  if( f==NULL )
    {
    err = -3;
    goto end;
    }
  char buf[BUFLEN];
  while( fgets( buf, sizeof(buf), f )==buf )
    {
    if( strncmp( buf, paddedUser, l+1 )!=0 )
      continue; /* someone else */

    (void)StripEOL(buf);

    time_t tWhen = 0;
    char* user = NULL;
    char* salt = NULL;
    char* hash = NULL;
    err = ParseHistoryRecord( buf, &user, &tWhen, &salt, &hash );
    if( err )
      {
      Warning( "Parsing password history record failed (%d)", err );
      continue;
      }

    int match = DoesPasswordMatchHash( password, salt, hash );
    if( match==0 )
      {
      err = 0;
      fclose( f );
      goto end;
      }
    }
  
  fclose( f );
  err = -1000;

  end:
  if( paddedUser!=NULL )
    free( paddedUser );

  return err;
  }

int RemoveUserFromHistory( char* lockPath, char* histFile, char* userID )
  {
  int err = 0;
  int lockFD = 0;

  if( EMPTY( histFile ) )
    return -1;

  if( StringMatchesUserIDFormat( userID ) )
    {
    return -2;
    }

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

  char tmpHistFile[BUFLEN];
  snprintf( tmpHistFile, sizeof( tmpHistFile ), "%s.tmp", histFile );
  FILE* f = fopen( histFile, "r+" );
  if( f==NULL )
    {
    err = -200;
    goto end;
    }

  char userColon[BUFLEN];
  snprintf( userColon, sizeof( userColon ), "%s:", userID );
  int l = strlen( userColon );

  for(;;)
    {
    long pos = ftell( f );
    char buf[BUFLEN];
    if( fgets( buf, sizeof(buf)-1, f )==buf )
      {
      if( strncmp( buf, userColon, l )==0 )
        {
        buf[0] = '*';
        fseek( f, pos, SEEK_SET );
        fputs( buf, f );
        }
      }
    else
      { /* file done */
      break;
      }
    }

  fclose( f );

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

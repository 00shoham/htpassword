#include "base.h"

#define LOCKFILE "/tmp/.htpasswd.test.lock"

enum operation { op_invalid, op_add, op_isuser, op_testpass, op_del, op_reset, op_change };

void Usage( char* cmd )
  {
  printf( "USAGE: %s ... arguments ... \n", cmd );
  printf( "Where arguments may be:\n");
  printf( "    -f PATH       to specify where the .htpasswd file is\n");
  printf( "    -u USERID     to specify the user whose entry will be operated on\n");
  printf( "    -o PASSWORD   to specify the old password (if verifying or changing)\n");
  printf( "    -n PASSWORD   to specify the new password (if adding or resetting)\n");
  printf( "    -add          add a new user\n");
  printf( "    -isuser       check if the user exists\n");
  printf( "    -testpass     check if the old password is correct for the user\n");
  printf( "    -del          remove an existing user\n");
  printf( "    -reset        force a new password for an existing user\n");
  printf( "    -change       change passwords from old to new values\n");
  printf( "    -h            print this\n");
  exit(0);
  }

int main( int argc, char** argv )
  {
  char* passwdFile = NULL;
  char* userID = NULL;
  char* oldPassword = NULL;
  char* newPassword = NULL;
  enum operation op = op_invalid;

  for( int i=1; i<argc; ++i )
    {
    if( strcmp( argv[i], "-f" )==0 && i+1<argc )
      {
      ++i;
      passwdFile = argv[i];
      }
    else if( strcmp( argv[i], "-u" )==0 && i+1<argc )
      {
      ++i;
      userID = argv[i];
      }
    else if( strcmp( argv[i], "-o" )==0 && i+1<argc )
      {
      ++i;
      oldPassword = argv[i];
      }
    else if( strcmp( argv[i], "-n" )==0 && i+1<argc )
      {
      ++i;
      newPassword = argv[i];
      }
    else if( strcmp( argv[i], "-add" )==0 )
      op = op_add;
    else if( strcmp( argv[i], "-isuser" )==0 )
      op = op_isuser;
    else if( strcmp( argv[i], "-testpass" )==0 )
      op = op_testpass;
    else if( strcmp( argv[i], "-del" )==0 )
      op = op_del;
    else if( strcmp( argv[i], "-reset" )==0 )
      op = op_reset;
    else if( strcmp( argv[i], "-change" )==0 )
      op = op_change;
    else if( strcmp( argv[i], "-h" )==0 )
      Usage( argv[0] );
    else
      {
      Warning( "Invalid argument: [%s]", argv[i] );
      Usage( argv[0] );
      }
    }

  int err = 0;
  switch( op )
    {
    case op_invalid:
      Error( "You must specify an operation" );

    case op_add:
      err = HTPasswdAddUser( LOCKFILE, passwdFile, userID, newPassword );
      break;

    case op_isuser:
      err = HTPasswdValidUser( LOCKFILE, passwdFile, userID );
      break;

    case op_testpass:
      err = HTPasswdCheckPassword( LOCKFILE, passwdFile, userID, oldPassword );
      break;

    case op_del:
      err = HTPasswdRemoveUser( LOCKFILE, passwdFile, userID );
      break;

    case op_reset:
      err = HTPasswdResetPassword( LOCKFILE, passwdFile, userID, newPassword );
      break;

    case op_change:
      err = HTPasswdChangePassword( LOCKFILE, passwdFile, userID, oldPassword, newPassword );
      break;
    }

  printf( "Got result: %d\n", err );

  return err;
  }


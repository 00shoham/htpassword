#include "base.h"

_ADMIN_USER* AddAdminUser( char* id, _ADMIN_USER* list )
  {
  if( EMPTY( id ) )
    Error( "Cannot add admin user with empty ID" );

  _ADMIN_USER* newp = (_ADMIN_USER*)SafeCalloc( 1, sizeof(_ADMIN_USER), "_ADMIN_USER" );
  newp->id = strdup( id );
  newp->next = list;
  return newp;
  }

int DeleteAdminUser( _ADMIN_USER** list, char* id )
  {
  if( list==NULL )
    return -1;
  if( EMPTY( id ) )
    return -2;

  int freed = 0;
  for( ; *list!=NULL; list = &( (*list)->next ) )
    {
    _ADMIN_USER* a = *list;
    if( a!=NULL && NOTEMPTY( a->id ) && strcmp( a->id, id )==0 )
      {
      *list = a->next;
      free( a->id );
      freed = 1;
      break;
      }
    }

  if( freed )
    return 0;

  /* not found */
  return -3;
  }

_DICTIONARY_FILE* AddDictionaryFile( char* path, _DICTIONARY_FILE* list )
  {
  if( EMPTY( path ) )
    Error( "Cannot add dictionary file with empty path" );
  if( FileExists( path )!=0 )
    Error( "Cannot open dictionary file in %s", path );

  _DICTIONARY_FILE* newp = (_DICTIONARY_FILE*)SafeCalloc( 1, sizeof(_DICTIONARY_FILE), "_DICTIONARY_FILE" );
  newp->path = strdup( path );
  newp->next = list;
  return newp;
  }

int DeleteDictionaryFile( _DICTIONARY_FILE** list, char* path )
  {
  if( list==NULL )
    return -1;
  if( EMPTY( path ) )
    return -2;

  int freed = 0;
  for( ; *list!=NULL; list = &( (*list)->next ) )
    {
    _DICTIONARY_FILE* a = *list;
    if( a!=NULL && NOTEMPTY( a->path ) && strcmp( a->path, path )==0 )
      {
      *list = a->next;
      free( a->path );
      freed = 1;
      break;
      }
    }

  if( freed )
    return 0;

  /* not found */
  return -3;
  }

int SetDefaultPolicy( _PASSWORD_POLICY* policy )
  {
  if( policy==NULL )
    return -1;

  policy->minLength = DEFAULT_POLICY_MIN_LENGTH;
  policy->maxLength = DEFAULT_POLICY_MAX_LENGTH;
  policy->minLetters = DEFAULT_POLICY_MIN_LETTERS;
  policy->maxLetters = DEFAULT_POLICY_MAX_LETTERS;
  policy->minDigits = DEFAULT_POLICY_MIN_DIGITS;
  policy->maxDigits = DEFAULT_POLICY_MAX_DIGITS;
  policy->minPunct = DEFAULT_POLICY_MIN_PUNCT;
  policy->maxPunct = DEFAULT_POLICY_MAX_PUNCT;
  policy->requireMixedCase = DEFAULT_POLICY_REQUIRE_MIXED_CASE;

  return 0;
  }

_PASSWORD_FILE* AddPasswordFile( char* id, _PASSWORD_FILE* list )
  {
  if( EMPTY( id ) )
    Error( "Cannot add password file with empty id" );

  _PASSWORD_FILE* newp = (_PASSWORD_FILE*)SafeCalloc( 1, sizeof(_PASSWORD_FILE), "_PASSWORD_FILE" );
  newp->id = strdup( id );
  if( SetDefaultPolicy( &(newp->policy) )!=0 )
    Error( "Cannot set default policy on password file %s", id );

  newp->next = list;
  return newp;
  }


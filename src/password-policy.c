#include "base.h"

#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGITS "0123456789"
#define PUNCTUATION " !@#$%^&*()-_=+[{]}\\|;:'\",<.>/?`~"
#define LOWERCASE "abcdefghijklmnopqrstuvwxyz"
#define UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

#define MIN_DICT_WORD_LEN 5

int CountChars( char* string, char* set )
  {
  int n = 0;
  for( char* ptr=string; *ptr!=0; ++ptr )
    {
    int c = *ptr;
    if( strchr( set, c )!=NULL )
      ++n;
    }

  return n;
  }

void UppercaseString( char* str )
  {
  for( char* ptr=str; *ptr!=0; ++ptr )
    {
    int c = *ptr;
    if( islower( c ) )
      {
      c = toupper( c );
      }
    }
  }

int WordInDictionary( char* path, char* word )
  {
  char uppercaseWord[BUFLEN];
  strncpy( uppercaseWord, word, sizeof(uppercaseWord)-1 );
  UppercaseString( uppercaseWord );

  FILE* f = fopen( path, "r" );
  if( f==NULL )
    {
    Warning( "Cannot open dictionary %s", path );
    return -1;
    }

  char buf[BUFLEN];
  while( fgets( buf, sizeof(buf), f )==buf )
    {
    (void)StripEOL( buf );
    if( strlen( buf ) < MIN_DICT_WORD_LEN  )
      continue;
    UppercaseString( buf );
    if( strstr( uppercaseWord, buf ) != NULL )
      {
      fclose( f );
      return 0;
      }
    }

  fclose( f );
  return 1;
  }

int TestPasswordQuality( char* userID,
                         char* proposedPassword,
                         char* historyFile,
                         _PASSWORD_POLICY* policy,
                         _TAG_VALUE** messagesPtr )
  {
  int err = 0;
  _TAG_VALUE* messages = NULL;

  if( EMPTY( proposedPassword ) )
    {
    err = -100;
    messages = NewTagValue( "ERROR", "Empty password", messages, 0 );
    goto end;
    }

  if( policy==NULL )
    {
    err = -101;
    messages = NewTagValue( "ERROR", "Empty policy", messages, 0 );
    goto end;
    }

  int n = strlen( proposedPassword );
  if( policy->minLength>0 && n<policy->minLength )
    {
    --err;
    char msg[BUFLEN];
    snprintf( msg, sizeof(msg)-1, "Password must have at least %d characters", policy->minLength );
    messages = NewTagValue( "ERROR", msg, messages, 0 );
    }
  if( policy->maxLength>0 && n>policy->maxLength )
    {
    --err;
    char msg[BUFLEN];
    snprintf( msg, sizeof(msg)-1, "Password cannot have more than %d characters", policy->maxLength );
    messages = NewTagValue( "ERROR", msg, messages, 0 );
    }

  int l = CountChars( proposedPassword, LETTERS );
  if( policy->minLetters>0 && l<policy->minLetters )
    {
    --err;
    char msg[BUFLEN];
    snprintf( msg, sizeof(msg)-1, "Password must have at least %d letters", policy->minLetters );
    messages = NewTagValue( "ERROR", msg, messages, 0 );
    }
  if( policy->maxLetters>0 && l>policy->maxLetters )
    {
    --err;
    char msg[BUFLEN];
    snprintf( msg, sizeof(msg)-1, "Password cannot have more than %d letters", policy->maxLetters );
    messages = NewTagValue( "ERROR", msg, messages, 0 );
    }

  int d = CountChars( proposedPassword, DIGITS );
  if( policy->minDigits>0 && d<policy->minDigits )
    {
    --err;
    char msg[BUFLEN];
    snprintf( msg, sizeof(msg)-1, "Password must have at least %d digits", policy->minDigits );
    messages = NewTagValue( "ERROR", msg, messages, 0 );
    }
  if( policy->maxDigits>0 && d>policy->maxDigits )
    {
    --err;
    char msg[BUFLEN];
    snprintf( msg, sizeof(msg)-1, "Password cannot have more than %d digits", policy->maxDigits );
    messages = NewTagValue( "ERROR", msg, messages, 0 );
    }

  int p = CountChars( proposedPassword, PUNCTUATION );
  if( policy->minPunct>0 && p<policy->minPunct )
    {
    --err;
    char msg[BUFLEN];
    snprintf( msg, sizeof(msg)-1, "Password must have at least %d punctuation marks or spaces", policy->minPunct );
    messages = NewTagValue( "ERROR", msg, messages, 0 );
    }
  if( policy->maxPunct>0 && p>policy->maxPunct )
    {
    --err;
    char msg[BUFLEN];
    snprintf( msg, sizeof(msg)-1, "Password cannot have more than %d punctuation marks or spaces", policy->maxPunct );
    messages = NewTagValue( "ERROR", msg, messages, 0 );
    }

  if( policy->requireMixedCase )
    {
    int lc = CountChars( proposedPassword, LOWERCASE );
    int uc = CountChars( proposedPassword, UPPERCASE );
    if( lc==0 || uc==0 )
      {
      --err;
      char msg[BUFLEN];
      snprintf( msg, sizeof(msg)-1, "Password must include both lower and uppercase letters" );
      messages = NewTagValue( "ERROR", msg, messages, 0 );
      }
    }

  for( _DICTIONARY_FILE* dict = policy->dictionaries; dict!=NULL; dict=dict->next )
    {
    if( WordInDictionary( dict->path, proposedPassword )==0 )
      {
      --err;
      char msg[BUFLEN];
      snprintf( msg, sizeof(msg)-1, "Password must not contain dictionary words" );
      messages = NewTagValue( "ERROR", msg, messages, 0 );
      }
    }

  if( NOTEMPTY( historyFile ) && NOTEMPTY( userID ) )
    {
    if( IsPasswordInHistory( historyFile, userID, proposedPassword )==0 )
      {
      --err;
      char msg[BUFLEN];
      snprintf( msg, sizeof(msg)-1, "You may not reuse an old password" );
      messages = NewTagValue( "ERROR", msg, messages, 0 );
      }
    }

  end:
  *messagesPtr = messages;
  return err;
  }


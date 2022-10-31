#ifndef _INCLUDE_HIBP
#define _INCLUDE_HIBP

#define DEFAULT_KEY "e42a0d9981ab4a6f9b909f5fc9ba88a0"
#define HIBP_PW_URL "https://api.pwnedpasswords.com/range"
#define HIBP_TIMEOUT 5 /* seconds */
#define HIBP_MY_USERAGENT "https://github.com/00shoham/htpassword"

int HaveIBeenPwned( char* apiKey, char* password );

#endif

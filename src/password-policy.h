#ifndef _INCLUDE_PASSWORD_POLICY
#define _INCLUDE_PASSWORD_POLICY

int TestPasswordQuality( char* userID,
                         char* proposedPassword,
                         char* historyFile,
                         _PASSWORD_POLICY* policy,
                         _TAG_VALUE** messagesPtr );

#endif

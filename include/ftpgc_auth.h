#ifndef ___FTPGC_AUTH_H_
#define ___FTPGC_AUTH_H_

#include <gctypes.h>

s32  ftpgc_auth_len_USER(void);
s32  ftpgc_auth_len_PASS(void);
BOOL ftpgc_auth_login(void);
BOOL ftpgc_auth_logged_in(void);
void ftpgc_auth_logout(void);
void ftpgc_auth_set_USER(const char *user);
void ftpgc_auth_set_PASS(const char *pass);

void _ftpgc_authenticate(void);

#endif
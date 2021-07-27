#ifndef ___FTPGC_AUTH_H_
#define ___FTPGC_AUTH_H_

#include <gctypes.h>

static BOOL ftpgc_authenticated = FALSE;

BOOL ftpgc_auth_login(void);
void ftpgc_auth_logout(void);
void ftpgc_auth_set_USER(const char *user);
void ftpgc_auth_set_PASS(const char *pass);
void _ftpgc_authenticate(void);

#endif
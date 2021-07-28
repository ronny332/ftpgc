#include "ftpgc_auth.h"

#include <stdio.h>
#include <string.h>

#include "ftpgc_const.h"

#define FTPGC_AUTH_USER_LEN sizeof(FTPGC_AUTH_USER) - 1
#define FTPGC_AUTH_PASS_LEN sizeof(FTPGC_AUTH_PASS) - 1

static BOOL ftpgc_authenticated = FALSE;

char ftpgc_USER[FTPGC_AUTH_USER_LEN + 1] = { 0 };
char ftpgc_PASS[FTPGC_AUTH_PASS_LEN + 1] = { 0 };

BOOL ftpgc_auth_login(void)
{
    _ftpgc_authenticate();

#ifdef FTPGC_DEBUG
    printf("DEBUG: authentication was%s successful.\n", ((ftpgc_authenticated) ? "" : " NOT"));
#endif

    return ftpgc_authenticated;
}

s32 ftpgc_auth_len_USER(void)
{
    return strlen(ftpgc_USER);
}

s32 ftpgc_auth_len_PASS(void)
{
    return strlen(ftpgc_PASS);
}

BOOL ftpgc_auth_logged_in(void)
{
    return ftpgc_authenticated;
}

void ftpgc_auth_logout(void)
{
    ftpgc_authenticated = FALSE;
    memset(ftpgc_USER, 0, FTPGC_AUTH_USER_LEN + 1);
    memset(ftpgc_PASS, 0, FTPGC_AUTH_PASS_LEN + 1);

    // TODO cancel all active data connections

#ifdef FTPGC_DEBUG
    printf("DEBUG: USER and PASS set to 0.\n");
#endif
}

void ftpgc_auth_set_USER(const char *user)
{
    if (strlen(user) <= FTPGC_AUTH_USER_LEN)
    {
        strncpy(ftpgc_USER, user, FTPGC_AUTH_USER_LEN);

#ifdef FTPGC_DEBUG
        printf("DEBUG: set USER to \"%s\"\n", ftpgc_USER);
#endif
        return;
    }
    memset(ftpgc_USER, 0, FTPGC_AUTH_USER_LEN + 1);
    ftpgc_USER[0] = ' ';

#ifdef FTPGC_DEBUG
    printf("DEBUG: USER \"%s\" was invalid.\n", user);
#endif
}

void ftpgc_auth_set_PASS(const char *pass)
{
    if (strlen(pass) <= FTPGC_AUTH_PASS_LEN)
    {
        strncpy(ftpgc_PASS, pass, FTPGC_AUTH_PASS_LEN);

#ifdef FTPGC_DEBUG
        printf("DEBUG: set PASS to \"%s\"\n", ftpgc_PASS);
#endif
        return;
    }
    memset(ftpgc_PASS, 0, FTPGC_AUTH_PASS_LEN + 1);

#ifdef FTPGC_DEBUG
    printf("DEBUG: PASS \"%s\" was invalid.\n", pass);
#endif
}

void _ftpgc_authenticate(void)
{
    if (strncmp(FTPGC_AUTH_USER, ftpgc_USER, FTPGC_AUTH_USER_LEN) == 0
        && strncmp(FTPGC_AUTH_PASS, ftpgc_PASS, FTPGC_AUTH_PASS_LEN) == 0)
    {
        ftpgc_authenticated = TRUE;
        return;
    }
    ftpgc_authenticated = FALSE;
}
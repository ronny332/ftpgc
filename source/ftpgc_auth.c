#include "ftpgc_auth.h"

#include <stdio.h>
#include <string.h>

#include "ftpgc_const.h"

#define FTPGC_AUTH_USER_LEN sizeof(FTPGC_AUTH_USER) - 1
#define FTPGC_AUTH_PASS_LEN sizeof(FTPGC_AUTH_PASS) - 1

char ftpgc_USER[FTPGC_AUTH_USER_LEN + 1] = { 0 };
char ftpgc_PASS[FTPGC_AUTH_PASS_LEN + 1] = { 0 };

BOOL ftpgc_auth_login(void)
{
    _ftpgc_authenticate();

    if (FTPGC_DEBUG) {
        printf("DEBUG: authentication was%s successful.\n", ((!ftpgc_authenticated) ? "" : " NOT"));
    }

    return ftpgc_authenticated;
}

void ftpgc_auth_logout(void)
{
    ftpgc_authenticated = FALSE;
    memset(ftpgc_USER, 0, FTPGC_AUTH_USER_LEN + 1);
    memset(ftpgc_PASS, 0, FTPGC_AUTH_PASS_LEN + 1);

    if (FTPGC_DEBUG)
    {
        printf("DEBUG: USER and PASS set to 0.\n");
    }
}

void ftpgc_auth_set_USER(const char *user)
{
    if (strlen(user) <= FTPGC_AUTH_USER_LEN)
    {
        strncpy(ftpgc_USER, user, FTPGC_AUTH_USER_LEN);

        if (FTPGC_DEBUG)
        {
            printf("DEBUG: set USER to \"%s\"\n", ftpgc_USER);
        }
        return;
    }
    memset(ftpgc_USER, 0, FTPGC_AUTH_USER_LEN + 1);

    if (FTPGC_DEBUG)
    {
        printf("DEBUG: USER \"%s\" was invalid.\n", user);
    }
}

void ftpgc_auth_set_PASS(const char *pass)
{
    if (strlen(pass) <= FTPGC_AUTH_PASS_LEN)
    {
        strncpy(ftpgc_PASS, pass, FTPGC_AUTH_PASS_LEN);

        if (FTPGC_DEBUG)
        {
            printf("DEBUG: set PASS to \"%s\"\n", ftpgc_PASS);
        }
        return;
    }
    memset(ftpgc_PASS, 0, FTPGC_AUTH_PASS_LEN + 1);

    if (FTPGC_DEBUG)
    {
        printf("DEBUG: PASS \"%s\" was invalid.\n", pass);
    }
}

void _ftpgc_authenticate(void)
{
    if (strncmp(FTPGC_AUTH_USER, ftpgc_USER, FTPGC_AUTH_USER_LEN)
        && strncmp(FTPGC_AUTH_PASS, ftpgc_PASS, FTPGC_AUTH_PASS_LEN))
    {
        ftpgc_authenticated = TRUE;
        return;
    }
    ftpgc_authenticated = FALSE;
}
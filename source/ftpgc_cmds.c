#include <ctype.h>
#include <stdio.h>
#include <string.h>
// #include <network.h>

#include "ftpgc_cmds.h"

#include "ftpgc_const.h"

static char cmd_tmp[5];

u32 i = 0;
s32 len = 0;
char *pos = NULL;
char reply_buffer[FTPGC_CONTROL_REPLY_LEN + 1];

s32 ftpgc_parse_cmd(const char *cmd, char **ret)
{
    _cmd_clean();
    _cmd_length(cmd);

    if (len > 0)
    {
        _cmd_reformat(cmd);
        if (_cmd_valid())
        {
            *ret = &(cmd_tmp[0]);
            printf("valid\n");

            return FTPGC_SUCCESS;
        }
    }

    return FTPGC_INVALID_COMMAND;
}

s32 ftpgc_write_reply(s32 csock, u32 code, const char *msg)
{
    _clear_reply_buffer();
    sprintf(reply_buffer, "%u %s\r\n", code, msg);
    
    return 0; //net_send(csock, reply_buffer, strlen(reply_buffer), 0);
}

void _cmd_clean()
{
    memset(&cmd_tmp, 0, 5);
}

void _cmd_length(const char *cmd)
{
    len = strlen(cmd);

    if (!len)
    {
        len = -1;
        return;
    }

    pos = strstr(cmd, "\r");

    if (pos)
    {
        len = pos - cmd;
    }

    pos = strstr(cmd, " ");

    if (!pos && len < 3)
    {
        len = -1;
        return;
    }
}

void _cmd_reformat(const char *cmd)
{
    for (i = 0; i < len; i++)
    {
        cmd_tmp[i] = toupper(cmd[i]);
    }
}

BOOL _cmd_valid()
{
    for (i = 0; i < sizeof(ftpgc_commands_global) / sizeof(ftpgc_commands_global[0]); i++)
    {
        if (strncmp(ftpgc_commands_global[i], cmd_tmp, 4) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void _clear_reply_buffer(void)
{
    memset(&reply_buffer, 0, FTPGC_CONTROL_REPLY_LEN + 1);
}
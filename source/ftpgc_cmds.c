#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <network.h>

#include "ftpgc_cmds.h"

#include "ftpgc_const.h"

static char cmd_tmp[5];

u32 cmd_i = 0;
s32 cmd_len = 0;
char *cmd_pos = NULL;
s32 cmd_ret = 0;
char cmd_reply_buffer[FTPGC_CONTROL_REPLY_LEN + 1];

BOOL ftpgc_handle_single_cmd(s32 csock, const char *cmd)
{
    if (strncmp(cmd, "NOOP", 4) == 0)
    {
        cmd_ret = ftpgc_write_reply(csock, 200, "Command okay.");
    }
    else if (strncmp(cmd, "QUIT", 4) == 0)
    {
        ftpgc_write_reply(csock, 221, "Goodbye.");
        return TRUE;
    }
    else if (strncmp(cmd, "SYST", 4) == 0)
    {
        cmd_ret = ftpgc_write_reply(csock, 215, "UNIX Type: L8");
    }
    else
    {
        cmd_ret = ftpgc_write_reply(csock, 500, "Command not understood.");
    }

    return (cmd_ret) ? FALSE : TRUE;
}

s32 ftpgc_parse_single_cmd(const char *cmd, char **ret)
{
    _cmd_clean();
    _cmd_length(cmd);

    if (cmd_len > 0)
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
    sprintf(cmd_reply_buffer, "%u %s\r\n", code, msg);

    return net_send(csock, cmd_reply_buffer, strlen(cmd_reply_buffer), 0);
}

void _cmd_clean()
{
    memset(&cmd_tmp, 0, 5);
}

void _cmd_length(const char *cmd)
{
    cmd_len = strlen(cmd);

    if (!cmd_len)
    {
        cmd_len = -1;
        return;
    }

    cmd_pos = strstr(cmd, "\r");

    if (cmd_pos)
    {
        cmd_len = cmd_pos - cmd;
    }

    cmd_pos = strstr(cmd, " ");

    if (!cmd_pos && cmd_len < 3)
    {
        cmd_len = -1;
        return;
    }
}

void _cmd_reformat(const char *cmd)
{
    for (cmd_i = 0; cmd_i < cmd_len; cmd_i++)
    {
        cmd_tmp[cmd_i] = toupper(cmd[cmd_i]);
    }
}

BOOL _cmd_valid()
{
    for (cmd_i = 0; cmd_i < sizeof(ftpgc_commands_global) / sizeof(ftpgc_commands_global[0]); cmd_i++)
    {
        if (strncmp(ftpgc_commands_global[cmd_i], cmd_tmp, 4) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void _clear_reply_buffer(void)
{
    memset(&cmd_reply_buffer, 0, FTPGC_CONTROL_REPLY_LEN + 1);
}
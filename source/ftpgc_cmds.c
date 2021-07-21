#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <network.h>

#include "ftpgc_cmds.h"

#include "ftpgc_const.h"

static char cmd_tmp[5];
struct ftpgc_cmd_hist_item *ftpgc_cmd_hist[FTPGC_CMD_HIST_LEN];

u32 cmd_i = 0;
s32 cmd_len = 0;
char *cmd_pos = NULL;
s32 cmd_ret = 0;
char cmd_reply_buffer[FTPGC_CONTROL_REPLY_LEN + 1];

BOOL ftpgc_handle_single_cmd(s32 csock, const char *cmd)
{
    if (!_cmd_needs_auth(cmd))
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
            cmd_ret = ftpgc_write_reply(csock, 215, "UNIX Type: L8.");
        }
        else
        {
            cmd_ret = ftpgc_write_reply(csock, 500, "Command not understood.");
        }
    }
    else
    {
        cmd_ret = ftpgc_write_reply(csock, 530, "Please login with USER and PASS.");
    }

    if (_append_cmd_hist_item(_create_cmd_hist_item(cmd, NULL)))
    {
        _print_cmd_hist();
    }

    return (cmd_ret) ? FALSE : TRUE;
}

s32 ftpgc_parse_cmd(const char *cmd, char **ret)
{
    _cmd_clean();
    _cmd_length(cmd);

    if (cmd_len > 0)
    {
        _cmd_reformat(cmd);
        if (_cmd_valid(Single))
        {
            *ret = &(cmd_tmp[0]);
            printf("valid\n");

            return FTPGC_CMD_SINGLE;
        }
        else if (_cmd_valid(Param))
        {
            // TODO
            return FTPGC_CMD_MULTI;
        }
    }

    return FTPGC_CMD_INVALID;
}

s32 ftpgc_write_reply(s32 csock, u32 code, const char *msg)
{
    _clear_reply_buffer();
    sprintf(cmd_reply_buffer, "%u %s\r\n", code, msg);

    return net_send(csock, cmd_reply_buffer, strlen(cmd_reply_buffer), 0);
}

BOOL _append_cmd_hist_item(struct ftpgc_cmd_hist_item *item)
{
    for (cmd_i = 0; cmd_i < FTPGC_CMD_HIST_LEN; cmd_i++)
    {
        if (ftpgc_cmd_hist[cmd_i] == NULL)
        {
            ftpgc_cmd_hist[cmd_i] = item;
            return TRUE;
        }
    }
    return FALSE;
}

void _clean_cmd_hist_item(struct ftpgc_cmd_hist_item *item)
{
    if (item->params != NULL)
    {
        free(item->params);
    }
    free(item);
}

void _clear_cmd_hist(void)
{
    for (cmd_i = 0; cmd_i < FTPGC_CMD_HIST_LEN; cmd_i++)
    {
        if (ftpgc_cmd_hist[cmd_i] != NULL)
        {
            _clean_cmd_hist_item(ftpgc_cmd_hist[cmd_i]);
            ftpgc_cmd_hist[cmd_i] = NULL;
        }
    }
}

void _clear_reply_buffer(void)
{
    memset(&cmd_reply_buffer, 0, FTPGC_CONTROL_REPLY_LEN + 1);
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

BOOL _cmd_needs_auth(const char *cmd)
{
    for (cmd_i = 0; cmd_i < sizeof(ftpgc_cmds_need_auth) / sizeof(ftpgc_cmds_need_auth[0]); cmd_i++)
    {
        if (strncmp(ftpgc_cmds_need_auth[cmd_i], cmd_tmp, 4) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void _cmd_reformat(const char *cmd)
{
    for (cmd_i = 0; cmd_i < cmd_len; cmd_i++)
    {
        cmd_tmp[cmd_i] = toupper(cmd[cmd_i]);
    }
}

BOOL _cmd_valid(enum ftpgc_cmd_type type)
{
    if (type == Single)
    {
        for (cmd_i = 0; cmd_i < sizeof(ftpgc_cmds_single) / sizeof(ftpgc_cmds_single[0]); cmd_i++)
        {
            if (strncmp(ftpgc_cmds_single[cmd_i], cmd_tmp, 4) == 0)
            {
                return TRUE;
            }
        }
    }
    else
    {
        // TODO
        return TRUE;
    }

    return FALSE;
}

struct ftpgc_cmd_hist_item *_create_cmd_hist_item(const char *cmd, const char *params)
{
    struct ftpgc_cmd_hist_item *item = calloc(1, sizeof(struct ftpgc_cmd_hist_item));
    strncpy(item->cmd, cmd, 4);

    s32 len = strlen(params);
    if (len)
    {
        s32 len_params = (len < FTPGC_CONTROL_REQ_LEN - 4) ? len : FTPGC_CONTROL_REQ_LEN - 4;
        item->params = calloc(len_params, sizeof(char));
        strncpy(item->params, params, len_params);
    }

    return item;
}

void _print_cmd_hist(void)
{
    for (cmd_i = 0; cmd_i < FTPGC_CMD_HIST_LEN; cmd_i++)
    {
        if (ftpgc_cmd_hist[cmd_i] != NULL)
        {
            printf("cmd: %s, params: %s\n", ftpgc_cmd_hist[cmd_i]->cmd, (ftpgc_cmd_hist[cmd_i]->params != NULL) ? ftpgc_cmd_hist[cmd_i] : "");
        }
    }
}
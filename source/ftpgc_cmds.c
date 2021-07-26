#include "ftpgc_cmds.h"

#include <ctype.h>
#include <network.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ftpgc_const.h"

static const char *ftpgc_cmds_need_auth[] = { "CWD" };
static const char *ftpgc_cmds_param[]     = { "USER", "PASS" };
static const char *ftpgc_cmds_single[]    = { "NOOP", "SYST", "QUIT", "CWD" };

struct ftpgc_cmd_hist_item *ftpgc_cmd_hist[FTPGC_CMD_HIST_LEN] = { NULL };

char  cmd_cmd[FTPGC_CMD_CMD_LEN];
s32   cmd_len = 0;
char  cmd_param[FTPGC_CMD_PARAM_LEN];
char *cmd_pos = NULL;
char  cmd_reply_buffer[FTPGC_CONTROL_REPLY_LEN + 1];
s32   cmd_ret = 0;

enum ftpgc_cmd_type cmd_type = Invalid;

BOOL ftpgc_cmd_handle_single(s32 csock, const char *cmd)
{
    if (cmd_type == Single)
    {
        if (!_cmd_needs_auth(cmd))
        {
            if (strncmp(cmd, "NOOP", 4) == 0)
            {
                cmd_ret = ftpgc_cmd_write_reply(csock, 200, "Command okay.");
            }
            else if (strncmp(cmd, "QUIT", 4) == 0)
            {
                ftpgc_cmd_write_reply(csock, 221, "Goodbye.");
                return TRUE;
            }
            else if (strncmp(cmd, "SYST", 4) == 0)
            {
                cmd_ret = ftpgc_cmd_write_reply(csock, 215, "UNIX Type: L8.");
            }
            else
            {
                cmd_ret = ftpgc_cmd_write_reply(csock, 500, "Command not understood.");
            }
        }
        else
        {
            ftpgc_cmd_write_reply(csock, 530, "Please login with USER and PASS.");
            cmd_ret = -1;
        }

        if (cmd_ret)
        {
            _cmd_hist_add_item(_cmd_hist_create_item(cmd, NULL));
        }

        _cmd_hist_print();

        return (cmd_ret) ? FALSE : TRUE;
    }
    else
    {
        return FALSE;
    }
}

s32 ftpgc_cmd_parse(const char *cmd, char **ret)
{
    _cmd_clean();
    _cmd_length(cmd);

    if (cmd_len > 0)
    {
        _cmd_copy(cmd);
        if (_cmd_valid(Single))
        {
            *ret = &(cmd_cmd[0]);

            if (FTPGC_DEBUG)
            {
                printf("DEBUG: cmd valid for single handling\n");
            }

            return FTPGC_CMD_SINGLE;
        }
        else if (_cmd_valid(Param))
        {
            // TODO

            return FTPGC_CMD_PARAM;
        }
    }

    if (FTPGC_DEBUG)
    {
        printf("DEBUG: cmd invalid\n");
    }

    cmd_type = Invalid;
    return FTPGC_CMD_INVALID;
}

void ftpgc_cmd_reset_hist(void)
{
    _cmd_reset_hist();
}

s32 ftpgc_cmd_write_reply(s32 csock, u32 code, const char *msg)
{
    _cmd_reset_reply_buffer();
    sprintf(cmd_reply_buffer, "%u %s\r\n", code, msg);

    return net_send(csock, cmd_reply_buffer, strlen(cmd_reply_buffer), 0);
}

void _cmd_clean()
{
    memset(&cmd_cmd, 0, FTPGC_CMD_CMD_LEN);
    memset(&cmd_param, 0, FTPGC_CMD_PARAM_LEN);
}

void _cmd_copy(const char *cmd)
{
    s32 i = 0;

    for (i = 0; i < ((cmd_len < 5) ? cmd_len : 5); i++) { cmd_cmd[i] = toupper(cmd[i]); }

    if (cmd_cmd[4] == '\r')
    {
        cmd_cmd[4] = '\0';
    }

    if (FTPGC_DEBUG)
    {
        printf("DEBUG: got cmd %s\n", cmd_cmd);
    }

    if (cmd_len > 5)
    {
        const char *param = cmd + 5;
        strncpy(cmd_param, param, FTPGC_CMD_PARAM_LEN);

        s32 param_len = strlen(cmd_param);
        if (cmd_param[param_len - 2] == '\r')
        {
            cmd_param[param_len - 2] = '\0';
        }

        if (FTPGC_DEBUG)
        {
            printf("DEBUG: got param %s\n", cmd_param);
        }
    }
    else
    {
        if (FTPGC_DEBUG)
        {
            printf("DEBUG: got no param\n");
        }
    }
}

void _cmd_hist_add_item(struct ftpgc_cmd_hist_item *item)
{
    s32 i = 0;

    for (i = 0; i < FTPGC_CMD_HIST_LEN; i++)
    {
        if (ftpgc_cmd_hist[i] == NULL)
        {
            ftpgc_cmd_hist[i] = item;
            return;
        }
    }

    _cmd_hist_del_item(ftpgc_cmd_hist[0]);

    if (FTPGC_DEBUG)
    {
        printf("DEBUG: clearing hist iten #%d\n", i);
    }

    for (i = 1; i < FTPGC_CMD_HIST_LEN; i++) { ftpgc_cmd_hist[i - 1] = ftpgc_cmd_hist[i]; }

    ftpgc_cmd_hist[FTPGC_CMD_HIST_LEN - 1] = item;

    return;
}

struct ftpgc_cmd_hist_item *_cmd_hist_create_item(const char *cmd, const char *params)
{
    struct ftpgc_cmd_hist_item *item = calloc(1, sizeof(struct ftpgc_cmd_hist_item));
    strncpy(item->cmd, cmd, 4);

    if (params)
    {
        s32 len = strlen(params);
        if (len)
        {
            s32 len        = strlen(params);
            s32 len_params = (len < FTPGC_CONTROL_REQ_LEN - 4) ? len : FTPGC_CONTROL_REQ_LEN - 4;

            item->params = calloc(len_params + 1, sizeof(char));
            memcpy(item->params, params, len_params * sizeof(char));
            item->params[len_params] = '0';
        }
    }

    return item;
}

void _cmd_hist_del_item(struct ftpgc_cmd_hist_item *item)
{
    if (item->params != NULL)
    {
        free(item->params);
    }
    free(item);
}

void _cmd_hist_print(void)
{
    if (FTPGC_DEBUG)
    {
        s32 i = 0;

        for (i = 0; i < FTPGC_CMD_HIST_LEN; i++)
        {
            if (ftpgc_cmd_hist[i] != NULL)
            {
                printf("DEBUG: cmd hist item #%d: %s, params: %s\n",
                       i,
                       ftpgc_cmd_hist[i]->cmd,
                       (ftpgc_cmd_hist[i]->params != NULL) ? ftpgc_cmd_hist[i]->params : "");
            }
        }
    }
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

    if (cmd_pos != NULL)
    {
        cmd_len = cmd_pos - cmd;
    }
    else
    {
        cmd_len = -1;
        return;
    }

    if (FTPGC_DEBUG)
    {
        printf("DEBUG: cmd length %d\n", cmd_len);
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
    s32 i = 0;

    for (i = 0; i < sizeof(ftpgc_cmds_need_auth) / sizeof(ftpgc_cmds_need_auth[0]); i++)
    {
        if (strncmp(ftpgc_cmds_need_auth[i], cmd_cmd, 4) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void _cmd_reset_hist(void)
{
    s32 i = 0;

    if (FTPGC_DEBUG) {
        printf("DEBUG: cmd hist reset\n");
    }

    for (i = 0; i < FTPGC_CMD_HIST_LEN; i++)
    {
        if (ftpgc_cmd_hist[i] != NULL)
        {
            _cmd_hist_del_item(ftpgc_cmd_hist[i]);
            ftpgc_cmd_hist[i] = NULL;
        }
    }
}

void _cmd_reset_reply_buffer(void)
{
    memset(&cmd_reply_buffer, 0, FTPGC_CONTROL_REPLY_LEN + 1);
}

BOOL _cmd_valid(enum ftpgc_cmd_type type)
{
    s32 i = 0;

    if (type == Single)
    {
        for (i = 0; i < sizeof(ftpgc_cmds_single) / sizeof(ftpgc_cmds_single[0]); i++)
        {
            if (strncmp(ftpgc_cmds_single[i], cmd_cmd, 5) == 0)
            {
                cmd_type = Single;
                return TRUE;
            }
        }
    }
    else
    {
        for (i = 0; i < sizeof(ftpgc_cmds_param) / sizeof(ftpgc_cmds_param[0]); i++)
        {
            if (strncmp(ftpgc_cmds_param[i], cmd_cmd, 5) == 0)
            {
                cmd_type = Param;
                return TRUE;
            }
        }
    }
    cmd_type = Invalid;
    return FALSE;
}
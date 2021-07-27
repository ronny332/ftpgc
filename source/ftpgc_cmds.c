#include "ftpgc_cmds.h"

#include <ctype.h>
#include <network.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ftpgc_auth.h"
#include "ftpgc_const.h"

static const char *ftpgc_cmds_need_auth[] = { "CWD" };
static const char *ftpgc_cmds_param[]     = { "USER", "PASS", "CWD" };
static const char *ftpgc_cmds_single[]    = { "NOOP", "SYST", "QUIT" };

struct ftpgc_cmd_hist_item *ftpgc_cmd_hist[FTPGC_CMD_HIST_LEN] = { NULL };

char  cmd_cmd[FTPGC_CMD_CMD_LEN];
s32   cmd_len = 0;
char  cmd_param[FTPGC_CMD_PARAM_LEN];
char *cmd_pos = NULL;
char  cmd_reply_buffer[FTPGC_CONTROL_REPLY_LEN + 1];
s32   cmd_ret = 0;

enum ftpgc_cmd_type cmd_type = Invalid;

s32 ftpgc_cmd_handle(s32 csock)
{
    if (!_cmd_needs_auth(cmd_cmd))
    {
        switch (cmd_type)
        {
        case Param: cmd_ret = ftpgc_cmd_write_reply(csock, 202, "Command not implemented yet, come back later."); break;
        case Single:
            if (strncmp(cmd_cmd, "NOOP", 4) == 0)
            {
                cmd_ret = ftpgc_cmd_write_reply(csock, 200, "Command okay.");
            }
            else if (strncmp(cmd_cmd, "QUIT", 4) == 0)
            {
                ftpgc_cmd_write_reply(csock, 221, "Goodbye.");
                return FTPGC_EXECUTION_END;
            }
            else if (strncmp(cmd_cmd, "SYST", 4) == 0)
            {
                cmd_ret = ftpgc_cmd_write_reply(csock, 215, "UNIX Type: L8.");
            }
            else
            {
                if (FTPGC_DEBUG)
                {
                    printf("ERROR: should never been reached.\n");
                }
                ftpgc_cmd_write_reply(csock, 421, "Internal Error.");
                return FTPGC_EXECUTION_END;
            }
            break;
        case Invalid:
            cmd_ret = ftpgc_cmd_write_reply(csock, 500, "Command not understood.");
            return FTPGC_EXECUTION_CONTINUE;
        }
    }
    else
    {
        cmd_ret = ftpgc_cmd_write_reply(csock, 530, "Please login with USER and PASS.");
    }

    _cmd_hist_add_item(_cmd_hist_create_item());
    _cmd_hist_print();

    return (cmd_ret) ? FTPGC_EXECUTION_CONTINUE : FTPGC_EXECUTION_END;
}

s32 ftpgc_cmd_parse(const char *cmd)
{
    _cmd_clean();
    _cmd_length(cmd);

    if (cmd_len > 0)
    {
        _cmd_split(cmd);
        if (_cmd_valid(Single))
        {
            if (FTPGC_DEBUG)
            {
                printf("DEBUG: cmd valid for single handling\n");
            }

            return FTPGC_CMD_SINGLE;
        }
        else if (_cmd_valid(Param))
        {
            if (FTPGC_DEBUG)
            {
                printf("DEBUG: cmd valid for param handling, but not ready yet\n");
            }

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

struct ftpgc_cmd_hist_item *_cmd_hist_create_item(void)
{
    struct ftpgc_cmd_hist_item *item = calloc(1, sizeof(struct ftpgc_cmd_hist_item));
    memcpy(item->cmd, cmd_cmd, FTPGC_CMD_CMD_LEN * sizeof(char));

    s32 len = strlen(cmd_param);
    if (len)
    {
        item->params = calloc(len + 1, sizeof(char));
        memcpy(item->params, cmd_param, len * sizeof(char));
        item->params[len] = '\0';
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

    if (FTPGC_DEBUG)
    {
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

void _cmd_split(const char *cmd)
{
    s32 i = 0;

    for (i = 0; i < ((cmd_len < 5) ? cmd_len : 5); i++)
    {
        cmd_cmd[i] = toupper(cmd[i]);

        if (cmd_cmd[i] == '\r' || cmd_cmd[i] == '\n' || cmd_cmd[i] == ' ')
        {
            cmd_cmd[i] = '\0';
        }
    }

    if (FTPGC_DEBUG)
    {
        printf("DEBUG: got cmd \"%s\"\n", cmd_cmd);
    }

    s32 pos = strstr(cmd, " ") - cmd;

    if (pos == 3 || pos == 4)
    {
        const char *param = cmd + pos + 1;
        strncpy(cmd_param, param, FTPGC_CMD_PARAM_LEN - 1);

        s32 param_len = strlen(cmd_param);
        if (cmd_param[param_len - 2] == '\r')
        {
            cmd_param[param_len - 2] = '\0';
        }

        if (FTPGC_DEBUG)
        {
            printf("DEBUG: got param \"%s\"\n", cmd_param);
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
#include "ftpgc_cmds.h"

#include <ctype.h>
#include <network.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ftpgc_auth.h"
#include "ftpgc_const.h"

static const char *ftpgc_cmds_need_auth[] = { "CWD", "PORT" };
static const char *ftpgc_cmds[]           = { "CWD", "NOOP", "PASS", "PORT", "QUIT", "SYST", "USER" };

typedef s32 (*ftpgc_cmd_handle_t)();
static const ftpgc_cmd_handle_t ftpgc_cmds_handles[]
    = { NULL, &_cmd_CWD, &_cmd_NOOP, &_cmd_PASS, &_cmd_PORT, &_cmd_QUIT, &_cmd_SYST, &_cmd_USER };

char  cmd_cmd[FTPGC_CMD_CMD_LEN]                    = { 0 };
s32   cmd_len                                       = 0;
s32   cmd_no                                        = 0;
char  cmd_param[FTPGC_CMD_PARAM_LEN]                = { 0 };
char *cmd_pos                                       = NULL;
char  cmd_reply_buffer[FTPGC_CONTROL_REPLY_LEN + 1] = { 0 };
s32   cmd_ret                                       = 0;
s32   csock                                         = 0;

#ifdef FTPGC_DEBUG
struct ftpgc_cmd_hist_item *ftpgc_cmd_hist[FTPGC_CMD_HIST_LEN] = { NULL };
#endif

s32 ftpgc_cmd_handle(s32 s)
{
    csock = s;

#ifdef FTPGC_DEBUG
    _cmd_hist_add_item(_cmd_hist_create_item());
    _cmd_hist_print();

    printf("DEBUG: got cmd no: %d\n", cmd_no);
#endif

    if (cmd_no)
    {
        cmd_ret = ftpgc_cmds_handles[cmd_no]();
#ifdef FTPGC_DEBUG
        printf("DEBUG: got cmd ret: %d\n", cmd_ret);
#endif
    }
    else
    {
        cmd_ret = __cmd_not_understood();
    }

    return (cmd_ret != FTPGC_EXECUTION_END) ? FTPGC_EXECUTION_CONTINUE : FTPGC_EXECUTION_END;
}

s32 ftpgc_cmd_parse(const char *cmd)
{
    _cmd_clean();
    _cmd_length(cmd);

    if (cmd_len > 0)
    {
        _cmd_split(cmd);
        if (_cmd_detect())
        {

#ifdef FTPGC_DEBUG
            printf("DEBUG: cmd valid.\n");
#endif
            return FTPGC_CMD_VALID;
        }
    }
#ifdef FTPGC_DEBUG
    printf("DEBUG: cmd invalid\n");
#endif
    return FTPGC_CMD_INVALID;
}

#ifdef FTPGC_DEBUG
void ftpgc_cmd_reset_hist(void)
{
    _cmd_hist_reset();
}
#endif

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

BOOL _cmd_detect()
{
    s32 i = 0;

    for (i = 0; i < sizeof(ftpgc_cmds) / sizeof(ftpgc_cmds[0]); i++)
    {
        if (strncmp(ftpgc_cmds[i], cmd_cmd, 5) == 0)
        {
            cmd_no = i + 1;
            return TRUE;
        }
    }
    cmd_no = FTPGC_CMD_INVALID;
    return FALSE;
}

#ifdef FTPGC_DEBUG
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

    printf("DEBUG: clearing hist iten #%d\n", i);

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

void _cmd_hist_reset(void)
{
    s32 i = 0;

    printf("DEBUG: cmd hist reset\n");

    for (i = 0; i < FTPGC_CMD_HIST_LEN; i++)
    {
        if (ftpgc_cmd_hist[i] != NULL)
        {
            _cmd_hist_del_item(ftpgc_cmd_hist[i]);
            ftpgc_cmd_hist[i] = NULL;
        }
    }
}
#endif

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

#ifdef FTPGC_DEBUG
    printf("DEBUG: cmd length %d\n", cmd_len);
#endif

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

#ifdef FTPGC_DEBUG
    printf("DEBUG: got cmd \"%s\"\n", cmd_cmd);
#endif

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

#ifdef FTPGC_DEBUG
        printf("DEBUG: got param \"%s\"\n", cmd_param);
#endif
    }
#ifdef FTPGC_DEBUG
    else
    {
        printf("DEBUG: got no param\n");
    }
#endif
}

s32 _cmd_CWD(void)
{
    if (!ftpgc_auth_logged_in())
    {
        return __cmd_not_logged_in();
    }
    else if (!strlen(cmd_param))
    {
        return __cmd_needs_parameter("CWD");
    }
    else
    {
        return ftpgc_cmd_write_reply(csock, 250, "CWD command successful.");
    }
}

s32 _cmd_NOOP(void)
{
    return ftpgc_cmd_write_reply(csock, 200, "Command okay.");
}

s32 _cmd_PASS(void)
{
    if (ftpgc_auth_logged_in())
    {
        return ftpgc_cmd_write_reply(csock, 503, "Already logged in.");
    }
    else if (strlen(cmd_param))
    {
        if (ftpgc_auth_len_USER())
        {
            ftpgc_auth_set_PASS(cmd_param);

            if (ftpgc_auth_len_PASS())
            {
                ftpgc_auth_login();

                if (ftpgc_auth_logged_in())
                {
                    return ftpgc_cmd_write_reply(csock, 230, "User logged in.");
                }
            }
            ftpgc_auth_logout();
            sleep(3);
            return ftpgc_cmd_write_reply(csock, 530, "Login incorrect.");
        }
        else
        {
            return ftpgc_cmd_write_reply(csock, 503, "PASS requires USER set first.");
        }
    }
    else
    {
        return __cmd_needs_parameter("PASS");
    }
}

s32 _cmd_PORT(void)
{
    if (!ftpgc_auth_logged_in())
    {
        return __cmd_not_logged_in();
    }

    struct
    {
        u32 h1;
        u32 h2;
        u32 h3;
        u32 h4;
        u32 p1;
        u32 p2;
        u32 ip;
        u16 port;
    } PORT_values = { 0 };

    s32 ret = sscanf(cmd_param,
                     "%u,%u,%u,%u,%u,%u",
                     &PORT_values.h1,
                     &PORT_values.h2,
                     &PORT_values.h3,
                     &PORT_values.h4,
                     &PORT_values.p1,
                     &PORT_values.p2);

    if (ret == 6)
    {
        PORT_values.ip   = ((0xffffffff & (PORT_values.h1 << 24)) | (0xffffff & (PORT_values.h2 << 16))
                          | (0xffff & (PORT_values.h3 << 8)) | (0xff & PORT_values.h4));
        PORT_values.port = (u16)((0xffff & (PORT_values.p1 << 8)) | (0xff & PORT_values.p2));
#ifdef FTPGC_DEBUG
        printf("DEBUG: raw PORT_values %u, %u.%u.%u.%u:%u.%u\n",
               ret,
               PORT_values.h1,
               PORT_values.h2,
               PORT_values.h3,
               PORT_values.h4,
               PORT_values.p1,
               PORT_values.p2);

        printf("DEBUG: ip:port %x:%d\n", PORT_values.ip, PORT_values.port);
#endif
        return __cmd_not_understood();
    }
    else
    {
        return __cmd_syntax_error();
    }
}

s32 _cmd_QUIT(void)
{
    ftpgc_cmd_write_reply(csock, 221, "Goodbye.");
    return FTPGC_EXECUTION_END;
}

s32 _cmd_SYST(void)
{
    return ftpgc_cmd_write_reply(csock, 215, "UNIX Type: L8.");
}

s32 _cmd_USER(void)
{
    if (strlen(cmd_param))
    {
        if (ftpgc_auth_logged_in())
        {
            ftpgc_auth_logout();
        }
        ftpgc_auth_set_USER(cmd_param);
        return ftpgc_cmd_write_reply(csock, 331, "Password required, use PASS now.");
    }
    else
    {
        return __cmd_needs_parameter("USER");
    }
}

s32 __cmd_needs_parameter(char *name)
{
    char out_tmp[28];
    sprintf(out_tmp, "%s requires a parameter.", name);
    return ftpgc_cmd_write_reply(csock, 500, out_tmp);
}

s32 __cmd_not_logged_in(void)
{
    return ftpgc_cmd_write_reply(csock, 530, "Please login with USER and PASS.");
}

s32 __cmd_not_understood(void)
{
    return ftpgc_cmd_write_reply(csock, 500, "Not understood.");
}

s32 __cmd_syntax_error(void)
{
    return ftpgc_cmd_write_reply(csock, 501, "Syntax error in parameter or arguments.");
}
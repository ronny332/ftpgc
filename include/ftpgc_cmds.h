#ifndef __FTPGC_CMDS_
#define __FTPGC_CMDS_

#include <gctypes.h>

static const char *ftpgc_cmds_param[]     = { "USER", "PASS" };
static const char *ftpgc_cmds_single[]    = { "NOOP", "SYST", "QUIT", "CWD" };
static const char *ftpgc_cmds_need_auth[] = { "CWD" };

enum ftpgc_cmd_type
{
    Single,
    Param
};

struct ftpgc_cmd_hist_item
{
    char  cmd[5];
    char *params;
};

BOOL ftpgc_handle_single_cmd(s32 csock, const char *cmd);
s32  ftpgc_parse_cmd(const char *cmd, char **ret);
s32  ftpgc_write_reply(s32 csock, u32 code, const char *msg);

void                        _append_cmd_hist_item(struct ftpgc_cmd_hist_item *item);
void                        _clear_cmd_hist(void);
void                        _clear_reply_buffer(void);
void                        _cmd_clean();
void                        _cmd_length(const char *cmd);
BOOL                        _cmd_needs_auth(const char *cmd);
void                        _cmd_reformat(const char *cmd);
BOOL                        _cmd_valid(enum ftpgc_cmd_type type);
struct ftpgc_cmd_hist_item *_create_cmd_hist_item(const char *cmd, const char *params);
void                        _print_cmd_hist(void);

#endif
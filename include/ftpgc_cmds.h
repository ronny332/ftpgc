#ifndef __FTPGC_CMDS_
#define __FTPGC_CMDS_

#include <gctypes.h>

enum ftpgc_cmd_type
{
    Invalid,
    Single,
    Param
};

struct ftpgc_cmd_hist_item
{
    char  cmd[5];
    char *params;
};

BOOL ftpgc_cmd_handle_single(s32 csock, const char *cmd);
s32  ftpgc_cmd_parse(const char *cmd, char **ret);
void ftpgc_cmd_reset_hist(void);
s32  ftpgc_cmd_write_reply(s32 csock, u32 code, const char *msg);

void                        _cmd_clean();
void                        _cmd_copy(const char *cmd);
void                        _cmd_hist_add_item(struct ftpgc_cmd_hist_item *item);
struct ftpgc_cmd_hist_item *_cmd_hist_create_item(const char *cmd, const char *params);
void                        _cmd_hist_del_item(struct ftpgc_cmd_hist_item *item);
void                        _cmd_hist_print(void);
void                        _cmd_length(const char *cmd);
BOOL                        _cmd_needs_auth(const char *cmd);
void                        _cmd_reset_hist(void);
void                        _cmd_reset_reply_buffer(void);
BOOL                        _cmd_valid(enum ftpgc_cmd_type type);

#endif
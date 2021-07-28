#ifndef __FTPGC_CMDS_
#define __FTPGC_CMDS_

#include <gctypes.h>

#include "ftpgc_const.h"

struct ftpgc_cmd_hist_item
{
    char  cmd[5];
    char *params;
};

s32  ftpgc_cmd_handle(s32 s);
s32  ftpgc_cmd_parse(const char *cmd);
#ifdef FTPGC_DEBUG
void ftpgc_cmd_reset_hist(void);
#endif
s32  ftpgc_cmd_write_reply(s32 csock, u32 code, const char *msg);

void _cmd_clean();
BOOL _cmd_detect();
#ifdef FTPGC_DEBUG
void                        _cmd_hist_add_item(struct ftpgc_cmd_hist_item *item);
struct ftpgc_cmd_hist_item *_cmd_hist_create_item(void);
void                        _cmd_hist_del_item(struct ftpgc_cmd_hist_item *item);
void                        _cmd_hist_print(void);
void                        _cmd_hist_reset(void);
#endif
void _cmd_length(const char *cmd);
BOOL _cmd_needs_auth(const char *cmd);
void _cmd_reset_reply_buffer(void);
void _cmd_split(const char *cmd);

s32 _cmd_CWD(void);
s32 _cmd_NOOP(void);
s32 _cmd_PASS(void);
s32 _cmd_PORT(void);
s32 _cmd_QUIT(void);
s32 _cmd_SYST(void);
s32 _cmd_USER(void);

s32 __cmd_needs_parameter(char *name);
s32 __cmd_not_logged_in();
s32 __cmd_not_understood();

#endif
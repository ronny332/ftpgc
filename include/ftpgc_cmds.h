#ifndef __FTPGC_COMMANDS_
#define __FTPGC_COMMANDS_

#include <gctypes.h>

static const char *ftpgc_commands_global[] = {"NOOP", "SYST", "QUIT"};

BOOL ftpgc_handle_single_cmd(s32 csock, const char *cmd);
s32 ftpgc_parse_single_cmd(const char *cmd, char **ret);
s32 ftpgc_write_reply(s32 csock, u32 code, const char *msg);

void _cmd_clean();
void _cmd_length(const char *cmd);
void _cmd_reformat(const char *cmd);
BOOL _cmd_valid();
void _clear_reply_buffer(void);

#endif
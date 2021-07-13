#ifndef __FTPGC_COMMANDS_
#define __FTPGC_COMMANDS_

static const char *ftpgc_commands_global[] = {
    "USER", "PASS", "CWD", "QUIT"
};

s32 ftpgc_parse_cmd(const char *cmd, char **ret);

#endif
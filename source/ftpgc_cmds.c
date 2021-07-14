#include <string.h>

#include "ftpgc_cmds.h"

#include "ftpgc_const.h"

char cmd_tmp[5];

s32 ftpgc_parse_cmd(const char *cmd, char **ret)
{
    memset(&cmd_tmp, 0, 5);

    if (strlen(cmd) <= 4)
    {
        return FTPGC_INVALID_COMMAND;
    }

    memcpy(&cmd_tmp, cmd, 4);

    *ret = &(cmd_tmp[0]);

    return FTPGC_SUCCESS;
}
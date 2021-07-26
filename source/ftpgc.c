#include "ftpgc.h"

#include <network.h>
#include <stdio.h>
#include <string.h>

#include "bba.h"
#include "ftpgc_thread_ctrl.h"

s32 ftpgc_init(void)
{
    init_network();

    if (bba_exists == 0 || net_initialized == 0)
        return FTPGC_NO_NETWORK;

    if (FTPGC_DEBUG)
    {
        printf("DEBUG: got IP address %s\n", bba_ip);
    }

    // while (true)
    for (int i = 0; i < 3; i++)
    {
        if (FTPGC_DEBUG)
        {
            printf("DEBUG: starting new control server...\n");
        }
        s32 ctrl_server_ret = ftpgc_create_ctrl_server();
        if (FTPGC_DEBUG)
        {
            printf("DEBUG: server start %d\n", ctrl_server_ret);
        }
        s32 crtl_server_join_ret = ftpgc_join_ctrl_server();
        if (FTPGC_DEBUG)
        {
            printf("DEBUG: server ended %d\n", crtl_server_join_ret);
        }
    }

    return FTPGC_SUCCESS;
}

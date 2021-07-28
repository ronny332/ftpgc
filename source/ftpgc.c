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
    {
        return FTPGC_NO_NETWORK;
    }

#ifdef FTPGC_DEBUG
    printf("DEBUG: " FTPGC_NAME " started\n");
    printf("DEBUG: got IP address %s\n", bba_ip);
#endif

    // while (true)
    for (int i = 0; i < 3; i++)
    {
#ifdef FTPGC_DEBUG
        printf("DEBUG: starting new control server...\n");
#endif
        s32 ctrl_server_ret = ftpgc_create_ctrl_server();
#ifdef FTPGC_DEBUG
        printf("DEBUG: server start %d\n", ctrl_server_ret);
#endif
        s32 crtl_server_join_ret = ftpgc_join_ctrl_server();
#ifdef FTPGC_DEBUG
        printf("DEBUG: server ended %d\n", crtl_server_join_ret);
#endif
    }

    return FTPGC_SUCCESS;
}

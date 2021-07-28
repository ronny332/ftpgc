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

    while (true)
    {
#ifdef FTPGC_DEBUG
        printf("DEBUG: starting new control server...\n");
        s32 ctrl_server_ret = ftpgc_create_ctrl_server();
        printf("DEBUG: server start %d\n", ctrl_server_ret);
#else
        ftpgc_create_ctrl_server();
#endif
#ifdef FTPGC_DEBUG
        s32 crtl_server_join_ret = ftpgc_join_ctrl_server();
        printf("DEBUG: server ended %d\n", crtl_server_join_ret);
#else
        ftpgc_join_ctrl_server();
#endif
    }

    return FTPGC_SUCCESS;
}

#include <network.h>
#include <stdio.h>
#include <string.h>

#include "bba.h"
#include "ftpgc.h"
#include "ftpgc_thread_ctrl.h"

s32 ftpgc_init(void)
{
    init_network();

    if (bba_exists == 0 || net_initialized == 0)
        return FTPGC_NO_NETWORK;

    printf("ip: %s\n", bba_ip);
    // while (true)
    for (int i = 0; i < 3; i++)
    {
        printf("new control server...\n");
        printf("server start: %d\n", ftpgc_create_ctrl_server());
        printf("server ended: %d\n", ftpgc_join_ctrl_server());
    }

    return FTPGC_SUCCESS;
}

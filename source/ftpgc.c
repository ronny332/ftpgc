#include <network.h>
#include <stdio.h>
#include <string.h>

#include "bba.h"
#include "ftpgc.h"
#include "ftpgc_control.h"

s32 ftpgc_init(void)
{
    init_network();

    if (bba_exists == 0 || net_initialized == 0)
        return FTPGC_NO_NETWORK;

    printf("ip: %s\n", bba_ip);
    // while (true)i
    for (int i = 0; i < 3; i++)
    {
        printf("new control server...\n");
        printf("%d\n", ftpgc_create_control_server());
        ftpgc_join_control_server();
    }

    return FTPGC_SUCCESS;
}

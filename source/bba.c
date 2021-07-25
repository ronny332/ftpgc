/* rebuild of needed Swiss-GC files, to make BBA handling identical */

#include <gctypes.h>
#include <network.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exi.h"

/* Network Globals */
int  net_initialized = 0;
int  bba_exists      = 0;
char bba_ip[16];

// Init the GC net interface (bba)
void init_network(void *args)
{

    int res    = 0;
    bba_exists = TRUE; // exi_bba_exists();
    printf("OK\n");

    if (bba_exists && !net_initialized)
    {
        res = if_config(bba_ip, NULL, NULL, TRUE, 20);
        if (res >= 0 && strcmp("255.255.255.255", bba_ip))
        {
            net_initialized = 1;
        }
        else
        {
            memset(bba_ip, 0, sizeof(bba_ip));
            net_initialized = 0;
        }
    }
}
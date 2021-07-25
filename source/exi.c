/* rebuild of needed Swiss-GC files, to make BBA handling identical */
#include "exi.h"

#include <ogc/exi.h>

#include <stdio.h>

int exi_bba_exists()
{
    return exi_get_id(EXI_CHANNEL_0, EXI_DEVICE_2) == EXI_BBA_ID;
}

unsigned int exi_get_id(int chn, int dev)
{
    u32 cid = 0;
    EXI_GetID(chn, dev, &cid);
    // printf("id: %d\nchn: %d, dev: %d\n", cid, chn, dev);
    return cid;
}
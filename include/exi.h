/* rebuild of needed Swiss-GC files, to make BBA handling identical */

#ifndef __EXI_H
#define __EXI_H

// EXI Device ID's
#define EXI_BBA_ID 		0x04020200

int exi_bba_exists();
unsigned int exi_get_id(int chn, int dev);

#endif
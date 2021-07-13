#ifndef __FTPCG_CONTROL_
#define __FTPCG_CONTROL_

#include <gctypes.h>

s32 ftpgc_create_control_server(void);

s32 ftpgc_join_control_server(void);

void _ftpgc_clear_req_buffer(void);

void _ftpgc_close_socket(void);

void *_ftpgc_control_handle(void *arg);

#endif
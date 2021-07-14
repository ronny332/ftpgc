#ifndef __FTPCG_THREAD_CTRL_
#define __FTPCG_THREAD_CTRL_

#include <gctypes.h>

s32 ftpgc_create_ctrl_server(void);

s32 ftpgc_join_ctrl_server(void);

void _ftpgc_clear_req_buffer(void);

void _ftpgc_close_socket(void);

void *_ftpgc_ctrl_handle(void *arg);

#endif
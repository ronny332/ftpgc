#ifndef __FTPCG_THREAD_CTRL_
#define __FTPCG_THREAD_CTRL_

#include <gctypes.h>

s32 ftpgc_create_ctrl_server(void);
s32 ftpgc_join_ctrl_server(void);

void _clear_req_buffer(void);
void _close_socket(BOOL shutdown);
void *_ctrl_handle(void *arg);

#endif
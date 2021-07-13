#ifndef __FTPGC_THREAD_
#define __FTPGC_THREAD_

#include <gccore.h>

typedef void *(*ftpgc_thread_callback)(void *ret);

enum ftpgc_thread_type
{
    Control,
    Data
};

s32 ftpgc_thread_create(enum ftpgc_thread_type type, ftpgc_thread_callback cb, void *ret_void_ptr);
s32 ftpgc_thread_join(enum ftpgc_thread_type type);

#endif
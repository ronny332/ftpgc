#ifndef __FTPGC_THREAD_
#define __FTPGC_THREAD_

#include <gccore.h>

typedef void *(*ftpgc_thread_callback)(void *arg);

static lwpq_t ftpgc_thread_control = (lwp_t)NULL;
static lwpq_t ftpgc_thread_data = (lwp_t)NULL;

enum ftpgc_thread_type
{
    Control,
    Data
};

s32 ftpgc_thread_create(enum ftpgc_thread_type type, ftpgc_thread_callback cb);

#endif
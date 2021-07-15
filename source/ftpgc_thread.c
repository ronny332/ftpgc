#include <stdio.h>
#include <unistd.h>

#include "ftpgc_thread.h"
#include "ftpgc_const.h"

lwpq_t t_control = 0;
lwpq_t t_data = 0;

s32 ftpgc_thread_create(enum ftpgc_thread_type type, ftpgc_thread_callback cb)
{
    switch (type)
    {
    case Control:
        if (!t_control)
        {
            return (LWP_CreateThread(&t_control, cb, NULL, NULL, 17 * 1024, 50) == 0) ? FTPGC_SUCCESS : FTPGC_CTRL_THREAD_ERROR;
        }
        else
        {
            return FTPGC_CTRL_THREAD_ALREADY_CREATED;
        }
    case Data:
        if (!t_data)
        {
            return (LWP_CreateThread(&t_data, cb, NULL, NULL, 16 * 1024, 50) == 0) ? FTPGC_SUCCESS : FTPGC_DATA_THREAD_ERROR;
        }
        else
        {
            return FTPGC_DATA_THREAD_ALREADY_CREATED;
        }
        return 0;
    }

    return FTPGC_SUCCESS;
}

s32 ftpgc_thread_join(enum ftpgc_thread_type type)
{
    s32 ret_thread = 0;
    s32 *ret_thread_ptr = &ret_thread;
    switch (type)
    {
    case Control:
        if (t_control)
        {
            LWP_JoinThread(t_control, (void **)&ret_thread_ptr);
            t_control = 0;
            return ret_thread;
        }
        else
        {
            return FTPGC_CTRL_THREAD_NOT_RUNNING;
        }
    case Data:
        if (t_data)
        {
            LWP_JoinThread(t_data, (void **)&ret_thread_ptr);
            t_data = 0;
            return ret_thread;
        }
        else
        {
            return FTPGC_DATA_THREAD_NOT_RUNNING;
        }
    }

    return FTPGC_THREAD_ERROR;
}

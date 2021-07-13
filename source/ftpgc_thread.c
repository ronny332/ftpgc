#include <stdio.h>
#include <unistd.h>

#include "ftpgc_thread.h"
#include "ftpgc_returns.h"

lwpq_t t_control = 0;
lwpq_t t_data = 0;

s32 ftpgc_thread_create(enum ftpgc_thread_type type, ftpgc_thread_callback cb, void *ret)
{
    switch (type)
    {
    case Control:
        if (!t_control)
        {
            return (LWP_CreateThread(&t_control, cb, ret, NULL, 16 * 1024, 50) == 0) ? FTPGC_SUCCESS : FTPGC_CONTROLTHREADERROR;
        }
        else
        {
            return FTPGC_CONTROLALREADYCREATED;
        }
    case Data:
        if (!t_data)
        {
            return (LWP_CreateThread(&t_data, cb, ret, NULL, 16 * 1024, 50) == 0) ? FTPGC_SUCCESS : FTPGC_DATATHREADERROR;
        }
        else
        {
            return FTPGC_DATAALREADYCREATED;
        }
        return 0;
    }

    return FTPGC_SUCCESS;
}

s32 ftpgc_thread_join(enum ftpgc_thread_type type)
{
    s32 ret;
    s32 *ret_ptr = &ret;
    switch (type)
    {
    case Control:
        if (t_control)
        {
            LWP_JoinThread(t_control, (void **)&ret_ptr);
            return ret;
        }
        else
        {
            return FTPGC_CONTROLNOTRUNNING;
        }
    case Data:
        if (t_data)
        {
            LWP_JoinThread(t_data, (void **)&ret_ptr);
            return ret;
        }
        else
        {
            return FTPGC_DATANOTRUNNING;
        }
    }

    return FTPGC_THREADERROR;
}

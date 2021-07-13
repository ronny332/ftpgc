#include <unistd.h>

#include "ftpgc_thread.h"
#include "ftpgc_returns.h"

s32 ftpgc_thread_create(enum ftpgc_thread_type type, ftpgc_thread_callback cb)
{
    switch (type)
    {
    case Control:
        int fake_val = 4711;
    
        if (ftpgc_thread_control == (lwp_t)NULL)
        {
            LWP_CreateThread(&ftpgc_thread_control, cb, &fake_val, NULL, 16 * 1024, 50);
            sleep(2);
            printf("%d\n", fake_val);
            return FTPGC_SUCCESS;
        }
        else
        {
            return FTPGC_CONTROLALREADYCREATED;
        }
    case Data:
        if (ftpgc_thread_control == (lwp_t)NULL)
        {
            LWP_CreateThread(&ftpgc_thread_data, cb, &fake_val, NULL, 16 * 1024, 50);
            return FTPGC_SUCCESS;
        }
        else
        {
            return FTPGC_DATAALREADYCREATED;
        }
        return 0;
    }

    return FTPGC_SUCCESS;
}
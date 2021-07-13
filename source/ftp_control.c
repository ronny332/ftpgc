#include "ftpgc_control.h"

#include "ftpgc_returns.h"
#include "ftpgc_thread.h"

s32 ftpgc_create_control_server()
{
    ftpgc_thread_create(Control, _ftpgc_control_handle);
    printf("created\n");
    return FTPGC_SUCCESS;
}

void *_ftpgc_control_handle(void *arg)
{
    int *val = arg;
    *(val) = 4712;
    while (true)
    {
        printf(".");
        sleep(1);
    }
}
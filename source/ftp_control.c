#include <stdio.h>
#include <unistd.h>
#include <network.h>
#include <stdio.h>
#include <string.h>

#include "ftpgc_constants.h"
#include "ftpgc_control.h"

#include "ftpgc_returns.h"
#include "ftpgc_thread.h"

s32 ret_server;

s32 ftpgc_create_control_server()
{
    ftpgc_thread_create(Control, _ftpgc_control_handle, (void *)&ret_server);
    return FTPGC_SUCCESS;
}

s32 ftpgc_join_control_server(void) {
    return ftpgc_thread_join(Control);
}

s32 csock, sock;
u32 client_len;
struct sockaddr_in client, server;
s32 ret;

void *_ftpgc_control_handle(void *ret_void)
{
    int *ret_s32 = (int *)ret_void;

    memset(&client, 0, sizeof(client));
    memset(&server, 0, sizeof(server));
    client_len = sizeof(client);

    sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sock == INVALID_SOCKET)
    {
        *ret_s32 = FTPGC_NOSOCKET;
        return NULL;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(FTPCG_PORT_CONTROL);
    server.sin_addr.s_addr = INADDR_ANY;
    ret = net_bind(sock, (struct sockaddr *)&server, sizeof(server));

    if (ret)
    {
        *ret_s32 = FTPGC_NOSOCKETBIND;
        return NULL;
    }

    ret = net_listen(sock, 5);
    printf("ret: %d\n", ret);

    csock = net_accept(sock, (struct sockaddr *)&client, &client_len);

    if (csock < 0)
    {
        *ret_s32 = FTPGC_SOCKETERROR;
        return NULL;
    }

    printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));

    if (ret)
    {
        *ret_s32 = FTPGC_NOCONTROL;
        return NULL;
    }

    net_close(csock);
    net_close(sock);

    *ret_s32 = FTPGC_SUCCESS;
    return NULL;
}
#include <stdio.h>
#include <unistd.h>
#include <network.h>
#include <stdio.h>
#include <string.h>

#include "ftpgc_commands.h"
#include "ftpgc_constants.h"
#include "ftpgc_control.h"
#include "ftpgc_returns.h"
#include "ftpgc_thread.h"

const char *ftpgc_welcome = "220 welcome to GCFTP\r\n";

s32 ret_server = 0;
s32 ret_thread = 0;
s32 csock = -1, sock = -1;
u32 client_len = -1;
struct sockaddr_in client, server;
char req_buffer[FTPGC_CONTROL_REQ_LEN + 1];
s32 ret = -1;
int *ret_s32_ptr = (int *)NULL;

s32 ftpgc_create_control_server()
{
    ret_thread = ftpgc_thread_create(Control, _ftpgc_control_handle, (void *)&ret_server);

    return (ret_server != FTPGC_SUCCESS) ? ret_server : ret_thread;
}

s32 ftpgc_join_control_server(void)
{
    return ftpgc_thread_join(Control);
}

void _ftpgc_clear_req_buffer(void)
{
    memset(&req_buffer, 0, FTPGC_CONTROL_REQ_LEN + 1);
}

void _ftpgc_close_socket(void)
{
    if (csock <= 0)
    {
        net_close(csock);
        csock = -1;
    }
}

void *_ftpgc_control_handle(void *ret_void_ptr)
{
    ret_s32_ptr = (s32 *)ret_void_ptr;
    _ftpgc_close_socket();

    memset(&client, 0, sizeof(client));
    memset(&server, 0, sizeof(server));
    client_len = sizeof(client);

    if (sock < 0)
    {
        sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

        if (sock == INVALID_SOCKET)
        {
            *ret_s32_ptr = FTPGC_NO_SOCKET;
            return NULL;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(FTPCG_PORT_CONTROL);
        server.sin_addr.s_addr = INADDR_ANY;
        ret = net_bind(sock, (struct sockaddr *)&server, sizeof(server));

        if (ret)
        {
            *ret_s32_ptr = FTPGC_NO_SOCKET_BIND;
            return NULL;
        }
    }

    ret = net_listen(sock, 5);

    if (ret)
    {
        *ret_s32_ptr = FTPGC_SOCKET_LISTEN_ERROR;
        return NULL;
    }

    csock = net_accept(sock, (struct sockaddr *)&client, &client_len);

    if (csock < 0)
    {
        *ret_s32_ptr = FTPGC_SOCKET_ERROR;
        return NULL;
    }

    printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));
    ret = net_send(csock, ftpgc_welcome, strlen(ftpgc_welcome), 0);

    while (true)
    {
        _ftpgc_clear_req_buffer();

        ret = net_recv(csock, req_buffer, FTPGC_CONTROL_REQ_LEN, 0);

        if (!ret)
        {
            *ret_s32_ptr = FTPGC_CTRL_THREAD_SEND_ERROR;
            return NULL;
        }

        if (ret <= 0)
        {
            *ret_s32_ptr = FTPGC_NO_INPUT;
            return NULL;
        }

        ret = net_send(csock, "111 OK\r\n", 8, 0);

        if (!ret)
        {
            *ret_s32_ptr = FTPGC_CTRL_THREAD_SEND_ERROR;
            return NULL;
        }

        char *cmd;
        if (ftpgc_parse_cmd(req_buffer, &cmd))
        {
            printf("cmd: %s\n", cmd);
        }
    }

    _ftpgc_close_socket();

    *ret_s32_ptr = FTPGC_SUCCESS;
    return NULL;
}

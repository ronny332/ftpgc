#include <stdio.h>
#include <unistd.h>
#include <network.h>
#include <stdio.h>
#include <string.h>

#include "ftpgc_cmds.h"
#include "ftpgc_const.h"
#include "ftpgc_thread_ctrl.h"
#include "ftpgc_thread.h"

const char *ftpgc_welcome = "220 welcome to GCFTP\r\n";

char *cmd;
BOOL execution_end = FALSE;
s32 ret_handle = 0;
s32 ret_thread = 0;
s32 csock = -1, sock = -1;
u32 client_len = -1;
struct sockaddr_in client, server;
char req_buffer[FTPGC_CONTROL_REQ_LEN + 1];
s32 ret = -1;

s32 ftpgc_create_ctrl_server()
{
    ret_thread = ftpgc_thread_create(Control, _ctrl_handle);

    return ret_thread;
}

s32 ftpgc_join_ctrl_server(void)
{
    ret_thread = ftpgc_thread_join(Control);
    return ret_handle;
}

void _clear_req_buffer(void)
{
    memset(&req_buffer, 0, FTPGC_CONTROL_REQ_LEN + 1);
}

void _close_socket(BOOL shutdown)
{
    if (csock > 0)
    {
        if (shutdown)
        {
            net_shutdown(csock, 2);
        }
        net_close(csock);
        csock = -1;
    }
}

void *_ctrl_handle(void *ret_void_ptr)
{
    _close_socket(TRUE);

    memset(&client, 0, sizeof(client));
    memset(&server, 0, sizeof(server));
    client_len = sizeof(client);

    if (sock < 0)
    {
        sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

        if (sock == INVALID_SOCKET)
        {
            ret_handle = FTPGC_NO_SOCKET;
            return NULL;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(FTPCG_PORT_CONTROL);
        server.sin_addr.s_addr = INADDR_ANY;
        ret = net_bind(sock, (struct sockaddr *)&server, sizeof(server));

        if (ret)
        {
            ret_handle = FTPGC_NO_SOCKET_BIND;
            return NULL;
        }
    };

    ret = net_listen(sock, 5);

    if (ret)
    {
        ret_handle = FTPGC_SOCKET_LISTEN_ERROR;
        return NULL;
    }

    csock = net_accept(sock, (struct sockaddr *)&client, &client_len);

    if (csock < 0)
    {
        ret_handle = FTPGC_SOCKET_ERROR;
        return NULL;
    }

    printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));
    ret = net_send(csock, ftpgc_welcome, strlen(ftpgc_welcome), 0);

    while (true)
    {
        execution_end = FALSE;
        _clear_req_buffer();

        ret = net_recv(csock, req_buffer, FTPGC_CONTROL_REQ_LEN, 0);

        if (!ret)
        {
            ret_handle = FTPGC_CTRL_THREAD_RECV_ERROR;
            return NULL;
        }

        if (ret <= 0)
        {
            ret_handle = FTPGC_NO_INPUT;
            return NULL;
        }

        if (ftpgc_parse_cmd(req_buffer, &cmd) == FTPGC_VALID)
        {
            if (strncmp(cmd, "NOOP", 4) == 0)
            {
                ret = ftpgc_write_reply(csock, 200, "Command okay.");
            }
            else if (strncmp(cmd, "QUIT", 4) == 0)
            {
                ret = ftpgc_write_reply(csock, 221, "Goodbye.");
                execution_end = TRUE;
            }
            else
            {
                ret = ftpgc_write_reply(csock, 500, "Command not understood.");
            }
        }
        else
        {
            ret = ftpgc_write_reply(csock, 502, "Command not implemented.");
        }
        if (!ret)
        {
            ret_handle = FTPGC_DATA_THREAD_SEND_ERROR;
            return NULL;
        }
        if (execution_end)
        {
            ret_handle = FTPGC_EXECUTION_END;
            _close_socket(TRUE);
            return NULL;
        }
    }

    _close_socket(TRUE);

    ret_handle = FTPGC_SUCCESS;
    return NULL;
}

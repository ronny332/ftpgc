#include <stdio.h>
#include <unistd.h>
#include <network.h>
#include <stdio.h>
#include <string.h>

#include "ftpgc_auth.h"
#include "ftpgc_cmds.h"
#include "ftpgc_const.h"
#include "ftpgc_thread_ctrl.h"
#include "ftpgc_thread.h"

const char *ftpgc_welcome = "220 welcome to GCFTP\r\n";

u32 ctrl_client_len = -1;
struct sockaddr_in ctrl_client, ctrl_server;
char *ctrl_cmd;
s32 ctrl_csock = -1, ctrl_sock = -1;
BOOL ctrl_execution_end = FALSE;
char ctrl_req_buffer[FTPGC_CONTROL_REQ_LEN + 1];
s32 ctrl_ret = -1;
s32 ctrl_ret_cmd = -1;
s32 ctrl_ret_handle = 0;
s32 ctrl_ret_thread = 0;

s32 ftpgc_create_ctrl_server()
{
    ctrl_ret_thread = ftpgc_thread_create(Control, _ctrl_handle);

    return ctrl_ret_thread;
}

s32 ftpgc_join_ctrl_server(void)
{
    ctrl_ret_thread = ftpgc_thread_join(Control);
    return ctrl_ret_handle;
}

void _clear_req_buffer(void)
{
    memset(&ctrl_req_buffer, 0, FTPGC_CONTROL_REQ_LEN + 1);
}

void _close_socket(BOOL shutdown)
{
    if (ctrl_csock > 0)
    {
        if (shutdown)
        {
            net_shutdown(ctrl_csock, 2);
        }
        net_close(ctrl_csock);
        ctrl_csock = -1;
    }
}

void *_ctrl_handle(void *ret_void_ptr)
{
    _close_socket(TRUE);

    memset(&ctrl_client, 0, sizeof(ctrl_client));
    memset(&ctrl_server, 0, sizeof(ctrl_server));
    ctrl_client_len = sizeof(ctrl_client);

    if (ctrl_sock < 0)
    {
        ctrl_sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

        if (ctrl_sock == INVALID_SOCKET)
        {
            ctrl_ret_handle = FTPGC_NO_SOCKET;
            return NULL;
        }

        ctrl_server.sin_family = AF_INET;
        ctrl_server.sin_port = htons(FTPCG_PORT_CONTROL);
        ctrl_server.sin_addr.s_addr = INADDR_ANY;
        ctrl_ret = net_bind(ctrl_sock, (struct sockaddr *)&ctrl_server, sizeof(ctrl_server));

        if (ctrl_ret)
        {
            ctrl_ret_handle = FTPGC_NO_SOCKET_BIND;
            return NULL;
        }
    };

    ctrl_ret = net_listen(ctrl_sock, 5);

    if (ctrl_ret)
    {
        ctrl_ret_handle = FTPGC_SOCKET_LISTEN_ERROR;
        return NULL;
    }

    ctrl_csock = net_accept(ctrl_sock, (struct sockaddr *)&ctrl_client, &ctrl_client_len);

    if (ctrl_csock < 0)
    {
        ctrl_ret_handle = FTPGC_SOCKET_ERROR;
        return NULL;
    }

    printf("Connecting port %d from %s\n", ctrl_client.sin_port, inet_ntoa(ctrl_client.sin_addr));
    ctrl_ret = net_send(ctrl_csock, ftpgc_welcome, strlen(ftpgc_welcome), 0);

    while (true)
    {
        ctrl_execution_end = FALSE;
        _clear_req_buffer();

        ctrl_ret = net_recv(ctrl_csock, ctrl_req_buffer, FTPGC_CONTROL_REQ_LEN, 0);

        if (!ctrl_ret)
        {
            ctrl_ret_handle = FTPGC_CTRL_THREAD_RECV_ERROR;
            return NULL;
        }

        if (ctrl_ret <= 0)
        {
            ctrl_ret_handle = FTPGC_NO_INPUT;
            return NULL;
        }

        ctrl_ret_cmd = ftpgc_parse_cmd(ctrl_req_buffer, &ctrl_cmd);
        if (ctrl_ret_cmd == FTPGC_CMD_SINGLE)
        {
            ctrl_execution_end = ftpgc_handle_single_cmd(ctrl_csock, ctrl_cmd);
        }
        else if (ctrl_ret_cmd == FTPGC_CMD_MULTI)
        {
            // TODO
        }
        else
        {
            ctrl_ret = ftpgc_write_reply(ctrl_csock, 502, "Command not implemented.");
        }
        if (!ctrl_ret)
        {
            ctrl_ret_handle = FTPGC_DATA_THREAD_SEND_ERROR;
            return NULL;
        }
        if (ctrl_execution_end)
        {
            ctrl_ret_handle = FTPGC_EXECUTION_END;
            _close_socket(TRUE);
            ftpgc_authentificated = FALSE;
            return NULL;
        }
    }

    _close_socket(TRUE);

    ctrl_ret_handle = FTPGC_SUCCESS;
    return NULL;
}

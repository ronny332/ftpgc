#include <network.h>
#include <stdio.h>
#include <string.h>

#include "bba.h"
#include "ftpgc.h"

s32 csock, sock;
u32 client_len;
struct sockaddr_in client, server;
s32 ret;

s32 ftpgc_init(void)
{
    init_network();

    if (bba_exists == 0 || net_initialized == 0)
        return FTPGC_NONETWORK;

    printf("ip: %s\n", bba_ip);

    memset(&client, 0, sizeof(client));
    memset(&server, 0, sizeof(server));
    client_len = sizeof(client);

    sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sock == INVALID_SOCKET)
        return FTPGC_NOSOCKET;

    server.sin_family = AF_INET;
    server.sin_port = htons(FTPCG_PORT_CONTROL);
    server.sin_addr.s_addr = INADDR_ANY;
    ret = net_bind(sock, (struct sockaddr *)&server, sizeof(server));

    if (ret)
        return FTPGC_NOSOCKETBIND;

    ret = net_listen(sock, 5);

    csock = net_accept(sock, (struct sockaddr *)&client, &client_len);

    if (csock < 0)
        return FTPGC_SOCKETERROR;

    printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));

    if (ret)
        return FTPGC_NOCONTROL;

    net_close(csock);
    net_close(sock);

    return FTPGC_SUCCESS;
}

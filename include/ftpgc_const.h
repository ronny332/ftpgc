#ifndef __FTPGC_CONST_
#define __FTPGC_CONST_

#define FTPGC_DEBUG

#define FTPGC_NAME "GCFTP"

#define FTPCG_PORT_CONTROL      21
#define FTPGC_PORT_DATA         20
#define FTPGC_CONTROL_REPLY_LEN 1024
#define FTPGC_CONTROL_REQ_LEN   1024
#define FTPGC_CMD_CMD_LEN       6
#define FTPGC_CMD_HIST_LEN      7
#define FTPGC_CMD_PARAM_LEN     1018

#define FTPGC_NO_NETWORK                  -1
#define FTPGC_NO_SOCKET                   -2
#define FTPGC_NO_SOCKET_BIND              -3
#define FTPGC_NO_CONTROL_THREAD           -4
#define FTPGC_SOCKET_ERROR                -5
#define FTPGC_CTRL_THREAD_ALREADY_CREATED -6
#define FTPGC_DATA_THREAD_ALREADY_CREATED -7
#define FTPGC_CTRL_THREAD_NOT_RUNNING     -8
#define FTPGC_DATA_THREAD_NOT_RUNNING     -9
#define FTPGC_CTRL_THREAD_ERROR           -10
#define FTPGC_DATA_THREAD_ERROR           -11
#define FTPGC_THREAD_ERROR                -12
#define FTPGC_SOCKET_LISTEN_ERROR         -13
#define FTPGC_NO_INPUT                    -14
#define FTPGC_CMD_INVALID                 -15
#define FTPGC_CTRL_THREAD_RECV_ERROR      -16
#define FTPGC_CTRL_THREAD_SEND_ERROR      -17
#define FTPGC_DATA_THREAD_RECV_ERROR      -18
#define FTPGC_DATA_THREAD_SEND_ERROR      -19
#define FTPGC_EXECUTION_END               -20

#define FTPGC_SUCCESS            1
#define FTPGC_VALID              1
#define FTPGC_EXECUTION_CONTINUE 2
#define FTPGC_CMD_VALID          3

#define FTPGC_AUTH_USER "test"
#define FTPGC_AUTH_PASS "1234"
#endif

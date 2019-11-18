#ifndef __COMM_H__
#define __COMM_H__
#include <board.h>

extern rt_mailbox_t com;
typedef struct
{
    rt_uint32_t buffer;
    rt_uint32_t type;
}opc_msg;


#endif // comm.h

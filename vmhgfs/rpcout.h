/*********************************************************
 * Copyright (C) 2007 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2 and no later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 *********************************************************/

/*
 * rpcout.h --
 *
 *    Remote Procedure Call between VMware and guest applications
 *    C declarations
 */


#ifndef __RPCOUT_H__
#   define __RPCOUT_H__

#include "vm_basic_types.h"

#define RPCI_PROTOCOL_NUM       0x49435052 /* 'RPCI' ;-) */

typedef struct RpcOut RpcOut;

RpcOut *RpcOut_Construct(void);
void RpcOut_Destruct(RpcOut *out);
Bool RpcOut_start(RpcOut *out);
Bool RpcOut_send(RpcOut *out, char const *request, size_t reqLen,
                 char const **reply, size_t *repLen);
Bool RpcOut_stop(RpcOut *out);


/*
 * This is the only method needed to send a message to vmware for
 * 99% of uses. I'm leaving the others defined here so people know
 * they can be exported again if the need arises. [greg]
 */
Bool RpcOut_sendOne(char **reply, size_t *repLen, char const *reqFmt, ...);

/* 
 * A new version of the above function that works with UTF-8 strings
 * and other data that would be corrupted by Win32's FormatMessage
 * function (which is used by RpcOut_sendOne()).
 */

Bool RpcOut_SendOneRaw(void *request, size_t reqLen, char **reply, size_t *repLen);

/*
 * As the above but must be run by admin/root to make the privileged RPC call successfully.
 */
Bool RpcOut_SendOneRawPriv(void *request, size_t reqLen, char **reply, size_t *repLen);

#endif /* __RPCOUT_H__ */

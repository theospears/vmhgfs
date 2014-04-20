/*********************************************************
 * Copyright (C) 1999 VMware, Inc. All rights reserved.
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
 * message.h --
 *
 *    Second layer of the internal communication channel between guest
 *    applications and vmware
 */

#ifndef __MESSAGE_H__
#   define __MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vm_basic_types.h"


typedef struct Message_Channel Message_Channel;

Message_Channel *
Message_Open(uint32 proto); // IN

Bool
Message_Send(Message_Channel *chan,    // IN/OUT
             const unsigned char *buf, // IN
             size_t bufSize);          // IN

Bool
Message_Receive(Message_Channel *chan, // IN/OUT
                unsigned char **buf,   // OUT
                size_t *bufSize);      // OUT

Bool
Message_Close(Message_Channel *chan); // IN/OUT

#ifdef __cplusplus
}
#endif

#endif /* __MESSAGE_H__ */

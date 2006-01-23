/*-------------------------------------------------------------------------
 * Copyright (C) 2000 Caldera Systems, Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    Neither the name of Caldera Systems nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * `AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CALDERA
 * SYSTEMS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-------------------------------------------------------------------------*/

/** Functions common to both versions of SLP wire protocol.
 *
 * @file       slp_message.c
 * @author     John Calcote (jcalcote@novell.com)
 * @attention  Please submit patches to http://www.openslp.org
 * @ingroup    CommonCode
 */

#include "slp_socket.h"
#include "slp_message.h"
#include "slp_xmalloc.h"
#include "slp_v2message.h"

#if defined(ENABLE_SLPv1)
# include "slp_v1message.h"
#endif

/** Extract a 16-bit big-endian buffer value into a native 16-bit word.
 *
 * @param[in,out] cpp - The address of a pointer from which to extract.
 *
 * @return A 16-bit unsigned value in native format; the buffer pointer
 *    is moved ahead by 2 bytes on return.
 */
uint16_t GetUINT16(const uint8_t ** cpp)
{
   uint16_t rv = AS_UINT16(*cpp);
   *cpp += 2;
   return rv;
}

/** Extract a 24-bit big-endian buffer value into a native 32-bit word.
 *
 * @param[in,out] cpp - The address of a pointer from which to extract.
 *
 * @return A 32-bit unsigned value in native format; the buffer pointer 
 *    is moved ahead by 3 bytes on return.
 */
uint32_t GetUINT24(const uint8_t ** cpp)
{
   uint32_t rv = AS_UINT24(*cpp);
   *cpp += 3;
   return rv;
}

/** Extract a 32-bit big-endian buffer value into a native 32-bit word.
 *
 * @param[in,out] cpp - The address of a pointer from which to extract.
 *
 * @return A 32-bit unsigned value in native format; the buffer pointer
 *    is moved ahead by 4 bytes on return.
 */
uint32_t GetUINT32(const uint8_t ** cpp)
{
   uint32_t rv = AS_UINT32(*cpp);
   *cpp += 4;
   return rv;
}

/** Extract a string buffer address into a character pointer.
 *
 * Note that this routine doesn't actually copy the string. It only casts
 * the buffer pointer to a character pointer and moves the value at @p cpp 
 * ahead by @p len bytes.
 *
 * @param[in,out] cpp - The address of a pointer from which to extract.
 * @param[in] len - The length of the string to extract.
 *
 * @return A pointer to the first character at the address pointed to by 
 *    @p cppstring pointer; the buffer pointer is moved ahead by @p len bytes
 *    on return. If @p len is zero, returns NULL.
 */
char * GetStrPtr(const uint8_t ** cpp, size_t len)
{
   char * sp = len? (char *)*cpp: 0;
   *cpp += len;
   return sp;
}

/** Insert a 16-bit native word into a buffer in big-endian format.
 *
 * @param[in,out] cpp - The address of a pointer where @p val is written.
 * @param[in]     val - A 16-bit native value to be inserted into @p cpp.
 *
 * @note The buffer address is moved ahead by 2 bytes on return.
 */
void PutUINT16(uint8_t ** cpp, size_t val)
{
   TO_UINT16(*cpp, val);
   *cpp += 2;
}

/** Insert a 24-bit native word into a buffer in big-endian format.
 *
 * @param[in,out] cpp - The address of a pointer where @p val is written.
 * @param[in]     val - A 24-bit native value to be inserted into @p cpp.
 *
 * @note The buffer address is moved ahead by 3 bytes on return.
 */
void PutUINT24(uint8_t ** cpp, size_t val)
{
   TO_UINT24(*cpp, val);
   *cpp += 3;
}

/** Insert a 32-bit native word into a buffer in big-endian format.
 *
 * @param[in,out] cpp - The address of a pointer where @p val is written.
 * @param[in]     val - A 32-bit native value to be inserted into @p cpp.
 *
 * @note The buffer address is moved ahead by 4 bytes on return.
 */
void PutUINT32(uint8_t ** cpp, size_t val)
{
   TO_UINT32(*cpp, val);
   *cpp += 4;
}

/** Free internal buffers in an SLP message.
 *
 * @param[in] message - The message to be freed.
 */
void SLPMessageFreeInternals(SLPMessage message)
{
   int i;

   switch (message->header.functionid)
   {
      case SLP_FUNCT_SRVRPLY:
         if (message->body.srvrply.urlarray)
         {
            for (i = 0; i < message->body.srvrply.urlcount; i++)
               if (message->body.srvrply.urlarray[i].autharray)
               {
                  xfree(message->body.srvrply.urlarray[i].autharray);
                  message->body.srvrply.urlarray[i].autharray = 0;
               }

            xfree(message->body.srvrply.urlarray);
            message->body.srvrply.urlarray = 0;
         }
         break;

      case SLP_FUNCT_SRVREG:
         if (message->body.srvreg.urlentry.autharray)
         {
            xfree(message->body.srvreg.urlentry.autharray);
            message->body.srvreg.urlentry.autharray = 0;
         }
         if (message->body.srvreg.autharray)
         {
            xfree(message->body.srvreg.autharray);
            message->body.srvreg.autharray = 0;
         }
         break;

      case SLP_FUNCT_SRVDEREG:
         if (message->body.srvdereg.urlentry.autharray)
         {
            xfree(message->body.srvdereg.urlentry.autharray);
            message->body.srvdereg.urlentry.autharray = 0;
         }
         break;

      case SLP_FUNCT_ATTRRPLY:
         if (message->body.attrrply.autharray)
         {
            xfree(message->body.attrrply.autharray);
            message->body.attrrply.autharray = 0;
         }
         break;

      case SLP_FUNCT_DAADVERT:
         if (message->body.daadvert.autharray)
         {
            xfree(message->body.daadvert.autharray);
            message->body.daadvert.autharray = 0;
         }
         break; 

      case SLP_FUNCT_SAADVERT:
         if (message->body.saadvert.autharray)
         {
            xfree(message->body.saadvert.autharray);
            message->body.saadvert.autharray = 0;
         }
         break; 

      case SLP_FUNCT_ATTRRQST:
      case SLP_FUNCT_SRVACK:
      case SLP_FUNCT_SRVRQST:
      case SLP_FUNCT_SRVTYPERQST:
      case SLP_FUNCT_SRVTYPERPLY:
      default:
         /* don't do anything */
         break;
   }
}

/** Allocate memory for an SLP message descriptor.
 *
 * @return A new SLP message object, or NULL if out of memory.
 */
SLPMessage SLPMessageAlloc(void)
{
   SLPMessage result = (SLPMessage)xmalloc(sizeof(struct _SLPMessage));
   if (result)
      memset(result, 0, sizeof(struct _SLPMessage));
   return result;
}

/** Reallocate memory for an SLP message descriptor.
 *
 * @param[in] msg - The message descriptor to be reallocated.
 *
 * @return A resized version of @p msg, or NULL if out of memory.
 */
SLPMessage SLPMessageRealloc(SLPMessage msg)
{
   if (msg == 0)
   {
      msg = SLPMessageAlloc();
      if (msg == 0)
         return 0;
   }
   else
      SLPMessageFreeInternals(msg);

   return msg;
}

/** Frees memory associated with an SLP message descriptor.
 *
 * @param[in] message - The SLP message descriptor to be freed.
 */
void SLPMessageFree(SLPMessage message)
{
   SLPMessageFreeInternals(message);
   xfree(message);
}

/** Switch on version field to parse v1 or v2 header.
 *
 * @param[in] buffer - The buffer to be parsed.
 * @param[out] header - The address of a message header to be filled.
 *
 * @return Zero on success, or SLP_ERROR_VER_NOT_SUPPORTED.
 */
int SLPMessageParseHeader(const SLPBuffer buffer, SLPHeader * header)
{
   /* switch on version field */
   switch (*buffer->curpos)
   {
#if defined(ENABLE_SLPv1)
      case 1: return SLPv1MessageParseHeader(buffer, header);
#endif
      case 2: return SLPv2MessageParseHeader(buffer, header);
   }
   return SLP_ERROR_VER_NOT_SUPPORTED;
}

/** Switch on version field to parse v1 or v2 message.
 *
 * This routine provides a common location between SLP v1 and v2
 * messages to perform operations that are common to both message
 * types. These include copying remote and local bindings into the
 * message buffer.
 *
 * @param[in] peeraddr - Remote address binding to store in @p message.
 * @param[in] localaddr - Local address binding to store in @p message.
 * @param[in] buffer - The buffer to be parsed.
 * @param[in] message - The message into which @p buffer should be parsed.
 *
 * @return Zero on success, SLP_ERROR_PARSE_ERROR, or
 *    SLP_ERROR_INTERNAL_ERROR if out of memory.
 *
 * @remarks On success, pointers in the SLPMessage reference memory in
 *    the parsed SLPBuffer. If SLPBufferFree is called then the pointers
 *    in @p message will be invalidated.
 */
int SLPMessageParseBuffer(void * peeraddr,
      const void * localaddr, const SLPBuffer buffer,
      SLPMessage message)
{
   /* Copy in the local and remote address info */
   if (peeraddr != 0)
      memcpy(&message->peer, peeraddr, sizeof(message->peer));
   if (localaddr != 0)
      memcpy(&message->localaddr, localaddr, sizeof(message->localaddr));

   /* Get ready to parse */
   SLPMessageFreeInternals(message);
   buffer->curpos = buffer->start;

   /* switch on version field */
   switch (*buffer->curpos)
   {
#if defined(ENABLE_SLPv1)
      case 1: return SLPv1MessageParseBuffer(buffer, message);
#endif
      case 2: return SLPv2MessageParseBuffer(buffer, message);
   }
   return SLP_ERROR_VER_NOT_SUPPORTED;
}

/*=========================================================================*/

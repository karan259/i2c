////////////////////////////////////////////////////////////////////////////////
//
// Package:   I2C
//
// Library:   libi2c
//
// File:      i2ccom.c
//
/*! \file
 *
 * $LastChangedDate: 2010-07-27 11:43:18 -0600 (Tue, 27 Jul 2010) $
 * $Rev: 510 $
 *
 * \brief Low-level I<sup>2</sup>C communication level implementation.
 *
 * This file has been modified from the original i2ccom.c source (see below).
 *
 * \todo Add ASCII line oriented input/output with cr-lf translation.
 * i2c_readline(), i2c_writeline().
 *
 * \author Robin Knight (robin.knight@roadnarrows.com)
 *
 * \par Copyright:
 * (C) 2007-2009.  RoadNarrows LLC.
 * (http://www.roadnarrows.com) \n
 * All Rights Reserved
 *
 * <hr>
 * \par Original Source and Copyright:
 *  Copyright (C) 2004 K-TEAM SA
 *
 * \author   Yves Piguet (Calerga Sarl)
 * \author   Pierre Bureau (K-Team SA)
 * \author   Cedric Gaudin (K-Team SA)
 *
 * \par Original Header:
 * See "Original Source Header EULA" in source file.
 *
 * <hr>
 */
// Permission is hereby granted, without written agreement and without
// license or royalty fees, to use, copy, modify, and distribute this
// software and its documentation for any purpose, provided that
// (1) The above copyright notice and the following two paragraphs
// appear in all copies of the source code and (2) redistributions
// including binaries reproduces these notices in the supporting
// documentation.   Substantial modifications to this software may be
// copyrighted by their authors and need not follow the licensing terms
// described here, provided that the new terms are clearly indicated in
// all files where they apply.
//
// IN NO EVENT SHALL THE AUTHOR, ROADNARROWS LLC, OR ANY MEMBERS/EMPLOYEES
// OF ROADNARROW LLC OR DISTRIBUTORS OF THIS SOFTWARE BE LIABLE TO ANY
// PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
// EVEN IF THE AUTHORS OR ANY OF THE ABOVE PARTIES HAVE BEEN ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
// THE AUTHOR AND ROADNARROWS LLC SPECIFICALLY DISCLAIM ANY WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN
// "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO
// PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
//
// Original Source Header EULA:
//
// This module is the low-level communication layer between
// the KoreBot Library and the Linux Operating System
//
// Id: i2ccom.c,v 1.4 2006/03/07 17:39:57 pbureau Exp
//
// Author: pbureau
// Date: 2008-11-10 15:34:14 -0700 (Mon, 10 Nov 2008)
// Revision: 14
//
// Log: i2ccom.c,v
// Revision 1.4  2006/03/07 17:39:57  pbureau
// Makefile update for clean x86 compiling
// i2c compilation cleaned for 2.6 kernel
//
// Revision 1.3  2004/09/22 18:55:08  pbureau
// *** empty log message ***
//
// Revision 1.1  2004/07/29 10:51:54  cgaudin
// New libkorebot release 1.2
//
////////////////////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/ioctl.h>

#include "rnr/rnrconfig.h"
#include "rnr/i2c-dev.h"
#include "rnr/i2c.h"

// ---------------------------------------------------------------------------
// Private Interface 
// ---------------------------------------------------------------------------

/*! 
 * \brief Selects the I<sup>2</sup>C device on a given I<sup>2</sup>C Bus.
 *
 * \param [in,out] i2c  Pointer to I<sup>2</sup>C Bus handle.
 * \param addr          I<sup>2</sup>C device's 7/10-bit address.
 *
 * \return
 * Returns 0 on success. Else errno is set appropriately and -1 is returned.
 */
static int i2c_select_device(i2c_t *i2c, i2c_addr_t addr)
{
  int rc;

  // change device address only if necessary
  if( i2c->addr != addr )
  {
    if( (rc = ioctl(i2c->fd, I2C_SLAVE, addr)) < 0 )
    {
      return rc;
    }
    i2c->addr = addr;
  }
  return 0;
}


// ---------------------------------------------------------------------------
// Public Interface 
// ---------------------------------------------------------------------------

/*! 
 * \brief Open the host I<sup>2</sup>C Bus device.
 *
 * \param [out] i2c Pointer to I<sup>2</sup>C Bus handle.\n
 *                  Note: This descriptor must be allocated by the caller.
 * \param device    A null-terminated string of the device name. 
 *                  If NULL, then the default '/dev/i2c/0' is used.
 * \return
 * Returns 0 on success. Else errno is set appropriately and -1 is returned.
 */   
int i2c_open(i2c_t *i2c, char const *device)
{
  // no device selected
  i2c->addr = (ushort_t)I2C_ADDR_NONE;      
    
  // set hte default I2C bus device
  if( device == NULL )
  {
    device = "/dev/i2c/0";
  }

  if( (i2c->fd = open(device, O_RDWR)) < 0)
  {
    return i2c->fd;
  }
  
  return 0;
}

/*! 
 * \brief Closes an I<sup>2</sup>C Bus. 
 * 
 * \param [in,out] i2c  Pointer to I<sup>2</sup>C Bus handle.
 */
void i2c_close(i2c_t *i2c)
{
  if( i2c->fd >= 0 )
  { 
    close(i2c->fd);
    i2c->fd   = -1;
    i2c->addr  = (ushort_t)I2C_ADDR_NONE;      
  }
}

/*! 
 * \brief Read from an I<sup>2</sup>C device.
 *
 * The i2c_read() primitive reads data from a device at the given address on
 * the given I<sup>2</sup>C Bus.
 *
 * \param [in,out] i2c  Pointer to I<sup>2</sup>C Bus handle.
 * \param addr          I<sup>2</sup>C device's 7/10-bit address.
 * \param [out] buf     Pointer to the buffer that will receive the data bytes.
 * \param len           Size of the buffer in bytes.
 *
 * \return
 * On success, returns \h_ge 0 number of bytes read.\n
 * Else errno is set appropriately and -1 is returned.
 */
int i2c_read(i2c_t *i2c, i2c_addr_t addr, byte_t *buf, uint_t len)
{
  int rc;

  if( (rc = i2c_select_device(i2c , addr)) != -1 )
  {
    rc = (int)read(i2c->fd, buf, (size_t)len);
  }

  return rc;
}

/*! 
 * \brief Write to an I<sup>2</sup>C device.
 *
 * The i2c_write() primitive writes data to a device at the given address
 * on the given I<sup>2</sup>C Bus.
 * 
 * \param [in,out] i2c  Pointer to I<sup>2</sup>C Bus handle.
 * \param addr          I<sup>2</sup>C device's 7/10-bit address.
 * \param buf           Pointer to the buffer that will be written.
 * \param len           Number of bytes to write.
 *
 * \return
 * On success, returns \h_ge 0 number of bytes written.\n
 * Else errno is set appropriately and -1 is returned.
 */
int i2c_write(i2c_t *i2c, i2c_addr_t addr, const byte_t *buf, uint_t len)
{
  int rc;

  if( (rc = i2c_select_device(i2c , addr)) != -1 )
  {
    rc = (int)write(i2c->fd , buf, (size_t)len);
  }
  
  return rc;
}

/*! 
 * \brief Perform a transaction with an I<sup>2</sup>C device.
 *
 * The i2c_transfer() primitive writes data to and then reads data from a
 * device at the given address on the given I<sup>2</sup>C Bus.
 * 
 * \param [in] i2c        Pointer to I<sup>2</sup>C Bus handle.
 * \param addr            I<sup>2</sup>C device's 7/10-bit address.
 * \param write_buf       Pointer to the buffer that contains the data 
 *                        to be written. 
 * \param write_len       Number of bytes to write. The data are in 
 *                        the write buffer.  
 * \param [out] read_buf  Pointer to the buffer that will receive the 
 *                        data. 
 * \param read_len        Size of the read buffer in bytes.
 *
 * \return
 * Returns \h_ge 0 on success.
 * Else errno is set appropriately and -1 is returned.
 */
int i2c_transfer(i2c_t *i2c , i2c_addr_t addr, 
                const byte_t *write_buf , uint_t write_len,
                byte_t *read_buf , uint_t read_len)
{
  i2c_msg_t             msgs[2];
  i2c_rdwr_ioctl_data_t msgset;
  int                   rc;  
  
  // write message
  msgs[0].addr  = addr;
  msgs[0].flags = I2C_M_NOFLAGS;
  msgs[0].buf   = (char *)write_buf;
  msgs[0].len   = (short)write_len;
      
  // read message
  msgs[1].addr  = addr;
  msgs[1].flags = I2C_M_RD;
  msgs[1].buf   = (char *)read_buf;
  msgs[1].len   = (short)read_len;
    
  msgset.msgs   = msgs;
  msgset.nmsgs  = 2;
  
  rc = ioctl(i2c->fd , I2C_RDWR, &msgset); 
 
  return rc;
}

/*! 
 * \brief Test the existance of a device at the given address on the 
 * given I<sup>2</sup>C Bus.
 * 
 * \param [in]  i2c  Pointer to I<sup>2</sup>C Bus handle.
 * \param addr       I<sup>2</sup>C device's 7/10-bit address.
 * 
 * \return
 * Returns 1 if the device is presentr. Else returns 0.
 */
int i2c_exists(i2c_t *i2c, i2c_addr_t addr)
{
  i2c_msg_t             msg;
  i2c_rdwr_ioctl_data_t msgset;
  int                   rc;
 
  msg.addr      = addr;
  msg.flags     = I2C_M_NOFLAGS;
  msg.buf       = NULL;
  msg.len       = 0;
    
  msgset.msgs   = &msg;
  msgset.nmsgs  = 1;

  rc = ioctl(i2c->fd, I2C_RDWR, &msgset);
  
  // answer from an I2C device
  return rc == 1? 1: 0;
} 
     
/*! 
 * \brief Scans the given I<sup>2</sup>C Bus to find all connected devices.
 * 
 * For each device found, the provided callback function is called.
 *
 * \param [in] i2c  Pointer to I<sup>2</sup>C Bus handle.
 * \param callback  A callback function called when a device is found.
 *                  If this callback function returns a value \<0 the 
 *                  bus scan stops immedialety and the value is 
 *                  used as return value by i2c_scan().
 * \param context   A user defined value or a pointer passed to the callback 
 *                  function.
 *
 * \return
 * Returns the number of deviced found on success. Else errno is set
 * appropriately and -1 is returned.
 */
int i2c_scan(i2c_t *i2c,
            int (*callback)(i2c_t *i2c, i2c_addr_t addr, void *context), 
            void * context)
{
  i2c_addr_t  addr;
  int         nFound = 0;
  int         rc;
 
  //
  // Scan the whole I<sup>2</sup>C Bus range.
  //
  for(addr=I2C_ADDR_DEV_LOW; addr<=I2C_ADDR_DEV_HIGH; ++addr)
  {
    if( i2c_exists(i2c, addr) )
    {
      ++nFound;

      if( callback != NULL )
      {
        if( (rc = callback(i2c, addr, context)) < 0 )
        {
          return rc;
        }
      }
    }
  }

  return nFound;
}

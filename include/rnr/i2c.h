////////////////////////////////////////////////////////////////////////////////
//
// Package:   I2C
//
// Library:   libi2c API
//
// File:      i2c.h
//
/*! \file
 *
 * $LastChangedDate: 2010-03-25 08:03:07 -0600 (Thu, 25 Mar 2010) $
 * $Rev: 309 $
 *
 * \brief Low-level I<sup>2</sup>C communication level.
 *
 * This file has been modified from the original i2ccom.h source (see below).
 *
 * \author Robin Knight (robin.knight@roadnarrows.com)
 *
 * \par Copyright:
 * (C) 2007-2010.  RoadNarrows LLC.
 * (http://www.roadnarrows.com) \n
 * All Rights Reserved
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
// Id: i2ccom.h,v 1.1 2004/07/29 10:51:54 cgaudin Exp
//
// Original author: Yves Piguet, 2004
// Author: cgaudin
// Date: 2009-09-04 15:13:29 -0600 (Fri, 04 Sep 2009)
// Revision: 126
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _I2C_H
#define _I2C_H

#include "rnr/rnrconfig.h"
#include "rnr/i2c-dev.h"

C_DECLS_BEGIN

/*! 
 * \brief I<sup>2</sup>C Device Address Type
 */
typedef ushort_t i2c_addr_t; 

#define I2C_ADDR_NONE    ((i2c_addr_t)(-1))   ///< no I2C address selected

/*!
 * \brief I<sup>2</sup>C Bus Handle Type
 */
typedef struct i2c_struct
{
  int         fd;     ///< opened file descriptor of the I2C bus device 
  i2c_addr_t  addr;   ///< address of the currently selected attached I2C device
} i2c_t;


// ---------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------

extern int  i2c_open(i2c_t *i2c , const char *device);

extern void i2c_close(i2c_t *i2c);

extern int  i2c_read(i2c_t *i2c, i2c_addr_t addr, byte_t *buf , uint_t len);

extern int  i2c_write(i2c_t *i2c, i2c_addr_t addr, const byte_t *buf,
                      uint_t len);

extern int  i2c_transfer(i2c_t *i2c, i2c_addr_t addr, const byte_t *write_buf, 
			                  uint_t write_len, byte_t *read_buf, uint_t read_len);

extern int  i2c_exists(i2c_t *i2c, i2c_addr_t addr);

extern int  i2c_scan(i2c_t *i2c,
                    int (*callback)(i2c_t *i2c, i2c_addr_t addr, void *context),
		                void *context);

C_DECLS_END


#endif // _I2C_H

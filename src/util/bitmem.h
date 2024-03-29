#ifndef BITMEM_H_
#define BITMEM_H_

enum memattr_t
{
    AT_CODE    = 1,             /* Opcode bits.                     */
    AT_OPER    = 2,             /* Operand to an opcode.            */
    AT_DATA    = 4,             /* Data field.                      */
    AT_DATAB   = 8,             /* Starting boundary of data field. */
    AT_BTRG    = 16,            /* Branch target.                   */
    AT_ALIGN   = 32,            /* Alignment-padding bits.          */
    AT_BITREV  = 64,            /* Bit-reversed field of some sort. */
    AT_LOCAL   = 128            /* Location is contained w/in ROM.  */
};

typedef struct bitmem_t
{
    uint_32 *data;              /* bitmem memory image.                 */
    uint_8  *attr;              /* memory attributes, bit granularity.  */
    uint_32 size;               /* size, in bits.                       */
    int     org;                /* base address of bit memory.          */
} bitmem_t;

bitmem_t *bitmem_create  (int org, int len);
void      bitmem_load    (bitmem_t *mem, FILE *f);
uint_32   bitmem_read_fwd(bitmem_t *mem, uint_32 addr, int len);
uint_32   bitmem_read_rev(bitmem_t *mem, uint_32 addr, int len);
void      bitmem_setattr (bitmem_t *mem, uint_32 addr, int len, uint_8 attr);
void      bitmem_clrattr (bitmem_t *mem, uint_32 addr, int len, uint_8 attr);
uint_8    bitmem_getattr (bitmem_t *mem, uint_32 addr, int len);

#define BYTE(b) ((b) << 3)

#endif

/* ======================================================================== */
/*  This program is free software; you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by    */
/*  the Free Software Foundation; either version 2 of the License, or       */
/*  (at your option) any later version.                                     */
/*                                                                          */
/*  This program is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*  General Public License for more details.                                */
/*                                                                          */
/*  You should have received a copy of the GNU General Public License       */
/*  along with this program; if not, write to the Free Software             */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               */
/* ======================================================================== */
/*                 Copyright (c) 1998-2001, Joseph Zbiciak                  */
/* ======================================================================== */

/*
 * ============================================================================
 *  Title:    STIC -- Standard Television Interface Chip, AY-3-8900
 *  Author:   J. Zbiciak
 * ============================================================================
 *  This module implements the STIC Chip.  Its functionality is currently
 *  far from complete.  For instance, I don't implement MOBs yet.
 * ============================================================================
 *  The STIC processor is active over several regions of memory:
 *
 *   0x0000 - 0x001F     -- MOB (Movable OBject) controls
 *                          0x0000 + n  X Position, MOB #n
 *                          0x0008 + n  Y Position, MOB #n
 *                          0x0010 + n  Attribute Register, MOB #n
 *                          0x0018 + n  Collision Detect, MOB #n
 *
 *   0x0020 - 0x0021     -- STIC control registers
 *                          0x0020      Display Enable
 *                          0x0021      Graphics Mode
 *
 *   0x0028 - 0x002C     -- Global color settings
 *                          0x0028 + n  Color Stack (n == 0..3)
 *                          0x002C      Border Color
 *
 *   0x0030 - 0x0032     -- Display framing
 *                          0x0030      Pixel Column Delay (0..7)
 *                          0x0031      Pixel Row Delay (0..7)
 *                          0x0032      Edge inhibit (bit0->left, bit1->top)
 *
 *   0x0200 - 0x035F     -- Background Card Table (BACKTAB)
 *
 *   0x3800 - 0x39FF     -- Graphics RAM
 *
 *                          Note:  BACKTAB and GRAM are actually physically
 *                          separate.  This STIC implementation 'snoops'
 *                          these accesses for performance reasons.
 *
 *  For now, I handle all of the control register accesses into one
 *  peripheral entry and all bus-snooping accesses in a second peripheral
 *  entry.
 * ============================================================================
 */

#ifndef STIC_H_
#define STIC_H_ 1

#include "cp1600/req_q.h"

/*
 * ============================================================================
 *  STIC_T           -- Main STIC structure.
 *
 *  Note: These bitfields seem to be broken at the present.
 *
 *  STIC_MOB_X_T     -- MOB X Position Bitfield Structure
 *  STIC_MOB_Y_T     -- MOB Y Position Bitfield Structure
 *  STIC_MOB_A_T     -- MOB Attribute Bitfield Structure
 *  STIC_MOB_C_T     -- MOB Collision Bitfield Structure
 * ============================================================================
 */

struct stic_t
{
    /* -------------------------------------------------------------------- */
    /*  The STIC is split into three separate peripherals, since a RD/WR    */
    /*  to something the STIC is interested in needs to go one of three     */
    /*  places -- either (1) to update the general state of the STIC via a  */
    /*  control register, (2) to update the BACKTAB, or (3) to update a     */
    /*  card definition in GRAM.                                            */
    /* -------------------------------------------------------------------- */
    periph_t    stic_cr;
    periph_t    snoop_btab;
    periph_t    snoop_gram;
    periph_t    alias_gram;

    /* -------------------------------------------------------------------- */
    /*  CPU accesses, INTAK, BUSAK can advance the STIC emulation state     */
    /*  ahead of what the normal periph-tick functionality performs.  This  */
    /*  keeps track of the 'effective now', which should be all of the      */
    /*  periph_t notions of 'now'.                                          */
    /* -------------------------------------------------------------------- */
    uint_64     eff_cycle;

    /* -------------------------------------------------------------------- */
    /*  Some other important times:                                         */
    /* -------------------------------------------------------------------- */
    uint_64     gmem_accessible;    /* CPU can access GRAM/GROM.            */
    uint_64     stic_accessible;    /* CPU can access STIC registers        */
    uint_64     vid_enable_cutoff;  /* Last cycle for vid-enable handshake  */
    uint_64     next_frame_render;  /* Cycle of next graphics render.       */
    uint_64     last_frame_intrq;   /* Most recent INTRQ cycle.             */
    uint_64     next_frame_intrq;   /* Next INTRQ coming up.                */

    /* -------------------------------------------------------------------- */
    /*  We just keep a raw image of the STIC registers and decode manually  */
    /* -------------------------------------------------------------------- */
    uint_32     raw [0x40];
    uint_8      gmem[0x200 * 8];

    /* -------------------------------------------------------------------- */
    /*  We store several bitmaps and display lists.                         */
    /* -------------------------------------------------------------------- */
    int         fifo_rd_ptr;            /* How far into BTAB STIC's fetched */
    int         fifo_wr_ptr;            /* Next loc to copy to in shadow.   */
    int         busrq_count;            /* Number of BUSRQs so far.         */
    uint_16     btab_sr [240];              /* BACKTAB as it is in Sys. RAM */
    uint_16     btab    [240];              /* BACKTAB as STIC sees it      */
    uint_16     btab_pr [240];              /* Prev BACKTAB as STIC sees it */
    uint_32     last_bg [12];               /* Last background color by row */
 /* uint_32     bt_img  [240*8]; */         /* BACKTAB 4-bpp display list.  */
    uint_8      bt_bmp  [240*8];            /* BACKTAB 1-bpp display list.  */
    uint_32     mob_img [ 16*16  / 8];      /* Expanded/mirrored MOB 4bpp.  */
    uint_16     mob_bmp [8][16];            /* Expanded/mirrored MOBs 1bpp. */
    uint_32     mpl_img [192*224 / 8];      /* MOB image.                   */
    uint_32     mpl_vsb [192*224 /32];      /* MOB visibility bitmap.       */
    uint_32     mpl_pri [192*224 /32];      /* MOB priority bitmap.         */
    uint_32     xbt_img [192*112 / 8];      /* Re-tiled BACKTAB image.      */
    uint_32     xbt_bmp [192*112 /32];      /* Re-tiled BACKTAB 1-bpp.      */
    uint_32     image   [192*224 / 8];      /* Final 192x224 image, 4-bpp.  */

    uint_8      *disp;
    gfx_t       *gfx;

    /* -------------------------------------------------------------------- */
    /*  IRQ and BUSRQ generation.                                           */
    /* -------------------------------------------------------------------- */
    req_q_t     *req_q;         /* Bus for INTRQ, BUSRQ, INTAK, BUSAK.      */

    /* -------------------------------------------------------------------- */
    /*  STIC internal flags.                                                */
    /* -------------------------------------------------------------------- */
    void        (*upd)(struct stic_t*); /* Update fxn for curr disp mode.   */
    uint_8      prev_vid_enable;    /* Previous video enable.               */
    uint_8      vid_enable;     /* Must be set every vsync to enable vid.   */
    uint_8      mode, p_mode;   /* 1 == FG/BG mode, 0 == Color Stack.       */
    uint_8      bt_dirty;       /* BACKTAB is dirty.                        */
    uint_8      gr_dirty;       /* GRAM is dirty.                           */
    uint_8      ob_dirty;       /* MOBs are dirty.                          */
    uint_8      rand_mem;       /* Flag: Randomize regs, memory on startup  */
    uint_8      pal;            /* Flag: 0 == NTSC, 1 == PAL                */
    uint_8      movie_active;   /* Suppress dropping frames if movie active */
    uint_8      is_hidden;      /* Drop frames if hidden but no movie.      */
    uint_8      type;           /* Normal STIC or STIC1A.                   */
    uint_8      gram_size;      /* 0 = 64, 1 = 128, 2 = 256 cards           */
    uint_16     gram_mask;      /* Address mask for GRAM.                   */
    int         drop_frame;     /* Frames to drop because we're behind.     */

    /* -------------------------------------------------------------------- */
    /*  Performance monitoring.  :-)                                        */
    /* -------------------------------------------------------------------- */
    struct stic_time_t
    {
        double  full_update;
        double  draw_btab;
        double  draw_mobs;
        double  fix_bord;
        double  merge_planes;
        double  push_vid;
        double  mob_colldet;
        double  gfx_vid_enable;
        int     total_frames;
    } time;

    /* -------------------------------------------------------------------- */
    /*  Demo recording                                                      */
    /* -------------------------------------------------------------------- */
    demo_t      *demo;

    /* -------------------------------------------------------------------- */
    /*  Debugger support                                                    */
    /* -------------------------------------------------------------------- */
    uint_32     debug_flags;
};

#ifndef STIC_T_
#define STIC_T_ 1
typedef struct stic_t stic_t;
#endif

/*
 * ============================================================================
 *  Debugger flags
 * ============================================================================
 */

#define STIC_SHOW_WR_DROP               (1 <<  0)
#define STIC_SHOW_RD_DROP               (1 <<  1)
#define STIC_SHOW_FIFO_LOAD             (1 <<  2)
#define STIC_DBG_CTRL_ACCESS_WINDOW     (1 <<  3)
#define STIC_DBG_GMEM_ACCESS_WINDOW     (1 <<  4)
#define STIC_HALT_ON_BLANK              (1 <<  5)
#define STIC_GRAMSHOT                   (1 <<  6)
#define STIC_DBG_REQS                   (1 <<  7)
#define STIC_HALT_ON_BUSRQ_DROP         (1 <<  8)
#define STIC_HALT_ON_INTRM_DROP         (1 <<  9)

/*
 * ============================================================================
 *  STIC types:
 *      STIC_8900       Standard STIC
 *      STIC_STIC1A     STIC1A chip from INTV88/TutorVision
 * ============================================================================
 */
enum stic_type { STIC_8900, STIC_STIC1A };

/*
 * ============================================================================
 *  STIC_INIT        -- Initialize the STIC
 *  STIC_TICK        -- Perform a STIC update
 * ============================================================================
 */

int stic_init
(
    stic_t         *const stic,
    uint_16        *const grom_img,
    req_q_t        *const req_q,
    gfx_t          *const gfx,
    demo_t         *const demo,
    int             const rand_regs,
    int             const pal_mode,
    int             const gram_size,
    enum stic_type  const stic_type
);

uint_32 stic_tick
(
    const periph_p per,
    const uint_32  len
);

/*
 * ============================================================================
 *  STIC_RESYNC  -- Resynchronize STIC internal state after a load.
 * ============================================================================
 */
void stic_resync(stic_t *const stic);

/*
 * ============================================================================
 *  STIC_GRAM_TO_GIF -- Generate GIF containing the current GRAM contents.
 * ============================================================================
 */
void stic_gram_to_gif(const stic_t *const stic);

/*
 * ============================================================================
 *  STIC_GRAM_TO_TEXT -- Display textual representation of GRAM.
 * ============================================================================
 */
void stic_gram_to_text(const stic_t *const stic, const int start, 
                       const int count, const int disp_width);

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
/*                 Copyright (c) 1998-2011, Joseph Zbiciak                  */
/* ======================================================================== */

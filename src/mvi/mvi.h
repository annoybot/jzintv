#ifndef MVI_H
#define MVI_H

#define MVI_MAX_X (256)
#define MVI_MAX_Y (256)

typedef struct mvi_t            /*  Movie-related stuff.                */
{
    FILE    *f;                 /*  Current movie file.                 */
    int     fr;                 /*  Current movie frame #.              */
    int     last_fr;            /*  Last movie frame # (decode only)    */
    int     x_dim, y_dim;       /*                                      */
    uint_8  *vid;               /*  Previous movie frame.               */
    uint_8  bbox[8][4];         /*  Previous bounding boxes.            */
    uint_32 tot_bytes;          /*  Total bytes in movie file.          */
    uint_32 rpt_frames;         /*  Number of frames skipped            */
    uint_32 rpt_rows;           /*  Number of repeated rows in frames   */
#ifndef NO_LZO
    uint_32 tot_lzosave;        /*  Total bytes saved by LZO            */
#endif
} mvi_t;


void mvi_init(mvi_t *movie, int x_dim, int y_dim);
void mvi_wr_frame(mvi_t *movie, uint_8 *vid, uint_8 bbox[8][4]);
int  mvi_rd_frame(mvi_t *movie, uint_8 *vid, uint_8 bbox[8][4]);

/* Flags returned by mvi_rd_frame */
#define MVI_FR_SAME (1)     /* set if movie file skipped the frame  */
#define MVI_BB_SAME (2)     /* set if movie file skipped the bbox   */
#define MVI_NEW_DIM (4)     /* set if movie changed dimensions.     */

#endif

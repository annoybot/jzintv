#ifdef NO_LZO
;
#else

#include <stdio.h>
#include <unistd.h>	//for isatty()
#include <readline/readline.h>
#include <readline/history.h>

#include "config.h"
#include "minilzo/minilzo.h"
#include "lzoe/lzoe.h"

char *rl_gets();

LZFILE            lzfile[MAX_LZOE_OPEN];
LOCAL lzoe_directory    *directory    = NULL;
LOCAL int               directory_len = 0;

#if LZO1X_MEM_DECOMPRESS > 0
LOCAL uint_8            lzo_wrk[ LZO1X_MEM_DECOMPRESS ];
#else
#   define lzo_wrk NULL
#endif

LOCAL LZFILE *get_lzfile(void)
{
    int i;

    for (i = 0; i < MAX_LZOE_OPEN; i++)
        if (lzfile[i].f == NULL && lzfile[i].l == NULL)
            return &lzfile[i];

    return NULL;
}

LOCAL void put_lzfile( LZFILE *lzf )
{
    if (lzf->l)
    {
        CONDFREE( lzf->l->ddata );
        free( lzf->l );
    }

    lzf->f = NULL;
    lzf->l = NULL;
}

LOCAL int get_directory_number( const char *fname )
{
    int i;

    for (i = 0; i < directory_len; i++)
        if (!strncmp( fname, directory[i].prefix, directory[i].pfx_len ))
            return i;

    return -1;
}

int  has_lzoe_prefix( const char *fname )
{
    return get_directory_number( fname ) >= 0;
}

void lzoe_register( const char *prefix, const lzoe_info *table )
{
    int i = directory_len++;

    directory = REALLOC( directory, lzoe_directory, directory_len );

    if (!directory)
    {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    directory[i].prefix  = prefix;
    directory[i].pfx_len = strlen( prefix );
    directory[i].table   = table;
}

LOCAL int locate_file( const char *fname, const lzoe_info **info )
{
    int d, i, pfx_len;
    const lzoe_info *table;

    *info = NULL;

    d = get_directory_number( fname );

    if (d == -1)
        return 0;   // not an LZOE file.

    /* Look for this file in the directory */
    table   = directory[d].table;
    pfx_len = directory[d].pfx_len;

    for (i = 0; table[i].fname != NULL; i++)
    {
        if ( !strcmp( table[i].fname, fname + pfx_len ) )
            break;
    }

    /* If we found it, set *info to point to it. */
    if ( table[i].fname )
        *info = &table[i];

    /* Return that the file is an LZOE file by virtue of having
       a path with an LZOE directory prefix.

       *info will be non-NULL if we also happened to find the file
       in the directory.
     */
    return 1;
}

LZFILE *lzoe_fopen ( const char *fname, const char *mode )
{
    LZFILE          *handle = get_lzfile();
    const lzoe_info *info;
    lzoe_file       *file;
    uint_8          *ddata;
    lzo_uint        decomp_len;
    int is_lzoe, r;

    if (!handle)
    {
        fprintf(stderr, "Too many files open\n");
        exit(1);
    }

    is_lzoe = locate_file( fname, &info );

    /* Not an LZOE path, so try to open as normal file */
    if (!is_lzoe)
    {
        FILE *f = fopen( fname, mode );

        if (!f) { put_lzfile( handle ); return NULL; }

        handle->f = f;
        return handle;
    }

    /* Ok, the file name is an LZOE file name.  Can only open RO. */

    if ( strcmp( mode, "r"  ) != 0 &&
         strcmp( mode, "rb" ) != 0 )
    {
        fprintf(stderr, "lzoe_fopen(\"%s\", \"%s\"): unsupported mode'\n",
                fname, mode);
        exit(1);
    }

    /* LZOE path, but file not found */
    if (!info)
    {
        put_lzfile( handle );
        return NULL;
    }

    /* "Open" it! */
    file  = CALLOC( lzoe_file, 1 );
    ddata = CALLOC( uint_8, info->orig_len );

    if (!file || !ddata)
    {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    handle->l    = file;
    file->info   = info;
    file->ddata  = ddata;
    file->offset = 0;

    r = lzo1x_decompress( (const lzo_bytep)info->cdata, info->comp_len,
                          (      lzo_bytep)ddata,       &decomp_len,
                          (      lzo_voidp)lzo_wrk );

    if (r != LZO_E_OK)
    {
        fprintf(stderr, "LZO error: %d\n", r);
        exit(1);
    }

    if ((long)decomp_len != (long)info->orig_len)
    {
        fprintf(stderr, "LZO error:  Decompressed len = %ld, orig_len = %ld\n",
                (long)decomp_len, info->orig_len);
        exit(1);
    }

    return handle;
}

char *lzoe_fgets ( char *buf, int size, LZFILE *f )
{
    int is_tty = isatty(fileno(f->f));

    int  i, max, eol = 0;
    long ofs;

    if (f->f) {
        if (is_tty) {

            char * rl_line = rl_gets();

            if( rl_line ) {
                strlcpy(buf, rl_line, size);
            } else {
                return NULL;
            }

            return buf;
        } else {
            return fgets(buf, size, f->f);
        }
    }

    max = f->l->info->orig_len - f->l->offset;

    if (max <= 0)
        return NULL;

    if (max > size - 1)
        max = size - 1;

    /* This tries to normalize UNIX, DOS and Mac line endings.  It will
       scan up until it sees \n, \r\n, \r, or EOF.  It will always
       convert a line ending to a single \n.
     */

    ofs = f->l->offset;

    for (i = 0; !eol && i < max; i++)
    {
        char c = f->l->ddata[ofs++];

        if (c == '\r')
        {
            eol = 1;
            c = '\n';                               /* convert \r   to \n */
            if (f->l->ddata[ofs] == '\n') ofs++;    /* convert \r\n to \n */
        } else if (c == '\n')
        {
            eol = 1;
        }

        buf[i] = c;
    }

    /* NUL-terminate */
    buf[i] = 0;

    return buf;
}


/* A static variable for holding the line. */
static char *line_read = (char *) NULL;

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *rl_gets() {
    // Disable Tab file completion
    //l_bind_key('\t', rl_insert);

    /* If the buffer has already been allocated, return the memory
       to the free pool. */
    if (line_read) {
        free(line_read);
        line_read = (char *) NULL;
    }

    /* Get a line from the user. */
    line_read = readline("");

    /* If the line has any text in it, save it on the history. */
    if (line_read && *line_read)
        add_history(line_read);

    return (line_read);
}

int lzoe_fgetc( LZFILE *f )
{
    if (f->f)
        return fgetc( f->f );

    if (f->l->offset >= f->l->info->orig_len)
        return EOF;

    return f->l->ddata[ f->l->offset++ ];
}

size_t lzoe_fread ( void *buf, size_t size, size_t nmemb, LZFILE *f )
{
    size_t bytes;
    size_t n;

    if (f->f)
        return fread( buf, size, nmemb, f->f );

    /* Ok, this will fail spectacularly when size * nmemb overflow a size_t,
       but frankly, we should never be reading anything nearly so large. */
    bytes = size * nmemb;

    /* Can't read beyond EOF */
    if (f->l->offset + (long)bytes > f->l->info->orig_len)
        bytes = f->l->info->orig_len - f->l->offset;

    /* quantize to units of 'size', so we can return nmemb correctly */
    n = bytes / size;
    bytes = n * size;

    /* If we're actually transferring bytes, let's copy them. */
    if (bytes)
        memcpy( buf, (void*)(f->l->ddata + f->l->offset), bytes );

    /* Advance our file offset accordingly */
    f->l->offset += bytes;

    return n;
}

int lzoe_fseek ( LZFILE *f, long offset, int whence )
{
    long newpos;

    if (f->f)
        return fseek( f->f, offset, whence );

    switch (whence)
    {
        case SEEK_SET: newpos = offset;                                 break;
        case SEEK_CUR: newpos = offset + f->l->offset;                  break;
        case SEEK_END: newpos = offset + f->l->info->orig_len - 1;      break;

        default:
        {
            fprintf(stderr, "fseek on %s:  What the whence? %d\n",
                    f->l->info->fname, whence);
            exit(1);
        }
    }

    if (newpos < 0)
    {
        fprintf(stderr, "fseek on %s:  Seek before start of file\n",
                f->l->info->fname);
        exit(1);
    }

    if (newpos >= f->l->info->orig_len)
    {
        fprintf(stderr, "fseek on %s:  Seek beyond end of file\n",
                f->l->info->fname);
        exit(1);
    }

    f->l->offset = newpos;

    return 0;
}

long lzoe_ftell ( LZFILE *f )
{
    if (f->f)
        return ftell( f->f );

    return f->l->offset;
}

int lzoe_fclose( LZFILE *f )
{
    int r = 0;

    if (f->f)
        r = fclose( f->f );

    put_lzfile( f );
    return r;
}

int lzoe_exists( const char *fname )
{
    int is_lzoe;
    const lzoe_info *info = NULL;

    is_lzoe = locate_file( fname, &info );

    /* Not an LZOE path, so treat as a normal file */
    if (!is_lzoe)
    {
#ifndef NO_ACCESS
        return access( fname, R_OK | F_OK) != -1;
#else
        FILE *f = fopen( fname, "r" );

        if (f)
            fclose(f);

        return f != NULL;
#endif
    }

    /* LZOE path:  Return true / false based on whether file is found */
    return info != NULL;
}

/* Wrap a FILE* in an LZFILE* */
LZFILE *lzoe_filep( FILE *f )
{
    LZFILE *handle = get_lzfile();

    if (!handle)
    {
        fprintf(stderr, "Too many files open\n");
        exit(1);
    }

    handle->f = f;
    return handle;
}


int lzoe_feof( LZFILE *f )
{
    if (f->f)
        return feof(f->f);

    return f->l->offset >= f->l->info->orig_len;
}

#endif




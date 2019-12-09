target_sources( jzintv
  PRIVATE
    adler32.c
    deflate.c
    trees.c
    zutil.c
  PUBLIC
    deflate.h
    trees.h
    zconf.h
    zlib.h
    zutil.h
  )


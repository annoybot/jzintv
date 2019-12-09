target_sources( jzintv
  PRIVATE
    #event_macos.c
    #event_null.c
    event_sdl.c
    #event_sdl2.c
    event_tbl.c
  PUBLIC
    event.h
    event_tbl.h
  )


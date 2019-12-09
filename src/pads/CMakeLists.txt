target_sources( jzintv
  PRIVATE
    #keyboard.c
    pads.c
    pads_cgc.c
    pads_cgc_linux.c
    #pads_cgc_win32.c
    #pads_intv2pc.c
  PUBLIC
    #keyboard.h
    pads.h
    pads_cgc.h
    pads_cgc_linux.h
    #pads_cgc_win32.h
    #pads_intv2pc.h
  )


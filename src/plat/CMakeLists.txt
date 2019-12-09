target_sources( jzintv
  PRIVATE
    gnu_getopt.c
#    main_null.c
#    main_sdl2.c
    plat_gen.c
#    plat_gp2x.c
    plat_lib.c
#    plat_mac.c
#    plat_null.c
    plat_sdl.c
#    plat_sdl2.c
#    plat_wii.c
  PUBLIC
    main_sdl.c
    gnu_getopt.h
    keyboard_macos.h
    plat.h
    plat_lib.h
    plat_lib_config.h
  )


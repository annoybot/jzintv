target_sources( jzintv
  PRIVATE
    #bc_test.c
    bincfg.c
    bincfg_grmr.tab.c
    bincfg_lex.c
    legacy.c
  PUBLIC
    bincfg.h
    bincfg_grmr.tab.h
    bincfg_lex.h
    legacy.h
  )


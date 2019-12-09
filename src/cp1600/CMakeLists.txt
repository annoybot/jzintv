target_sources( jzintv
  PRIVATE
    cp1600.c
    emu_link.c
    #mk_tbl.c
    op_decode.c
    op_exec.c
    op_exec_ext.c
    req_q.c
  PUBLIC
    cp1600.h
    emu_link.h
    op_decode.h
    op_exec.h
    op_exec_ext.h
    op_tables.h
    req_q.h
  )


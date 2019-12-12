add_subdirectory( asm )
add_subdirectory( avi )
add_subdirectory( ay8910 )
add_subdirectory( bincfg )
add_subdirectory( cfg )
add_subdirectory( cheat )
add_subdirectory( cp1600 )
add_subdirectory( cp1600/tbl )
add_subdirectory( dasm )
add_subdirectory( debug )
add_subdirectory( demo )
add_subdirectory( ecs )
add_subdirectory( emscripten )
add_subdirectory( event )
add_subdirectory( file )
add_subdirectory( gfx )
add_subdirectory( gif )
add_subdirectory( icart )
add_subdirectory( imasm )
#add_subdirectory( imasm/imcmd )
add_subdirectory( ivoice )
add_subdirectory( jlp )
add_subdirectory( joy )
add_subdirectory( locutus )
add_subdirectory( lzoe )
add_subdirectory( mem )
add_subdirectory( metadata )
add_subdirectory( minilzo )
add_subdirectory( misc )
add_subdirectory( mouse )
add_subdirectory( mvi )
add_subdirectory( name )
add_subdirectory( pads )
add_subdirectory( periph )
add_subdirectory( plat )
add_subdirectory( scale )
add_subdirectory( serializer )
add_subdirectory( snd )
add_subdirectory( speed )
add_subdirectory( stic )
add_subdirectory( util )
add_subdirectory( zlib )

target_sources( jzintv
  PRIVATE
    jzintv.c
    #jzintv_em.c
    #jzintvWii-frontend.c
    SDLMain.m
    svn_revision.c
  PUBLIC
    #_SDL.h
    #_SDL_main.h
    config.h
    sdl_jzintv.h
    SDLMain.h
  )


cmake_minimum_required(VERSION 3.13)
project(jzintv)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#variable_watch(CMAKE_BUILD_TYPE)

add_executable(jzintv src/cheat/cheat.c)

add_subdirectory(src)

target_include_directories(jzintv
        PUBLIC
          src
          /Library/Frameworks/SDL.framework/Headers/
          /usr/local/include
        )

# IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

link_directories(/Library/Frameworks/SDL.framework,  /usr/local/lib)

find_path(Readline_ROOT_DIR
        NAMES
          include/readline/readline.h
          include/readline/history.h
        PATHS
          /usr/local/opt/readline/ /opt/local/ /usr/local/ /usr/
        NO_DEFAULT_PATH
        )

find_library(sdl_framework
        NAMES SDL
        HINT "/Library/Frameworks/SDL.framework"
        REQUIRED)


#target_link_libraries(jzintv PUBLIC -framework SDL -framework AppKit -framework Foundation objc)
target_link_libraries(jzintv
        PUBLIC
          ${sdl_framework}
          "-framework AppKit"
          "-framework Foundation"
          objc
          /usr/local/lib/libreadline.a
          /usr/local/lib/libhistory.a
          /usr/local/lib/libpcre2-8.a
          termcap
        )

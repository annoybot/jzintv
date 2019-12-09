#!/bin/bash

for d in `find . -type d`
do
  SOURCES=$( cd $d && find . -maxdepth 1  \( -name '*.c' -o -name '*.cpp' \) | sed -e 's|^./||' -e 's|^|    |' )
  HEADERS=$( cd $d && find . -maxdepth 1  -name '*.h'  | sed -e 's|^./||' -e 's|^|    |' )

  #printf  "%s: %s\n%s\n\n" "$d" "$SOURCES" "$HEADERS"
  echo "==> $d"
  printf  "target_sources( jzintv\n  PRIVATE\n%s\n  PUBLIC\n%s\n  )\n\n" "$SOURCES" "$HEADERS" >$d/CMakeLists.txt
done

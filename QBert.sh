#!/bin/sh

cd /Applications/jzintv

cat << EOF > /tmp/qbert-kb.txt
MAP 1
;ESCAPE QUIT
JS0_BTN_08   PAUSE
JS0_BTN_09   RESET
JS0_HAT0_NW  PD0L_D_NW
JS0_HAT0_NE  PD0L_D_NE
JS0_HAT0_SW  PD0L_D_SW
JS0_HAT0_SE  PD0L_D_SE

TAB   CHEAT1
EOF

#bin/jzintv -d -z1 -m1 --cheat="p $173 06"  --gfx-palette=palette.txt --kbdhackfile=/tmp/qbert-kb.txt   rom/q-bert\ \(1983\)\ \(parker\ bros\).int
cmake-build-debug/jzintv -d -z1 -f1 -m1  --cheat='p $173 06' --gfx-palette=palette.txt --kbdhackfile=/tmp/qbert-kb.txt   rom/q-bert\ \(1983\)\ \(parker\ bros\).int


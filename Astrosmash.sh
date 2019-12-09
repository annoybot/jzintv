#!/bin/sh

cd /Applications/jzintv

cat << EOF > /tmp/astrosmash-kb.txt  
MAP 1
;ESCAPE QUIT
JS0_BTN_08   PAUSE
JS0_BTN_09   RESET
JS0_BTN_10   PD0R_KP3
JS0_BTN_11   PD0L_KP3

; Map right stick as an alternate.
JS0B_E      PD0L_J_E
JS0B_ENE    PD0L_J_ENE
JS0B_NE     PD0L_J_NE
JS0B_NNE    PD0L_J_NNE
JS0B_N      PD0L_J_N
JS0B_NNW    PD0L_J_NNW
JS0B_NW     PD0L_J_NW
JS0B_WNW    PD0L_J_WNW
JS0B_W      PD0L_J_W
JS0B_WSW    PD0L_J_WSW
JS0B_SW     PD0L_J_SW
JS0B_SSW    PD0L_J_SSW
JS0B_S      PD0L_J_S
JS0B_SSE    PD0L_J_SSE
JS0B_SE     PD0L_J_SE
JS0B_ESE    PD0L_J_ESE
EOF

bin/jzintv -d  -z1 -f1 -m1 --gfx-palette=palette.txt --kbdhackfile=/tmp/astrosmash-kb.txt rom/astrosmash\ -\ meteor\ \(1981\)\ \(mattel\).int 


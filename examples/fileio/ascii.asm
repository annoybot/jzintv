
;; All this just to get \n defined.  Joy.

ascii   charset
        chardef " !\"#$%&\'",$20,$21,$22,$23,$24,$25,$26,$27
        chardef "()*+,-./",  $28,$29,$2a,$2b,$2c,$2d,$2e,$2f
        chardef "01234567",  $30,$31,$32,$33,$34,$35,$36,$37
        chardef "89:;<=>?",  $38,$39,$3a,$3b,$3c,$3d,$3e,$3f
        chardef "@ABCDEFG",  $40,$41,$42,$43,$44,$45,$46,$47
        chardef "HIJKLMNO",  $48,$49,$4a,$4b,$4c,$4d,$4e,$4f
        chardef "PQRSTUVW",  $50,$51,$52,$53,$54,$55,$56,$57
        chardef "XYZ[\\]^_", $58,$59,$5a,$5b,$5c,$5d,$5e,$5f
        chardef "`abcdefg",  $60,$61,$62,$63,$64,$65,$66,$67
        chardef "hijklmno",  $68,$69,$6a,$6b,$6c,$6d,$6e,$6f
        chardef "pqrstuvw",  $70,$71,$72,$73,$74,$75,$76,$77
        chardef "xyz{|}~",   $78,$79,$7a,$7b,$7c,$7d,$7e
        chardef '"', $22 ; not the same table entry as '\"'
        chardef "'", $27 ;
        chardef "\n\t\v\b\r\f\a", $0a,$09,$0b,$08,$0d,$0c,$07
        charuse ascii


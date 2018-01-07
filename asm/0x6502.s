.MACPACK generic
.MACPACK longbranch

.zeropage

;*------------------------------------------------------------------*
;*                    Interrupt Service Routine                     *
;*------------------------------------------------------------------*
.code
irq:        pha                 ; Save registers to current stack
            txa
            pha
            tya
            pha

            pla
            tay
            pla
            tax
            pla
            rti

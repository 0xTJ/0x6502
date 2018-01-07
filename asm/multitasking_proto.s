.MACPACK generic
.MACPACK longbranch

TaskCount = 4

.ZEROPAGE
CurTsk: .byte   0
NxtTsk: .byte   0
StkPgP: .word   0
StkPnt: .repeat TaskCount
        .byte  0
        .endrepeat
StkPgs: .repeat TaskCount
        .byte  0
        .endrepeat
Tmp0:   .byte   0
Tmp1:   .byte   0
Tmp2:   .byte   0
Tmp3:   .byte   0

;*------------------------------------------------------------------*
;*                    Interrupt Service Routine                     *
;*------------------------------------------------------------------*
.code
irq:        pha                 ; Save registers to current stack
            txa
            pha
            tya
            pha
            
savstk:
            ldy     CurTsk
            lda     #00
            sta     StkPgP
            lda     StkPgs,y
            sta     StkPgP + 1
            tsx
            stx     StkPnt,y
            stx     Tmp2
            ldy     #<$0100
            sty     Tmp0
            ldy     #>$0100
            sty     Tmp1
            ldy     Tmp2
savstk_loop:
            lda     (Tmp0),y
            sta     (StkPgP),y
            cpy     #$ff
            beq     lodstk
            iny
            jmp     savstk_loop
lodstk:
            ldy     NxtTsk
            lda     #00
            sta     StkPgP
            lda     StkPgs,y
            sta     StkPgP + 1
            ldx     StkPnt,y
            txs
            stx     Tmp2
            ldy     #<$0100
            sty     Tmp0
            ldy     #>$0100
            sty     Tmp1
            ldy     Tmp2
lodstk_loop:
            lda     (StkPgP),y
            sta     (Tmp0),y
            cpy     #$ff
            beq     irq_ret
            iny
            jmp     lodstk_loop
irq_ret:
            lda     NxtTsk
            sta     CurTsk
            pla
            tay
            pla
            tax
            pla
            rti
            

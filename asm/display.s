.ifndef INC_DISPLAY_S
INC_DISPLAY_S = 1

.include "helpers.s"
.import __DISP_START__
.import __DISP_SIZE__

;*------------------------------------------------------------------*
;*                             Display                              *
;*------------------------------------------------------------------*
.scope  Display
Width = 40
Height = 25
Base = __DISP_START__
Top = __DISP_START__ + __DISP_SIZE__ - 1

.zeropage
CurAdr: .word   0

.code
.proc   Init
        ldax    #Base
        stax    CurAdr
        rts
.endproc

.proc   IncCur
        lda     CurAdr + 1
        cmp     #>Top
        blt     @Inc
        lda     CurAdr
        cmp     #<Top
        blt     @Inc
        ldax    #Base
        stax    CurAdr
        bra     Done
@Inc:   inc16   CurAdr
Done:  rts
.endproc

.proc   PutC
        ldx     #0
        lda     (KArg,x)
        sta     (CurAdr,x)
        jsr     IncCur
        rts
.endproc

.proc   PutS
        ldx     #0
        ldy     #0
        lda     (KArg),y
        bze     @Done
@Loop:  sta     (CurAdr,x)
        jsr     IncCur
        cpy     #$FF
        beq     @Done
        iny
        lda     (KArg),y
        bnz     @Loop
@Done:  rts    
.endproc
.endscope

.rodata
CharArr:
.repeat 256, I
.byte   I
.endrep

.endif  ; NOT INC_DISPLAY_S

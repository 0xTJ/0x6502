.ifndef INC_TASKING_S
INC_TASKING_S = 1

.include "helpers.s"
; .import __STACK_START__
; .import __STACK_SIZE__

__STACK_START__ = $1000
__STECK_SIZE__ = $1000

;*------------------------------------------------------------------*
;*                             Tasking                              *
;*------------------------------------------------------------------*
.scope Tasking
TskCnt = 4
.if TskCnt > $7F
.error  "Too many tasks"
.endif

.zeropage
CurTsk: .byte 0

.repeat TskCnt, I
.ident (.sprintf ("S%02xBas", I)) = __STACK_START__ + (I * (__STECK_SIZE__ / TskCnt))
.ident (.sprintf ("S%02xTop", I)) = __STACK_START__ + ((I + 1) * (__STECK_SIZE__ / TskCnt)) - 1
.endrep

.data
SBases:
.repeat TskCnt, I
        .addr   .ident (.sprintf ("S%02xBas", I))
.endrep

STops:
.repeat TskCnt, I
        .addr   .ident (.sprintf ("S%02xTop", I))
.endrep

.zeropage
Sps:
.repeat TskCnt, I
        .addr   0
.endrep
.rodata   ; Loadable data for Sps
SpsD:
.repeat TskCnt, I
        .addr   .ident (.sprintf ("S%02xTop", I))
.endrep

.code
.macro  tpha
        sta     Reg0
        
        lda     <Tasking::CurTsk
        asl     A
        sta     <Tasking::CurTsk
        ldx     <Tasking::CurTsk
        
        lda     Reg0    
        sta     (Tasking::Sps,x)
        
        dec     <Tasking::Sps,x
        
        lda     <Tasking::CurTsk
        lsr     A
        sta     <Tasking::CurTsk
.endmacro

.macro  tpla
        lda     <Tasking::CurTsk
        asl     A
        sta     <Tasking::CurTsk
        ldx     <Tasking::CurTsk
        
        lda     (Tasking::Sps,x)
        sta     Reg0
        
        inc     <Tasking::Sps,x
        
        lda     <Tasking::CurTsk
        lsr     A
        sta     <Tasking::CurTsk
        
        lda     Reg0
.endmacro

.code

.proc   Init
        ldx     #0
@Loop:  lda     SpsD,x
        sta     Sps,x
        inx
        cpx     #2 * TskCnt
        blt     @Loop
        
        lda     #0
        sta     CurTsk
        rts
.endproc

.endscope

.endif  ; NOT INC_TASKING_S

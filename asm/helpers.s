.ifndef INC_HELPERS_S
INC_HELPERS_S = 1

;*------------------------------------------------------------------*
;*                             Helpers                              *
;*------------------------------------------------------------------*
.code
.macro  bra     arg
        clv
        bvc     arg
.endmacro

.macro  ldax    arg
        .if (.match (.left (1, {arg}), #))
            ; immediate mode
            lda     #<(.right (.tcount ({arg})-1, {arg}))
            ldx     #>(.right (.tcount ({arg})-1, {arg}))
        .else
            ; assume absolute or zero page
            lda     arg
            ldx     1+(arg)
        .endif
.endmacro

.macro  stax    arg
        ; assume absolute or zero page
        sta     arg
        stx     1+(arg)
.endmacro

.macro  addax   arg
        .if (.match (.left (1, {arg}), #))
            ; immediate mode
            .error "NOT DEFINED"
            ; lda     #<(.right (.tcount ({arg})-1, {arg}))
            ; ldx     #>(.right (.tcount ({arg})-1, {arg}))
        .else
            ; assume absolute or zero page
            add     arg
            bcc     @Done
            inx
    @Done:  
        .endif
.endmacro

.macro  subax   arg
        .if (.match (.left (1, {arg}), #))
            ; immediate mode
            .error "NOT DEFINED"
            ; lda     #<(.right (.tcount ({arg})-1, {arg}))
            ; ldx     #>(.right (.tcount ({arg})-1, {arg}))
        .else
            ; assume absolute or zero page
            sub     arg
            bsc     @Done
            dex
    @Done:  
        .endif
.endmacro

.macro  inc16   arg
        .local  Skip
        inc     arg
        bne     Skip
        inc     1+(arg)
Skip:
.endmacro

.endif  ; NOT INC_HELPERS_S

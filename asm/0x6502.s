.MACPACK generic
.MACPACK longbranch
.include "helpers.s"
.include "display.s"
.include "tasking.s"

;*------------------------------------------------------------------*
;*                              System                              *
;*------------------------------------------------------------------*
.zeropage
Reg0:   .byte   0
Reg1:   .byte   0
Reg2:   .byte   0
Reg3:   .byte   0
KArg:   .word   0

.data

.rodata
WelStr: .asciiz "Welcome to"
OsStr:  .asciiz "0x6502"
ByStr:  .asciiz "By"
AutStr: .asciiz "0xTJ"

;*------------------------------------------------------------------*
;*                               Init                               *
;*------------------------------------------------------------------*
.code
init:   sei
        jsr     Display::Init
        
        ; Show boot message.
        ldax    #Display::Base + (11 * 40) + 11
        stax    Display::CurAdr
        ldax    #WelStr
        stax    KArg
        jsr     Display::PutS
        ldax    #CharArr + ' '
        stax    KArg
        jsr     Display::PutC
        ldax    #OsStr
        stax    KArg
        jsr     Display::PutS
        ldax    #Display::Base + (12 * 40) + 16
        stax    Display::CurAdr
        ldax    #ByStr
        stax    KArg
        jsr     Display::PutS
        ldax    #CharArr + ' '
        stax    KArg
        jsr     Display::PutC
        ldax    #AutStr
        stax    KArg
        jsr     Display::PutS
        ; Done showing boot message.
        
        ; Setup tasking
        jsr Tasking::Init
        lda    #$11
        tpha
        lda    #$15
        tpha
        lda    #44
        tpla
        
        jmp     *
        
        
;*------------------------------------------------------------------*
;*                    Interrupt Service Routine                     *
;*------------------------------------------------------------------*
.code
irq:    pha                 ; Save registers to current stack
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

.segment "VECTOR"
        .word init
        .word init
        .word irq

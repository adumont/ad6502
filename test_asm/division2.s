.segment  "CODE"

.include  "fpga.inc"

.export   _init

.debuginfo      +       ; Generate debug info

_init:
; init routine, we initialize SP to $ff
    cli ; clear the interrupt-disable bit in the processor status register so the processor will respond to interrupts
    ldx #$ff
    txs
; and clear the D flag
    cld

_start:
    lda ro_dividend
    sta dividend
    lda ro_divisor
    sta divisor
    lda #0
    sta quotient
    sta remainder

; show arguments on GPIO
    lda dividend
    sta GPIO_DATA

    lda divisor
    sta GPIO_DATA

; Division, 8-bit / 8-bit = 8-bit quotient, 8-bit remainder (unsigned)
; http://6502org.wikidot.com/software-math-intdiv
    lda #0
    ldx #8
    asl dividend
@loop:
    rol
    cmp divisor
    bcc @nosbc
    sbc divisor
@nosbc:
    rol dividend
    dex
    bne @loop

    sta remainder
    lda dividend
    sta quotient
   
; rts
   

; show results on GPIO
    lda quotient
    sta GPIO_DATA

    lda remainder
    sta GPIO_DATA

    brk

.segment  "RODATA"
ro_dividend:  .byte $64
ro_divisor:   .byte $09

.segment  "DATA"
dividend:  .byte $00
divisor:   .byte $00
quotient:  .byte $00
remainder: .byte $00

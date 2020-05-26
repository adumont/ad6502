.segment  "CODE"

.include  "fpga.inc"

.export   _init

_init:
; init routine, we initialize SP to $ff
    ldx #$ff
    txs
; and clear the D flag
    cld

loop:
    lda GPIO_DATA
    sta GPIO_DATA
    jmp loop

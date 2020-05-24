.segment  "CODE"

.include  "fpga.inc"

.export   _init

_init:

    ldx #$ff
    txs
    cld

loop:
    lda GPIO_DATA
    sta GPIO_DATA
    jmp loop

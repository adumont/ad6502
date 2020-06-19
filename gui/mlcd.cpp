#include "mlcd.h"

#define RESET_CYCLES 20
#define BUSY 1
#define NOT_BUSY 0

MLcd::MLcd(QWidget *parent) : QWidget(parent)
{
    mtime = 0;
    prev_main_time = 0;
}

bool MLcd::eval(unsigned char ctrl, unsigned char data, bool clk, bool rst)
{
    // see https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
    // and https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller

    // supports only 8-bit operation

    // ctrl: 3 bits: RS, RW_, E
    // data: 8 bits: DB7..DB0

    // if clock low, don't do anything
    if( !clk ) return isBusy();

    // handle reset:
    if( rst ) reset();

    // TODO if busy, we need to handle "busy flag read" somehow
    if( isBusy() ) return 1;

    // Signals driven by MPU

    // RS: selects registers
    //   0: Instruction register (for write) Busy flag: address counter (for read)
    //   1: Data register (for write and read)
    bool RS  = ctrl >> 2 & 1;

    // Selects read or write.
    //   0: Write
    //   1: Read
    bool RW_ = ctrl >> 1 & 1;

    // Starts data read/write
    bool E   = ctrl >> 0 & 1;

    // we return the busy flag
    return isBusy();


}

bool MLcd::isBusy()
{
    return (main_time < busy_until);
}

void MLcd::reset()
{
    busy_until = main_time + 2*(RESET_CYCLES);
}

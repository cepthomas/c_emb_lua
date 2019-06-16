
#ifndef BOARD_MODULE_H
#define BOARD_MODULE_H

#include "common_module.h"

/// @file Interface to the board module.


//---------------- Public API ----------------------//

/// Physical IO.
#define NUM_DIG_PINS 16

#define DIG_IN_1  1
#define DIG_IN_2  2
#define DIG_IN_3  3
#define DIG_OUT_1 4
#define DIG_OUT_2 5
#define DIG_OUT_3 6


//---------------- Definitions ---------------------//

/// Type: Function pointer for registration of digital input change callbacks.
/// @param which The digital input whose state has changed.
/// @param value The new state of the input.
typedef void (*fpDigInterrupt)(unsigned int which, bool value);

/// Type: Function pointer for handling of timer ticks.
typedef void (*fpTimerInterrupt)(void);


//---------------- General Functions -----------------//

/// Initialize the module.
/// @return Status.
status_t board_init(void);

/// Turn interrupts on/off.
/// @param env On/off.
/// @return Status.
status_t board_enbInterrupts(bool enb);

/// Send to the log.
/// @param txt Text to log.
/// @return Status.
status_t board_log(const char* txt);

/// Clean up module resources.
/// @return Status.
status_t board_destroy(void);


//---------------- Digital IO Functions -----------------//

/// Register for digital input interrupt.
/// @param fp Callback function.
/// @return Status.
status_t board_regDigInterrupt(fpDigInterrupt fp);

/// Write a digital output.
/// @param pin Specific pin number.
/// @param value Value to write.
/// @return Status.
status_t board_writeDig(unsigned int pin, bool value);

/// Read a digital input or output.
/// @param pin Specific pin number.
/// @param value Where to place the value.
/// @return Status.
status_t board_readDig(unsigned int pin, bool* value);


//---------------- Serial Functions -----------------//

/// Open a serial port.
/// @param channel Specific channel.
/// @return Status.
status_t board_serOpen(unsigned int channel);

/// Read from a serial channel.
/// @param channel Specific channel.
/// @param buff Data buffer. Will be a zero-terminated string.
/// @param num Length of buff.
/// @return Status.
status_t board_serReadLine(unsigned int channel, char* buff, int num);

/// Write to a serial channel.
/// @param channel Specific channel.
/// @param buff What to send as a zero-terminated string.
/// @return Status.
status_t board_serWriteLine(unsigned int channel, char* buff);


//---------------- Timer Functions -----------------//

/// Timer interrupts.
/// @param period How oftern in msec.
/// @param fp Callback function.
/// @return Status.
status_t board_regTimerInterrupt(unsigned int period, fpTimerInterrupt fp);

/// Get number of microseconds since beginning.
/// @return Microseconds.
uint64_t board_getCurrentUsec(void);

#endif // BOARD_MODULE_H

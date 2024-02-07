
#ifndef BOARD_H
#define BOARD_H

#include "common.h"

/// @brief Interface to the hardware board. This has simulated digital IO, timer, crude CLI.

//---------------- Public API ----------------------//

/// Max line.
#define CLI_BUFF_LEN 128

/// Physical IO.
#define NUM_DIG_PINS 16

#define DIG_IN_1  1
#define DIG_IN_2  2
#define DIG_IN_3  3
#define DIG_OUT_1 4
#define DIG_OUT_2 5
#define DIG_OUT_3 6


//---------------- Main Functions -----------------//

/// Type: Function pointer for registration of digital input change callbacks.
/// @param which The digital input whose state has changed.
/// @param value The new state of the input.
typedef void (*board_DigInterrupt_t)(unsigned int which, bool value);

/// Initialize the component.
/// @return Status.
int board_Init(void);

/// Turn interrupts on/off.
/// @param env On/off.
/// @return Status.
int board_EnableInterrupts(bool enb);

/// Clean up component resources.
/// @return Status.
int board_Destroy(void);


//---------------- Digital IO Functions -----------------//

/// Client registration for digital input interrupt.
/// @param fp Callback function.
/// @return Status.
int board_RegDigInterrupt(board_DigInterrupt_t fp);

/// Write a digital output.
/// @param pin Specific pin number.
/// @param value Value to write.
/// @return Status.
int board_WriteDig(unsigned int pin, bool value);

/// Read a digital input or output.
/// @param pin Specific pin number.
/// @param value Where to place the value.
/// @return Status.
int board_ReadDig(unsigned int pin, bool* value);


//---------------- CLI Functions -----------------//

/// Open a cli (polled).
/// @param channel Specific channel.
/// @return Status.
int board_CliOpen(unsigned int channel);

/// Read a line from a cli. This does not block. Buffers chars until EOL.
/// @param buff Data buffer. Will be a zero-terminated string.
/// @param num Max length of buff.
/// @return Status. 0 if buff is valid.
int board_CliReadLine(char* buff, unsigned int num);

/// Write a line to a cli.
/// @param buff What to send as a zero-terminated string.
/// @return Status.
int board_CliWriteLine(const char* format, ...);


//---------------- Timer Functions -----------------//

/// Get number of microseconds since beginning.
/// In an embedded system this would be supplied by a hardware clock source.
/// @return Microseconds.
uint64_t board_GetCurrentUsec(void);


#endif // BOARD_H

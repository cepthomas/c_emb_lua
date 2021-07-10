
#ifndef BOARD_H
#define BOARD_H

#include "common.h"

/// @brief Interface to the hardware board. Ours is simulated, yours would be completely different.

/// Max line.
#define SER_BUFF_LEN 128

/// Physical IO.
#define NUM_DIG_PINS 16

#define DIG_IN_1  1
#define DIG_IN_2  2
#define DIG_IN_3  3
#define DIG_OUT_1 4
#define DIG_OUT_2 5
#define DIG_OUT_3 6


//---------------- Digital IO Functions -----------------//

/// Type: Function pointer for registration of digital input change callbacks.
/// @param which The digital input whose state has changed.
/// @param value The new state of the input.
typedef void (*board_DigInterrupt_t)(unsigned int which, bool value);

/// Type: Function pointer for handling of timer ticks.
typedef void (*board_TimerInterrupt_t)(void);

/// Initialize the component.
/// @return Status.
status_t board_Init(void);

/// Turn interrupts on/off.
/// @param env On/off.
/// @return Status.
status_t board_EnableInterrupts(bool enb);

/// Clean up component resources.
/// @return Status.
status_t board_Destroy(void);


//---------------- Digital IO Functions -----------------//

/// Client registration for digital input interrupt.
/// @param fp Callback function.
/// @return Status.
status_t board_RegDigInterrupt(board_DigInterrupt_t fp);

/// Write a digital output.
/// @param pin Specific pin number.
/// @param value Value to write.
/// @return Status.
status_t board_WriteDig(unsigned int pin, bool value);

/// Read a digital input or output.
/// @param pin Specific pin number.
/// @param value Where to place the value.
/// @return Status.
status_t board_ReadDig(unsigned int pin, bool* value);


//---------------- Serial Functions -----------------//

/// Open a serial port.
/// @param channel Specific channel.
/// @return Status.
status_t board_SerOpen(unsigned int channel);

/// Read a line from a serial channel. This does not block. Buffers chars until EOL.
/// @param buff Data buffer. Will be a zero-terminated string.
/// @param num Length of buff.
/// @return Status.
status_t board_SerReadLine(char* buff, unsigned int num);

/// Write a line to a serial channel.
/// @param buff What to send as a zero-terminated string.
/// @return Status.
status_t board_SerWriteLine(const char* format, ...);


//---------------- Timer Functions -----------------//

/// Client registration for timer interrupts.
/// @param msec How often in msec.
/// @param fp Callback function.
/// @return Status.
status_t board_RegTimerInterrupt(unsigned int msec, board_TimerInterrupt_t fp);

/// Get number of microseconds since beginning.
/// In an embedded system this would be supplied by a hardware clock source.
/// @return Microseconds.
uint64_t board_GetCurrentUsec(void);


#endif // BOARD_H


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <conio.h>
#include <stdarg.h>

#include "board.h"


//---------------- Private ------------------------------//

/// Registered client callback.
static board_DigInterrupt_t p_dig_interrupt;

/// Registered client callback.
static board_TimerInterrupt_t p_timer_interrupt;

/// Interrupts enabled?
static bool p_enb_interrupts;

/// Serial receive buffer to collect input chars. In this simulator we will used stdio for serial IO.
static char p_rx_buff[SER_BUFF_LEN];


//---------------- Simulator Stuff -----------------------//

/// Simulated digital IO pins.
static bool p_dig_pins_sim[NUM_DIG_PINS];

/// Windows periodic timer.
#ifdef WIN32
#include <windows.h>
static HANDLE p_win_handle;
static VOID CALLBACK p_WinTimerHandler(PVOID, BOOLEAN);
VOID CALLBACK p_WinTimerHandler(PVOID lpParameter, BOOLEAN TimerOrWaitFired) { p_timer_interrupt(); }
#endif


//---------------- Public Implementation -----------------//

//--------------------------------------------------------//
status_t board_Init(void)
{
    status_t stat = STATUS_OK;

    memset(p_rx_buff, 0, SER_BUFF_LEN);

    p_enb_interrupts = false;
    p_dig_interrupt = NULL;
    p_timer_interrupt = NULL;

    for(int i = 0; i < NUM_DIG_PINS; i++)
    {
        p_dig_pins_sim[i] = false;
    }

    return stat;
}

//--------------------------------------------------------//
status_t board_Destroy(void)
{
    status_t stat = STATUS_OK;

#ifdef WIN32
    DeleteTimerQueueTimer(NULL, p_win_handle, NULL);
    CloseHandle(p_win_handle);
#endif

    return stat;
}

//--------------------------------------------------------//
status_t board_EnableInterrupts(bool enb)
{
    status_t stat = STATUS_OK;

    p_enb_interrupts = enb;

    return stat;
}

//--------------------------------------------------------//
status_t board_RegDigInterrupt(board_DigInterrupt_t fp)
{
    status_t stat = STATUS_OK;

    p_dig_interrupt = fp;

    return stat;
}

//--------------------------------------------------------//
status_t board_RegTimerInterrupt(unsigned int msec, board_TimerInterrupt_t fp)
{
    status_t stat = STATUS_OK;

    p_timer_interrupt = fp;

#ifdef WIN32
    if(CreateTimerQueueTimer(&p_win_handle, NULL, (WAITORTIMERCALLBACK)p_WinTimerHandler, NULL, msec, msec, WT_EXECUTEINTIMERTHREAD) == 0)
    {
        stat = STATUS_ERROR;
    }
#endif

    return stat;
}

//--------------------------------------------------------//
uint64_t board_GetCurrentUsec(void)
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    long usec = tv.tv_sec * 1000000 + tv.tv_usec;
    return (uint64_t)usec;
}

//--------------------------------------------------------//
status_t board_WriteDig(unsigned int pin, bool value)
{
    status_t stat = STATUS_OK;

    if(pin < NUM_DIG_PINS)
    {
        p_dig_pins_sim[pin] = value;
    }
    else
    {
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t board_ReadDig(unsigned int pin, bool* value)
{
    status_t stat = STATUS_OK;

    if(pin < NUM_DIG_PINS)
    {
        *value = p_dig_pins_sim[pin];
    }
    else
    {
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t board_SerOpen(unsigned int channel)
{
    (void)channel;

    status_t stat = STATUS_OK;

    // Prompt.
    board_SerWriteLine("\r\n>");

    return stat;
}

//--------------------------------------------------------//
status_t board_SerReadLine(char* buff, unsigned int num)
{
    status_t stat = STATUS_OK;

    // Default.
    buff[0] = 0;

    char c = (char)_getch();

    if(c != 0)
    {
        switch(c)
        {
            case '\n':
                // Ignore.
                break;

            case '\r':
                // Echo return.
                board_SerWriteLine("");
                // Copy to client buff.
                strncpy(buff, p_rx_buff, num);
                // Clear.
                memset(p_rx_buff, 0, SER_BUFF_LEN);
                // Echo prompt.
                board_SerWriteLine("");
                break;

            default:
                // Echo char.
                putchar(c);
                // Save it.
                p_rx_buff[strlen(p_rx_buff)] = c;
                break;
        }
    }

    return stat;
}

//--------------------------------------------------------//
status_t board_SerWriteLine(const char* format, ...)
{
    status_t stat = STATUS_OK;

    static char buff[SER_BUFF_LEN];

    va_list args;
    va_start(args, format);
    vsnprintf(buff, SER_BUFF_LEN-1, format, args);

    // Add a prompt.
    printf("%s\r\n>", buff);

    return stat;    
}

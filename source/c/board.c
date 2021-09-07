
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <conio.h>
#include <stdarg.h>

#include "board.h"


//---------------- Private ------------------------------//

/// Registered client callback for IO pin changes.
static board_DigInterrupt_t p_dig_interrupt = NULL;

/// Registered client callback for periodic timer ticks.
static board_TimerInterrupt_t p_timer_interrupt = NULL;

/// Interrupts enabled?
static bool p_enb_interrupts = false;

/// CLI buffer to collect input chars. This simulator uses stdio, a real system would probably use a serial port.
static char p_cli_buff[CLI_BUFF_LEN];


//---------------- Simulator Stuff -----------------------//

/// Simulated digital IO pins.
static bool p_dig_pins_sim[NUM_DIG_PINS];

/// Windows periodic timer.
#ifdef WIN32
#include <windows.h>
static HANDLE p_win_handle;
static VOID CALLBACK p_WinTimerHandler(PVOID lpParameter, BOOLEAN TimerOrWaitFired) { p_timer_interrupt(); }
#endif


//---------------- Public Implementation -----------------//

//--------------------------------------------------------//
int board_Init(void)
{
    int stat = RS_PASS;

    memset(p_cli_buff, 0, CLI_BUFF_LEN);

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
int board_Destroy(void)
{
    int stat = RS_PASS;

#ifdef WIN32
    DeleteTimerQueueTimer(NULL, p_win_handle, NULL);
    CloseHandle(p_win_handle);
#endif

    return stat;
}

//--------------------------------------------------------//
int board_EnableInterrupts(bool enb)
{
    int stat = RS_PASS;

    p_enb_interrupts = enb;

    return stat;
}

//--------------------------------------------------------//
int board_RegDigInterrupt(board_DigInterrupt_t fp)
{
    int stat = RS_PASS;

    p_dig_interrupt = fp;

    return stat;
}



// #ifdef WIN32
//     void _timeProc(int id, int msg, int user, int param1, int param2)
//     {

//     }
// #endif


//--------------------------------------------------------//
int board_RegTimerInterrupt(unsigned int msec, board_TimerInterrupt_t fp)
{
    int stat = RS_PASS;

    p_timer_interrupt = fp;

#ifdef WIN32
    if(CreateTimerQueueTimer(&p_win_handle, NULL, (WAITORTIMERCALLBACK)p_WinTimerHandler, NULL, msec, msec, WT_EXECUTEINTIMERTHREAD) == 0)
    {
        stat = RS_ERR;
    }

    // const int MMTIMER_PERIOD = 1;
    // // Create and start periodic timer. resolution is 0 or 1, mode is 1=TIME_PERIODIC
    // int _timerID = timeSetEvent(MMTIMER_PERIOD, 0, _timeProc, NULL, 1);
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
int board_WriteDig(unsigned int pin, bool value)
{
    int stat = RS_PASS;

    if(pin < NUM_DIG_PINS)
    {
        p_dig_pins_sim[pin] = value;
    }
    else
    {
        stat = RS_ERR;
    }

    return stat;
}

//--------------------------------------------------------//
int board_ReadDig(unsigned int pin, bool* value)
{
    int stat = RS_PASS;

    if(pin < NUM_DIG_PINS)
    {
        *value = p_dig_pins_sim[pin];
    }
    else
    {
        stat = RS_ERR;
    }

    return stat;
}

//--------------------------------------------------------//
int board_CliOpen(unsigned int channel)
{
    (void)channel;

    int stat = RS_PASS;

    // Prompt.
    board_CliWriteLine("\r\n>");

    return stat;
}

//--------------------------------------------------------//
int board_CliReadLine(char* buff, unsigned int num)
{
    int stat = RS_PASS;

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
                board_CliWriteLine("");
                // Copy to client buff.
                strncpy(buff, p_cli_buff, num);
                // Clear.
                memset(p_cli_buff, 0, CLI_BUFF_LEN);
                // Echo prompt.
                board_CliWriteLine("");
                break;

            default:
                // Echo char.
                putchar(c);
                // Save it.
                p_cli_buff[strlen(p_cli_buff)] = c;
                break;
        }
    }

    return stat;
}

//--------------------------------------------------------//
int board_CliWriteLine(const char* format, ...)
{
    int stat = RS_PASS;

    static char buff[CLI_BUFF_LEN];

    va_list args;
    va_start(args, format);
    vsnprintf(buff, CLI_BUFF_LEN-1, format, args);

    // Add a prompt.
    printf("%s\r\n>", buff);

    return stat;    
}

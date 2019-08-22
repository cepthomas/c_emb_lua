
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <conio.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "board.h"
#include "stringx.h"


//---------------- Private ------------------------------//

#define SER_BUFF_LEN 128

/// Registered client callback.
static fpDigInterrupt p_digInterrupt;

/// Registered client callback.
static fpTimerInterrupt p_timerInterrupt;

/// Interrupts enabled?
static bool p_enbInterrupts;

/// Serial receive buffer to collect input chars.
/// In this simulator we will used stdio for serial IO.
static char p_rxBuff[SER_BUFF_LEN];

//---------------- Simulator Stuff -----------------------//

/// Simulated digital IO pins.
static bool p_digPinsSim[NUM_DIG_PINS];

/// Windows periodic timer.
#ifdef WIN32
static HANDLE p_winHandle;
static VOID CALLBACK p_winTimerHandler(PVOID, BOOLEAN);
VOID CALLBACK p_winTimerHandler(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
    (void)lpParameter;
    (void)TimerOrWaitFired;
    // Call our timer.
    p_timerInterrupt();
}
#endif


//---------------- Public Implementation -----------------//

//--------------------------------------------------------//
status_t board_init(void)
{
    status_t stat = STATUS_OK;

    memset(p_rxBuff, 0, SER_BUFF_LEN);

    p_enbInterrupts = false;
    p_digInterrupt = NULL;
    p_timerInterrupt = NULL;

    for(int i = 0; i < NUM_DIG_PINS; i++)
    {
        p_digPinsSim[i] = false;
    }

    return stat;
}

//--------------------------------------------------------//
status_t board_destroy(void)
{
    status_t stat = STATUS_OK;

#ifdef WIN32
    DeleteTimerQueueTimer(NULL, p_winHandle, NULL);
    CloseHandle(p_winHandle);
#endif

    return stat;
}

//--------------------------------------------------------//
status_t board_enbInterrupts(bool enb)
{
    status_t stat = STATUS_OK;

    p_enbInterrupts = enb;

    return stat;
}

//--------------------------------------------------------//
status_t board_log(const char* txt)
{
    status_t stat = STATUS_OK;

    board_serWriteLine(txt);

    return stat;
}

//--------------------------------------------------------//
status_t board_regDigInterrupt(fpDigInterrupt fp)
{
    status_t stat = STATUS_OK;

    p_digInterrupt = fp;

    return stat;
}

//--------------------------------------------------------//
status_t board_regTimerInterrupt(unsigned int msec, fpTimerInterrupt fp)
{
    status_t stat = STATUS_OK;

    p_timerInterrupt = fp;

#ifdef WIN32
    if(CreateTimerQueueTimer(&p_winHandle, NULL, (WAITORTIMERCALLBACK)p_winTimerHandler,
         NULL, msec, msec, WT_EXECUTEINTIMERTHREAD) == 0)
    {
        stat = STATUS_ERROR;
    }
#endif

    return stat;
}

//--------------------------------------------------------//
uint64_t board_getCurrentUsec(void)
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    long usec = tv.tv_sec * 1000000 + tv.tv_usec;
    return (uint64_t)usec;
}

//--------------------------------------------------------//
status_t board_writeDig(unsigned int pin, bool value)
{
    status_t stat = STATUS_OK;

    if(pin < NUM_DIG_PINS)
    {
        p_digPinsSim[pin] = value;
    }
    else
    {
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t board_readDig(unsigned int pin, bool* value)
{
    status_t stat = STATUS_OK;

    if(pin < NUM_DIG_PINS)
    {
        *value = p_digPinsSim[pin];
    }
    else
    {
        stat = STATUS_ERROR;
    }

    return stat;
}

//--------------------------------------------------------//
status_t board_serOpen(unsigned int channel)
{
    (void)channel;

    status_t stat = STATUS_OK;

    // Prompt.
    board_serWriteLine("\r\n>");

    return stat;
}

//--------------------------------------------------------//
status_t board_serReadLine(char* buff, unsigned int num)
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
                board_serWriteLine("");
                // Copy to client buff.
                strncpy(buff, p_rxBuff, num);
                // Clear.
                memset(p_rxBuff, 0, SER_BUFF_LEN);
                // Echo prompt.
                board_serWriteLine("");
                break;

            default:
                // Echo char.
                putchar(c);
                // Save it.
                p_rxBuff[strlen(p_rxBuff)] = c;
                break;
        }
    }

    return stat;
}

//--------------------------------------------------------//
status_t board_serWriteLine(const char* buff)
{
    status_t stat = STATUS_OK;

    // Add a prompt.
    printf("%s\r\n>", buff);

    return stat;    
}

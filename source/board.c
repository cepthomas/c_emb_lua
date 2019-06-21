
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <conio.h>

//#ifdef WIN32
//#include <windows.h>
//#endif

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

/// Main timer period in msec.
static unsigned int p_timerPeriod;

//---------------- Simulator Stuff -----------------------//

/// Serial receive buffer. In this simulator we will used stdio for serial IO.
static char p_rxBufSim[SER_BUFF_LEN];

/// Simulated digital pins.
static bool p_digPinsSim[NUM_DIG_PINS];


//---------------- Public Implementation -----------------//

//--------------------------------------------------------//
status_t board_init(void)
{
    status_t stat = STATUS_OK;

    memset(p_rxBufSim, 0, SER_BUFF_LEN);

    p_enbInterrupts = false;
    p_digInterrupt = NULL;
    p_timerInterrupt = NULL;
    p_timerPeriod = 0; // not running

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
status_t board_regTimerInterrupt(unsigned int when, fpTimerInterrupt fp) //TODOX need some fake timer for demo purposes.
{
    (void)when;

    status_t stat = STATUS_OK;

    p_timerInterrupt = fp;

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
    (void)pin;
    (void)value;

    status_t stat = STATUS_OK;

    return stat;
}

//--------------------------------------------------------//
status_t board_readDig(unsigned int pin, bool* value)
{
    (void)pin;
    (void)value;

    status_t stat = STATUS_OK;

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
                strncpy(buff, p_rxBufSim, num);
                // Clear.
                memset(p_rxBufSim, 0, SER_BUFF_LEN);
                // Echo prompt.
                board_serWriteLine("");
                break;

            default:
                // Echo char.
                putchar(c);
                // Save it.
                p_rxBufSim[strlen(p_rxBufSim)] = c;
                break;
        }
    }

    return stat;
}

//--------------------------------------------------------//
status_t board_serWriteLine(const char* buff)
{
    status_t stat = STATUS_OK;

    // Prompt.
    printf("%s\r\n>", buff);

    return stat;    
}

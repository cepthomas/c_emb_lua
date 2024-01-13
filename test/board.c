
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <conio.h>
#include <stdarg.h>

#include "board.h"


//---------------- Private ------------------------------//

/// Registered client callback for IO pin changes.
static board_DigInterrupt_t p_dig_interrupt = NULL;

/// Interrupts enabled?
static bool p_enb_interrupts = false;

/// CLI buffer to collect input chars. This simulator uses stdio, a real system would probably use a serial port.
static char p_cli_buff[CLI_BUFF_LEN];

/// Simulated digital IO pins.
static bool p_dig_pins_sim[NUM_DIG_PINS];


//---------------- Public Implementation -----------------//

//--------------------------------------------------------//
int board_Init(void)
{
    int stat = 0;

    memset(p_cli_buff, 0, CLI_BUFF_LEN);

    p_enb_interrupts = false;
    p_dig_interrupt = NULL;

    for(int i = 0; i < NUM_DIG_PINS; i++)
    {
        p_dig_pins_sim[i] = false;
    }

    return stat;
}

//--------------------------------------------------------//
int board_Destroy(void)
{
    int stat = 0;
    p_enb_interrupts = false;
    return stat;
}

//--------------------------------------------------------//
int board_EnableInterrupts(bool enb)
{
    int stat = 0;
    p_enb_interrupts = enb;
    return stat;
}

//--------------------------------------------------------//
int board_RegDigInterrupt(board_DigInterrupt_t fp)
{
    int stat = 0;
    p_dig_interrupt = fp;
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
    int stat = 0;

    if(pin < NUM_DIG_PINS)
    {
        p_dig_pins_sim[pin] = value;
    }
    else
    {
        stat = 1;
    }

    return stat;
}

//--------------------------------------------------------//
int board_ReadDig(unsigned int pin, bool* value)
{
    int stat = 0;

    if(pin < NUM_DIG_PINS)
    {
        *value = p_dig_pins_sim[pin];
    }
    else
    {
        stat = 1;
    }

    return stat;
}

//--------------------------------------------------------//
int board_CliOpen(unsigned int channel)
{
    (void)channel;

    int stat = 0;

    // Prompt.
    board_CliWriteLine("\r\n>");

    return stat;
}

//--------------------------------------------------------//
int board_CliReadLine(char* buff, unsigned int num)
{
    int stat = 1;

    // Default.
    buff[0] = 0;

    if (_kbhit())
    {
        char c = (char)_getch();
        switch(c)
        {
            case '\n':
                // Ignore.
                break;

            case '\r':
                // Echo return.
                board_CliWriteLine("");

                // Copy to client buff. Should be 0 terminated.
                strncpy(buff, p_cli_buff, num);
                stat = 0;

                // Clear buffer.
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
    int stat = 0;

    static char buff[CLI_BUFF_LEN];

    va_list args;
    va_start(args, format);
    vsnprintf(buff, CLI_BUFF_LEN-1, format, args);
    va_end(args);

    // Add a prompt.
    printf("%s\r\n>", buff);

    return stat;    
}

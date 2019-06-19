
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "board.h"





//----------------- Sim stuff ------------------------//
#define BUFF_LEN 128

/// Current digital pins. Index is pin num not enum.
static bool p_digPins[NUM_DIG_PINS];

/// Main timer period in msec.
static unsigned int p_timerPeriod;

// /// Most recent serial port write.
// static char p_lastSerWrite[BUFF_LEN];

// /// Next simulated serial read.
// static char p_nextSerRead[BUFF_LEN];

// /// Most recent log write.
// static char p_lastLogWrite[BUFF_LEN];

// /// Registered client callback.
// static fpDigInterrupt p_digInterrupt;

// /// Registered client callback.
// static fpAnaInterrupt p_anaInterrupt;

// /// Registered client callback.
// static fpTimerInterrupt p_timerInterrupt;

// /// Interrupts enable on/off.
// static bool p_enbInterrupts;


// /// Tickle an input pin.
// /// @param pin Specific pin number.
// /// @param state What to set it to.
// void hal_sim_injectDigInput(unsigned int pin, bool state);

// /// Clear all pin states.
// void hal_sim_clearDigPins();

// /// Get a cached pin state.
// /// @param pin Specific pin number.
// /// @return The value.
// bool hal_sim_getDigPin(unsigned int pin);

// /// Get the most recent serial port write.
// /// @return The text sent.
// const char* hal_sim_getLastSerWrite();

// /// Get the most recent log write.
// /// @return The text logged.
// const char* hal_sim_getLastLogWrite();

// /// Preload the next simulated serial read.
// /// @param s The text.
// void hal_sim_setNextSerRead(const char* s);

// /// Do a system timer tick.
// void hal_sim_timerTick();



//---------------- Private --------------------------//

/// Registered client callback.
static fpDigInterrupt p_digInterrupt;

/// Registered client callback.
static fpTimerInterrupt p_timerInterrupt;

/// Interrupts enabled?
static bool p_enbInterrupts;


//---------------- Public Implementation -------------//


//--------------------------------------------------------//
status_t board_init(void)
{
    status_t stat = STATUS_OK;

    p_enbInterrupts = false;
    p_digInterrupt = NULL;
    p_timerInterrupt = NULL;
    p_timerPeriod = 0; // not running

    for(int i = 0; i < NUM_DIG_PINS; i++)
    {
        p_digPins[i] = false;
    }


    // memset(p_lastSerWrite, 0x00, sizeof(p_lastSerWrite));
    // memset(p_nextSerRead, 0x00, sizeof(p_nextSerRead));
    // memset(p_lastLogWrite, 0x00, sizeof(p_lastLogWrite));

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

    printf("%s", txt);

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
status_t board_regTimerInterrupt(unsigned int when, fpTimerInterrupt fp)
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

    return stat;
}

//--------------------------------------------------------//
status_t board_serReadLine(unsigned int channel, char* buff, int num)
{
    (void)channel;
    (void)buff;
    (void)num;

    status_t stat = STATUS_OK;

    // strcpy(buff, p_nextSerRead);
    // p_nextSerRead[0] = 0;

    return stat;
}

//--------------------------------------------------------//
status_t board_serWriteLine(unsigned int channel, char* buff)
{
    (void)channel;
    (void)buff;

    status_t stat = STATUS_OK;
    
    // strncpy(p_lastSerWrite, buff, BUFF_LEN);
    // p_lastSerWrite[0] = 0;

    return stat;    
}


// //---------------- Simulator Support ---------------------//


// //--------------------------------------------------------//
// void hal_sim_clearDigPins()
// {
//     for(int i = 0; i < NUM_DIG_PINS; i++)
//     {
//         p_digPins[i] = false;
//     }
// }

// //--------------------------------------------------------//
// void hal_sim_injectDigInput(unsigned int pin, bool state)
// {
//     p_digPins[pin] = state;

//     if(p_digInterrupt != NULL)
//     {
//         p_digInterrupt(pin, state);
//     }
// }

// //--------------------------------------------------------//
// bool hal_sim_getDigPin(unsigned int pin)
// {
//     return p_digPins[pin];
// }

// //--------------------------------------------------------//
// const char* hal_sim_getLastSerWrite()
// {
//     return p_lastSerWrite;
// }

// //--------------------------------------------------------//
// const char* hal_sim_getLastLogWrite()
// {
//     return p_lastLogWrite;
// }

// //--------------------------------------------------------//
// void hal_sim_setNextSerRead(const char* s)
// {
//     strncpy(p_nextSerRead, s, BUFF_LEN);
// }

// //--------------------------------------------------------//
// const char* hal_sim_getNextSerRead(void)
// {
//     return p_nextSerRead;
// }

// //--------------------------------------------------------//
// void hal_sim_timerTick()
// {
//     p_timerInterrupt();   
// }

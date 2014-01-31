/**
 * IR decoder for SYMA S107C HElIcopter remote codes.
 *
 * @author Michal Krombholz
 * @license GNU General Public License (GPL) v2 or later
 */


//------------------------------ IR decoder ----------------------------------
// "SYMA S107C Heli" RC remote protocol description.
// (may fit other helis from "Protocol" but is different then SIMA Helis.
//
// The remote pulses IR @38kHz carrier
// The command consist of:
// - preamble (start) 2ms on 2 ms off
// - 32 command bits
//   * zero: ~320us on + 280us off = ~ 600us cycle
//   * one:  340us on + 660us off  = ~ 1000us cycle
// - final IR PULSE
//
// Detection Algorithm
//
// The symbols are timed from falling edge of IR to the next fallling edge of IR (ie end of burst).
// However since the IR detector uses "0" level as "active" (ie IR present)
// the pin interrupts as active on "rising" edge of detector signal.
// The algoritm looks for the START symbol, and then expects the bits to follow.
// The first 500us burst is only used to time next symbol (ie START symbol).
// However, the alg is not perfect as it hinges on receiving and recognizing correct START symbol.
// A possibility exist that wrapping of time ticks would produce a delta time matching start by chance.
// This is however rather unlikely (more unlikely when timer wraparound is rare/longer).
// Also, the alg has no build in timeout and when not all bits are received in one IR command
// (due to interference) it will continue to read bits of next one and eventually err (too many bits).
// Hence it will fail receiving both commands (bad one and next potentially good one).
//

#include <stdint.h>

// IR protocol "constants" that are filled at the start
// hence they are in "tick count" their value is only know in run-time
// to avoid their computation each time in an interrupt routine, compute it here
// symbol timing (in "ticks")
uint32_t IR_SYMBOL_START;
uint32_t IR_SYMBOL_LONG;
uint32_t IR_SYMBOL_SHORT;
uint32_t IR_SYMBOL_TOLERANCE;

// number of data bits in a command
const int IR_BIT_COUNT = 32;

// fully received command (gets overwritten by next one if note read)
static volatile uint32_t irReceivedCommand;


/**
* @brief  initialize protocol parameters dependent on time tick
* @param  ticksPerUs number of tick in 1us
* @retval None
*/
void irRcvInitProtocol(uint32_t ticksPerUs)
{
    IR_SYMBOL_START = (4000 * ticksPerUs);
    IR_SYMBOL_LONG  = (1000 * ticksPerUs);
    IR_SYMBOL_SHORT = (600 * ticksPerUs);
    IR_SYMBOL_TOLERANCE = (200 * ticksPerUs);
    irReceivedCommand = 0;
}

//------------------------------ IR decoder ----------------------------------

#define CHECK_SYMBOL(time,NAME) ((time>IR_SYMBOL_##NAME-IR_SYMBOL_TOLERANCE)&&(time<IR_SYMBOL_##NAME+IR_SYMBOL_TOLERANCE))

/**
* @brief return next command received or 0 if none
* @retval IR 33bit command (zero==none)
*/
uint32_t irRcvGetCommand()
{
    uint32_t cmd = irReceivedCommand;
    // clear the command so we know next time if we received anything
    irReceivedCommand = 0;
    return cmd;
}

/**
 * @brief called from interrupt handler when a falling edge of IR signal burst is detected
 * @param timeNowTicks - time stamp in system ticks
 */
void irRcvReportEdgeDetected(uint32_t timeNowTicks)
{
    // locals only used here, hence "static"
    // temp to accumulate command bits during receiving
    static uint32_t tempCmd;
    // error (must but greater then IR_BIT_COUNT)
    const int ERROR = 9999;
    // current bit count
    // 0 marks reception of start and hence start of command
    // > IR_BIT_COUNT means not started or after error
    static int bitCount = ERROR;
    // previous edge's timestamp
    static uint32_t prev;

    // measure time from prev burst to this burst
    uint32_t now = timeNowTicks;
    uint32_t delta = now > prev ? now-prev : ((uint32_t)0xFFFFFFFF) - (prev-now);
    prev = now;

    // classify received symbol based on timing since last edge
    if( CHECK_SYMBOL(delta,START) )
    {
        // start interval
        bitCount = 0;
        tempCmd = 0;
    }
    else if( bitCount <= IR_BIT_COUNT )
    {
        // one, zero or error
        char one = CHECK_SYMBOL(delta,LONG);
        char zero = CHECK_SYMBOL(delta,SHORT);
        if( one || zero )
        {
            // good bit of zero or one and we have space to store it
            // shift command and count the bit
            tempCmd <<= 1;
            bitCount ++;
            if( one ) {
                // long interval ==> bit=1
                tempCmd |= 1;
            }
        }
        else // symbol timing error
        {
            // mark bad command
            bitCount = ERROR;
        }
    }

    // store command when all bit are received
    if( bitCount == IR_BIT_COUNT ) {
        irReceivedCommand = tempCmd;
        // mark end of command
        bitCount = ERROR;
    }
}

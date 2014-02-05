/**
 * IR decoder for PROTOCOL TracerJet, SYMA 107G, U810 RC helicopter IR codes.
 * see description of IR codes at the end of the file.
 *
 * @author Michal Krombholz
 * @license Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0)
 * http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Works may contain parts by others and are licencsed by their corresponding, original licenses.
 */

//------------------------------ IR decoder ----------------------------------

#include <stdint.h>

#include "irRcv.h"

// IR protocol "constants" that are filled at the start
// hence they are in "system tick count" their value is only know in run-time
// to avoid their computation each time in an interrupt routine, compute it here
// symbol timing (in "ticks")
uint32_t IR_SYMBOL_START;
uint32_t IR_SYMBOL_ONE;
uint32_t IR_SYMBOL_ZERO;
uint32_t IR_SYMBOL_TOLERANCE;

// number of data bits in a command
const int IR_BIT_COUNT = 32;

// fully received command (gets overwritten by next one if not read)
static volatile uint32_t irReceivedCommand = 0;

volatile uint32_t irRcvErrorCount = 0;


/**
* @brief  initialize protocol parameters dependent on time tick
* @param  ticksPerUs number of tick in 1us
* @retval None
*/
void irRcvInitProtocol(uint32_t ticksPerUs)
{
#ifdef IR_USERIES
    IR_SYMBOL_START = (2000 * ticksPerUs);
    IR_SYMBOL_ONE   = (1200 * ticksPerUs);
    IR_SYMBOL_ZERO  = (800 * ticksPerUs);
    IR_SYMBOL_TOLERANCE = (200 * ticksPerUs);
#else
#ifdef IR_SYMA
    IR_SYMBOL_START = (4000 * ticksPerUs);
    IR_SYMBOL_ONE   = (1000 * ticksPerUs);
    IR_SYMBOL_ZERO  = (600 * ticksPerUs);
    IR_SYMBOL_TOLERANCE = (150 * ticksPerUs);
#else
#ifdef IR_TRACERJET
    IR_SYMBOL_START = (2000 * ticksPerUs);
    IR_SYMBOL_ONE   = (1200 * ticksPerUs);
    IR_SYMBOL_ZERO  = (800 * ticksPerUs);
    IR_SYMBOL_TOLERANCE = (150 * ticksPerUs);
#else
#error "no IR protocol type defined"
#endif
#endif
#endif
    irReceivedCommand = 0;
}

//------------------------------ IR decoder ----------------------------------

#define CHECK_SYMBOL(time,NAME) ((time>IR_SYMBOL_##NAME-IR_SYMBOL_TOLERANCE)&& \
                                 (time<IR_SYMBOL_##NAME+IR_SYMBOL_TOLERANCE))

/**
* @brief return next command received or 0 if none
* @retval IR 33bit command (zero==none)
*/
uint32_t irRcvGetCommand()
{
    uint32_t cmd = irReceivedCommand;
    // clear the command so we know next time if we received anything
    // yes...we may clobber an unread one if the time is right but it's a low possiblitiy
    irReceivedCommand = 0;
    return cmd;
}

/**
 * @brief called from interrupt handler when IR signal burst is detected
 * @param delta - time since last IR edge, in [system ticks]
 */
void irRcvReportIRDetected(uint32_t delta)
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

    // symbol coding: 0 = 0bit; 1 = 1bit; 2 = start; 3 = unrecognized
    int symbol = (CHECK_SYMBOL(delta,ZERO) ? 0 :
                 (CHECK_SYMBOL(delta,ONE)  ? 1 :
                 (CHECK_SYMBOL(delta,START) ? 2 : 3)));

    // classify received symbol based on timing since last edge
    switch( symbol )
    {
        // zero or one
        case 0:
        case 1:
            if( bitCount < IR_BIT_COUNT ) {
                // good bit of zero or one and we have space to store it
                // shift command , set lowest bit and count the bit
                tempCmd <<= 1;
                bitCount ++;
                if( symbol == 1 ) {
                    tempCmd |= 1;
                }
                // store command when all bits are received
                if( bitCount == IR_BIT_COUNT )
                {
                    irReceivedCommand = tempCmd;
                }
            }
            break;

        case 2: // start symbol
            bitCount = 0;
            tempCmd = 0;
            break;

        default: // symbol timing error (could be the gap between commands)
            if( bitCount < IR_BIT_COUNT )
            {
                bitCount = ERROR;
                irRcvErrorCount++;
                //printf("%d ", delta);
            }
            break;
    }
}

//
// Detection Algorithm
//
// The symbols are timed from an edge of IR to the next edge of IR signal detector
// (ie beginning to begining of next IR burst exept for TRACERJET where it's end to the end of next burst)
// Note: the IR detector uses "0" level as "active" (ie IR present) hence the reversed logic.
// Ideally, one would measure the time of IR burst (hence actin on both edges) but one-edge approach saves interrupts.
// The detection algoritm looks for the START symbol, and then expects the bits to follow.
// However, the alg is not perfect as it hinges on receiving and recognizing correct START symbol.
// A possibility exist that wrapping of time ticks would produce a delta time matching start by chance.
// This is however rather unlikely (more unlikely when timer wraparound is rare/longer).
// Also, the alg has no build in timeout and when not all bits are received in one IR command
// (due to interference) it will continue to read bits of next one and eventually err (too many bits).
// Hence it will fail receiving both commands (bad one and next potentially good one).
//
// IR remote protocol description
//
// "Protocol TracerJet Heli"
//
// IR bursts for variable time to mean one of 3 SYMBOLs:
// START, ZERO and ONE.
// The command consist of sequence of:
// leading burst (eqiv to zero bit) then START then 32 bits of data.
// the gap between IR bursts is ~400us
// the gap between commands is about ~170ms
//
// start symbol + 32bit, IR burst off time is 400us
// symbol times of IR burst
// start    = 1600us
// long(1)  =  800us
// short(0) =  400us
//
//
// "SYMA S107C Heli"
//
// The remote pulses IR @38kHz carrier.
// The gap between 300us IR butst means bit 0 or 1.
// The command consist of:
// - preamble (start symbol) 2ms on + 2 ms off = 4000us
// - 32 command bits
//   * zero: ~300us on + 300us off = ~ 600us cycle
//   * one:  ~300us on + 700us off = ~ 1000us cycle
// - final IR burst
//
// "U-series Heli"
//
// note my U810 remote seems awfully flaky with lr/fb being very imprecise&jittering
//
// IR bursts for variable time to mean one of 3 SYMBOLs:
// bit-1 is long, bit-0 is short
// The remote pulses IR @38kHz carrier
// The command consist of:
// - preamble (start) 1600us ms on + 400us off = 2000us
// - 32 command bits:
//   * zero: 800us on + 400us off = ~ 1200us
//   * one:  400us on + 400us off = ~ 800us cycle
// - final IR PULSE
// burst gap of about 400us
// cmd repeats in about 100ms
//
// command encoding
// PWR(cmd) ((cmd >> 24) & 0xFF) // 0..0xC8
// TRM(cmd) ((cmd >> 8) & 0xFF) // 0..0xff
// buttons are in lowest byte (left, right, mode)
// BTNL(cmd)((cmd >> 4) & 0x1)
// BTNR(cmd)((cmd >> 3) & 0x1)
// BTNM(cmd)((cmd >> 2) & 0x1)
// CH(cmd)  ((cmd >> 0) & 0x3) // A=2 B=1 C=0
// SUM(cmd) ((cmd >> 5) & 0x7) // some kind of 3-bit checksum
// FB(cmd) ((cmd >> 20) &0xF) // u2 fwd > 0 ; bkd < 0
// RL(cmd) ((cmd >> 16) &0xF) // u2 lft > 0 ; rht < 0
//

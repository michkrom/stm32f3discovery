#ifndef _IRRCV_H
#define _IRRCV_H
/**
 * IR receiver/decoder for PROTOCOL TracerJet RC codes.
 *
 * @author Michal Krombholz
 * @license Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0)
 * http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Works may contain parts by others and are licencsed by their corresponding, original licenses.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define IR_TRACERJET
//#define IR_SYMA
//#define IR_USERIES

/**
* @brief initialize IR recever hardware & interrupts
* @retval None
* @note this is defined elswhere (in irRcvInput.c)
*/
void irRcvInit();

/**
* @brief return next command received or 0 if none; (destructive read)
* @retval IR 32bit command (zero==none received since last time it was called)
*/
uint32_t irRcvGetCommand();


/**
* @brief global count of IR timing errors
*/
extern volatile uint32_t irRcvErrorCount;

#ifdef IR_TRACERJET

// PROTOCOL TRACERJET's IR command fields

// Power: 0-100 (127?); 128 - off; > 128 dynamic boost (seen 130ish to 228max only)
#define IR_CMD_PWR(cmd) (((cmd) >> 24) & 0xFF)
// Left/Right  (15...1); 8 neutral
#define IR_CMD_LR(cmd)  (((cmd) >> 20) & 0xF)
// Forward/Backward (15...1); 8 neutral
#define IR_CMD_FB(cmd)  (((cmd) >> 16) & 0xF)
// channel 0-A; 2-B; 3-C; (value of 1 is missing on my RC)
#define IR_CMD_CH(cmd)  (((cmd) >> 14) & 0x3)
// 0..63; 6 bits
#define IR_CMD_TRM(cmd) (((cmd) >> 8 ) & 0x3F)
// assume these are "buttons" ie a 0.5ch extra (not physicly present on my RC)
#define IR_CMD_BTNL(cmd) (((cmd) >> 6) & 0x1)
#define IR_CMD_BTNR(cmd) (((cmd) >> 7) & 0x1)
#define IR_CMD_BTNM(cmd) (0)
// checksum
#define IR_CMD_SUM(cmd) ((cmd) & 0x3F)

// compute checksum
#define IR_CMD_COMPUTE_SUM(cmd) (((((cmd)>>24)+(((cmd)>>16) & 0xFF)+(((cmd)>>8) & 0xFF))) & 0x3F)

// validate command
#define IR_CMD_VALID(cmd) ((cmd!=0)&&(IR_CMD_SUM(cmd)==IR_CMD_COMPUTE_SUM(cmd)))


#define IR_EDGE_RISING

#else

#ifdef IR_SYMA

// SYMA S107G IR command fields

//int unk1 = (cmd >> 7) & 0x1; // top bit in trm (unused)
//int unk2 = (cmd >> 31) & 0x1; // top bit in lr (unused)
//int unk3 = (cmd >> 23) & 0x1; // top bit in fb (unused)

// Power: 0-127
#define IR_CMD_PWR(cmd) ((cmd >> 8) & 0x7F)
// Left/Right  0...125 (lt...rt); couples with trim
#define IR_CMD_LR_ORG(cmd)  ((cmd >> 24) & 0xFF)

// note trim offsets the lr when pwr>0
// trim=63 (middle) ==> lr=63 (middle)
// "untrimmed" lr value formula (keeps lr nutral between 61-65 - not perfect)

// Left/Right  0...125 (lt...rt); attempt to uncaple (not ideal)
#define IR_CMD_LR(cmd)  (IR_CMD_LR_ORG(cmd) - ((IR_CMD_TRM(cmd)-63)/3))
// Forward/Backward 0..127 (fwd...bwd)
#define IR_CMD_FB(cmd)  ((cmd >> 16) & 0xFF)
// channel 0-A; 1-B; top of pwd byte
#define IR_CMD_CH(cmd)  ((cmd >> (8+7)) & 0x1)
// 0..127; 7 bits (1..125 on my remote)
#define IR_CMD_TRM(cmd) ((cmd >> 0) & 0x7F)

// are there SYMA RCs with buttons?
#define IR_CMD_BTNL(cmd) (0)
#define IR_CMD_BTNR(cmd) (0)
#define IR_CMD_BTNM(cmd) (0)

// no checksum in SYMA's cmd
// checksum
#define IR_CMD_SUM(cmd) (0)
// compute checksum
#define IR_CMD_COMPUTE_SUM(cmd) (0)
// validate command
#define IR_CMD_VALID(cmd) (cmd!=0)

#define IR_EDGE_FALLING

#else

#ifdef IR_USERIES

#define IR_CMD_PWR(cmd) ((cmd >> 24) & 0xFF) // 0..0xC8
#define IR_CMD_FB(cmd)  ((cmd >> 20) &0xF) // u2 fwd > 0 ; bkd < 0
#define IR_CMD_RL(cmd)  ((cmd >> 16) &0xF) // u2 lft > 0 ; rht < 0
#define IR_CMD_TRM(cmd) ((cmd >> 8) & 0xFF) // 0..0xff

// buttons are in lowest byte (left, right, mode)
#define IR_CMD_BTNL(cmd)((cmd >> 4) & 0x1)
#define IR_CMD_BTNR(cmd)((cmd >> 3) & 0x1)
#define IR_CMD_BTNM(cmd)((cmd >> 2) & 0x1)

#define IR_CMD_CH(cmd)  ((cmd >> 0) & 0x3) // A=2 B=1 C=0

// TODO: there is a 3-bit checksum but I do not care
#define IR_CMD_SUM(cmd) ((cmd >> 5) & 0x7) // some kind of 3-bit checksum
// compute checksum
#define IR_CMD_COMPUTE_SUM(cmd) (0)
// validate command
#define IR_CMD_VALID(cmd) (cmd!=0)

#define IR_EDGE_FALLING

#endif

#endif

#endif


// should be called from the interrupt when edge of IR is detected
void irRcvReportIRDetected(uint32_t delta);


#ifdef __cplusplus
}
#endif

#endif // _IRRCV_H

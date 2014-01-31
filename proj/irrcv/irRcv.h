#ifndef _IRRCV_H
#define _IRRCV_H
/**
 * IR receiver/decoder for PROTOCOL TracerJet RC codes.
 *
 * @author Michal Krombholz
 * @license creative commons attibution
 */

/**
* @brief initialize IR recever hardware & interrupts
* @retval None
*/
void irRcvInit();

/**
* @brief return next command received or 0 if none; (destructive read)
* @retval IR 32bit command (zero==none received since last time it was called)
*/
uint32_t irRcvGetCommand();

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
// assume these are "buttons" ie a 0.5ch extra (not present on my RC)
#define IR_CMD_BTN(cmd) (((cmd) >> 6) & 0x3)
// checksum
#define IR_CMD_SUM(cmd) ((cmd) & 0x3F)

// compute checksum
#define IR_CMD_COMPUTE_SUM(cmd) (((((cmd)>>24)+(((cmd)>>16) & 0xFF)+(((cmd)>>8) & 0xFF))) & 0x3F)

// validate command
#define IR_CMD_VALID(cmd) ((cmd!=0)&&(IR_CMD_SUM(cmd)==IR_CMD_COMPUTE_SUM(cmd)))


#endif

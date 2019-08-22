/**
 * @file	lpc17xx_can_hlp.h
 * @Author	turgay (turgay@dtsis.com)
 * @date	20131002
 * @brief	lpc17xx can bus init, rx, tx message olarak kullanim.
 *
 * 	Acceptance Filter Section Table structure (AF_SectionDef) desteklenmiyor.
 * (FullCAN, Standard ID Frame Format, Group of Standard ID Frame Format, Extended ID Frame Format, Group of Extended ID Frame Format)
 *
 */

#ifndef LPC17XX_CAN_HLP_H
#define LPC17XX_CAN_HLP_H	1

#include "lpc17xx_libcfg_default.h"

#ifdef _CAN

#include "lpc17xx_can.h"

/**
 * @breaf	initialize can1
 * @param	baudrate: 125000 / 500000 vs
 * @param	rxbufsize 5/10/20 gibi
 * @param	txbufsize 5/10/20 gibi
 * @return	0 on success
 */
extern int can1_init(uint32_t baudrate, uint8_t rxbufsize, uint8_t txbufsize);

extern void can1_release(void);
extern int can1_putmsg(CAN_MSG_Type *msg);
extern int can1_getmsg(CAN_MSG_Type *msg);
extern void sendKeyArrayCanMessage(int can_msg_id, int* key_array);
extern void canReinit(void);

#endif //#ifdef _CAN

#endif //#ifndef LPC17XX_CAN_HLP_H
/* eof */

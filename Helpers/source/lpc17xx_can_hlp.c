#include "lpc17xx_can_hlp.h"
#ifdef _CAN

#include "lpc17xx_can.h"
#include <stdint.h>
#include <string.h>
#include "ringbuf.h"
#include <stdlib.h>	/* malloc() */
#include "macros.h"

#include <stdio.h>	/* pritnf() */
#define dlogt(...)	printf(__VA_ARGS__)
#define dpoint(...)	printf(__VA_ARGS__)

#define FREE_AND_NULL(a)	do{ free(a); (a) = NULL; } while(0)

static RB_TYPE *can1_rxbuf = NULL;
static RB_TYPE *can1_txbuf = NULL;
//static RB_TYPE *can2_rxbuf;


/**
 * @breaf	initialize can1
 * @param	baudrate: 125000 / 500000 vs
 * @param	rxbufsize 5/10/20 gibi
 * @param	txbufsize 5/10/20 gibi
 * @return	0 on success
 */
int can1_init(uint32_t baudrate, uint8_t rxbufsize, uint8_t txbufsize)
{
	if (can1_rxbuf)
		return -1;	/* already init */

	can1_rxbuf = malloc(sizeof(RB_TYPE));
	if (can1_rxbuf == NULL)
		return -2;	/* out of memory */

	can1_txbuf = malloc(sizeof(RB_TYPE));
	if (can1_txbuf == NULL) {
		FREE_AND_NULL(can1_rxbuf);
		return -2;	/* out of memory */
	}

	if (rb_init(can1_rxbuf, sizeof(CAN_MSG_Type), rxbufsize) != RB_OK) {
		FREE_AND_NULL(can1_rxbuf);
		FREE_AND_NULL(can1_txbuf);
		return -2;	/* out of memory */
	}

	if (rb_init(can1_txbuf, sizeof(CAN_MSG_Type), txbufsize) != RB_OK) {
		rb_free(can1_rxbuf);
		FREE_AND_NULL(can1_rxbuf);
		FREE_AND_NULL(can1_txbuf);
		return -2;	/* out of memory */
	}

	//Initialize CAN1
	CAN_Init(LPC_CAN1, baudrate);

	//Enable Interrupt
	CAN_IRQCmd(LPC_CAN1, CANINT_RIE, ENABLE);
	CAN_IRQCmd(LPC_CAN1, CANINT_TIE1, ENABLE);

	CAN_SetAFMode(LPC_CANAF,CAN_AccBP);
	//CAN_InitMessage();

	//Enable CAN Interrupt
	NVIC_EnableIRQ(CAN_IRQn);

	return 0;
}

void can1_release(void)
{
	if (!can1_rxbuf)
		return;	/* already released / not init */

	CAN_DeInit(LPC_CAN1);

	rb_free(can1_rxbuf);
	rb_free(can1_txbuf);
	FREE_AND_NULL(can1_rxbuf);
	FREE_AND_NULL(can1_txbuf);
}

int can1_putmsg(CAN_MSG_Type *msg)
{
	int res = -1;

	if (ISSET(LPC_CAN1->SR, CAN_GSR_TBS)) {
		/* if TBS is 1 All three Transmit Buffers are available */
		if (rb_elements_count(can1_txbuf) > 0) {
			dpoint("faulty state, internal error!\n");
		}
		if (CAN_SendMsg(LPC_CAN1, msg) == SUCCESS)
			res = 0;
	} else {
		CAN_IRQCmd(LPC_CAN1, CANINT_TIE1, DISABLE);
		if (rb_write_1_element(can1_txbuf, (uint8_t*)msg) == RB_OK)
			res = 0;
		CAN_IRQCmd(LPC_CAN1, CANINT_TIE1, ENABLE);
	}
	return res;
}

int can1_getmsg(CAN_MSG_Type *msg)
{
	int res;
	CAN_IRQCmd(LPC_CAN1, CANINT_RIE, DISABLE);
	res = ((rb_xfer_1_element(can1_rxbuf, (uint8_t*)msg) == RB_OK) ? 0 : -1);
	CAN_IRQCmd(LPC_CAN1, CANINT_RIE, ENABLE);
	return res;
}

void sendKeyArrayCanMessage(int can_msg_id, int* key_array)
{
	CAN_MSG_Type canmsg;
	memset(&canmsg, 0,sizeof(CAN_MSG_Type));
	canmsg.id 	= can_msg_id;
	canmsg.format 	= STD_ID_FORMAT;
	canmsg.type 	= DATA_FRAME;
	canmsg.len	= 8;
	canmsg.dataA[0] = key_array[0];
	canmsg.dataA[1] = key_array[1];
	canmsg.dataA[2] = key_array[2];
	canmsg.dataA[3] = key_array[3];
	canmsg.dataB[0] = key_array[4];
	canmsg.dataB[1] = key_array[5];
	canmsg.dataB[2] = 0;
	canmsg.dataB[3] = 0;
	can1_putmsg(&canmsg);
	memset(&canmsg, 0,sizeof(CAN_MSG_Type));
}

/*********************************************************************//**
 * @brief	CAN reinit function.
 **********************************************************************/
void canReinit(void)
{
	printf("Relase CAN\n");
	can1_release();
	for(int i = 0; i<500000;i++)
	{
	}
	printf("Reinit CAN\n");
	can1_init(125000, 10, 10);
}

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief	CAN_IRQ Handler, control receive message operation
 * param[in]	none
 * @return 	none
 **********************************************************************/
void CAN_IRQHandler()
{
	uint8_t IntStatus;
	CAN_MSG_Type msg;

	/* CAN1 */

	/* get interrupt status
	 * Note that: Interrupt register CANICR will be reset after read.
	 * So function "CAN_IntGetStatus" should be call only one time
	 */
	IntStatus = CAN_IntGetStatus(LPC_CAN1);

	//check receive interrupt
	if((IntStatus>>0)&0x01) {
		CAN_ReceiveMsg(LPC_CAN1, &msg);
		rb_write_1_element(can1_rxbuf, (uint8_t*)&msg);
	}

	if ((IntStatus>>0)&0x02) {
		/* ready for tx */
		if (rb_xfer_1_element(can1_txbuf, (uint8_t*)&msg) == RB_OK) {
			CAN_SendMsg(LPC_CAN1, &msg);
		}
	}

	/*! CAN1 */

	/* CAN2 */
	/* TODO: can2 islemleri...
	 * yukaridakiler ile ayni farki deskenler.
	 */
	/*! CAN2 */

}

#endif //#ifdef _CAN
/* eof */

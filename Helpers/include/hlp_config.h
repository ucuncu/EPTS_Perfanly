
#ifndef _HLP_CONFIG_H_
#define _HLP_CONFIG_H_


#define HLP_CONFIG_UARTS_ENABLE        0x09    // uart 0 uart 3 enable
// </h>
// <h>RS232 / RS485
// <o> UART0 <0=> RS232 <2=> RS485
#define HLP_CONFIG_UARTS_MODE_0        0
// <o> UART1 <0=> RS232 <2=> RS485
#define HLP_CONFIG_UARTS_MODE_1        0
// <o> UART2 <0=> RS232 <2=> RS485
#define HLP_CONFIG_UARTS_MODE_2        0
// <o> UART3 <0=> RS232 <2=> RS485
#define HLP_CONFIG_UARTS_MODE_3        0
// </h>

//#define HLP_CONFIG_UARTS_0_SAM_TX_FUNC      events_sam_uart_0_tx()
//#define HLP_CONFIG_UARTS_0_SAM_RX_FUNC      events_sam_uart_0_rx()
// 
//#define HLP_CONFIG_UARTS_1_SAM_TX_FUNC      events_sam_uart_1_tx()
//#define HLP_CONFIG_UARTS_1_SAM_RX_FUNC      events_sam_uart_1_rx()

//#define HLP_CONFIG_UARTS_2_SAM_TX_FUNC      events_sam_uart_2_tx()
//#define HLP_CONFIG_UARTS_2_SAM_RX_FUNC      events_sam_uart_2_rx()
// 
//#define HLP_CONFIG_UARTS_3_SAM_TX_FUNC      events_sam_uart_3_tx()
//#define HLP_CONFIG_UARTS_3_SAM_RX_FUNC      events_sam_uart_3_rx()
//   
//#define HLP_CONFIG_UARTS_0_485_TX_FUNC      events_uart_0_485_rx_clr()
//#define HLP_CONFIG_UARTS_0_485_RX_FUNC      events_uart_0_485_rx_set()

//#define HLP_CONFIG_UARTS_1_485_TX_FUNC      events_uart_1_485_rx_clr()
//#define HLP_CONFIG_UARTS_1_485_RX_FUNC      events_uart_1_485_rx_set()

//#define HLP_CONFIG_UARTS_2_485_TX_FUNC      events_uart_2_485_rx_clr()
//#define HLP_CONFIG_UARTS_2_485_RX_FUNC      events_uart_2_485_rx_set()

//#define HLP_CONFIG_UARTS_3_485_TX_FUNC      events_uart_3_485_rx_clr()
//#define HLP_CONFIG_UARTS_3_485_RX_FUNC      events_uart_3_485_rx_set()

#define HLP_CONFIG_UARTS_0_TX_BUF_CAPACITY  (EVENTS_BUF_TX_0_LEN)
#define HLP_CONFIG_UARTS_0_TX_BUF_BUFFER    &_events_buf_tx_0[0]

#define HLP_CONFIG_UARTS_0_RX_BUF_CAPACITY  (EVENTS_BUF_RX_0_LEN)
#define HLP_CONFIG_UARTS_0_RX_BUF_BUFFER    &_events_buf_rx_0[0]

//#define HLP_CONFIG_UARTS_1_TX_BUF_CAPACITY  (EVENTS_BUF_TX_1_LEN)
//#define HLP_CONFIG_UARTS_1_TX_BUF_BUFFER    &_events_buf_tx_1[0]

//#define HLP_CONFIG_UARTS_1_RX_BUF_CAPACITY  (EVENTS_BUF_RX_1_LEN)
//#define HLP_CONFIG_UARTS_1_RX_BUF_BUFFER    &_events_buf_rx_1[0]

//#define HLP_CONFIG_UARTS_2_TX_BUF_CAPACITY  (EVENTS_BUF_TX_2_LEN)
//#define HLP_CONFIG_UARTS_2_TX_BUF_BUFFER    &_events_buf_tx_2[0]

//#define HLP_CONFIG_UARTS_2_RX_BUF_CAPACITY  (EVENTS_BUF_RX_2_LEN)
//#define HLP_CONFIG_UARTS_2_RX_BUF_BUFFER    &_events_buf_rx_2[0]

#define HLP_CONFIG_UARTS_3_TX_BUF_CAPACITY  (EVENTS_BUF_TX_3_LEN)
#define HLP_CONFIG_UARTS_3_TX_BUF_BUFFER    &_events_buf_tx_3[0]

#define HLP_CONFIG_UARTS_3_RX_BUF_CAPACITY  (EVENTS_BUF_RX_3_LEN)
#define HLP_CONFIG_UARTS_3_RX_BUF_BUFFER    &_events_buf_rx_3[0]
/* End of UART*/


#endif //#ifndef _HLP_CONFIG_H_

/***************************************************************************************************
 * END OF FILE
 **************************************************************************************************/

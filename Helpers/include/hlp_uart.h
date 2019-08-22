

#ifndef _HLP_UART_H_
#define _HLP_UART_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
/* include file(s) */
#include "hlp_type.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/* definitions */

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//// <e>Sample
//// <i>Örnek olmasý için eklenmiþtir.
//// ===========================
//#define SAMPLE     0
//// </e>

//------------- <<< end of configuration section >>> -----------------------
#define HLP_INTERNAL_ERROR(uart_no,err)   

////////////////////////////////////////////////////////////////////////////////////////////////////
/* types */
typedef __packed struct
{
    U16 head;
    U16 tail;
    U16 capacity;
    U8 *buffer;        
} THelpBuffer;

typedef __packed struct
{    
    THelpBuffer Tx;
    THelpBuffer Rx;
} THelperUartBuffer;


typedef __packed struct 
{
    U8 status; /* tx enable, disable */
    
    THelperUartBuffer buffer[4];
} THelpUart;

////////////////////////////////////////////////////////////////////////////////////////////////////
/* export functions */

/* DO NOT CHANGE INTERRUPT FUNCTION NAMES !!!*/
extern void hlp_uart_init(void);

extern void hlp_uart_init_port(U8 uart_no,
                               U32 baudrate,
                               U8 parity);

extern void UART0_IRQHandler(void);

extern void UART1_IRQHandler(void);

extern void UART2_IRQHandler(void);

extern void UART3_IRQHandler(void);

extern int UART0_PutChar (int c);
extern int UART0_GetChar (void);
extern void UART0_wait(void);
extern void UART0_PutCharWait(int ch);
extern U8 UART0_is_tx_empty(void);                             
extern U8 UART0_is_rx_empty(void);

extern int UART1_PutChar (int c);
extern int UART1_GetChar (void);
extern void UART1_wait(void);
extern void UART1_PutCharWait(int ch);
extern U8 UART1_is_tx_empty(void);                             
extern U8 UART1_is_rx_empty(void);

extern int UART2_PutChar (int c);
extern int UART2_GetChar (void);
extern void UART2_wait(void);
extern void UART2_PutCharWait(int ch);
extern U8 UART2_is_tx_empty(void);                             
extern U8 UART2_is_rx_empty(void);

extern int UART3_PutChar (int c);
extern int UART3_GetChar (void);
extern void UART3_wait(void);
extern void UART3_PutCharWait(int ch);
extern U8 UART3_is_tx_empty(void);                             
extern U8 UART3_is_rx_empty(void);


////////////////////////////////////////////////////////////////////////////////////////////////////
/* export variables */

#endif //#ifndef _HLP_UART_H_

/***************************************************************************************************
 * END OF FILE
 **************************************************************************************************/

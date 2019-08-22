/***************************************************************************************************
 * filename     : hlp_uart.c
 * description  : To use easyly LPC17XX CMSIS library

 **************************************************************************************************/

/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////
/* include files */
#include "lpc17xx_uart.h"
#include "lpc17xx_libcfg.h" 
#include "lpc17xx_libcfg_default.h"
#include "hlp_uart.h"
#include "hlp_type.h"
#include "hlp_config.h"
#include "hlp_vic.h"
#include "flags.h"
#include <stdio.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
/* extern calls */
int RecDataToGPSbuffer( uint8_t GpsDatChar);
void testing_loop(int);
////////////////////////////////////////////////////////////////////////////////////////////////////
/* local (static) types, definitions */

#define HLP_UART_PARITY_NONE    0
#define HLP_UART_PARITY_ODD     1
#define HLP_UART_PARITY_EVEN    2
#define HLP_UART_PARITY_SP_1    3
#define HLP_UART_PARITY_SP_0    4

#define hlp_set_tx_stat(X)   _hlp_uart.status |= (1<<X)
#define hlp_clr_tx_stat(X)   _hlp_uart.status &= ~(1<<X)
#define hlp_get_tx_stat(X)   (_hlp_uart.status & (1<<X))

#define __HLP_BUF_IS_EMPTY(X)   (X.head == X.tail)
#define __HLP_BUF_IS_FULL(X)    ((X.tail % X.capacity) == ((X.head+1) % X.capacity))
#define __HLP_BUF_INCR_H(X)     X.head = ((X.head+1) % X.capacity)
#define __HLP_BUF_INCR_T(X)     X.tail = ((X.tail+1) % X.capacity)
 
 
//..................................................................................................
/* SAM card ayarlarý */           
       
#define UART0_ENABLED       (1<<0)
#define UART1_ENABLED       (1<<1)
#define UART2_ENABLED       (1<<2)
#define UART3_ENABLED       (1<<3)   


#define UART_RS232                  0
#define UART_RS485                  2



////////////////////////////////////////////////////////////////////////////////////////////////////
/* local (static) functions */
static void hlp_uart_IntReceive(LPC_UART_TypeDef *this,U8 uart_no);

static void hlp_uart_IntTransmit(LPC_UART_TypeDef *this,U8 uart_no);

static LPC_UART_TypeDef *hlp_uart_get_struct(U8 uart_no);

////////////////////////////////////////////////////////////////////////////////////////////////////
/* global variables */
static THelpUart _hlp_uart;

////////////////////////////////////////////////////////////////////////////////////////////////////
/* export variables */
#define EVENTS_BUF_TX_0_LEN   256
#define EVENTS_BUF_RX_0_LEN   128
                                
                              
#define EVENTS_BUF_TX_3_LEN   512
#define EVENTS_BUF_RX_3_LEN   512

/* for uart buffers */
U8 _events_buf_tx_0[EVENTS_BUF_TX_0_LEN];
U8 _events_buf_rx_0[EVENTS_BUF_RX_0_LEN];


U8 _events_buf_tx_3[EVENTS_BUF_TX_3_LEN];
U8 _events_buf_rx_3[EVENTS_BUF_RX_3_LEN];

//BLOCKING, NONE_BLOCKING
#define UART_BLOCKING_TYPE					NONE_BLOCKING

/***************************************************************************************************
 * IMPLEMENTATIONS
 **************************************************************************************************/

/********************************************************************//**
 * @brief 		UART receive function (ring buffer used)
 * @param[in]	None
 * @return 		None
 *********************************************************************/
void hlp_uart_IntReceive(LPC_UART_TypeDef *this,U8 uart_no)
{
	int i,j;
	uint8_t tmpc;
	uint32_t rLen;

	for (i=0; i<16; i++) //UART_FIFO_TRGLEV3 == 14 char
	{
		rLen = UART_Receive(this, &tmpc, 1, UART_BLOCKING_TYPE);	// Call UART read function in UART driver
		if (!rLen) break;

		/* Check if buffer is more space, If no more space, remaining character will be trimmed out */
		if ( !__HLP_BUF_IS_FULL(_hlp_uart.buffer[uart_no].Rx) )
		{
			_hlp_uart.buffer[uart_no].Rx.buffer[_hlp_uart.buffer[uart_no].Rx.head] = tmpc;
			__HLP_BUF_INCR_H(_hlp_uart.buffer[uart_no].Rx);
		}
		if ( uart_no == 0 )
		{	
			testing_loop(tmpc);
			if(fUart3to0Bride)
			{
				UART3_PutChar(tmpc);
			}
		}
		if ( uart_no == 3 )
		{
			if(fUart3to0Bride)
			{
				if (fInitGPS == FALSE)
				{
					printf("%c", tmpc);
				}
//				else
//				{
//					if (tmpc == 0xB5)
//					{
//						printf("\n\n\n\n");
//					}
//					printf("%x ", tmpc);
//				}
			}
			else
			{
				j=RecDataToGPSbuffer(tmpc);
				if (( j == -1)&&(fEnableDebugLog))
				{
					printf("ERROR-RecDataToGPSbuffer failed");
				}
			}
		}
	}	
}

/********************************************************************//**
 * @brief 		UART transmit function (ring buffer used)
 * @param[in]	None
 * @return 		None
 *********************************************************************/
void hlp_uart_IntTransmit(LPC_UART_TypeDef *this,U8 uart_no)
{
    // Disable THRE interrupt
    UART_IntConfig(this, UART_INTCFG_THRE, DISABLE);

	/* Wait for FIFO buffer empty, transfer UART_TX_FIFO_SIZE bytes
	 * of data or break whenever ring buffers are empty */
	/* Wait until THR empty */
    //while (UART_CheckBusy(this) == SET);
    

	while (!__HLP_BUF_IS_EMPTY(_hlp_uart.buffer[uart_no].Tx))
    {
        /* Move a piece of data into the transmit FIFO */
    	if (UART_Send(this, (uint8_t *)&_hlp_uart.buffer[uart_no].Tx.buffer[_hlp_uart.buffer[uart_no].Tx.tail], 1, UART_BLOCKING_TYPE))
        {
            /* Update transmit ring FIFO tail pointer */
            __HLP_BUF_INCR_T(_hlp_uart.buffer[uart_no].Tx);
    	} else {
    		break;
    	}
    }

    /* If there is no more data to send, disable the transmit
       interrupt - else enable it or keep it enabled */
	if (__HLP_BUF_IS_EMPTY(_hlp_uart.buffer[uart_no].Tx)) 
    {
    	UART_IntConfig(this, UART_INTCFG_THRE, DISABLE);
    	// Reset Tx Interrupt state
    	hlp_clr_tx_stat(uart_no);

        switch(uart_no)
        {
            case 0: 
#if (HLP_CONFIG_UARTS_ENABLE & UART0_ENABLED) && (HLP_CONFIG_UARTS_MODE_0 == UART_RS485)
                HLP_CONFIG_UARTS_0_485_RX_FUNC;  /* tx için hazýrlayalým */
#endif 
#if (HLP_CONFIG_UARTS_ENABLE & UART0_ENABLED) && (HLP_CONFIG_UARTS_0_SAM)         
                HLP_CONFIG_UARTS_0_SAM_RX_FUNC; 
#endif     
            break;
            case 1: 
#if (HLP_CONFIG_UARTS_ENABLE & UART1_ENABLED) && (HLP_CONFIG_UARTS_MODE_1 == UART_RS485)
                HLP_CONFIG_UARTS_1_485_RX_FUNC;  /* tx için hazýrlayalým */
#endif     
#if (HLP_CONFIG_UARTS_ENABLE & UART1_ENABLED) && (HLP_CONFIG_UARTS_1_SAM)         
                HLP_CONFIG_UARTS_1_SAM_RX_FUNC; 
#endif    
            break;
            case 2: 
#if (HLP_CONFIG_UARTS_ENABLE & UART2_ENABLED) && (HLP_CONFIG_UARTS_MODE_2 == UART_RS485)
                HLP_CONFIG_UARTS_2_485_RX_FUNC;  /* tx için hazýrlayalým */
#endif 
#if (HLP_CONFIG_UARTS_ENABLE & UART2_ENABLED) && (HLP_CONFIG_UARTS_2_SAM)         
                HLP_CONFIG_UARTS_2_SAM_RX_FUNC; 
#endif    
            break;
            default: 
#if (HLP_CONFIG_UARTS_ENABLE & UART3_ENABLED) && (HLP_CONFIG_UARTS_MODE_3 == UART_RS485)
                HLP_CONFIG_UARTS_3_485_RX_FUNC;  /* tx için hazýrlayalým */
#endif 
#if (HLP_CONFIG_UARTS_ENABLE & UART3_ENABLED) && (HLP_CONFIG_UARTS_3_SAM)         
                HLP_CONFIG_UARTS_3_SAM_RX_FUNC; 
#endif      
            break;
        }

    }
    else{
      	// Set Tx Interrupt state
    	hlp_set_tx_stat(uart_no);
    	UART_IntConfig(this, UART_INTCFG_THRE, ENABLE);
    }
} 

LPC_UART_TypeDef *hlp_uart_get_struct(U8 uart_no)
{
    switch(uart_no) 
    {
        case 0:
            return LPC_UART0;
        case 1:
            return (LPC_UART_TypeDef *)LPC_UART1;
        case 2:
            return LPC_UART2;
        default:
            return LPC_UART3;
    }   
}

void hlp_uart_init_port(U8 uart_no,
                        U32 baudrate,
                        U8 parity)
{
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
    
    LPC_UART_TypeDef *uart_struct;



    if(((uart_no == 0) && (!(HLP_CONFIG_UARTS_ENABLE & UART0_ENABLED))) ||
       ((uart_no == 1) && (!(HLP_CONFIG_UARTS_ENABLE & UART1_ENABLED))) ||
       ((uart_no == 2) && (!(HLP_CONFIG_UARTS_ENABLE & UART2_ENABLED))) ||
       ((uart_no == 3) && (!(HLP_CONFIG_UARTS_ENABLE & UART3_ENABLED))) ||
       (uart_no > 3))
    {
        /* port is not active
         *
         * */
        return;
    }


    uart_struct = hlp_uart_get_struct(uart_no);  

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);  
    UARTConfigStruct.Baud_rate = baudrate;

    switch(parity)
    {
        case HLP_UART_PARITY_ODD:           
            UARTConfigStruct.Parity = UART_PARITY_ODD;
        break;
        case HLP_UART_PARITY_EVEN:           
            UARTConfigStruct.Parity = UART_PARITY_EVEN;
        break;
        case HLP_UART_PARITY_SP_1:           
            UARTConfigStruct.Parity = UART_PARITY_SP_1;
        break;
        case HLP_UART_PARITY_SP_0:           
            UARTConfigStruct.Parity = UART_PARITY_SP_0;
        break; 
        default:           
            UARTConfigStruct.Parity = UART_PARITY_NONE;
        break;
    }
    
	// Initialize UART0 peripheral with given to corresponding parameter
	UART_Init(uart_struct, &UARTConfigStruct);  
    
	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);  
    UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV3;
	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig(uart_struct, &UARTFIFOConfigStruct); 

    hlp_clr_tx_stat(uart_no);
    
    _hlp_uart.buffer[uart_no].Tx.head = 0;
    _hlp_uart.buffer[uart_no].Tx.tail = 0;
    
    _hlp_uart.buffer[uart_no].Rx.head = 0;
    _hlp_uart.buffer[uart_no].Rx.tail = 0;   
    
	// Enable UART Transmit
	UART_TxCmd(uart_struct, ENABLE);

    /* Enable UART Rx interrupt */
	UART_IntConfig(uart_struct, UART_INTCFG_RBR, ENABLE);
	/* Enable UART line status interrupt */
	UART_IntConfig(uart_struct, UART_INTCFG_RLS, ENABLE);

    
    switch(uart_no)
    {
        case 0:  
            hlp_vic_set(HLP_VIC_INT_UART0_IRQHandler, (U32)UART0_IRQHandler);
        break;
        case 1:  
            hlp_vic_set(HLP_VIC_INT_UART1_IRQHandler, (U32)UART1_IRQHandler);
        break;
        case 2:  
            hlp_vic_set(HLP_VIC_INT_UART2_IRQHandler, (U32)UART2_IRQHandler);
        break;
        default: 
            hlp_vic_set(HLP_VIC_INT_UART3_IRQHandler, (U32)UART3_IRQHandler); 
        break;
    }
                     
}


/*********************************************************************//**
 * @brief		Helper initialize
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void hlp_uart_init(void)
{
    _hlp_uart.status = 0;

#if HLP_CONFIG_UARTS_ENABLE & UART0_ENABLED
    _hlp_uart.buffer[0].Tx.head = 0;
    _hlp_uart.buffer[0].Tx.tail = 0;
    _hlp_uart.buffer[0].Tx.capacity = HLP_CONFIG_UARTS_0_TX_BUF_CAPACITY;
    _hlp_uart.buffer[0].Tx.buffer = HLP_CONFIG_UARTS_0_TX_BUF_BUFFER;

    _hlp_uart.buffer[0].Rx.head = 0;
    _hlp_uart.buffer[0].Rx.tail = 0;
    _hlp_uart.buffer[0].Rx.capacity = HLP_CONFIG_UARTS_0_RX_BUF_CAPACITY;
    _hlp_uart.buffer[0].Rx.buffer = HLP_CONFIG_UARTS_0_RX_BUF_BUFFER;
#endif

#if HLP_CONFIG_UARTS_ENABLE & UART1_ENABLED
    _hlp_uart.buffer[1].Tx.head = 0;
    _hlp_uart.buffer[1].Tx.tail = 0;
    _hlp_uart.buffer[1].Tx.capacity = HLP_CONFIG_UARTS_1_TX_BUF_CAPACITY;
    _hlp_uart.buffer[1].Tx.buffer = HLP_CONFIG_UARTS_1_TX_BUF_BUFFER;

    _hlp_uart.buffer[1].Rx.head = 0;
    _hlp_uart.buffer[1].Rx.tail = 0;
    _hlp_uart.buffer[1].Rx.capacity = HLP_CONFIG_UARTS_1_RX_BUF_CAPACITY;
    _hlp_uart.buffer[1].Rx.buffer = HLP_CONFIG_UARTS_1_RX_BUF_BUFFER;
#endif

#if HLP_CONFIG_UARTS_ENABLE & UART2_ENABLED
    _hlp_uart.buffer[2].Tx.head = 0;
    _hlp_uart.buffer[2].Tx.tail = 0;
    _hlp_uart.buffer[2].Tx.capacity = HLP_CONFIG_UARTS_2_TX_BUF_CAPACITY;
    _hlp_uart.buffer[2].Tx.buffer = HLP_CONFIG_UARTS_2_TX_BUF_BUFFER;

    _hlp_uart.buffer[2].Rx.head = 0;
    _hlp_uart.buffer[2].Rx.tail = 0;
    _hlp_uart.buffer[2].Rx.capacity = HLP_CONFIG_UARTS_2_RX_BUF_CAPACITY;
    _hlp_uart.buffer[2].Rx.buffer = HLP_CONFIG_UARTS_2_RX_BUF_BUFFER;
#endif

#if HLP_CONFIG_UARTS_ENABLE & UART3_ENABLED
    _hlp_uart.buffer[3].Tx.head = 0;
    _hlp_uart.buffer[3].Tx.tail = 0;
    _hlp_uart.buffer[3].Tx.capacity = HLP_CONFIG_UARTS_3_TX_BUF_CAPACITY;
    _hlp_uart.buffer[3].Tx.buffer = HLP_CONFIG_UARTS_3_TX_BUF_BUFFER;

    _hlp_uart.buffer[3].Rx.head = 0;
    _hlp_uart.buffer[3].Rx.tail = 0;
    _hlp_uart.buffer[3].Rx.capacity = HLP_CONFIG_UARTS_3_RX_BUF_CAPACITY;
    _hlp_uart.buffer[3].Rx.buffer = HLP_CONFIG_UARTS_3_RX_BUF_BUFFER;
#endif

}

#if (HLP_CONFIG_UARTS_ENABLE & UART0_ENABLED)

#define F 0
#define A _hlp_uart.buffer[F].Tx.head
#define B _hlp_uart.buffer[F].Tx.tail
#define C _hlp_uart.buffer[F].Tx.capacity
#define D _hlp_uart.buffer[F].Tx.buffer
#define G _hlp_uart.buffer[F].Rx.head
#define H _hlp_uart.buffer[F].Rx.tail
#define I _hlp_uart.buffer[F].Rx.capacity
#define J _hlp_uart.buffer[F].Rx.buffer 

int UART0_PutChar (int c)
{
	U32 n;

	/* Temporarily lock out UART transmit interrupts during this
	   read so the UART transmit interrupt won't cause problems
	   with the index values */
    UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, DISABLE);


    /* calculate the bytes in buffer */
	if (A == B)	
    {
		n = 0;
	}
	else
	if (A > B) 
    {
		n = (A - B);
	}
	else 
    {
		n = C - B + A;
	}

	if (n == (C-1))
    {
        UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, ENABLE);
        return (-1);
    }

	D [A] = c;  
    A =  (++A) % C;

	if (!hlp_get_tx_stat(F)) 
    {
#if HLP_CONFIG_UARTS_MODE_0 == UART_RS485
        HLP_CONFIG_UARTS_0_485_TX_FUNC;  /* tx için hazýrlayalým */
#endif   
#if HLP_CONFIG_UARTS_0_SAM 
        HLP_CONFIG_UARTS_0_SAM_TX_FUNC;
#endif

#if (!HLP_CONFIG_UART_0_INT_ENABLE)
        while (UART_CheckBusy(LPC_UART0) == SET);
#endif

        hlp_uart_IntTransmit(LPC_UART0,F);
  	}
    else
    {
        UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, ENABLE);    
    }    

	return (0);
}

int UART0_GetChar (void)
{
    int a;

	if (G == H)
    {
        /* buffer boþ
         *
         * */
        G = 0;
        H = 0;
	  	return (-1);
    }

    a = J [H];
    H =  (++H) % I;

    return a;
}

void UART0_wait(void)
{
    while((A != B))
    {
        hlp_uart_IntTransmit(LPC_UART0,F);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UART0_PutCharWait(int ch)
{
    UART0_PutChar(ch);
    UART0_wait();
} //void UART0_PutCharWait(int ch)

U8 UART0_is_tx_empty(void)
{
    return (A == B);    
}

U8 UART0_is_rx_empty(void)
{
    return (G == H);   
}

#undef A
#undef B
#undef C
#undef D
#undef F
#undef G
#undef H
#undef I
#undef J 

#endif

#if (HLP_CONFIG_UARTS_ENABLE & UART1_ENABLED)


#define F 1
#define A _hlp_uart.buffer[F].Tx.head
#define B _hlp_uart.buffer[F].Tx.tail
#define C _hlp_uart.buffer[F].Tx.capacity
#define D _hlp_uart.buffer[F].Tx.buffer
#define G _hlp_uart.buffer[F].Rx.head
#define H _hlp_uart.buffer[F].Rx.tail
#define I _hlp_uart.buffer[F].Rx.capacity
#define J _hlp_uart.buffer[F].Rx.buffer 

int UART1_PutChar (int c)
{
	U32 n;

	/* Temporarily lock out UART transmit interrupts during this
	   read so the UART transmit interrupt won't cause problems
	   with the index values */
    UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_THRE, DISABLE);


    /* calculate the bytes in buffer */
	if (A == B)	
    {
		n = 0;
	}
	else
	if (A > B) 
    {
		n = (A - B);
	}
	else 
    {
		n = C - B + A;
	}

	if (n == (C-1))
    {
        UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_THRE, ENABLE);
        return (-1);
    }

	D [A] = c;  
    A =  (++A) % C;

	if (!hlp_get_tx_stat(F)) 
    {
#if HLP_CONFIG_UARTS_MODE_1 == UART_RS485
        HLP_CONFIG_UARTS_1_485_TX_FUNC;  /* tx için hazýrlayalým */
#endif    
#if HLP_CONFIG_UARTS_1_SAM 
        HLP_CONFIG_UARTS_1_SAM_TX_FUNC;
#endif  

#if (!HLP_CONFIG_UART_1_INT_ENABLE)
        while (UART_CheckBusy((LPC_UART_TypeDef *)LPC_UART1) == SET);
#endif
 
        hlp_uart_IntTransmit((LPC_UART_TypeDef *)LPC_UART1,F);
  	}
    else
    {
        UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_THRE, ENABLE);    
    }    

	return (0);
}

int UART1_GetChar (void)
{
    int a;

	if (G == H)
    {
        /* buffer boþ
         *
         * */
        G = 0;
        H = 0;
	  	return (-1);
    }

    a = J [H];
    H =  (++H) % I;

    return a;
}

void UART1_wait(void)
{
    while((A != B))
    {   
        hlp_uart_IntTransmit((LPC_UART_TypeDef *)LPC_UART1,F);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UART1_PutCharWait(int ch)
{
    UART1_PutChar(ch);
    UART1_wait();
} //void UART0_PutCharWait(int ch)

U8 UART1_is_tx_empty(void)
{
    return (A == B);    
}

U8 UART1_is_rx_empty(void)
{
    return (G == H);   
}

#undef A
#undef B
#undef C
#undef D
#undef F
#undef G
#undef H
#undef I
#undef J

#endif

#if (HLP_CONFIG_UARTS_ENABLE & UART2_ENABLED)

#define F 2
#define A _hlp_uart.buffer[F].Tx.head
#define B _hlp_uart.buffer[F].Tx.tail
#define C _hlp_uart.buffer[F].Tx.capacity
#define D _hlp_uart.buffer[F].Tx.buffer
#define G _hlp_uart.buffer[F].Rx.head
#define H _hlp_uart.buffer[F].Rx.tail
#define I _hlp_uart.buffer[F].Rx.capacity
#define J _hlp_uart.buffer[F].Rx.buffer 

int UART2_PutChar (int c)
{
	U32 n;

	/* Temporarily lock out UART transmit interrupts during this
	   read so the UART transmit interrupt won't cause problems
	   with the index values */
    UART_IntConfig(LPC_UART2, UART_INTCFG_THRE, DISABLE);


    /* calculate the bytes in buffer */
	if (A == B)	
    {
		n = 0;
	}
	else
	if (A > B) 
    {
		n = (A - B);
	}
	else 
    {
		n = C - B + A;
	}

	if (n == (C-1))
    {
        UART_IntConfig(LPC_UART2, UART_INTCFG_THRE, ENABLE);
        return (-1);
    }

	D [A] = c;  
    A =  (++A) % C;

	if (!hlp_get_tx_stat(F)) 
    {  
#if HLP_CONFIG_UARTS_MODE_2 == UART_RS485
    HLP_CONFIG_UARTS_2_485_TX_FUNC;  /* tx için hazýrlayalým */
#endif    
#if HLP_CONFIG_UARTS_2_SAM 
    HLP_CONFIG_UARTS_2_SAM_TX_FUNC;
#endif  

#if (!HLP_CONFIG_UART_2_INT_ENABLE)
    while (UART_CheckBusy(LPC_UART2) == SET);
#endif
 
        hlp_uart_IntTransmit(LPC_UART2,F);
  	}
    else
    {
        UART_IntConfig(LPC_UART2, UART_INTCFG_THRE, ENABLE);    
    }    

	return (0);
}

int UART2_GetChar (void)
{
    int a;

	if (G == H)
    {
        /* buffer boþ
         *
         * */
        G = 0;
        H = 0;
	  	return (-1);
    }

    a = J [H];
    H =  (++H) % I;

    return a;
}

void UART2_wait(void)
{
    while((A != B))
    {  
        hlp_uart_IntTransmit(LPC_UART2,F);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UART2_PutCharWait(int ch)
{
    UART2_PutChar(ch);
    UART2_wait();
} 

U8 UART2_is_tx_empty(void)
{
    return (A == B);    
}

U8 UART2_is_rx_empty(void)
{
    return (G == H);   
}

#undef A
#undef B
#undef C
#undef D
#undef F
#undef G
#undef H
#undef I
#undef J

#endif 

#if (HLP_CONFIG_UARTS_ENABLE & UART3_ENABLED)

#define F 3
#define A _hlp_uart.buffer[F].Tx.head
#define B _hlp_uart.buffer[F].Tx.tail
#define C _hlp_uart.buffer[F].Tx.capacity
#define D _hlp_uart.buffer[F].Tx.buffer
#define G _hlp_uart.buffer[F].Rx.head
#define H _hlp_uart.buffer[F].Rx.tail
#define I _hlp_uart.buffer[F].Rx.capacity
#define J _hlp_uart.buffer[F].Rx.buffer 

int UART3_PutChar (int c)
{
	U32 n;

	/* Temporarily lock out UART transmit interrupts during this
	   read so the UART transmit interrupt won't cause problems
	   with the index values */
    UART_IntConfig(LPC_UART3, UART_INTCFG_THRE, DISABLE);


    /* calculate the bytes in buffer */
	if (A == B)	
    {
		n = 0;
	}
	else
	if (A > B) 
    {
		n = (A - B);
	}
	else 
    {
		n = C - B + A;
	}

	if (n == (C-1))
    {
        UART_IntConfig(LPC_UART3, UART_INTCFG_THRE, ENABLE);
        return (-1);
    }

	D [A] = c;  
    A =  (++A) % C;

	if (!hlp_get_tx_stat(F)) 
    { 
#if HLP_CONFIG_UARTS_MODE_3 == UART_RS485
        HLP_CONFIG_UARTS_3_485_TX_FUNC;  /* tx için hazýrlayalým */
#endif      
#if HLP_CONFIG_UARTS_3_SAM 
        HLP_CONFIG_UARTS_3_SAM_TX_FUNC;
#endif

#if (!HLP_CONFIG_UART_3_INT_ENABLE)
        while (UART_CheckBusy(LPC_UART3) == SET);
#endif

        hlp_uart_IntTransmit(LPC_UART3,F);
  	}
    else
    {
        UART_IntConfig(LPC_UART3, UART_INTCFG_THRE, ENABLE);    
    }    

	return (0);
}

int UART3_GetChar (void)
{
    int a;

	if (G == H)
    {
        /* buffer boþ
         *
         * */
        G = 0;
        H = 0;
	  	return (-1);
    }

    a = J [H];
    H =  (++H) % I;

    return a;
}

void UART3_wait(void)
{
    while((A != B))
    {   
        hlp_uart_IntTransmit(LPC_UART3,F);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UART3_PutCharWait(int ch)
{
    UART3_PutChar(ch);
    UART3_wait();
} //void UART0_PutCharWait(int ch) 

U8 UART3_is_tx_empty(void)
{
    return (A == B);    
}

U8 UART3_is_rx_empty(void)
{
    return (G == H);   
}

#undef A
#undef B
#undef C
#undef D
#undef F
#undef G
#undef H
#undef I
#undef J

#endif

#if (HLP_CONFIG_UARTS_ENABLE & UART0_ENABLED)

/*********************************************************************//**
 * @brief		UART0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART0_IRQHandler(void)
{
	uint32_t intsrc, tmp, tmp1;

	/* Determine the interrupt source */
	intsrc = UART_GetIntId(LPC_UART0);
	tmp = intsrc & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (tmp == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus(LPC_UART0);
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
				| UART_LSR_BI | UART_LSR_RXFE);
		// If any error exist
		if (tmp1) {
				HLP_INTERNAL_ERROR(0,tmp1);
		}
	}

	// Receive Data Available or Character time-out
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)){
			hlp_uart_IntReceive(LPC_UART0,0);
	}

	// Transmit Holding Empty
	if (tmp == UART_IIR_INTID_THRE){
			hlp_uart_IntTransmit(LPC_UART0,0);
	}

}

#endif

#if (HLP_CONFIG_UARTS_ENABLE & UART1_ENABLED)
 
/*********************************************************************//**
 * @brief		UART0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART1_IRQHandler(void)
{
	uint32_t intsrc, tmp, tmp1;

	/* Determine the interrupt source */
	intsrc = UART_GetIntId((LPC_UART_TypeDef *)LPC_UART1);
	tmp = intsrc & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (tmp == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus((LPC_UART_TypeDef *)LPC_UART1);
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
				| UART_LSR_BI | UART_LSR_RXFE);
		// If any error exist
		if (tmp1) {
				HLP_INTERNAL_ERROR(1,tmp1);
		}
	}

	// Receive Data Available or Character time-out
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)){
			hlp_uart_IntReceive((LPC_UART_TypeDef *)LPC_UART1,1);
	}

	// Transmit Holding Empty
	if (tmp == UART_IIR_INTID_THRE){
			hlp_uart_IntTransmit((LPC_UART_TypeDef *)LPC_UART1,1);
	}

}

#endif

#if (HLP_CONFIG_UARTS_ENABLE & UART2_ENABLED)

/*********************************************************************//**
 * @brief		UART0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART2_IRQHandler(void)
{
	uint32_t intsrc, tmp, tmp1;

	/* Determine the interrupt source */
	intsrc = UART_GetIntId(LPC_UART2);
	tmp = intsrc & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (tmp == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus(LPC_UART2);
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
				| UART_LSR_BI | UART_LSR_RXFE);
		// If any error exist
		if (tmp1) {
				HLP_INTERNAL_ERROR(2,tmp1);
		}
	}

	// Receive Data Available or Character time-out
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)){
			hlp_uart_IntReceive(LPC_UART2,2);
	}

	// Transmit Holding Empty
	if (tmp == UART_IIR_INTID_THRE){
			hlp_uart_IntTransmit(LPC_UART2,2);
	}

}

#endif 

#if (HLP_CONFIG_UARTS_ENABLE & UART3_ENABLED)

/*********************************************************************//**
 * @brief		UART0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART3_IRQHandler(void)
{
	uint32_t intsrc, tmp, tmp1;
	/* Determine the interrupt source */
	intsrc = UART_GetIntId(LPC_UART3);
	tmp = intsrc & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (tmp == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus(LPC_UART3);
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
				| UART_LSR_BI | UART_LSR_RXFE);
		// If any error exist
		if (tmp1) {
				HLP_INTERNAL_ERROR(3,tmp1);
		}
	}

	// Receive Data Available or Character time-out
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)){
			hlp_uart_IntReceive(LPC_UART3,3);
	}

	// Transmit Holding Empty
	if (tmp == UART_IIR_INTID_THRE){
			hlp_uart_IntTransmit(LPC_UART3,3);
	}

}

#endif

/* End of Interrupts */

/***************************************************************************************************
 * END OF FILE
 **************************************************************************************************/

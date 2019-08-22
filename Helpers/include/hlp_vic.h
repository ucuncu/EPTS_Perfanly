/***************************************************************************************************
 * filename     : hlp_vic.h
 * description  : Handle interrupt functions...
 * author       : Ayhan KILINÇ
 * copyright    : Dijital Tanýma Sistemleri
 * datetime     : 20110105
 *
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:  20110105: First release.
 *
 *
 **************************************************************************************************/

/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/

#ifndef _HLP_VIC_H_
#define _HLP_VIC_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
/* include file(s) */   
#include <LPC17xx.H>
   
//#include "hlp_header.h"  
#include "hlp_type.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/* definitions */

#define HLP_VIC_INT___initial_sp              ( 0) /* ; Top of Stack */
#define HLP_VIC_INT_Reset_Handler             ( 1) /* ; Reset Handler  */
#define HLP_VIC_INT_NMI_Handler               ( 2) /* ; NMI Handler    */
#define HLP_VIC_INT_HardFault_Handler         ( 3) /* ; Hard Fault Handler  */
#define HLP_VIC_INT_MemManage_Handler         ( 4) /* ; MPU Fault Handler    */
#define HLP_VIC_INT_BusFault_Handler          ( 5) /* ; Bus Fault Handler     */
#define HLP_VIC_INT_UsageFault_Handler        ( 6) /* ; Usage Fault Handler   */
#define HLP_VIC_INT_RFU0                      ( 7) /* ; Reserved             */
#define HLP_VIC_INT_RFU1                      ( 8) /* ; Reserved             */
#define HLP_VIC_INT_RFU2                      ( 9) /* ; Reserved             */
#define HLP_VIC_INT_RFU3                      (10) /* ; Reserved             */
#define HLP_VIC_INT_SVC_Handler               (11) /* ; SVCall Handler       */
#define HLP_VIC_INT_DebugMon_Handler          (12) /* ; Debug Monitor Handler */
#define HLP_VIC_INT_RFU4                      (13) /* ; Reserved              */
#define HLP_VIC_INT_PendSV_Handler            (14) /* ; PendSV Handler        */
#define HLP_VIC_INT_SysTick_Handler           (15) /* ; SysTick Handler       */
//
//                ; External Interrupts
#define HLP_VIC_INT_WDT_IRQHandler            (16) /* ; 16: Watchdog Timer    */
#define HLP_VIC_INT_TIMER0_IRQHandler         (17) /* ; 17: Timer0            */
#define HLP_VIC_INT_TIMER1_IRQHandler         (18) /* ; 18: Timer1             */
#define HLP_VIC_INT_TIMER2_IRQHandler         (19) /* ; 19: Timer2            */
#define HLP_VIC_INT_TIMER3_IRQHandler         (20) /* ; 20: Timer3           */
#define HLP_VIC_INT_UART0_IRQHandler          (21) /* ; 21: UART0             */
#define HLP_VIC_INT_UART1_IRQHandler          (22) /* ; 22: UART1             */
#define HLP_VIC_INT_UART2_IRQHandler          (23) /* ; 23: UART2             */
#define HLP_VIC_INT_UART3_IRQHandler          (24) /* ; 24: UART3             */
#define HLP_VIC_INT_PWM1_IRQHandler           (25) /* ; 25: PWM1              */
#define HLP_VIC_INT_I2C0_IRQHandler           (26) /* ; 26: I2C0              */
#define HLP_VIC_INT_I2C1_IRQHandler           (27) /* ; 27: I2C1              */
#define HLP_VIC_INT_I2C2_IRQHandler           (28) /* ; 28: I2C2             */
#define HLP_VIC_INT_SPI_IRQHandler            (29) /* ; 29: SPI               */
#define HLP_VIC_INT_SSP0_IRQHandler           (30) /* ; 30: SSP0                 */
#define HLP_VIC_INT_SSP1_IRQHandler           (31) /* ; 31: SSP1                  */
#define HLP_VIC_INT_PLL0_IRQHandler           (32) /* ; 32: PLL0 Lock (Main PLL)  */
#define HLP_VIC_INT_RTC_IRQHandler            (33) /* ; 33: Real Time Clock       */
#define HLP_VIC_INT_EINT0_IRQHandler          (34) /* ; 34: External Interrupt 0  */
#define HLP_VIC_INT_EINT1_IRQHandler          (35) /* ; 35: External Interrupt 1  */
#define HLP_VIC_INT_EINT2_IRQHandler          (36) /* ; 36: External Interrupt 2  */
#define HLP_VIC_INT_EINT3_IRQHandler          (37) /* ; 37: External Interrupt 3  */
#define HLP_VIC_INT_ADC_IRQHandler            (38) /* ; 38: A/D Converter         */
#define HLP_VIC_INT_BOD_IRQHandler            (39) /* ; 39: Brown-Out Detect     */
#define HLP_VIC_INT_USB_IRQHandler            (40) /* ; 40: USB                  */
#define HLP_VIC_INT_CAN_IRQHandler            (41) /* ; 41: CAN                  */
#define HLP_VIC_INT_DMA_IRQHandler            (42) /* ; 42: General Purpose DMA  */
#define HLP_VIC_INT_I2S_IRQHandler            (43) /* ; 43: I2S                  */
#define HLP_VIC_INT_ENET_IRQHandler           (44) /* ; 44: Ethernet               */
#define HLP_VIC_INT_RIT_IRQHandler            (45) /* ; 45: Repetitive Interrupt Timer */
#define HLP_VIC_INT_MCPWM_IRQHandler          (46) /* ; 46: Motor Control PWM           */
#define HLP_VIC_INT_QEI_IRQHandler            (47) /* ; 47: Quadrature Encoder Interface   */
#define HLP_VIC_INT_PLL1_IRQHandler           (48) /* ; 48: PLL1 Lock (USB PLL)             */
#define HLP_VIC_INT_USBActivity_IRQHandler	  (49) /* ; 49: USB Activity interrupt to wakeup */
#define HLP_VIC_INT_CANActivity_IRQHandler	  (50) /* ; 50: CAN Activity interrupt to wakeup  */

////////////////////////////////////////////////////////////////////////////////////////////////////
/* types */

////////////////////////////////////////////////////////////////////////////////////////////////////
/* export functions */
extern void hlp_vic_init(void);

#define hlp_vic_set(irq,func)       hlp_vic_set_preemp_subpri((irq),(func),1,1)   

extern void hlp_vic_set_preemp_subpri(U8 irq, U32 function, U8 pre_emption, U8 sub_priority);

extern void hlp_vic_clr(U8 irq);

extern void hlp_vic_disable_all_irq(void);

////////////////////////////////////////////////////////////////////////////////////////////////////
/* export variables */

#endif //#ifndef _HLP_VIC_H_

/***************************************************************************************************
 * END OF FILE
 **************************************************************************************************/

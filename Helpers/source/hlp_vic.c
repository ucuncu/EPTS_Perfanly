/***************************************************************************************************
 * filename     : hlp_vic.c
 * description  : Handle interrupt functions...
 * author       : Ayhan KILINÇ
 * copyright    : Dijital Tanýma Sistemleri
 * datetime     : 20110105
 *
 ***************************************************************************************************
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0: 20110105:  First release.
 *
 ***************************************************************************************************
 * hlp_vic properties:
 *
 ***************************************************************************************************
 *  NOTES:
 *      
 *
 **************************************************************************************************/

/***************************************************************************************************
 * DECLARATIONS
 **************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////
/* include files */     
#include <LPC17xx.H>

#include "lpc17xx_nvic.h"

#include "hlp_vic.h"
#include "hlp_type.h"

#include <string.h>  

#include "absacc.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
/* extern calls */

////////////////////////////////////////////////////////////////////////////////////////////////////
/* local (static) types, definitions */
// unit debug definitions 
#define UNIT_NAME           "HLPVIC"
//#define UNIT_DBG_LEVEL        DDEBUG
//#define UNIT_DBG_LEVEL      DERR  
#define UNIT_DBG_LEVEL      HLP_DBG_HLP_VIC  
  

////////////////////////////////////////////////////////////////////////////////////////////////////
/* local (static) functions */

////////////////////////////////////////////////////////////////////////////////////////////////////
/* global variables */


#define HLP_VIC_ROM_VTOR_OFFSET         0x00000000

#define HLP_VIC_VECTOR_COUNT            (HLP_CONFIG_VIC_SIZE / 4) 

#if (HLP_VIC_VECTOR_COUNT < 64) || (HLP_VIC_VECTOR_COUNT > 256)
    //#error "Vector size must be in 256 and 1024"
#endif

#if (HLP_CONFIG_VIC_SIZE + HLP_CONFIG_VIC_ADDRESS) > 0x20084000
    #error "Index out of bounds! Please correct interrupt sizes."
#endif

#if (HLP_CONFIG_VIC_TYPE==1)

static U32 hlp_vic_vectors[HLP_VIC_VECTOR_COUNT] __at(HLP_CONFIG_VIC_ADDRESS);

#endif


/***************************************************************************************************
 * IMPLEMENTATIONS
 **************************************************************************************************/

/*********************************************************************//**
 * @brief       Init vic system
 * @param       None
 * @return      None
 ***********************************************************************/
void hlp_vic_init(void)
{
#if (HLP_CONFIG_VIC_TYPE==1) && ((HLP_VIC_VECTOR_COUNT > 64))  
    U16 i;
#endif

#if (HLP_CONFIG_VIC_TYPE==1) && ((HLP_VIC_VECTOR_COUNT > 64))
    /* sram mode
     *
     * */

    /* Disable all interrupts.
     *
     * */
    for(i=0;i<=34;i++)
    {
        NVIC_DisableIRQ((IRQn_Type)i);
    }

    /* backup system interrupts!
     *
     * */
    for(i=0;i<HLP_VIC_VECTOR_COUNT;i++)
    {
        hlp_vic_vectors[i] = ((U32 *)HLP_VIC_ROM_VTOR_OFFSET)[i];  
    }  
        
    NVIC_SetVTOR((uint32_t)hlp_vic_vectors); 
#else
    /* rom mode
     *
     * */
    /* NOTHING TO DO */
#endif
}

/*********************************************************************//**
 * @brief       Set interrupt system and enable it.
 * @param       None
 * @return      None
 ***********************************************************************/
void hlp_vic_set_preemp_subpri(U8 irq, U32 function, U8 pre_emption, U8 sub_priority)
{


#if HLP_CONFIG_VIC_TYPE==1
    /* sram mode
     *
     * */
    hlp_vic_vectors[(U32)irq] = function; 
    dlog(DINFO,"hlp_vic_set irq = %d, func = %0.8X, (IRQn_Type)(irq - 16) = %d\n",
               irq,
               function,
               (IRQn_Type)(irq - 16));
    dwait(DINFO);
#endif  

    if(irq >= HLP_VIC_INT_WDT_IRQHandler &&
       irq <= HLP_VIC_INT_CANActivity_IRQHandler)
    {        
        /* preemption = 1, sub-priority = 1 */
        NVIC_SetPriority((IRQn_Type)(irq - 16), ((pre_emption<<3)|sub_priority));
    	/* Enable Interrupt for UART0 channel */
        NVIC_EnableIRQ((IRQn_Type)(irq - 16));           
    }

}

/*********************************************************************//**
 * @brief       Disable interrupt
 * @param       None
 * @return      None
 ***********************************************************************/
void hlp_vic_clr(U8 irq)
{
    if(irq >= HLP_VIC_INT_WDT_IRQHandler &&
       irq <= HLP_VIC_INT_CANActivity_IRQHandler)
    {        
    	/* Disable Interrupt for UART0 channel */
        NVIC_DisableIRQ((IRQn_Type)(irq - 16));           
    }    
}


void hlp_vic_disable_all_irq(void)
{
    NVIC_DeInit();
}


/***************************************************************************************************
 * END OF FILE
 **************************************************************************************************/

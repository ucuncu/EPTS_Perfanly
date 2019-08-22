/*****************************************************************************************

 * @file	testing functions.c
 * @target LPC1768
 * @date	01.06.2018
 * @swrevision v0.0


********************************* Header Files ********************************************/

#include <stdio.h>
#include "lpc17xx_clkpwr.h"
#include "flags.h"
//#include "sdcard.h"
#include "fat32.h"

//**********************************
void testing_loop(int test_case)
{
	int test_case_sel;
	test_case_sel = test_case;
	if ((test_case_sel=='1')&&(fUart3to0Bride == FALSE))  //Enable Debug Log
	{
		if(fEnableDebugLog)
		{
			printf("...Debug Log Disabled...\n");	//Disable Debug Log
			fEnableDebugLog = FALSE;
		}
		else
		{
			printf("...Debug Log Enabled...\n");
			fEnableDebugLog = TRUE;
		}
	}
	else if((test_case_sel=='2')&&(fUart3to0Bride == FALSE))
	{
		if(fEnableGPSDataLog)
		{
			printf("...GPS DATA Log Disabled...\n");	//Disable GPS Data Log
			fEnableGPSDataLog = FALSE;
		}
		else
		{
			printf("...GPS DATA Log Enabled...\n");	//Enable GPS Data Log
			fEnableGPSDataLog = TRUE;
		}
	}
	
	else if((test_case_sel=='4')&&(fUart3to0Bride == FALSE))
	{
		if(fshowSDCardStatus)
		{
			printf("...PRODUCTION MOD - Close SD Card Statues Logging...\n");
			fshowSDCardStatus = FALSE;
		}
		else
		{
			printf("...DEBUG MOD - Show SD Card Statues - DEBUG PURPOSE ONLY..\n");
			fshowSDCardStatus = TRUE;
		}
	}
	else if(test_case_sel=='*')
	{
		if(fUart3to0Bride)
		{
			printf("...UART3 to UART0 Bridge Disabled...\n");	//Disable UART3 to UART0 Bridge Log
			fUart3to0Bride = FALSE;
		}
		else
		{
			printf("...UART3 to UART0 Bridge Enabled...\n");	//Enable UART3 to UART0 Bridge Log
			fUart3to0Bride = TRUE;
		}
	}
	else if((test_case_sel=='!') && (fEnableDebugLog)) // Sadece Dubug Modunda Sistem Uart Üzerinden Resetlenebilir.
	{
//		fileConfig_st *srcFilePtr;	
//		FILE_Close(srcFilePtr);
		NVIC_SystemReset();
	}
}



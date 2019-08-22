/*****************************************************************************************

 * @file	main.c
 * @target LPC1768
 * @date	28.04.2018
 * @swrevision v0.0
 * @brief	Main Program File for ALY Performance Analyser for the PCB "PERFALY_V0_0"

******************************************************************************************/

/******************************************************************************************
********************************* Header Files ********************************************
******************************************************************************************/
#include "lpc17xx_gpio.h"
#include "lpc17xx_clkpwr.h"
#include "hlp_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_libcfg_default.h"	
#include "lpc17xx_can_hlp.h"
#include "lpc17xx_wdt.h"
#include "lpc17xx_timer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>	
#include "pin_definitions.h"
#include "lpc17xx_uart.h" 
#include "hlp_gps.h"
#include "flags.h"
#include "debug_frmwrk.h"
#include "adc.h"
#include "iis2mdc_reg.h"
#include "extintr.h"
#include "i2c.h"
#include "fxas21002.h"
#include "mma8452q.h"
#include <lpc17xx.h>
#include "sdcard.h"
#include "stdutils.h"
#include "delay.h"
#include "spi.h"     
#include "usb.h"
#include "usbreg.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "mscuser.h"
#include "memory.h"
#include "fat32.h"


/************************** PRIVATE DEFINTIONS *************************/
#define WDT_TIMEOUT 	5000000
#define MAGNETOMETER_I2C_BUS 0
#define ACCELEROMETER_I2C_BUS 1
#define GYROSCOPE_I2C_BUS 2
#define PORT_USED		0
#define MMA8452Q_ADD 0x38
#define MMA8452Q_WHO_AM_I_ADD 0x0D



/* Push Button Definitions */
#define PBINT  0x00000001  /* P0.0 */

/* LED Definitions */
#define LEDMSK 0x000000FF /* P2.0..7 */

///* Private variables --------------------*/
//static axis3bit16_t data_raw_magnetic;
//static axis1bit16_t data_raw_temperature;
//static float magnetic_mG[3];
//static float temperature_degC;

/* flags ----------------------------------*/

BOOL f10ms = FALSE;
BOOL f100ms = FALSE;
BOOL f500ms = FALSE;
BOOL f1s = FALSE;
BOOL f2s = FALSE;
BOOL f3s = FALSE;
BOOL f10s = FALSE;
BOOL f1m = FALSE;
BOOL fReadAcc = FALSE;
BOOL fReadGyro = FALSE;
BOOL fReadMagneto = FALSE;
BOOL fSDbufferFull = FALSE;


BOOL fPowerDown = FALSE;
BOOL fGPSdataUpdate = FALSE;
BOOL fEnableDebugLog = FALSE;
BOOL fEnableGPSDataLog = FALSE; 
BOOL fUart3to0Bride = FALSE;
BOOL fcheckSensors = FALSE;
BOOL frecordJSONtoSD  = TRUE;
BOOL fshowSDCardStatus  = FALSE;
BOOL fSDCardStatus  = FALSE;
BOOL fErrorToggleStatus = FALSE;
BOOL fResetLongPressed = FALSE;
BOOL fInitGPS = FALSE;

extern uint8_t InReport;
extern uint8_t OutReport;

/* HID Demo Functions */
extern void GetInReport  (void);
extern void SetOutReport (void);

uint8_t InReport;                                                                 
                                            
uint8_t OutReport;                            
                                          

uint8_t sensorReadCnt = 0;
uint8_t sensorReadErrorCnt = 0;
fileConfig_st *filePtr = 0;	

unsigned int uPoll;
unsigned int uPinState = 0;
unsigned int uPrevPinState = 0;

void SysTick_Handler( void );
void Delay( unsigned long tick );
void main_uart_init( void );
void initIO( void );
void initI2C( void );
void initWDT( void );
void initLEDs( void );
void butread( void );
void ADC_init(void);
void testing_loop(int);
void powerDown(void);
void ledUpdate(void);
void initWait(void);
void start_magneto(void);
void start_gyro(void);
void start_acc(void);
void I2CRead(void);
void GPIO_IRQ_init(void);
void GPIO_INT_IRQ (void); 
void GPSdataUpdate(void);
void checkEEPROM(void);
void convertJsonRecSDbuf(void);
void checkSDcard(void);
void initSDcard(void);
void checkBatteryVoltage(void);
void recordtoSDcard(void);
void checkONOFFButtontoWakeUP(void);
void resetButRead(void);
uint32_t RdCmdDat (uint32_t cmd);
extern char sourceFileName[12];
/*********************************************************************//*
 * @brief MAIN PROGRAM
 ***********************************************************************/
int main(void)
{	
	SysTick_Config(SystemCoreClock/1000); /* Generate interrupt each 1 ms   */
//	uint32_t n;
	debug_frmwrk_init();
	main_uart_init();
	initIO();
	initLEDs();	
	initI2C();
	I2C0Init();			/* initialize I2c0 */
	initWait();
//	printf("****************************************\n");  HATALI !!!! OKUMADA SIRASINDA KITLENIYOR, INCELENECEK !!!
//	printf("Cheking Battery Voltage...\n");
//	ADC_init();       /* Initialize the ADC module */
//	Delay(50);
//	checkBatteryVoltage();
	printf("****************************************\n");
	printf("Looking for Gyroscope...\n");
	Delay(10);
	checkGyro();
	printf("****************************************\n");	
	printf("****************************************\n");
	printf("Looking for Magnetometer...\n");
	Delay(20);
	checkMagnetometer();
	printf("****************************************\n");	
	printf("****************************************\n");
	printf("Looking for Accelerometer...\n");	
	Delay(20);
	checkAccelerometer();
	printf("****************************************\n");	
	printf("****************************************\n");
	printf("Looking for EEPROM...\n");	
	Delay(20);
	checkEEPROM();
	printf("****************************************\n");
	printf("****************************************\n");
	printf("Looking for SD CARD...\n");
	initSDcard();
	Delay(20);
	printf("****************************************\n");
	printf("****************************************\n");
	uint8_t returnStatus;
	filePtr = FILE_Open(sourceFileName,WRITE,&returnStatus);  // OPEN FILE	fileConfig_st *srcFilePtr;	
	Delay(20);
	printf("Connected SD CARD and File Opened...\n");	
	printf("****************************************\n");	
	printf("****************************************\n");
	printf("Looking for USB Hardware...\n");
  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */	
	Delay(20);
	printf("USB Connection succesfully initialised...\n");		
	printf("****************************************\n");
	printf("****************************************\n");	
	printf("****************************************\n");
	printf("Looking for GPS...\n");
	initGPS();
	printf("****************************************\n");	
	GPIO_IRQ_init();
	initWDT();
	start_acc();
	start_magneto();
	start_gyro();
	while(TRUE)
	{
		WDT_Feed();					
		if (fPowerDown == TRUE)
		{
			checkONOFFButtontoWakeUP();
		}
		else
		{
			if ( fSDbufferFull == TRUE )  	// Write SD Card if Buffer is FULL
			{
				fSDbufferFull = FALSE;
				recordtoSDcard();
			}			
			if ( fGPSdataUpdate == TRUE )   // 10Hz GPS mi 20Hz mi ölçülmeli ???
			{
				fGPSdataUpdate = FALSE;
				GPSdataUpdate();
				convertJsonRecSDbuf();
			}	
			if (fReadAcc == TRUE)  //100Hz
			{
				read_acc();
				sensorReadCnt = sensorReadCnt + 5;
				if(sensorReadCnt == 23)
				{
					sensorReadCnt = 0;
				}		
			}		
			if (fReadMagneto == TRUE)  //100Hz
			{
				read_magneto();	
				sensorReadCnt = sensorReadCnt + 7;				
				if(sensorReadCnt == 23)
				{
					sensorReadCnt = 0;
				}		
			}				
			if (fReadGyro == TRUE)  //100Hz
			{
				read_gyro();
				sensorReadCnt = sensorReadCnt + 11;
				if(sensorReadCnt == 23)
				{
					sensorReadCnt = 0;
				}							
			}
			if(sensorReadCnt > 23)   // 100 Hz ile 3 sensörü interrupt ile okudugumuz için interrupt kaçirabiliyoruz. Kontrol yapip kitlenen sensörü tekrar aktif ediyoruz
			{
				if (fEnableDebugLog)
				{
					printf("Sensor Interupt Uncatched!..\n");
				}
				sensorReadCnt = 0;
				sensorReadErrorCnt = sensorReadErrorCnt + 1;
				if (sensorReadErrorCnt >= 3)
				{
					sensorReadErrorCnt = 0;
					fReadAcc = TRUE;
					fReadMagneto = TRUE;
					fReadGyro = TRUE;
					if (fEnableDebugLog)
					{
						printf("Reread All IMU Sensors Registers\n");
						printf("Activate All External Interrupts\n");	
					}						
				}
			}			
			if (f10ms)
			{
				f10ms = FALSE;				
				butread();				 // Button Read
				resetButRead();
				GetInReport();
			}
			if (f100ms)
			{
				f100ms = FALSE;		
			}
			if (f500ms)
			{
				f500ms = FALSE;	
				ledUpdate();	
			}
			if (f2s)
			{
				f2s = FALSE;
			}
			if (f10s)
			{
				f10s = FALSE;
				checkSDcard();          // Check SD card status every 2 seconds if there is an error
//				printf("****************************************\n");
//				printf("Checking Battery Voltage...\n");				
//				checkBatteryVoltage();
//				printf("****************************************\n");				
			}
			if (f1m)
			{
				f1m = FALSE;
			}			
		}
	}
}

void checkONOFFButtontoWakeUP(void)
{
	uPinState = GPIO_PinRead(P0_0);
	if (uPinState == uPrevPinState)
	{
		uPoll++;
		if( uPoll == 125000)  // 1/f = T,  1/32Khz =  0,00003125   --> 64.000 * 0,00003125 = 2 sec ???
		{
			if(uPinState == 0)
			{
				NVIC_SystemReset();
			}						
		}
	}
	else
	{
		uPoll = 0;
		uPrevPinState = uPinState;
	}
}


/*
 *  Get HID Input Report -> InReport
 */

void GetInReport (void) {

  if ((LPC_GPIO0 -> FIOPIN & PBINT) == 0) {             /* Check if PBINT is pressed */
    InReport = 0x01;
  } else {
    InReport = 0x00;
  }
}


/*
 *  Set HID Output Report <- OutReport
 */

void SetOutReport (void) {
	//Because 8 LEDs are not ordered, so we have check each bit
	//of OurReport to turn on/off LED correctly
	uint8_t led_num;
	LPC_GPIO2 -> FIOCLR = LEDMSK;
	LPC_GPIO1 -> FIOCLR = 0xF0000000;
	//LED0 (P2.6)
	led_num = OutReport & (1<<0);
	if(led_num == 0)
		LPC_GPIO2 -> FIOCLR |= (1<<6);
	else
		LPC_GPIO2 -> FIOSET |= (1<<6);
}

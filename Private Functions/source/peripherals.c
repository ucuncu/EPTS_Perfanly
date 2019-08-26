
#include "lpc17xx_clkpwr.h"
#include "pin_definitions.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_clkpwr.h"
#include "iis2mdc_reg.h"
#include "lpc17xx_libcfg_default.h"	
#include "lpc17xx_wdt.h"
#include "hlp_uart.h"
#include "fxas21002.h" 
#include "mma8452q.h"
#include "LPC17xx.h"                   
#include "lpc17xx_gpio.h" 
#include <stdio.h>
#include <i2c.h>
#include "extintr.h"
#include "flags.h"
#include "hlp_gps.h"
#include "json_serialiser.h"
#include <lpc17xx.h>
#include "sdcard.h"
#include "fat32.h"
#include "stdutils.h"
#include "delay.h"
#include "spi.h" 
#include "hlp_type.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "adc.h"
#include "lpc17xx_uart.h"     
#include "usbhw.h"


#define WDT_TIMEOUT 	5000000
#define ButtDebounceUs 	200000
#define SDCARDBUFFERHIGHT 	1      //Dafa fazla RAM olan bir islemciye geçilirse toplu yazma yapilabilir. Toplu Yazmanin avantaji olabilir sensör interrupi kaçirmamak için. Osiloskopta sürelere bakilmali.
#define SDCARDBUFFERWIDTH 	300

char SDCARDBUFFER[SDCARDBUFFERHIGHT][SDCARDBUFFERWIDTH];

void GPIO_INT_IRQ (void);

int counter;
uint8_t fillSdBufferCnt = 0;

unsigned short AD_last;
void Delay (unsigned long tick);
const char *returnSensorError( uint8_t status);	

uint32_t getSetNextCluster(char *inputBuffer,uint32_t clusterNumber,uint8_t get_set,uint32_t clusterEntry);
void initWait(void);
void WDT_IRQHandler( void );
void butread(void);
void onOffButton_GPI_IRQ(void);
void powerDown(void);

volatile unsigned long SysTickCnt;
uint8_t f10msCnt = 0;
uint8_t f100msCnt = 0;
uint8_t f500msCnt = 0;
uint8_t f1sCnt = 0;
uint8_t f2sCnt = 0;
uint8_t f10sCnt = 0;
uint8_t f1mCnt = 0;

uint8_t RGB200msCnt = 0;

uint16_t butdat;
uint16_t prvdat;
uint16_t vdcnt;
uint16_t lvdcnt;
uint16_t prvbut;
uint16_t button;
uint16_t presdb;

uint16_t resbutdat;
uint16_t resprvdat;
uint16_t resvdcnt;
uint16_t reslvdcnt;
uint16_t resprvbut;
uint16_t resbutton;
uint16_t respresdb;

uint16_t hardresbutdat;
uint16_t hardresprvdat;
uint16_t hardresvdcnt;
uint16_t hardreslvdcnt;
uint16_t hardresprvbut;
uint16_t hardresbutton;
uint16_t hardrespresdb;

BOOL switchStatues = TRUE;

char sourceFileName[12] = {'e', 'p', 't', 's', 'd','a', 't', '.', 't', 'x', 't','\0'} ;

/*********************************************************************//**
 * @brief		WDT interrupt handler sub-routine
 **********************************************************************/
void WDT_IRQHandler(void)
{
	NVIC_DisableIRQ(WDT_IRQn);
	LPC_RTC->GPREG0 = counter;
	WDT_ClrTimeOutFlag();
//	LPC_RTC->GPREG4++;
}

/*********************************************************************//**
 * @brief	Initializing GPIO pins
 ************************************************************************/
void initIO(void) 
{
	/* enable gpio pwr */
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE);
	
	/* set gpio directions and startup states */
	GPIO_SetDir(2, ON_OFF, 0);
	
	GPIO_SetDir(1, GPS_INT, 1);
	GPIO_SetValue(1, GPS_INT);
	
	GPIO_SetDir(0, ACC_INT, 0);
	GPIO_SetDir(0, GYRO_INT1, 0);
	GPIO_SetDir(0, MAGNO_INT, 0);
}
void checkEEPROM(void)
{
	/**********************************************************************
 * @brief		Check Gyroscope - Read "Who am I" flag
 **********************************************************************/
	uint8_t comStadus = 0;
  /* Write SLA(W), address, SLA(R), and read one byte back. */
  I2CWriteLength[PORT_USED] = 5;
  I2CReadLength[PORT_USED] = 0;
  I2CMasterBuffer[PORT_USED][0] = 0xA0;;
  I2CMasterBuffer[PORT_USED][1] = 0x01;		/* address MSB */
	I2CMasterBuffer[PORT_USED][2] = 0x00;		/* address LSB */
	I2CMasterBuffer[PORT_USED][3] = 0x43;		/* DATA */
  I2CMasterBuffer[PORT_USED][4] = 0xA0;
  I2CEngine( PORT_USED );
	Delay(200);
  /* Write SLA(W), address, SLA(R), and read one byte back. */
  I2CWriteLength[PORT_USED] = 3;
  I2CReadLength[PORT_USED] = 1;
  I2CMasterBuffer[PORT_USED][0] = 0xA0;
  I2CMasterBuffer[PORT_USED][1] = 0x01;		/* address MSB */
	I2CMasterBuffer[PORT_USED][2] = 0x00;		/* address LSB */
  I2CMasterBuffer[PORT_USED][3] = 0xA0 | RD_BIT;
  comStadus = I2CEngine( PORT_USED );
	if ((comStadus == 12 )&&(I2CSlaveBuffer[PORT_USED][0] == 0x43))
	{
		printf("EEPROM has been found\n");
		printf("Succesfully Communicated\n");		
		printf("Communication Status : Read/Write_OK\n");
	}	
	else
	{
		printf("EEPROM Read/Write Status : !!! ERROR !!!\n");		
	}
}

/*********************************************************************//**
 * @brief	UART INIT
 ************************************************************************/
void main_uart_init (void)
{
  LPC_PINCON->PINSEL0 |= 0x00000050;  /* P0.2, P0.3 Enable TxD0 and RxD0 */	 
	//LPC_PINCON->PINSEL4 |= 0x0000000A;   /* P2.0, P2.1 Enable TxD1 and RxD1 */
	LPC_PINCON->PINSEL9 |= 0x0F000000; /* P4.28, P4.29 Enable TxD3 and RxD3 */
	hlp_uart_init();
	hlp_uart_init_port(	0, 115200, 0);   /* Init UART0 */
	//hlp_uart_init_port(	1, 9600, 0);   /* Init UART1 */	
	hlp_uart_init_port(	3, 9600, 0);   /* Init UART3 */	
}
/*********************************************************************//**
 * @brief	WDT INIT
 ************************************************************************/
void initWDT()
{
	if (WDT_ReadTimeOutFlag())
	{
		WDT_ClrTimeOutFlag(); // Clear WDT TimeOut
	}
	// Initialize WDT, IRC OSC, interrupt mode, timeout = 5000000us = 5s
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_INT_ONLY); 
	WDT_Start(WDT_TIMEOUT); // Start watchdog with timeout given
	NVIC_SetPriority(WDT_IRQn, 0x10);
	NVIC_EnableIRQ(WDT_IRQn);
	LPC_RTC->GPREG0 = counter;
}

/*********************************************************************//**
 * @brief	Initializing I2C pins and peripherals
 ************************************************************************/
void initI2C()
{
	// Set I2C bus active
	LPC_SC->PCONP 		|= (1<<7); 				// Set the I2C0 bus power
	LPC_SC->PCLKSEL0 	&=~(1<<14)|(1<<15);		// Set PCLK clock for I2C0 bus bits 14 and 15 = 0 -> 
																					// PCLK_peripheral = CCLK/4 CCLK = 100 MHz, PCLK = 100 MHz/4 = 25 MHz for bitrate frequency

	// First configure pins P0.27 and P0.28 as normal GPIO pins
	LPC_PINCON->PINSEL1 &=~(1<<22);	// Set pin P0.27 as GPIO
	LPC_PINCON->PINSEL1 &=~(1<<23); // Set pin P0.27 as GPIO
	LPC_PINCON->PINSEL1 &=~(1<<24); // Set pin P0.28 as GPIO
	LPC_PINCON->PINSEL1 &=~(1<<25);	// Set pin P0.28 as GPIO
	LPC_PINCON->PINSEL1 |= (1<<22);	// Set pin P0.27 as SDA0 I2C0
	LPC_PINCON->PINSEL1 &=~(1<<23); // Set pin P0.27 as SDA0 I2C0
	LPC_PINCON->PINSEL1 |= (1<<24); // Set pin P0.28 as SCL0 I2C0
	LPC_PINCON->PINSEL1 &=~(1<<25);	// Set pin P0.28 as SCL0 I2C0
	
	// Reset (clear) the STAC andSIC bits for configure the Master Mode I2C0 bus
	LPC_I2C0->I2CONCLR =  (1<<3);	// Reset (clear) SIC  bit (I2C interrupt)
	LPC_I2C0->I2CONCLR =  (1<<5);	// Reset (clear) STAC bit (START flag)
	
	/* Calculate and set the proper value of I2C0 clock registers for 100 kHz bitrate frequency
	 * I2C0_Freq = PCLK/(SCLH + SCLL)
	 * SCLH + SCLL = 25000 kHz/100 kHz = 250
	 * SCLH = SCLL = 125 => 50% duty cycle
	*/
	LPC_I2C0->I2SCLH	= 125;		// SCL prescaler High register
	LPC_I2C0->I2SCLL	= 125;		// SCL prescaler Low register
	
	// Configure I2C0 pin in the Standard Drive Mode
	LPC_PINCON->I2CPADCFG &=~(1<<0)|(1<<1)|(1<<2)|(1<<3); 	// Standard Drive Mode (100 kbps)
															// bit 0 reset to 0 - The SDA0 pin is in the standard drive mode
															// bit 1 reset to 0 - The SDA0 pin has I2C glitch filtering and slew rate control enabled
															// bit 2 reset to 0 - The SCL0 pin is in the standard drive mode
															// bit 3 reset to 0 - The SCL0 pin has I2C glitch filtering and slew rate control enabled

	NVIC_EnableIRQ (I2C0_IRQn);		// Set the I2C0 Interrupts in the Global Interrupts; active
	LPC_I2C0->I2CONSET |= (1<<6);	// Enable the I2C0; bus bit 6	
}

/*********************************************************************//**
 * @brief	Initializing onboard LED indicators
 ************************************************************************/
void initLEDs()
{
	/* enable gpio pwr */
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE);
	/* set gpio directions and startup states */
	GPIO_SetDir(1, RGB_CONTROL|RGB_GREEN|RGB_RED|RGB_BLUE, 1);
	GPIO_SetValue(1, RGB_CONTROL|RGB_RED|RGB_BLUE|RGB_GREEN);
	GPIO_ClearValue(1, RGB_RED);
	GPIO_SetDir(2, ON_OFF_LED, 1);
	GPIO_SetValue(2, ON_OFF_LED);
	GPIO_SetDir(2, USB_CONNECT, 1);
	GPIO_SetValue(2, USB_CONNECT);
	//GPIO_SetValue(1, RGB_CONTROL);						//
}
/*********************************************************************//**
 * @brief	Init ADC Conversiton ( BATTERY VOLTAGE LEVEL MEASUREMENT)
 ************************************************************************/
void ADC_init (void) {
  LPC_PINCON->PINSEL3 &= ~(3<<29);			// P1.30 is GPIO - 1. MODE - is it necessary?
  LPC_PINCON->PINSEL3 |=  (3<<29);      	// P1.30 is AD0.4 - 3. MODE
  LPC_SC->PCONP       |=  (1<<12);			// Enable power to ADC block
  LPC_ADC->ADCR        =  (1<< 4) |			// select AD0.4 pin
                          (4<< 8) |			// ADC clock is 25MHz/5
                          (1<<21);			// enable ADC
  LPC_ADC->ADINTEN     =  (1<< 8);			// global interrupr enable - interrupt generated on DONE flag
  NVIC_EnableIRQ(ADC_IRQn);					// enable ADC Interrupt
}

/*********************************************************************//**
 * @brief	ADC EOC INTERRUPT
 ************************************************************************/
void ADC_IRQHandler(void) {

  AD_last = (LPC_ADC->ADGDR>>4) & 0xFFF;/* Read Conversion Result             */
  LPC_ADC->ADCR &= ~(1<<24);            /* Stop A/D Conversion               */

}

/*********************************************************************//**
 * @brief	CHECK BATTERY VOLTAJE ON ADC PIN 5
 ************************************************************************/
void checkBatteryVoltage(void) {
	
	int adcValue;
	float temp;
	adcValue = ADC_GetAdcValue(5); // Read the ADC value of channel 5 ( P1.31 ) - Battery Voltage
	temp = adcValue*0.0010663;  // (3.3V / 4096 ) sample resolution  ---> 6.8 / (2.2 + 6.8) voltage divider --> 0.0010663 
	//(ADC Readins : 3946 --> Battery Voltage : 4,2076198)//(ADC Readins : 3846 --> Battery Voltage : 4,1009898)//(ADC Readins : 3756 --> Battery Voltage : 4,0050228)
	//(ADC Readins : 3656 --> Battery Voltage : 3,8983928)//(ADC Readins : 3566 --> Battery Voltage : 3,8024258)//(ADC Readins : 3476 --> Battery Voltage : 3,7064588)
	printf("Battery Voltage:%f\n\r",temp);

}


/*********************************************************************//**
 * @brief	Init GPIO Interrupt Request
 ************************************************************************/
void GPIO_IRQ_init (void) 
{
	EINT_AttachInterrupt(EINT3, GPIO_INT_IRQ, FALLING);
	LPC_GPIOINT -> IO0IntEnF |= ACC_INT;
	LPC_GPIOINT -> IO0IntEnF |= GYRO_INT1;
	LPC_GPIOINT -> IO0IntEnR |= MAGNO_INT;    // Sinyali Osiloskop'ta incelenmeli, pull up gerekebilir !!!
	//NVIC_SetPriority(EINT3_IRQn, 0x80);
	NVIC_SetPriority(EINT3_IRQn, 0x00);  // Highest Possibility ?????
	NVIC_EnableIRQ(EINT3_IRQn);	
}

/*********************************************************************//**
 * @brief	GPIO Interrupt Routines
 ************************************************************************/
void GPIO_INT_IRQ (void) 
{
	__disable_irq();
	if ((LPC_GPIOINT->IO0IntStatR & ACC_INT) || (LPC_GPIOINT->IO0IntStatF & ACC_INT))
  {
		LPC_GPIOINT->IO0IntClr = ACC_INT;
		fReadAcc = TRUE;
  }	
	if ((LPC_GPIOINT->IO0IntStatR & GYRO_INT1) || (LPC_GPIOINT->IO0IntStatF & GYRO_INT1))
  {
		LPC_GPIOINT->IO0IntClr = GYRO_INT1;
		fReadGyro = TRUE;
  }	
	if ((LPC_GPIOINT->IO0IntStatR & MAGNO_INT) || (LPC_GPIOINT->IO0IntStatF & MAGNO_INT))
  {
		LPC_GPIOINT->IO0IntClr = MAGNO_INT;
		fReadMagneto = TRUE;
  }	
	__enable_irq();	
		NVIC_EnableIRQ(EINT3_IRQn);
}

/***********************************************************************
 * @brief		Initialising Wait - Wait for Clock and voltage stabilization
						& Check Magnetometer & Check Accelerometer & Check Gyroscope
 ***********************************************************************/
void initWait(void)
{
	printf("****************************************\n");
	Delay(200);
	printf("****************************************\n");
	Delay(300);	
	printf("*********SYSTEM IS INITIALISING*********\n");
	Delay(300);
	printf("****************************************\n");
	Delay(200);
	printf("****************************************\n");	
	printf("Device Name is : i-NOVA Tracker Basic\n");
	printf("Device ID: 00000001\n");
	printf("Send '1' to Enable/Disable Debug Log\n");
	printf("Send '2' to Enable/Disable Sensor Data Log\n");
	printf("Send '*' to Enable/Disable UART BRIDGE\n");
	printf("****************************************\n");
	Delay(500);	
//*************************************************	
}



/*************************************************************************
 * @brief		On Off Button Pressed - GPIO Pin Chance Interrupt Routine (1ms)
 *************************************************************************/
void onOffButton_GPI_IRQ(void)
{	  
    if ((LPC_GPIOINT->IO0IntStatR & (1 << 0)) || (LPC_GPIOINT->IO0IntStatF & (1 << 0)))
    {
        LPC_GPIOINT->IO0IntClr = (1 << 0);
    }	
}

/**********************************************************************
 * @brief		Power Down Mode
 **********************************************************************/
void powerDown(void)
{
		if (fEnableDebugLog)
		{
			printf("****************************************\n");	
			printf("*********** POWER DOWN MODE ************\n");
			printf("****************************************\n");				
		}	
		Delay(50);
		/* CLOSE FILE */
		FILE_PutCh(SD_EOF);
		FILE_Close(filePtr);
		if (fEnableDebugLog)
		{	
			printf("File Closed, SD Card Power Off...\n");				
		}	
		Delay(50);
		
		
		/* Reset Gpio Directions - Output olan pinler open drain olacak */
		//GPIO_ClearValue(1, GPS_INT);
		GPIO_SetValue(1, RGB_CONTROL|RGB_GREEN|RGB_RED|RGB_BLUE);
		GPIO_ClearValue(2, ON_OFF_LED);
		GPIO_SetDir(2, ON_OFF_LED, 1);
		GPIO_ClearValue(2, ON_OFF_LED);
		
		// ADC yi open drain yap
		
		if (fEnableDebugLog)
		{	
			printf("Leds OFF, GPIO Pin Directions RESET...\n");				
		}	
		/* USB Disabled */
		USB_Connect(FALSE); 
		USB_Reset();
		if (fEnableDebugLog)
		{	
			printf("USB Connection Disabled...\n");				
		}	
		
		
		/* GPS Power Save Mode */
		gps_power_down();
		Delay(10);
		/* Activate Magnetometer Low Power Mode */
		magneto_low_power_mode();
		Delay(10);
		
		/* Activate Accelerometer Low Power Mode */
		acc_low_power_mode();
		Delay(10);
				
		/* Activate Gyroscope Low Power Mode */
		gyro_low_power_mode();
		Delay(10);
		
		
		/* EEPROM operations  */
		// GEREKLI OLURSA EKLENECEK
		Delay(50);
		
		/*---------- Disable and disconnect the main PLL0 before enter into Deep-Sleep or Power-Down mode ------------*/
		LPC_SC->PLL0CON &= ~(1<<1); /* Disconnect the main PLL (PLL0) */
		LPC_SC->PLL0FEED = 0xAA; /* Feed */
		LPC_SC->PLL0FEED = 0x55; /* Feed */
		while ((LPC_SC->PLL0STAT & (1<<25)) != 0x00); /* Wait for main PLL (PLL0) to disconnect */
		LPC_SC->PLL0CON &= ~(1<<0); /* Turn off the main PLL (PLL0) */
		LPC_SC->PLL0FEED = 0xAA; /* Feed */
		LPC_SC->PLL0FEED = 0x55; /* Feed */
		while ((LPC_SC->PLL0STAT & (1<<24)) != 0x00); /* Wait for main PLL (PLL0) to shut down */


		/* Disable WDT Int */
		NVIC_DisableIRQ(WDT_IRQn);
		WDT_ClrTimeOutFlag();		
	
		
		/* Enable P0_0 int */
		EINT_AttachInterrupt(EINT3,onOffButton_GPI_IRQ,FALLING);
		LPC_GPIOINT -> IO0IntEnF |= (1<<0);
		NVIC_SetPriority(EINT3_IRQn, 0x80);
		NVIC_EnableIRQ(EINT3_IRQn);	

		/* Disable UARTS */
		LPC_PINCON->PINSEL0 &= 0xFFFFFFAF;  /* P0.2, P0.3 Enable TxD0 and RxD0 */	 
		//LPC_PINCON->PINSEL4 &= 0xFFFFFFF5;   /* P2.0, P2.1 Enable TxD1 and RxD1 */
		LPC_PINCON->PINSEL9 &= 0xF0FFFFFF; /* P4.28, P4.29 Enable TxD3 and RxD3 */
				

		/* Enter target power down mode */
		fPowerDown = TRUE;
		CLKPWR_PowerDown();					
}	

/**********************************************************************
 * @brief		checkSDcard is Valid or Not
 **********************************************************************/
void initSDcard(void)
{
//  fileInfo fileList;
//  uint32_t totalMemory,freeMemory;
	uint8_t returnStatus,sdcardType;
	//fileConfig_st *srcFilePtr;
	returnStatus = SD_Init(&sdcardType, 100);
	if(returnStatus == SDCARD_INIT_SUCCESSFUL)
	{
		fSDCardStatus = TRUE;
		printf("SD Card Detected!...\n");
		printf("SD Card Data Source : %s\n",&sourceFileName[0]);
	}	
	else if(returnStatus == SDCARD_NOT_DETECTED)
	{
			printf("SD card not detected...\n");
			fSDCardStatus = FALSE;
	}
	else if(returnStatus == SDCARD_INIT_FAILED)
	{
			printf("Card Initialization failed...\n");
			fSDCardStatus = FALSE;
	}
	else if(returnStatus == SDCARD_FAT_INVALID)
	{
			printf("Invalid Fat filesystem...\n");
			fSDCardStatus = FALSE;
	}
	printf("****************************************\n");		
}


/**********************************************************************
 * @brief		checkSDcard is Valid or Not
 **********************************************************************/
void checkSDcard(void)
{
	uint8_t returnStatus,sdcardType;
	returnStatus = SD_Init(&sdcardType, 10);
	if(returnStatus == SDCARD_INIT_SUCCESSFUL)
	{
		fSDCardStatus = TRUE;
		if (fEnableDebugLog)
		{
			printf("SD Card Detected!...\n");
			printf("SD Card Data Source : %s\n",&sourceFileName[0]);
		}
	}	
	else if(returnStatus == SDCARD_NOT_DETECTED)
	{
		if (fEnableDebugLog)
		{
			printf("SD card not detected...\n");
			fSDCardStatus = FALSE;
		}
	}
	else if(returnStatus == SDCARD_INIT_FAILED)
	{
		if (fEnableDebugLog)
		{
			printf("Card Initialization failed...\n");
			fSDCardStatus = FALSE;
		}
	}
	else if(returnStatus == SDCARD_FAT_INVALID)
	{
		if (fEnableDebugLog)
		{
			printf("Invalid Fat filesystem...\n");
			fSDCardStatus = FALSE;
		}
	}		
}

/**********************************************************************
 * @brief		Functions to Convert Float to String
 **********************************************************************/
// reverses a string 'str' of length 'len' 
void reverse(char *str, int len) 
{ 
    int i=0, j=len-1, temp; 
    while (i<j) 
    { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; j--; 
    } 
} 
  
 // Converts a given integer x to string str[].  d is the number 
 // of digits required in output. If d is more than the number 
 // of digits in x, then 0s are added at the beginning. 
int intToStr(int x, char str[], int d) 
{ 
    int i = 0; 
    while (x) 
    { 
        str[i++] = (x%10) + '0'; 
        x = x/10; 
    } 
  
    // If number of digits required is more, then 
    // add 0s at the beginning 
    while (i < d) 
        str[i++] = '0'; 
  
    reverse(str, i); 
    str[i] = '\0'; 
    return i; 
} 
void ftoa(float n, char *res, int afterpoint) 
{ 
    // Extract integer part 
    int ipart = (int)n; 
  
    // Extract floating part 
    float fpart = n - (float)ipart; 
  
    // convert integer part to string 
    int i = intToStr(ipart, res, 0); 
  
    // check for display option after point 
    if (afterpoint != 0) 
    { 
        res[i] = '.';  // add dot 
  
        // Get the value of fraction part upto given no. 
        // of points after dot. The third parameter is needed 
        // to handle cases like 233.007 
        fpart = fpart * pow(10, afterpoint); 
  
        intToStr((int)fpart, res + i + 1, afterpoint); 
    } 
} 

/**********************************************************************
 * @brief		Convert Data to JsonFormat to Store SD Card
 **********************************************************************/
void convertJsonRecSDbuf(void)
{
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;
	float lat = 0;
	float latNew = 0;
	double latInt= 0;
	float latFraction= 0;
	char latString[12] = {NULL};
	float lon= 0;
	float lonNew= 0;
	double lonInt= 0;
	float lonFraction= 0;
	char lonString[12] = {NULL};
	
	//GPS value format dönüsümü
	lat = atof(&GPS_LAT_BUF[0][0])/100;
	lon = atof(&GPS_LON_BUF[0][0])/100;
	latFraction =100*modf(lat, &latInt)/60;
	lonFraction =100*modf(lon, &lonInt)/60;
	latNew = latInt + latFraction;
	lonNew = lonInt + lonFraction;
	ftoa(latNew, latString, 8); 
	ftoa(lonNew, lonString, 8);		
	
//		printf(" lat : %f, latInt : %g, latFraction : %g, latNew : %2.8f, latString : %s\n" , lat, latInt, latFraction, latNew, latString); 
//		printf(" lon : %f, lonInt : %g, lonFraction : %g, lonNew : %2.8f, lonString : %s\n" , lon, lonInt, lonFraction, lonNew, lonString);
	
	//json_object_set_string(root_object, "ID", "EPTS00001");
	
	json_object_dotset_string(root_object, "GPS.Fix",&GPS_FIX_DATA_BUF[0][0]);
	json_object_dotset_string(root_object, "GPS.Date", &GPS_DATE_BUF[0][0]);
	json_object_dotset_string(root_object, "GPS.Time", &GPS_TIME_BUF[0][0]);
	json_object_dotset_string(root_object, "GPS.Lat", latString);
	json_object_dotset_string(root_object, "GPS.Lon", lonString);
	json_object_dotset_string(root_object, "GPS.Alt", &GPS_ALTITUDE_DATA_BUF[0][0]);
	json_object_dotset_string(root_object, "GPS.Vel", &GPS_VELOCITY_BUF[0][0]);
	json_object_dotset_number(root_object, "Acc.X", ACC_X_BUF);
	json_object_dotset_number(root_object, "Acc.Y", ACC_Y_BUF);
	json_object_dotset_number(root_object, "Acc.Z", ACC_Z_BUF);
	json_object_dotset_number(root_object, "Gyro.X", GYRO_X_BUF);
	json_object_dotset_number(root_object, "Gyro.Y", GYRO_Y_BUF);
	json_object_dotset_number(root_object, "Gyro.Z", GYRO_Z_BUF);
	json_object_dotset_number(root_object, "Mag.X", MAG_X_BUF);
	json_object_dotset_number(root_object, "Mag.Y", MAG_Y_BUF);
	json_object_dotset_number(root_object, "Mag.Z", MAG_Z_BUF);
	
	//serialized_string = json_serialize_to_string_pretty(root_value);  // JSON FORMATI DAHA OKUNABILIR
	
	serialized_string = json_serialize_to_string(root_value); // JSON u TEK SATIRDA GOSTEREBILIR
	
	if (fEnableGPSDataLog)   // Show data on the terminal
	{	
		puts(serialized_string);
		printf("---\n");	
	}

	for ( int k = 0; k<SDCARDBUFFERWIDTH; k++)   // BURAYI SDCARDBUFFERWIDTH 'e kadar yapma, serialized stringin sonuna kadar yap
	{
		SDCARDBUFFER[fillSdBufferCnt][k] = serialized_string[k];
	}
	fillSdBufferCnt = fillSdBufferCnt + 1;
	if (fillSdBufferCnt == SDCARDBUFFERHIGHT)  // SD CARD BUFFER FULL RECORD IT TO SD CARD
	{
		fillSdBufferCnt = 0;
		fSDbufferFull = TRUE;
	}
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

void recordtoSDcard (void)
{
	int ch = 0;
	uint8_t cntJsonValue = 0;	
	
	for( int k=0; k < SDCARDBUFFERHIGHT; k++)
	{
		for( int t=0; t < SDCARDBUFFERWIDTH; t++)    ///// Osiloskopta süresine bakilmali. Yazarken 4096 BYTE da (dogrusu allacation unit size ne ayarlanmissa) yazmaya devam edemiyor. Cluster restlenmeli. hatali yaziyor
		{ 
			if((SDCARDBUFFER[k][ch] == '}')&&( cntJsonValue == 5))
			{
				ch = 0;
				cntJsonValue = 0;
				FILE_PutCh('}');
				break;
			}
			else
			{ 
				if(ch == 0)
				{
					FILE_PutCh('\r');     // her data yeni satirsa baslasin, 
				}
				FILE_PutCh(SDCARDBUFFER[k][ch]);
				ch = ch + 1;
				if(SDCARDBUFFER[k][ch] == '}')
				{
					cntJsonValue = cntJsonValue + 1;
				}							
			}
		}
	}	
}

/**********************************************************************
 * @brief		Update Leds
 **********************************************************************/
void ledUpdate(void)
{
	if(switchStatues == FALSE)   // kapama tusuna uzun basinca hemen ledleri söndürelim, arka tarafta diger arabirimler kapatilacak
	{
		GPIO_SetValue(1, RGB_RED|RGB_BLUE|RGB_GREEN);		//		//  
	}
	else
	{
		if (fSDCardStatus == FALSE) // POWER ON, SD CARD OFF
		{
			if (fErrorToggleStatus)
			{
				fErrorToggleStatus = FALSE;
				GPIO_SetValue(1, RGB_RED|RGB_BLUE|RGB_GREEN);		
				GPIO_ClearValue(1, RGB_RED);
			}
			else
			{
				fErrorToggleStatus = TRUE;
				GPIO_SetValue(1, RGB_RED|RGB_BLUE|RGB_GREEN);		
			}					
		}
		else // POWER ON, SD CARD ON, GPS NOT FIXED
		{
			if ((GPS_FIX_DATA_BUF[0][0] != '1') && (GPS_FIX_DATA_BUF[0][0] != '2'))  // GPS not fixed, looking for...
			{
				if (RGB200msCnt == 1)		
				{
					GPIO_SetValue(1, RGB_RED|RGB_BLUE|RGB_GREEN);			
					GPIO_ClearValue(1, RGB_BLUE);		
				}
				else if (RGB200msCnt == 2)		
				{
					GPIO_SetValue(1, RGB_RED|RGB_BLUE|RGB_GREEN);		
					GPIO_ClearValue(1, RGB_RED);		
				}
				else if (RGB200msCnt == 3)
				{
					GPIO_SetValue(1, RGB_RED|RGB_BLUE|RGB_GREEN);		
					GPIO_ClearValue(1, RGB_GREEN);					
				}			
				else
				{
					GPIO_SetValue(1, RGB_RED|RGB_BLUE|RGB_GREEN);		
					RGB200msCnt = 0;
				}
				RGB200msCnt++;
			}
			else if (GPS_FIX_DATA_BUF[0][0] == '1') //// POWER ON, SD CARD ON, GPS FIXED, LED BLUE
			{
				GPIO_SetValue(1, RGB_RED|RGB_BLUE|RGB_GREEN);		
				GPIO_ClearValue(1, RGB_BLUE);	
				RGB200msCnt = 0;
			}
			else if (GPS_FIX_DATA_BUF[0][0] == '2') //// POWER ON, SD CARD ON, DGPS FIXED, LED GREEN
			{
				GPIO_SetValue(1, RGB_RED|RGB_BLUE|RGB_GREEN);		
				GPIO_ClearValue(1, RGB_GREEN);
				RGB200msCnt = 0;				
			}
		}		
	}
}

/**********************************************************************
 * @brief		SysTick handler sub-routine (1ms)
 **********************************************************************/
void SysTick_Handler (void) {
	SysTickCnt++;
	f10msCnt++;
	if (f10msCnt == 10)
	{
		f10msCnt = 0;
		f10ms = TRUE;
		f100msCnt++;
		if (f100msCnt == 10)
		{
			f100msCnt = 0;
			f100ms = TRUE;
			f500msCnt++;
			if (f500msCnt == 5)
			{
				f500msCnt = 0;
				f500ms = TRUE;					
				f1sCnt++;
				if (f1sCnt == 2)
				{
					f1sCnt = 0;
					f1s = TRUE;
					f2sCnt++;
					if (f2sCnt == 2) 
					{
						f2sCnt = 0;
						f2s = TRUE;
						f10sCnt++;	
						if (f10sCnt == 5)
						{
							f10sCnt = 0;
							f10s = TRUE;
							if (f1mCnt == 6) // 10s*6 = 1minute
							{
								f1mCnt = 0;
								f1m = TRUE;
							}
						}
					}
				}					
			}	
		}
	}	
}
/**********************************************************************
 * @brief		Delay ( paramater * 1ms)
 **********************************************************************/
void Delay (unsigned long tick) {
  unsigned long systickcnt;

  systickcnt = SysTickCnt;
  while ((SysTickCnt - systickcnt) < tick);
}

/**********************************************************************
 * @brief		On Off Button Long Pressed
 **********************************************************************/
void onOffLongPres(void)
{
		if(switchStatues)
		{
			switchStatues = FALSE;	
			powerDown();
		}
		else
		{
			switchStatues = TRUE;
		}
}

/**********************************************************************
 * @brief		Reset Button Long Pressed
 **********************************************************************/
void resetLongPres(void)
{
		FILE_PutCh(SD_EOF);
		FILE_Close(filePtr);
		Delay(100);
		NVIC_SystemReset(); 
}


/**********************************************************************
 * @brief		Button Read Subroutine
 **********************************************************************/
void butread(void)
{
	butdat = GPIO_PinRead(P0_0);
	if (butdat==prvdat)
	{
		vdcnt++;								
		if (vdcnt == 5)						//10*5=50ms
		{
			vdcnt=0;
			if (lvdcnt!= 255)
			{
				lvdcnt++;
				if (lvdcnt == 40)			//40 * 50ms = 2 sec Long Button Press
				{
					if (butdat == 0)
					{
						onOffLongPres();
					}
				}
			}
		}
	}
	else
	{
		vdcnt = 0;
		lvdcnt = 0;
		prvdat = butdat;
	}
}
/**********************************************************************
 * @brief		Reset Button Read Subroutine
 **********************************************************************/
void resetButRead(void)
{
	resbutdat = GPIO_PinRead(P2_10);
		
	if (resbutdat==resprvdat)
	{
		resvdcnt++;								
		if (resvdcnt == 50)						//50*10=500ms
		{
			resvdcnt=0;
			if (reslvdcnt!= 255)
			{
				reslvdcnt++;
				if (reslvdcnt == 10)			//10 * 500ms = 5 sec Long Button Press
				{
					if (resbutdat == 0)
					{
						fResetLongPressed = TRUE;
					}
				}
			}
		}
	}
	else
	{
		if (fResetLongPressed == TRUE)
		{
			resetLongPres();
		}
		resvdcnt = 0;
		reslvdcnt = 0;
		resprvdat = resbutdat;
	}
}

/* eof */

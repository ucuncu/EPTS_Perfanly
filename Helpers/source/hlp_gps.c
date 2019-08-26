/*****************************************************************************************

 * @file	hlp_gps.c
 * @target GMS-G9 & LPC1768
 * @date	06.06.2018
 * @swrevision v0.0

******************************************************************************************/

/******************************************************************************************
********************************* Header Files ********************************************
******************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>	
#include "lpc_types.h"
#include "flags.h"
#include "lpc17xx_uart.h"                    // Device header
#include "lpc17xx_gpio.h" 
#include "pin_definitions.h" 
#include "hlp_gps.h"

int i,j,k = 0;

char GPS_DATA_BUF[GPS_BUFFER_HEIGHT][GPS_BUFFER_LENGHT];
char GPS_DATA_BUF_TMP[GPS_BUFFER_HEIGHT][GPS_BUFFER_LENGHT];
char GPS_TIME_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_TIME_BUFFER_LENGHT] = {'0','0','0','0','0','0','.','0','0','\0'};
char GPS_LAT_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_LAT_BUFFER_LENGHT] = {'0','0','0','0','.','0','0','0','0','0','\0'};
char GPS_LON_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_LON_BUFFER_LENGHT] = {'0','0','0','0','0','.','0','0','0','0','0','\0'};
char GPS_FIX_DATA_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_FIX_BUFFER_LENGHT] = {'0','\0'};
char GPS_VELOCITY_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_VELOCITY_BUFFER_LENGHT] = {'0','.','0','0','0','\0'};
char GPS_DATE_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_DATE_BUFFER_LENGHT] = {'0','1','0','1','1','9','\0'};      // GPS cold start yaparken zamanin 0 yada null olmasi web uygulamada JSON dönüsümünde hataya yol açiyor.Init deger verdik
char GPS_ALTITUDE_DATA_BUF[GPS_DATA_BUFFER_HEIGHT][GPS_ALTITUDE_BUFFER_LENGHT] = {'0','.','0','0','0','\0'};

void Delay (unsigned long tick);

//-------------------------------------------------------------------------------------------------
//const unsigned char UBLOX_INIT[] PROGMEM = {
//  // Disable NMEA
//  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x23, // GxGGA off
//  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2A, // GxGLL off
//  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x31, // GxGSA off
//  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x38, // GxGSV off
//  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x3F, // GxRMC off
//  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x46, // GxVTG off

//  // Disable UBX
//  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0xDC, //NAV-PVT off
//  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0xB9, //NAV-POSLLH off
//  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x13,0xC0, //NAV-STATUS off

//  // Enable UBX
//  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x18,0xE1, //NAV-PVT on
//  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, //NAV-POSLLH on
//  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x14,0xC5, //NAV-STATUS on

//  // Rate
//  0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, //(10Hz)
//  0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
//  0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 //(1Hz)
//};
//---------------------------------------------------------------------------------------------------


void initGPSubxData(void)
{
	//fInitGPS = TRUE;
	
	//GGA Off
	int i = 0;
//	uint8_t GPSConfig[16] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x23};	
//	for ( i = 0; i<16; i++)
//	{
//		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, GPSConfig[i]);
//	}
//	Delay(100);
	//GxGLL off
	uint8_t GPSConfig1[16] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2A};	
	for ( i = 0; i<16; i++)
	{
		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, GPSConfig1[i]);
	}
	// GxGSA off
	Delay(100);
	uint8_t GPSConfig2[16] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x31};	
	for ( i = 0; i<16; i++)
	{
		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, GPSConfig2[i]);
	}
	Delay(100);
	// GxGSV of
	uint8_t GPSConfig3[16] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x38};	
	for ( i = 0; i<16; i++)
	{
		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, GPSConfig3[i]);
	}
	Delay(100);
//	 // GxRMC off
//	uint8_t GPSConfig4[16] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x3F};	
//	for ( i = 0; i<16; i++)
//	{
//		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, GPSConfig4[i]);
//	}
//	Delay(100);
	// GxVTG off
	uint8_t GPSConfig5[16] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x46};	
	for ( i = 0; i<16; i++)
	{
		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, GPSConfig5[i]);
	}
	// RATE 10 Hz
	Delay(100);
	uint8_t GPSConfig6[14] = {0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12};	
	for ( i = 0; i<14; i++)
	{
		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, GPSConfig6[i]);
	}
	Delay(100);	

//	// time pulse config
//	uint8_t timePulse[40] = {0xB5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x32, 
//														0x00, 0x00, 0x00 , 0x40, 0x42, 0x0F, 0x00, 0x40, 0x42, 0x0F, 0x00, 
//														0x00, 0x00, 0x00, 0x00, 0xA0, 0x86, 0x01, 0x00, 0x00, 0x00, 0x00, 
//														0x00, 0x77, 0x00, 0x00, 0x00, 0x4A,0xB6};
//	
//	for ( i = 0; i<40; i++)
//	{
//		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, timePulse[i]);
//	}
//	Delay(100);	
//	
	// time pulse 2 config ( not active )
//	uint8_t timePulse2[40] = {0xB5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x01, 0x01, 0x00, 0x00, 0x32, 
//														0x00, 0x00, 0x00 , 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
//														0x48, 0xE8, 0x01, 0x00, 0xA0, 0x86, 0x01, 0x00, 0x00, 0x00, 0x00, 
//														0x00, 0x70, 0x00, 0x00, 0x00, 0x58,0x28};															
//		for ( i = 0; i<40; i++)
//	{
//		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, timePulse2[i]);
//	}
//	Delay(100);	
//	
//		// time pulse 0 selected
//	uint8_t timePulseSelect[9] = {0xB5, 0x62, 0x06, 0x31, 0x01, 0x00, 0x00, 0x38,0xE5};															
//	for ( i = 0; i<9; i++)
//	{
//		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, timePulseSelect[i]);
//	}
//	
	
	Delay(100);
	uint8_t ubloxSbasInit[16] = { 0xB5, 0x62, 0x06, 0x16, 0x08, 0x00, 0x03, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0xE5 };
	for ( i = 0; i<16; i++)
	{
		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, ubloxSbasInit[i]);
	}		
	//Delay(500);	
	//fInitGPS = FALSE;	
}

/*********************************************************************//**
 * @brief	INIT GPS
 ************************************************************************/

void initGPS(void)
{
	initGPSubxData();
	printf("GPS has been succesfully initialised\n");
}

/*********************************************************************//**
 * @brief	GPS POWER UP
 ************************************************************************/

void gps_power_up(void)
{
	//GPIO_SetValue(0, GPS_Reset);
}

/*********************************************************************//**
 * @brief	GPS POWER DOWN
 ************************************************************************/
void gps_power_down(void)
{
//	fInitGPS = TRUE;	

//	// time pulse 1 selected ( reset time pulse )
//	uint8_t timePulseSelect[9] = {0xB5, 0x62, 0x06, 0x31, 0x01, 0x00, 0x01, 0x39,0xE6};															
//	for ( i = 0; i<9; i++)
//	{
//		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, timePulseSelect[i]);
//	}
//	Delay(100);	
//	
////	// configure power save mode to be on off mode - UBX-CFG-PM2
//	uint8_t setupPSM[] = {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x80, 0x00, 0x01, 0x10, 0x27, 
//												0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 
//												0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 
//												0x01, 0x00, 0xD0,0x9E } ;
////															
//	Delay(100);
//	uint8_t setPSM[10] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x01, 0x22, 0x92 };
//	for (int i = 0; i<10; i++)
//	{
//		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, setPSM[i]);	
//	}
			
	Delay(100);
	uint8_t GPSoff_all[16] = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B};	
	for (int i = 0; i<16; i++)
	{
		UART_SendByte( (LPC_UART_TypeDef *)LPC_UART3, GPSoff_all[i]);	
	}	
	Delay(1000);
//	GPIO_ClearValue(1, GPS_INT);
//	Delay(5000);	
//	fInitGPS = FALSE;	
	printf("GPS Power Off Mode Activated...\n");
}

/* -----------------------------------------------------------------------------*/
/* ---------------------------  RecDataToGPSbuffer  ----------------------------*/
/* -----------------------------------------------------------------------------*/
	
int RecDataToGPSbuffer( uint8_t GpsDatChar)
{
	uint8_t tmp = GpsDatChar;

	if(tmp == '$') //New NMEA Data is comming from GPS
	{
		j=0;
		GPS_DATA_BUF[i][j]=tmp;
		j++;
	}
	else if(tmp == '*') // NMEA Data ended, next data will be checksum (not used, if needed, add validation later ) !!! USE MALLOC LATER !!!
	{
		for (int z=j; z<GPS_BUFFER_LENGHT ; z++)
		{
			GPS_DATA_BUF[i][z]='*'; // No empty Area left
		}
		j=0;
		i++;
		if ( i >= GPS_BUFFER_HEIGHT )
		{
			i=0;
			//Now all data is ready,
			memcpy(&GPS_DATA_BUF_TMP[0][0], &GPS_DATA_BUF[0][0],(GPS_BUFFER_HEIGHT*GPS_BUFFER_LENGHT));
			// Add termination line
			for ( int h=0; h<GPS_BUFFER_HEIGHT; h++)
			{
				GPS_DATA_BUF_TMP[h][GPS_BUFFER_LENGHT-1]='\0';
			}
			fGPSdataUpdate = TRUE;    // Set GPS Data Update Flag
		}
	}
	else 
	{
		GPS_DATA_BUF[i][j]=tmp;
		j++;
		if ( j >= (GPS_BUFFER_LENGHT+1) )
		{
			j=0;
			return -1;
		}
	}
return 0;
}
/* -----------------------------------------------------------------------------*/
/* -----------------------------  NMEA_GxGGA_RECEIVED  -------------------------*/
/* -----------------------------------------------------------------------------*/
/*****************************************************************************
	GGA - essential fix data which provide 3D location and accuracy data.

 $GxGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
															 4 = Real Time Kinematic
															 5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
															 7 = Manual input mode
															 8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
     *47          the checksum data, always begins with *
		 
		 NOTE: NMEA Data cannot have more then 80 characters ( without line terminators )
****************************************************************************************/
void NMEA_GxGGA_RECEIVED(int bufferline)
{
	int bufline = bufferline;
	int i,j = 0;
	uint8_t comma[14];
	//uint8_t time[10]={};
	//printf("GPGGA data has beed updated --> bufferline: %d\n",bufline);
	
	for (i=0; i<GPS_BUFFER_LENGHT; i++)
	{
	 if(GPS_DATA_BUF_TMP[bufline][i] == ',')
	 {
		comma[j]= i;
		j++;
	 }
	 if (j > 14)
	 {
		 break;
	 }
	}
	memcpy(&GPS_TIME_BUF[0][0],&GPS_DATA_BUF_TMP[bufline][comma[0]+1],(comma[1]-comma[0]-1));
	GPS_TIME_BUF[0][10]='\0';
	memcpy(&GPS_LAT_BUF[0][0],&GPS_DATA_BUF_TMP[bufline][comma[1]+1],(comma[2]-comma[1]-1));
	GPS_LAT_BUF[0][9]='\0';
	memcpy(&GPS_LON_BUF[0][0],&GPS_DATA_BUF_TMP[bufline][comma[3]+1],(comma[4]-comma[3]-1));
	GPS_LON_BUF[0][10]='\0';
	memcpy(&GPS_FIX_DATA_BUF[0][0],&GPS_DATA_BUF_TMP[bufline][comma[5]+1],(comma[6]-comma[5]-1));
	GPS_FIX_DATA_BUF[0][1]='\0';
	memcpy(&GPS_ALTITUDE_DATA_BUF[0][0],&GPS_DATA_BUF_TMP[bufline][comma[8]+1],(comma[9]-comma[8]-1));
	GPS_ALTITUDE_DATA_BUF[0][5]='\0';
}




/* -----------------------------------------------------------------------------*/
/* -----------------------------  NMEA_GxRMC_RECEIVED  -------------------------*/
/* -----------------------------------------------------------------------------*/
/*********************************************************************************
$GxRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
     *6A          The checksum data, always begins with *
*************************************************************************************/
void NMEA_GxRMC_RECEIVED(int bufferline)
{

	int bufline = bufferline;
	int i,j = 0;
	uint8_t comma[14];
	//uint8_t time[10]={};
	//printf("GPRMC data has beed updated --> bufferline: %d\n",bufline);	
	
	for (i=0; i<GPS_BUFFER_LENGHT; i++)
	{
	 if(GPS_DATA_BUF_TMP[bufline][i] == ',')
	 {
		comma[j]= i;
		j++;
	 }
	 if (j > 14)
	 {
		 break;
	 }
	}
	memcpy(&GPS_VELOCITY_BUF[0][0],&GPS_DATA_BUF_TMP[bufline][comma[6]+1],(comma[7]-comma[6]-1));
	GPS_VELOCITY_BUF[0][5]='\0';
	memcpy(&GPS_DATE_BUF[0][0],&GPS_DATA_BUF_TMP[bufline][comma[8]+1],(comma[9]-comma[8]-1));
	GPS_DATE_BUF[0][6]='\0';
}


/* -----------------------------------------------------------------------------*/
/* -----------------------------  GPSdataUpdate  -------------------------------*/
/* -----------------------------------------------------------------------------*/

void GPSdataUpdate(void)
{
	for ( int k=0; k<GPS_BUFFER_HEIGHT; k++ )
	{
		if (GPS_DATA_BUF_TMP[k][0] == '$')
		{
			if (GPS_DATA_BUF_TMP[k][1] == 'G')
			{
				if (GPS_DATA_BUF_TMP[k][2] == 'P' || GPS_DATA_BUF_TMP[k][2] == 'L' || GPS_DATA_BUF_TMP[k][2] == 'S' || GPS_DATA_BUF_TMP[k][2] == 'N' )
				{
					if (GPS_DATA_BUF_TMP[k][3] == 'G')          
					{
						if (GPS_DATA_BUF_TMP[k][4] == 'G')				//  
						{
							if (GPS_DATA_BUF_TMP[k][5] == 'A')			// $GPGGA,$GLGGA,$GSGGA,$GNGGA
							{
								NMEA_GxGGA_RECEIVED(k);									
							}							
						}
					}
					else if (GPS_DATA_BUF_TMP[k][3] == 'R')		//
					{
						if (GPS_DATA_BUF_TMP[k][4] == 'M')			//  
						{
							if (GPS_DATA_BUF_TMP[k][5] == 'C')		// $GPRMC,$GLRMC,$GNRMC,$GSRMC
							{
								NMEA_GxRMC_RECEIVED(k);								
							}						
						}		
					}
				}					
			}		
		}	
	}
}

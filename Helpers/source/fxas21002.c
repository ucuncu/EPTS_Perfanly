
/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "flags.h"
#include <stdint.h>
#include <stdbool.h>
#include "fxas21002.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>	
#include "i2c.h"
#include "math.h"

void recordToGyroBuffer( int gyroX, int gyroY, int gyroZ);
void filterGyroData(int a[], int b[], int c[], int array_size);
//void updateGyroBuffer( void );
 
volatile int16_t newGyro[3];

int TMP_GYRO_X_BUF[10];
int TMP_GYRO_Y_BUF[10];
int TMP_GYRO_Z_BUF[10];

//int GYRO_X_BUF[10];
//int GYRO_Y_BUF[10];
//int GYRO_Z_BUF[10];

int GYRO_X_BUF = 0;
int GYRO_Y_BUF = 0;
int GYRO_Z_BUF = 0;

//int GYRO_ERR_BUF = 0;

int gyroErrCnt = 0;
int gyroCnt = 0;

/**********************************************************************
 * @brief		Check Gyroscope - Read "Who am I" flag
 **********************************************************************/
void checkGyro(void)
{
	const char *returnString;
	uint8_t gyroStatues = 0;
	uint32_t i;
  for ( i = 0; i < BUFSIZE; i++ )
  {
	I2CSlaveBuffer[PORT_USED][i] = 0x00;
  }
  /* Write SLA(W), address, SLA(R), and read one byte back. */
  I2CWriteLength[PORT_USED] = 2;
  I2CReadLength[PORT_USED] = 1;
  I2CMasterBuffer[PORT_USED][0] = FXAS21002_ADD;
  I2CMasterBuffer[PORT_USED][1] = FXAS21002_WHO_AM_I_ADD;		/* address */
  I2CMasterBuffer[PORT_USED][2] = FXAS21002_ADD | RD_BIT;
  gyroStatues = I2CEngine( PORT_USED );
	returnString = returnSensorError( gyroStatues);	
	if ((gyroStatues == 12) && (I2CSlaveBuffer[PORT_USED][0] == -41))
	{
		printf("Gyroscope has been found\n");
		printf("Succesfully Communicated\n");
		printf("Communication Status  :  %s\n",returnString);
	}
	else
	{
		printf("!!!! - Gyroscope ERROR -  !!!!\n");
		printf("I2CSlaveBuffer  :  %d\n",I2CSlaveBuffer[PORT_USED][0]);
		printf("Gyroscope Error Type  :  %s\n",returnString);
	}
}



/**********************************************************************
 * @brief		INIT AND START GYROSCOPE
 **********************************************************************/
void start_gyro (void) 
{     
	unsigned char reg_val = 0; 
	int i = 0;
	I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_CTRLREG1, 0x40);      // Reset all registers to POR values                                                                   // ~1ms delay   
	
	for(i=0; i <10000; i ++);	 // a little delay
	do                                                              // Wait for the RST bit to clear     
	{         
		reg_val = I2C_Read_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_CTRLREG1) & 0x40;     
	}    
	while (reg_val);
		I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_CTRLREG0, 0x1F);      // Active HPF, cut of freq 0.248Hz, 250 dps scale
//	I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_RT_THS, 0x05);     // Set threshold to 96 dps             
//	I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_RT_COUNT, 0x02);   // Set debounce timer period to 20 ms    
//	I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_RT_CFG, 0x0B);     // Enable rate threshold detection for X and Y axis, latch enabled 
	//I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_CTRLREG2, 0x30);      // Rate threshold interrupt enabled and routed to INT1    
	I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_CTRLREG2, 0x0C);      // Data Ready interrupt enabled on INT1 
	I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_CTRLREG1, 0x0E);      // ODR = 100 Hz, Active mode 
}


/**********************************************************************
 * @brief		INIT AND START GYROSCOPE
 **********************************************************************/
void gyro_low_power_mode (void) 
{     
	unsigned char reg_val = 0; 
	int i = 0;
	I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_CTRLREG1, 0x40);      // Reset all registers to POR values                                                                   // ~1ms delay   
	
	for(i=0; i <10000; i ++);	 // a little delay
	do                                                              // Wait for the RST bit to clear     
	{         
		reg_val = I2C_Read_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_CTRLREG1) & 0x40;     
	}    
	while (reg_val);
	
	I2C_Write_One_Byte(PORT_USED, FXAS21002_ADD, FXAS21002_REG_CTRLREG1, 0x1C);      // ODR = 12.5 Hz, Standby mode 
	printf("Gyroscope Low Power Mode Activated...\n");
}


void read_gyro (void)
{
	
	uint32_t i;
	//const char *returnString;
	uint8_t gyroStatues = 0;
	
  for ( i = 0; i < BUFSIZE; i++ )
  {
		I2CSlaveBuffer[PORT_USED][i] = 0x00;
  }	
	
  /* Write SLA(W), address, SLA(R), and read one byte back. */
  I2CWriteLength[PORT_USED] = 2;
  I2CReadLength[PORT_USED] = 6;
  I2CMasterBuffer[PORT_USED][0] = FXAS21002_ADD;
  I2CMasterBuffer[PORT_USED][1] = 0x01;		/* address */
  I2CMasterBuffer[PORT_USED][2] = FXAS21002_ADD | RD_BIT;
  gyroStatues = I2CEngine( PORT_USED );
	//returnString = returnSensorError( gyroStatues);	
	if (gyroStatues == 12)
	{
		fReadGyro = FALSE; // Communication is ok, all register read 
	}
//	else
//	{
//			if (gyroErrCnt <= 0xFFFF)
//			{
//				gyroErrCnt ++;
//			}
//			GYRO_ERR_BUF = gyroErrCnt;
		////printf(" GyroErr : %s\n",returnString);
//	}

	 for (int i=0; i<6; i+=2)
	{
		newGyro[i/2] = ((I2CSlaveBuffer[PORT_USED][i] << 8) | I2CSlaveBuffer[PORT_USED][i+1]);  // Turn the MSB and LSB into a 16-bit value
	}		
	//printf("Gyro(mdps) on X, Y, Z: %d , %d , %d \n",newGyro[0],newGyro[1],newGyro[2]);
		recordToGyroBuffer(newGyro[0],newGyro[1],newGyro[2]);		
}

void recordToGyroBuffer( int gyroX, int gyroY, int gyroZ)
{
	int sampleNumber = 10;
	TMP_GYRO_X_BUF[gyroCnt] = gyroX;
	TMP_GYRO_Y_BUF[gyroCnt] = gyroY;
	TMP_GYRO_Z_BUF[gyroCnt] = gyroZ;
	
	gyroCnt +=1;
	if (gyroCnt >=  sampleNumber)
	{		
		gyroCnt = 0;
		filterGyroData(TMP_GYRO_X_BUF, TMP_GYRO_Y_BUF, TMP_GYRO_Z_BUF,  sampleNumber);
		 //updateGyroBuffer();
	}	
}


/**********************************************************************
 * @brief		UPDATE GYRO BUFFER
 **********************************************************************/
// void updateGyroBuffer( void )
// {
//	 memcpy(GYRO_X_BUF, TMP_GYRO_X_BUF, sizeof(TMP_GYRO_X_BUF));
//	 memcpy(GYRO_Y_BUF, TMP_GYRO_Y_BUF, sizeof(TMP_GYRO_Y_BUF));
//	 memcpy(GYRO_Z_BUF, TMP_GYRO_Z_BUF, sizeof(TMP_GYRO_Z_BUF));
// } 

 void filterGyroData(int a[], int b[], int c[], int array_size)
 {
		int i, j, temp;
	 // Data sorting
		for (i = 0; i < (array_size - 1); ++i)
		{
			for (j = 0; j < array_size - 1 - i; ++j )
			{
				 if (a[j] > a[j+1])
				 {
						temp = a[j+1];
						a[j+1] = a[j];
						a[j] = temp;
				 }
				 if (b[j] > b[j+1])
				 {
						temp = b[j+1];
						b[j+1] = b[j];
						b[j] = temp;
				 }
				 if (c[j] > c[j+1])
				 {
						temp = c[j+1];
						c[j+1] = c[j];
						c[j] = temp;
				 }
			}
		}
		
		// eleminete max and min value and mean value
		
		GYRO_X_BUF = (a[1] + a[2] + a[3] + a[4] + a[5] + a[6] + a[7] + a[8])/8;
		GYRO_Y_BUF = (b[1] + b[2] + b[3] + b[4] + b[5] + b[6] + b[7] + b[8])/8;
		GYRO_Z_BUF = (c[1] + c[2] + c[3] + c[4] + c[5] + c[6] + c[7] + c[8])/8;
			
 }  


/*FUNCTION****************************************************************
*
* Function Name    : fxas21002_init
* Returned Value   : result
* Comments         : Initialize FXAS21002 Gyro sensor.
*
*END*********************************************************************/
//bool fxas21002_init(gyro_sensor_t* pThisGyro)
//{
//    uint8_t txBuffer;
//    uint8_t cmdBuffer[2];

//    pThisGyro->fDegPerSecPerCount = FXAS21002_DEGPERSECPERCOUNT;

//    // Write 0000 0000 = 0x00 to CTRL_REG1 to place FXOS21002 in Standby
//    // [7]: ZR_cond=0
//    // [6]: RST=0
//    // [5]: ST=0 self test disabled
//    // [4-2]: DR[2-0]=000 for 800Hz
//    // [1-0]: Active=0, Ready=0 for Standby mode
//    cmdBuffer[0] = BOARD_I2C_FXAS21002_ADDR << 1;
//    cmdBuffer[1] = FXAS21002_CTRL_REG1;
//    txBuffer = 0x00;
//    if (!I2C_XFER_SendDataBlocking(cmdBuffer, 2, &txBuffer, 1))
//        return false;

//    // write 0000 0000 = 0x00 to CTRL_REG0 to configure range and filters
//    // [7-6]: BW[1-0]=00, LPF disabled
//    // [5]: SPIW=0 4 wire SPI (irrelevant)
//    // [4-3]: SEL[1-0]=00 for 10Hz HPF at 200Hz ODR
//    // [2]: HPF_EN=0 disable HPF
//    // [1-0]: FS[1-0]=00 for 1600dps (TBD CHANGE TO 2000dps when final trimmed parts available)
//    cmdBuffer[0] = BOARD_I2C_FXAS21002_ADDR << 1;
//    cmdBuffer[1] = FXAS21002_CTRL_REG0;
//    txBuffer = 0x00;
//    if (!I2C_XFER_SendDataBlocking(cmdBuffer, 2, &txBuffer, 1))
//        return false;

//    // write 0000 0010 = 0x02 to CTRL_REG1 to configure 800Hz ODR and enter Active mode
//    // [7]: ZR_cond=0
//    // [6]: RST=0
//    // [5]: ST=0 self test disabled
//    // [4-2]: DR[2-0]=000 for 800Hz ODR
//    // [1-0]: Active=1, Ready=0 for Active mode
//    cmdBuffer[0] = BOARD_I2C_FXAS21002_ADDR << 1;
//    cmdBuffer[1] = FXAS21002_CTRL_REG1;
//    txBuffer = 0x02;
//    if (!I2C_XFER_SendDataBlocking(cmdBuffer, 2, &txBuffer, 1))
//        return false;

//    return true;
//}

///*******************************************************************************
// * EOF
// ******************************************************************************/

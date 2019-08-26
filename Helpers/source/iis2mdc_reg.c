/*
 ******************************************************************************
 * @file    iis2mdc_reg.c
 * @author  Sensors Software Solution Team
 * @brief   IIS2MDC driver file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
*/
#include "flags.h"
#include "iis2mdc_reg.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "i2c.h"

void recordToMagBuffer( int magX, int magY, int magZ);
void filterMagData(int a[], int b[], int c[], int array_size);
//void updateMagBuffer( void );
	 
volatile int16_t newMag[3]; 

int TMP_MAG_X_BUF[10];
int TMP_MAG_Y_BUF[10];
int TMP_MAG_Z_BUF[10];

//int MAG_X_BUF[10];
//int MAG_Y_BUF[10];
//int MAG_Z_BUF[10];

int MAG_X_BUF = 0;
int MAG_Y_BUF = 0;
int MAG_Z_BUF = 0;

//int MAG_ERR_BUF = 0;

int magErrCnt = 0;
int magCnt = 0;

/**
  * @defgroup  IIS2MDC
  * @brief     This file provides a set of functions needed to drive the
  *            iis2mdc enhanced inertial module.
  * @{
  *
  */

/**
  * @defgroup  IIS2MDC_Interfaces_Functions
  * @brief     This section provide a set of functions used to read and
  *            write a generic register of the device.
  *            MANDATORY: return 0 -> no Error.
  * @{
  *
  */


/**********************************************************************
 * @brief		Check Magnetometer - Read "Who am I" flag
 **********************************************************************/
void checkMagnetometer(void)
{
	 /* In order to start the I2CEngine, the all the parameters 
  must be set in advance, including I2CWriteLength, I2CReadLength,
  I2CCmd, and the I2cMasterBuffer which contains the stream
  command/data to the I2c slave device. 
  (1) If it's a I2C write only, the number of bytes to be written is 
  I2CWriteLength, I2CReadLength is zero, the content will be filled 
  in the I2CMasterBuffer. 
  (2) If it's a I2C read only, the number of bytes to be read is 
  I2CReadLength, I2CWriteLength is 0, the read value will be filled 
  in the I2CMasterBuffer. 
  (3) If it's a I2C Write/Read with repeated start, specify the 
  I2CWriteLength, fill the content of bytes to be written in 
  I2CMasterBuffer, specify the I2CReadLength, after the repeated 
  start and the device address with RD bit set, the content of the 
  reading will be filled in I2CMasterBuffer index at 
  I2CMasterBuffer[I2CWriteLength+2]. 
   */
	const char *returnString;
	uint8_t magnetometerStatues = 0;
	uint32_t i;
  for ( i = 0; i < BUFSIZE; i++ )
  {
	I2CSlaveBuffer[PORT_USED][i] = 0x00;
  }
  /* Write SLA(W), address, SLA(R), and read one byte back. */
  I2CWriteLength[PORT_USED] = 2;
  I2CReadLength[PORT_USED] = 1;
  I2CMasterBuffer[PORT_USED][0] = IIS2MDC_I2C_ADD;
  I2CMasterBuffer[PORT_USED][1] = 0x4F;		/* address */
  I2CMasterBuffer[PORT_USED][2] = IIS2MDC_I2C_ADD | RD_BIT;
  magnetometerStatues = I2CEngine( PORT_USED );
	returnString = returnSensorError( magnetometerStatues);
	if ((magnetometerStatues == 12) && (I2CSlaveBuffer[PORT_USED][0] == 64))
	{
		printf("Magtometer has been found\n");
		printf("Succesfully Communicated\n");
		printf("Communication Status  :  %s\n",returnString);
	}
	else
	{
		printf("!!!! - MAGNETOMETER ERROR -  !!!!\n");
		printf("I2CSlaveBuffer  :  %d\n",I2CSlaveBuffer[PORT_USED][0]);
		printf("Magnetometer Error Type  :  %s\n",returnString);
	}
}

/**********************************************************************
 * @brief		INIT AND START MAGNETOMETER
 **********************************************************************/
void start_magneto(void)
{
	I2C_Write_One_Byte(PORT_USED, IIS2MDC_I2C_ADD, IIS2MDC_CFG_REG_A, 0x20);  // RESET ALL REGISTERS
	I2C_Write_One_Byte(PORT_USED, IIS2MDC_I2C_ADD, IIS2MDC_CFG_REG_A, 0x8C);  // Temp Comp Enabled, 100Hz ODR, Continious Mode
	I2C_Write_One_Byte(PORT_USED, IIS2MDC_I2C_ADD, IIS2MDC_CFG_REG_B, 0x03);  //OFFSET CANCEL, ENABLE LPF
	I2C_Write_One_Byte(PORT_USED, IIS2MDC_I2C_ADD, IIS2MDC_CFG_REG_C, 0x01);  // INT ENABLED, Data ready signal enabled on register
	I2C_Write_One_Byte(PORT_USED, IIS2MDC_I2C_ADD, IIS2MDC_INT_CRTL_REG, 0xE3);  // INT LATCHED, INT occurded signal enabled on register
}

/**********************************************************************
 * @brief		ACTIVATE MAGNETOMETER LOW POWER MODE
 **********************************************************************/
void magneto_low_power_mode(void)
{
	I2C_Write_One_Byte(PORT_USED, IIS2MDC_I2C_ADD, IIS2MDC_CFG_REG_A, 0x20);  // RESET ALL REGISTERS
	I2C_Write_One_Byte(PORT_USED, IIS2MDC_I2C_ADD, IIS2MDC_CFG_REG_A, 0x13);  // Temp Comp Disabled, 10Hz ODR, Idle Mode
	I2C_Write_One_Byte(PORT_USED, IIS2MDC_I2C_ADD, IIS2MDC_CFG_REG_B, 0x02);  //OFFSET CANCEL
	printf("Magnetometer Low Power Mode Activated...\n");
}


/**********************************************************************
 * @brief		READ MAGNETOMETER ROUTINE
 **********************************************************************/
void read_magneto(void)
{
	uint32_t i;
	//const char *returnString;
	uint8_t magnetometerStatues = 0;
	
  for ( i = 0; i < BUFSIZE; i++ )
  {
		I2CSlaveBuffer[PORT_USED][i] = 0x00;
  }
  /* Write SLA(W), address, SLA(R), and read one byte back. */
  I2CWriteLength[PORT_USED] = 2;
  I2CReadLength[PORT_USED] = 6;
  I2CMasterBuffer[PORT_USED][0] = IIS2MDC_I2C_ADD;
  I2CMasterBuffer[PORT_USED][1] = 0x68;		/* address */
  I2CMasterBuffer[PORT_USED][2] = IIS2MDC_I2C_ADD | RD_BIT;
  magnetometerStatues = I2CEngine( PORT_USED );
	//returnString = returnSensorError( magnetometerStatues);	
	if (magnetometerStatues == 12)
	{
		fReadMagneto = FALSE; // Communication is ok, all register read 
	}
//	else
//	{
//			if (magErrCnt <= 0xFFFF)
//			{
//				magErrCnt ++;
//			}
//			MAG_ERR_BUF = magErrCnt;
//		//printf(" MagErr : %s\n",returnString);
//	}	
	
	for (int i=0; i<6; i+=2)
  {
    newMag[i/2] = ((I2CSlaveBuffer[PORT_USED][i + 1 ] << 8) | I2CSlaveBuffer[PORT_USED][i]); // place MSB and LSB
  }
	
	//READ INT SOURCE REGISTER TO CLEAR INTERRUPT
	I2CWriteLength[PORT_USED] = 2;
  I2CReadLength[PORT_USED] = 1;
  I2CMasterBuffer[PORT_USED][0] = IIS2MDC_I2C_ADD;
  I2CMasterBuffer[PORT_USED][1] = IIS2MDC_INT_SOURCE_REG;		/* address */
  I2CMasterBuffer[PORT_USED][2] = IIS2MDC_I2C_ADD | RD_BIT;	
	
		//printf("Mag (mGaus) on X, Y, Z : %d , %d , %d\n",newMag[0],newMag[1],newMag[2]);
	recordToMagBuffer(newMag[0],newMag[1],newMag[2]);
}

void recordToMagBuffer( int magX, int magY, int magZ)
{
	int sampleNumber = 10;
	
	TMP_MAG_X_BUF[magCnt] = magX;
	TMP_MAG_Y_BUF[magCnt] = magY;
	TMP_MAG_Z_BUF[magCnt] = magZ;
	
	magCnt +=1;
	
	if (magCnt >=  sampleNumber)
	{		
		magCnt = 0;
		filterMagData(TMP_MAG_X_BUF, TMP_MAG_Y_BUF, TMP_MAG_Z_BUF,  sampleNumber);
		 //updateMagBuffer();
	}	
}

/**********************************************************************
 * @brief		UPDATE MAGNETOMETER BUFFER
 **********************************************************************/
// void updateMagBuffer( void )
// {
//	 memcpy(MAG_X_BUF, TMP_MAG_X_BUF, sizeof(TMP_MAG_X_BUF));
//	 memcpy(MAG_Y_BUF, TMP_MAG_Y_BUF, sizeof(TMP_MAG_Y_BUF));
//	 memcpy(MAG_Z_BUF, TMP_MAG_Z_BUF, sizeof(TMP_MAG_Z_BUF));
// } 


 void filterMagData(int a[], int b[], int c[], int array_size)
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
		
		MAG_X_BUF = (a[1] + a[2] + a[3] + a[4] + a[5] + a[6] + a[7] + a[8])/8;
		MAG_Y_BUF = (b[1] + b[2] + b[3] + b[4] + b[5] + b[6] + b[7] + b[8])/8;
		MAG_Z_BUF = (c[1] + c[2] + c[3] + c[4] + c[5] + c[6] + c[7] + c[8])/8;
	
 }  





/* 
In the ISR, only the interrupt flag is cleared and the RT_SRC register 
(0x0F) is read in order to clear the EA status bit and deassert the INT1 pin, as shown on the screenshot below. 
0x4C in the RT_SRC register indicates that the rate threshold event has been detected on the Y-axis and was negative.
*/

//void PORTA_IRQHandler() {    
//PORTA_PCR5 |= PORT_PCR_ISF_MASK;                                   // Clear the interrupt flag     
//IntSource = I2C_ReadRegister(FXAS21002_I2C_ADDRESS, RT_SRC_REG);   // Read the RT_SRC register to clear the EA flag and deassert the INT1 pin
//EventCounter++;       
//}

float iis2mdc_from_lsb_to_mgauss(int16_t lsb)
{
  return ((float)lsb) * 1.5f;
}

float iis2mdc_from_lsb_to_celsius(int16_t lsb)
{
  return (((float)lsb / 8.0f) + 25.0f);
}







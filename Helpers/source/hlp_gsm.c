/*****************************************************************************************

 * @file	hlp_gsm.c
 * @target SIM800H & LPC1768
 * @date	01.06.2018
 * @swrevision v0.0

******************************************************************************************/

/******************************************************************************************
********************************* Header Files ********************************************
******************************************************************************************/
#include <stdio.h>
#include "lpc17xx_uart.h"                    // Device header
void Delay (unsigned long tick);

void test_GSM (void) 	
{
		uint8_t AT_Command[]= "AT\r\n";
		printf("\n");
		printf("GSM MODUL ILETISIMI TEST EDILIYOR...\n");
		printf("'AT' KOMUTU GONDERILIYOR... \n");		
		UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 4, NONE_BLOCKING);			
		printf("'AT' KOMUTU GONDERILDI ! \n");		
		printf("GSM MODUL CEVABI :  \n");		
}

void test_signal_quality(void)
{
		uint8_t AT_Command[]= "AT+CSQ\r\n";
		printf("\n");
		printf("'AT+CSQ' KOMUTU GONDERILIYOR... \n");		
		UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 8, NONE_BLOCKING);			
		printf("'AT+CSQ' KOMUTU GONDERILDI ! \n");						
		printf("GSM MODUL CEVABI :  \n");
}
void make_test_phone_call(void)
{
	  uint8_t AT_Command[]= "ATD<5558468577>\r\n";
		printf("\n");
		printf("'ATD<5558468577>' KOMUTU GONDERILIYOR... \n");	
		UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 16, NONE_BLOCKING);
		printf("'ATD<5558468577>' KOMUTU GONDERILDI... \n");
		printf("GSM MODUL CEVABI :  \n");
}
void check_manufacturer(void)
{
		uint8_t AT_Command[]= "AT+GMI\r\n";
		printf("\n");
		printf("'AT+GMI' KOMUTU GONDERILIYOR... \n");	
		UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 8, NONE_BLOCKING);
		printf("'AT+GMI' KOMUTU GONDERILDI... \n");
		printf("GSM MODUL CEVABI :  \n");
}
void check_imei_num(void)
{
		uint8_t AT_Command[]= "AT+GSN\r\n";
		printf("\n");
		printf("'AT+GSN' KOMUTU GONDERILIYOR... \n");	
		UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 8, NONE_BLOCKING);
		printf("'AT+GSN' KOMUTU GONDERILDI... \n");
		printf("GSM MODUL CEVABI :  \n");
}
void pin_check(void)
{
		uint8_t AT_Command[]= "AT+CPIN=?\r\n";
		printf("\n");
		printf("'AT+CPIN=?' KOMUTU GONDERILIYOR... \n");	
		UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 11, NONE_BLOCKING);
		printf("'AT+CPIN=?' KOMUTU GONDERILDI... \n");
		printf("GSM MODUL CEVABI :  \n");
}
void show_registered_operator(void)
{
	uint8_t AT_Command[]= "AT+COPS?\r\n";
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 10, NONE_BLOCKING);
	//uint8_t AT_Command1[]= "AT+CMGS=<05558468577><CR>deneme<ctrl-z/ESC>\r\n";
	//UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command1, 44, NONE_BLOCKING);
	printf("SHOW REGISTERED OPERATOR ... \n");
}
void list_evailable_operators(void)
{
	uint8_t AT_Command[]= "AT+COPS=?\r\n";
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 11, NONE_BLOCKING);
	//uint8_t AT_Command1[]= "AT+CMGS=<05558468577><CR>deneme<ctrl-z/ESC>\r\n";
	//UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command1, 44, NONE_BLOCKING);
	printf("LIST AVAILABLE OPERATORS... \n");
}

void setup_test_SMS ( void )
{
	uint8_t AT_Command[]= "AT+CMGF=1\r\n";
	uint8_t AT_Command1[]= "AT+CSCS=\"GSM\"\r\n";	
	uint8_t AT_Command2[]= "AT+CMGS=\"05558";
	uint8_t AT_Command3[]= "468577\"\r\n";
	
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 11, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command1, 15, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command2, 14, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command3, 9, NONE_BLOCKING);

}
void send_test_SMS ( void )
{
	uint8_t AT_Command4[]= "TEST";
	uint8_t AT_Command5[] = "\x1a\r\n";
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command4, 4, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command5, 5, NONE_BLOCKING);
}

void get_GPRSWAP_settings ( void )
{
	uint8_t AT_Command[]= "AT+CMGF=1\r\n";
	uint8_t AT_Command1[]= "AT+CSCS=\"GSM\"\r\n";	
	uint8_t AT_Command2[]= "AT+CMGS=\"5555";
	uint8_t AT_Command3[]= "GPRSWAP AYAR";
	uint8_t AT_Command4[] = "\x1a\r\n";
	
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 11, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command1, 15, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command2, 13, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command3, 12, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command4, 4, NONE_BLOCKING);
}

void do_GPRSWAP_settings (void)
{
	uint8_t AT_Command[]="AT+CGATT=1\r\n";
	uint8_t AT_Command1[]="AT+CGDCONT=1,\"IP";
	uint8_t AT_Command2[]="\",\"internet\"";
	uint8_t AT_Command3[]="AT+CSTT=\"interne";
	uint8_t AT_Command4[]="t\",\"wap\",\"wap\"";
	uint8_t AT_Command5[]="AT+CIICR";
	
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 12, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command1, 16, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command2, 12, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command3, 16, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command4, 14, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command5, 8, NONE_BLOCKING);
}



void test_server_connection ( void )
{
	
	uint8_t AT_Command[]= "AT+HTTPINIT\r\n";    // http init
	uint8_t AT_Command1[]= "AT+HTTPPARA=\"CID";    // start by sending up the http bearer profile identifier
	uint8_t AT_Command2[]= "\",1\r\n"; // cont for 16 byte issue :(
	uint8_t AT_Command3[] = "AT+HTTPPARA=\"URL";
	uint8_t AT_Command4[] = "\",\"http://www.al";
	uint8_t AT_Command5[] = "yelektronik.com/";
	uint8_t AT_Command6[] = "\"\r\n";
	uint8_t AT_Command7[] = "AT+HTTPACTION=0";
	uint8_t AT_Command8[] = "\r\n";
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command, 13, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command1, 16, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command2, 5, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command3, 16, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command4, 16, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command5, 16, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command6, 3, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command7, 15, NONE_BLOCKING);
	Delay(200);
	UART_Send( (LPC_UART_TypeDef *)LPC_UART1, AT_Command8, 2, NONE_BLOCKING);
}

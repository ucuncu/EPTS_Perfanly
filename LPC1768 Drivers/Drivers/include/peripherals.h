

#ifndef PERIPHERALS_H
#define PERIPHERALS_H	


#ifndef __SERIAL_H
#define __SERIAL_H

extern void SER_init      (int uart);
extern int  SER_getChar   (int uart);
extern int  SER_getChar_nb(int uart);
extern int  SER_putChar   (int uart, int c);
extern void SER_putString (int uart, unsigned char *s);

#endif

int main_uart_init(void);
void initIO(void);
void initWDT();
void gsm_power_up(void)	;
void gsm_power_down(void);
void gps_power_up(void);
void gps_power_down(void);
void initCAN();
void initI2C();
void initLEDs();
void Delay (unsigned long tick);
void SysTick_Handler (void);
void WDT_IRQHandler(void);
void SER_init (int uart);
void ADC_init (void);
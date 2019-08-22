/**
 * @file	fifo.h
 * @Author	turgay (turgay@dtsis.com)
 * @date	20131001
 * @brief	casitli interruptli islemci peripherallarda kullanimi için. uygulamalar da kullanabilir.
 *
 * source url: http://www.coactionos.com/embedded-design/111-a-fifo-buffer-implementation.html
 */

#ifndef FIFO_H
#define FIFO_H		1

#include <stdint.h>

typedef struct {
	volatile uint8_t * buf;
	volatile uint16_t head;
	volatile uint16_t tail;
		 uint16_t size;
} fifo_t;

/* Check fifo is empty */
#define FIFO_IS_EMPTY(f)	((f)->head == (f)->tail)
/* Check fifo is full or not */
#define FIFO_IS_FULL(f)		((f)->tail == ((f)->head+1))

//This initializes the FIFO structure with the given buffer and size
extern void fifo_init(fifo_t * f, uint8_t * buf, int size);

//This reads nbytes bytes from the FIFO
//The number of bytes read is returned
extern uint16_t fifo_read(fifo_t * f, void * buf, uint16_t nbytes);

//This writes up to nbytes bytes to the FIFO
//If the head runs in to the tail, not all bytes are written
//The number of bytes written is returned
extern uint16_t fifo_write(fifo_t * f, const void * buf, uint16_t nbytes);

/* lpc17xx uart hlp driver inda gerekti.
 * veri fifodan okunuyor, gönderilmeye çalisiliyor basarili gönderilir ise tail artiriliyor.
 */
extern int fifo_read_ch(fifo_t * f, uint8_t * buf, uint8_t inc_tail);

/* fifo_read_ch() tamamliyor. */
extern void fifo_inc_tail(fifo_t * f);

#endif //#ifndef FIFO_H
/* eof */

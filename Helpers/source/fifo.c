#include "fifo.h"

//This initializes the FIFO structure with the given buffer and size
void fifo_init(fifo_t * f, uint8_t * buf, int size)
{
     f->head = 0;
     f->tail = 0;
     f->size = size;
     f->buf = buf;
}

//This reads nbytes bytes from the FIFO
//The number of bytes read is returned
uint16_t fifo_read(fifo_t * f, void * buf, uint16_t nbytes)
{
     uint16_t i;
     char * p;
     p = (char*)buf;
     for(i=0; i < nbytes; i++){
          if( f->tail != f->head ){ //see if any data is available
               *p++ = f->buf[f->tail];  //grab a byte from the buffer
               f->tail++;  //increment the tail
               if( f->tail == f->size ){  //check for wrap-around
                    f->tail = 0;
               }
          } else {
               return i; //number of bytes read 
          }
     }
     return nbytes;
}
 
//This writes up to nbytes bytes to the FIFO
//If the head runs in to the tail, not all bytes are written
//The number of bytes written is returned
uint16_t fifo_write(fifo_t * f, const void * buf, uint16_t nbytes)
{
     uint16_t i;
     const char * p;
     p = (char*)buf;
     for(i=0; i < nbytes; i++){
           //first check to see if there is space in the buffer
           if( (f->head + 1 == f->tail) ||
                ( (f->head + 1 == f->size) && (f->tail == 0) ) ){
                 return i; //no more room
           } else {
               f->buf[f->head] = *p++;
               f->head++;  //increment the head
               if( f->head == f->size ){  //check for wrap-around
                    f->head = 0;
               }
           }
     }
     return nbytes;
}

/* lpc17xx uart hlp driver inda gerekti.
 * veri fifodan okunuyor, gönderilmeye çalisiliyor basarili gönderilir ise tail artiriliyor.
 */
int fifo_read_ch(fifo_t * f, uint8_t * buf, uint8_t inc_tail)
{
	if( f->tail != f->head ){ //see if any data is available
		*buf = f->buf[f->tail];  //grab a byte from the buffer
		if (inc_tail) {
			f->tail++;  //increment the tail
			if( f->tail == f->size ){  //check for wrap-around
			    f->tail = 0;
			}
		}
		return (*buf);
	}
	return (-1);
}

/* fifo_read_ch() tamamliyor. */
void fifo_inc_tail(fifo_t * f)
{
	if( f->tail != f->head ) {
		/* overlap olamsin diye dikkat edelim. */
		f->tail++;  //increment the tail
		if( f->tail == f->size ){  //check for wrap-around
		    f->tail = 0;
		}
	}
}

/* eof */

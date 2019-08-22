#ifndef __RINGBUF_H__
#define __RINGBUF_H__

typedef struct 
{
		 unsigned char * rb_elements_buf;
		 unsigned char rb_element_size;
		 unsigned int rb_size;
	volatile unsigned int rb_read_ptr;
	volatile unsigned int rb_write_ptr;
} RB_TYPE;


#define	RB_OK                   0
#define	RB_NOT_ENOUGH_MEM       -1      /* no enouugh mem for buffer allocation */
#define	RB_BAD_PTR              -2
#define	RB_FULL                 -3
#define	RB_NOT_ENOUGH_ROOM      -4      /* no enough rom for adding required elements count */
#define	RB_EMPTY                -5
#define RB_NOT_INIT             -6


extern int rb_init(				RB_TYPE * rb_var,
								unsigned char element_size,
								unsigned int rb_size);

extern int rb_elements_count(	RB_TYPE * r_bfr);

extern int rb_xfer_1_element(	RB_TYPE * r_bfr,
								unsigned char * bfr);
extern int rb_write_1_element_overlapped(RB_TYPE * r_bfr, unsigned char * bfr);

extern int rb_xfer_x_elements(	RB_TYPE * r_bfr,
								unsigned int elements_count_to_xfer, 
								unsigned int * xferred_elements_count, 
								unsigned char * bfr);

extern int rb_write_1_element(	RB_TYPE * r_bfr,
								unsigned char * bfr);

extern int rb_write_x_elements(	RB_TYPE * r_bfr,
								unsigned int elements_count_to_write, 
								unsigned char * bfr);

extern int rb_read_x_elements(	RB_TYPE * r_bfr,
								unsigned int first_element_ptr, 
								unsigned int elements_count_to_read, 
								unsigned char * bfr);
extern int 	rb_free(RB_TYPE * r_bfr);
extern int	rb_reinit(RB_TYPE * r_bfr);


#endif

#include "ringbuf.h"
#include <stdlib.h>     /* malloc */
#include <string.h>     /* memcpy */

#define RB_DISABLE_IRQ_ON_MEMCPY        0


// ****************************************************************************
int rb_init(	RB_TYPE * rb_var,
				unsigned char element_size,
				unsigned int rb_size)
// ****************************************************************************
{
   unsigned int u4Size;

	rb_var->rb_read_ptr     = 0;
	rb_var->rb_write_ptr    = 0;
	rb_var->rb_size         = 0; /* altta taþýdým *//* yetersiz bellek durumunda daha sonraki komutlar hata vermesin diye. */
	rb_var->rb_element_size = element_size;

	u4Size = rb_size * element_size;
	//u4Size = ((u4Size + 3) & ~0x3);      /* round up to DWORD */

	rb_var->rb_elements_buf = malloc(u4Size);

	if (!rb_var->rb_elements_buf) 				// malloc returns NULL if no enough sistem memory
		return RB_NOT_ENOUGH_MEM;

	rb_var->rb_size 		= rb_size;
	return RB_OK;
}

// ****************************************************************************
int rb_elements_count(	RB_TYPE * r_bfr)
// ****************************************************************************
{
	if ( r_bfr->rb_write_ptr>r_bfr->rb_size || r_bfr->rb_read_ptr>r_bfr->rb_size ) 
		return RB_BAD_PTR;
	
	if ( r_bfr->rb_write_ptr >= r_bfr->rb_read_ptr ) 
		return (r_bfr->rb_write_ptr - r_bfr->rb_read_ptr);
	else 
		return( r_bfr->rb_size +r_bfr->rb_write_ptr - r_bfr->rb_read_ptr );
}

// ****************************************************************************
int rb_xfer_1_element(	RB_TYPE * r_bfr,
						unsigned char * bfr)
// ****************************************************************************
{
#if RB_DISABLE_IRQ_ON_MEMCPY
	unsigned int SaveInts;
#endif
																								
	if ( r_bfr->rb_write_ptr>r_bfr->rb_size || r_bfr->rb_read_ptr>r_bfr->rb_size ) 
		return RB_BAD_PTR;

	if (rb_elements_count(r_bfr)==0)
		return RB_EMPTY;

#if RB_DISABLE_IRQ_ON_MEMCPY
	SaveInts = VICIntEnable;
	VICIntEnClr = 0xFFFFFFFF;
#endif

	memcpy ( bfr, 
			(unsigned char *)(r_bfr->rb_elements_buf + r_bfr->rb_element_size*r_bfr->rb_read_ptr), 
			r_bfr->rb_element_size );

	if ( ++r_bfr->rb_read_ptr >= r_bfr->rb_size ) 
		r_bfr->rb_read_ptr=0;

#if RB_DISABLE_IRQ_ON_MEMCPY
	VICIntEnable = SaveInts;
#endif

	return RB_OK;
}

// ****************************************************************************
int rb_xfer_x_elements(	RB_TYPE * r_bfr,
						unsigned int elements_count_to_xfer, 
						unsigned int * xferred_elements_count, 
						unsigned char * bfr)
// ****************************************************************************
{
	static int k;
	
	*xferred_elements_count=0;

	k = rb_elements_count(r_bfr);
	
	if (k<0)
		return k;
	
	if (!k)
		return RB_OK;//RB_EMPTY;
	
	do
	{
		k = rb_xfer_1_element(r_bfr, bfr+((*xferred_elements_count)* r_bfr->rb_element_size));
		switch (k)
		{
			case RB_OK:
				(*xferred_elements_count)++;
				break;
			
			case RB_EMPTY:
				return RB_OK;

			default:
				return k;

		}
	}while (((*xferred_elements_count)<elements_count_to_xfer) && (rb_elements_count(r_bfr)>0) );
	
	return RB_OK;
}

// ****************************************************************************
int rb_write_1_element(	RB_TYPE * r_bfr,
						unsigned char * bfr)
// ****************************************************************************
{
	static int k;
#if RB_DISABLE_IRQ_ON_MEMCPY
	unsigned int SaveInts;
#endif

	k = rb_elements_count(r_bfr);
	if (k<0)
		return k;

	if (k == r_bfr->rb_size - 1)
		return RB_FULL;

#if RB_DISABLE_IRQ_ON_MEMCPY
	SaveInts = VICIntEnable;
	VICIntEnClr = 0xFFFFFFFF;
#endif

	memcpy ( (unsigned char *)(r_bfr->rb_elements_buf + r_bfr->rb_element_size*r_bfr->rb_write_ptr), bfr, r_bfr->rb_element_size );

	if ( ++r_bfr->rb_write_ptr >= r_bfr->rb_size )
		r_bfr->rb_write_ptr=0;
#if RB_DISABLE_IRQ_ON_MEMCPY
	VICIntEnable = SaveInts;
#endif
	return RB_OK;
}

// ****************************************************************************
int rb_write_1_element_overlapped(	RB_TYPE * r_bfr,
						unsigned char * bfr)
// ****************************************************************************
{
	static int k;
#if RB_DISABLE_IRQ_ON_MEMCPY
	unsigned int SaveInts;
#endif

	k = rb_elements_count(r_bfr);
	if (k<0)
		return k;

// 	if (k == r_bfr->rb_size - 1)
// 		return RB_FULL;

#if RB_DISABLE_IRQ_ON_MEMCPY
	SaveInts = VICIntEnable;
	VICIntEnClr = 0xFFFFFFFF;
#endif

	memcpy ( (unsigned char *)(r_bfr->rb_elements_buf + r_bfr->rb_element_size*r_bfr->rb_write_ptr), bfr, r_bfr->rb_element_size );
	
	if ( ++r_bfr->rb_write_ptr >= r_bfr->rb_size )
		r_bfr->rb_write_ptr=0;

//++++++++
   if (r_bfr->rb_write_ptr == r_bfr->rb_read_ptr)
      if ( ++r_bfr->rb_read_ptr >= r_bfr->rb_size )
         r_bfr->rb_read_ptr=0;
//--------

#if RB_DISABLE_IRQ_ON_MEMCPY
	VICIntEnable = SaveInts;
#endif
	return RB_OK;
}

// ****************************************************************************
int rb_write_x_elements(RB_TYPE * r_bfr,
						unsigned int elements_count_to_write, 
						unsigned char * bfr)
// ****************************************************************************
{
	static int k, elements_written;

	if (!elements_count_to_write) 
		return RB_OK;
	
	k = rb_elements_count(r_bfr);

	if (k<0)
		return k;	

	if (k == r_bfr->rb_size - 1)
		return RB_FULL;

	if ((k+elements_count_to_write) > r_bfr->rb_size)
		return RB_NOT_ENOUGH_ROOM;

	elements_written=0;

	do
	{
		k=rb_write_1_element ( r_bfr, bfr + elements_written * r_bfr->rb_element_size );
		if (k) 
			return k;
	}while(++elements_written<elements_count_to_write);
	


	return RB_OK;
}

////////////////////////////////////////////////////////////////////////////////
int rb_free(RB_TYPE * r_bfr)
{
	if (!r_bfr->rb_elements_buf)
		return RB_NOT_INIT;

	r_bfr->rb_read_ptr			=
	r_bfr->rb_write_ptr			=
	r_bfr->rb_size 				=
	r_bfr->rb_element_size 	    = 0;
	
	free(r_bfr->rb_elements_buf);
	r_bfr->rb_elements_buf = NULL;
	return RB_OK;
}

////////////////////////////////////////////////////////////////////////////////
int rb_reinit(RB_TYPE * r_bfr)
{
	if (!r_bfr->rb_elements_buf)
		return RB_NOT_INIT;

	r_bfr->rb_read_ptr		= 0;
	r_bfr->rb_write_ptr	    = 0;
	return RB_OK;
}


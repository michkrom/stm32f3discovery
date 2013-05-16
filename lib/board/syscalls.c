/***********************************************************************/
/*                                                                     */
/*  SYSCALLS.C:  System Calls Remapping for gcc/newlib                 */
/*  most of this is from newlib-lpc and a Keil-demo                    */
/*                                                                     */
/*  these are "reentrant functions" as needed by                       */
/*  the WinARM-newlib-config, see newlib-manual                        */
/*  collected and modified by Martin Thomas                            */
/*  TODO: some more work has to be done on this                        */
/***********************************************************************/

<<<<<<< HEAD
<<<<<<< HEAD
=======
#include <stdlib.h>
#include <reent.h>
#include <sys/stat.h>
>>>>>>> d7c7073da17cfc360f35d3ebd6555c81e3cfc5cb
=======
#include <stdlib.h>
#include <reent.h>
#include <sys/stat.h>
>>>>>>> d7c7073da17cfc360f35d3ebd6555c81e3cfc5cb
#include "serial.h"
  
#include "stm32f30x_conf.h"
#include "stm32f30x.h"

#include "stm32f30x_rcc.h"

<<<<<<< HEAD

//******************************************************************************
// Hosting of stdio functionality through USART1
//******************************************************************************
 
#include <stdio.h>

#ifdef UVISION

#include <rt_misc.h>
 
#pragma import(__use_no_semihosting_swi)
 
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
 
int fputc(int ch, FILE *f)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
  USART_SendData(USART2, ch);
  return(ch);
}
 
int fgetc(FILE *f)
{
  char ch;
  while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET); 
  ch = USART_ReceiveData(USART2);
	return((int)ch);
}
 
int ferror(FILE *f)
{
  /* Your implementation of ferror */
  return EOF;
}
 
void _ttywrch(int ch)
{
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
  USART_SendData(USART2, ch);
}

#else

#include <stdlib.h>
#include <reent.h>
#include <sys/stat.h>

unsigned __errno;
=======
unsigned __errno;


// new code for _read_r provided by Alexey Shusharin - Thanks
_ssize_t _read_r(struct _reent *r, int file, void *ptr, size_t len)
{
  char c;
  int  i;
  unsigned char *p;
>>>>>>> d7c7073da17cfc360f35d3ebd6555c81e3cfc5cb

  p = (unsigned char*)ptr;

<<<<<<< HEAD
// new code for _read_r provided by Alexey Shusharin - Thanks
_ssize_t _read_r(struct _reent *r, int file, void *ptr, size_t len)
{
  char c;
  int  i;
  unsigned char *p;

  p = (unsigned char*)ptr;

  for (i = 0; i < len; i++)
  {      
    c = USART_ReceiveData(USART2);
    *p++ = c;
    USART_SendData(USART2, c);

=======
  for (i = 0; i < len; i++)
  {      
    c = USART_ReceiveData(USART2);
    *p++ = c;
    USART_SendData(USART2, c);

>>>>>>> d7c7073da17cfc360f35d3ebd6555c81e3cfc5cb
    if (c == 0x0D && i <= (len - 2))
    {
      *p = 0x0A;
      USART_SendData(USART2, 0x0A);
      return i + 2;
    }
  }
<<<<<<< HEAD
<<<<<<< HEAD

#endif
=======
=======
>>>>>>> d7c7073da17cfc360f35d3ebd6555c81e3cfc5cb
  return i;
}


_ssize_t _write_r (
    struct _reent *r, 
    int file, 
    const void *ptr, 
    size_t len)
{
	int i;
	const unsigned char *p;
	
	p = (const unsigned char*) ptr;
	
	for (i = 0; i < len; i++) {
		if (*p == '\n' ) USART_SendData(USART2,'\r');
		USART_SendData(USART2, *p++);
	}
	
	return len;
}


int _close_r(
    struct _reent *r, 
    int file)
{
	return 0;
}


_off_t _lseek_r(
    struct _reent *r, 
    int file, 
    _off_t ptr, 
    int dir)
{
	return (_off_t)0;	/*  Always indicate we are at file beginning.	*/
}


int _fstat_r(
    struct _reent *r, 
    int file, 
    struct stat *st)
{
	/*  Always set as character device.				*/
	st->st_mode = S_IFCHR;	
		/* assigned to strong type with implicit 	*/
		/* signed/unsigned conversion.  Required by 	*/
		/* newlib.					*/

	return 0;
}

int isatty(int file); /* avoid warning */

int _isatty(int file)
{
	return 1;
}

#if 0
static void _exit (int n) {
label:  goto label; /* endless loop */
}
#endif 

/* "malloc clue function" */

/**** Locally used variables. ****/
extern char end[];              /*  end is set in the linker command 	*/
				/* file and is the end of statically 	*/
				/* allocated data (thus start of heap).	*/

static char *heap_ptr;		/* Points to current end of the heap.	*/

/************************** _sbrk_r *************************************/
/*  Support function.  Adjusts end of heap to provide more memory to	*/
/* memory allocator. Simple and dumb with no sanity checks.		*/
/*  struct _reent *r	-- re-entrancy structure, used by newlib to 	*/
/*			support multiple threads of operation.		*/
/*  ptrdiff_t nbytes	-- number of bytes to add.			*/
/*  Returns pointer to start of new heap area.				*/
/*  Note:  This implementation is not thread safe (despite taking a	*/
/* _reent structure as a parameter).  					*/
/*  Since _s_r is not used in the current implementation, the following	*/
/* messages must be suppressed.						*/

void * _sbrk_r(
    struct _reent *_s_r, 
    ptrdiff_t nbytes)
{
	char  *base;		/*  errno should be set to  ENOMEM on error	*/

	if (!heap_ptr) {	/*  Initialize if first time through.		*/
		heap_ptr = end;
	}
	base = heap_ptr;	/*  Point to end of heap.			*/
	heap_ptr += nbytes;	/*  Increase heap.				*/
	
	return base;		/*  Return pointer to start of new heap area.	*/
}
<<<<<<< HEAD
>>>>>>> d7c7073da17cfc360f35d3ebd6555c81e3cfc5cb
=======
>>>>>>> d7c7073da17cfc360f35d3ebd6555c81e3cfc5cb

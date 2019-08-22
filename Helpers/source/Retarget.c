#include <stdio.h>
#include <string.h>
#include <rt_sys.h>
//#include <File_Config.h>
#include "lpc17xx_libcfg_default.h"
#ifdef _UART
#ifdef _UART0
	#include "hlp_uart.h"
#endif
#endif

#pragma import(__use_no_semihosting_swi)

/* The following macro definitions may be used to translate this file:

  STDIO - use standard Input/Output device
          (default is NOT used)
 */

#ifdef _UART
#ifdef _UART0
	#define STDIO               1
#endif
#endif

//#define FLASH_FS_ENABLE     1   /* rl-arm flashfs file system supprot */

/* Standard IO device handles. */
#define STDIN   0x8001
#define STDOUT  0x8002
#define STDERR  0x8003

typedef unsigned char U8;
typedef unsigned int U32;

/* Standard IO device name defines. */
const char __stdin_name[]  = "STDIN";
const char __stdout_name[] = "STDOUT";
const char __stderr_name[] = "STDERR";

struct __FILE { int handle; /* Add whatever you need here */ };

#ifdef STDIO
/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int sendchar (int c) {

  if (c == '\n')  {
    UART0_PutChar(0x0D);
  }
  UART0_PutChar(c);

  return (c);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int getkey (void) {

  return (UART0_GetChar());
}
#endif

/*--------------------------- _ttywrch --------------------------------------*/

void _ttywrch (int ch) {
#ifdef STDIO
  sendchar(ch);
#endif
}

/*--------------------------- _sys_open -------------------------------------*/

FILEHANDLE _sys_open (const char *name, int openmode) {
  /* Register standard Input Output devices. */
  if (strcmp(name, "STDIN") == 0) {
    return (STDIN);
  }
  if (strcmp(name, "STDOUT") == 0) {
    return (STDOUT);
  }
  if (strcmp(name, "STDERR") == 0) {
    return (STDERR);
  }
#if FLASH_FS_ENABLE
  return (__sys_open (name, openmode));
#else
    return -1;
#endif
}

/*--------------------------- _sys_close ------------------------------------*/

int _sys_close (FILEHANDLE fh) {
  if (fh > 0x8000) {
    return (0);
  }
#if FLASH_FS_ENABLE
  return (__sys_close (fh));
#else
    return -1;
#endif
}

/*--------------------------- _sys_write ------------------------------------*/

int _sys_write (FILEHANDLE fh, const U8 *buf, U32 len, int mode) {
#ifdef STDIO
  if (fh == STDOUT) {
    /* Standard Output device. */
    for (  ; len; len--) {
      sendchar (*buf++);
    }
    return (0);
  }
#endif
  if (fh > 0x8000) {
    return (-1);
  }
#if FLASH_FS_ENABLE
  return (__sys_write (fh, buf, len));
#else
    return -1;
#endif
}

/*--------------------------- _sys_read -------------------------------------*/

int _sys_read (FILEHANDLE fh, U8 *buf, U32 len, int mode) {
#ifdef STDIO
  if (fh == STDIN) {
    /* Standard Input device. */
    for (  ; len; len--) {
      *buf++ = getkey ();
    }
    return (0);
  }
#endif
  if (fh > 0x8000) {
    return (-1);
  }
#if FLASH_FS_ENABLE
  return (__sys_read (fh, buf, len));
#else
    return -1;
#endif
}

/*--------------------------- _sys_istty ------------------------------------*/

int _sys_istty (FILEHANDLE fh) {
  if (fh > 0x8000) {
    return (1);
  }
  return (0);
}

/*--------------------------- _sys_seek -------------------------------------*/

int _sys_seek (FILEHANDLE fh, long pos) {
  if (fh > 0x8000) {
    return (-1);
  }
#if FLASH_FS_ENABLE
  return (__sys_seek (fh, pos));
#else
    return -1;
#endif
}

/*--------------------------- _sys_ensure -----------------------------------*/

int _sys_ensure (FILEHANDLE fh) {
// #ifdef STDIO
//   if (fh == STDOUT) {
// 	sendchar('\n');
//     /* Standard Output device. */
//     while(uart0_tx_empty() != 1)
// 	;
//     return (0);
//   }
// #endif

  if (fh > 0x8000) {
    return (-1);
  }
#if FLASH_FS_ENABLE
  return (__sys_ensure (fh));
#else
    return -1;
#endif
}

/*--------------------------- _sys_flen -------------------------------------*/

long _sys_flen (FILEHANDLE fh) {
  if (fh > 0x8000) {
    return (0);
  }
#if FLASH_FS_ENABLE
  return (__sys_flen (fh));
#else
    return -1;
#endif
}

/*--------------------------- _sys_tmpnam -----------------------------------*/

int _sys_tmpnam (char *name, int sig, unsigned maxlen) {
  return (1);
}

/*--------------------------- _sys_command_string ---------------------------*/

char *_sys_command_string (char *cmd, int len) {
  return (cmd);
}

/*--------------------------- _sys_exit -------------------------------------*/

void _sys_exit (int return_code) {
  /* Endless loop. */
  while (1);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

#define TEST_XMODEM_SEND

#include <stdio.h>
#include <limits.h>     // For INT_MAX
#include <stdlib.h>     // Temporary debug for exit()

#include "xmodem.h"

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>

int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf ("error %d setting term attributes", errno);
}


char portname[] = "/dev/ttyUSB0";

int sp;

/**
 * @brief Timed read of one byte from device associated with the global filedescriptor sp
 *
 * @param timeout The timeout to read a byte (in ms)
 *
 * @return The byte read, or -1 if there was a failure or -2 if no byte was available after the timeout was reached
**/
int _inbyte(unsigned short timeout) { /* timeout in ms */
    unsigned char buf;
    
    ssize_t n = read(sp, &buf, 1);
    if (n<0) {
        printf("Error %d while reading from serial port\n", errno);
    }
    if (n<=0) {
        return -1;
    }
    return buf;
}

/**
 * @brief Write one byte to the device associated with the global filedescriptor sp
 *
 * @param c The byte to write
 *
 * @note Parameter @p c should have a value between 0x00 and 0xff included
**/
void _outbyte(int c) {
    
    unsigned char byte = c;
    write(sp, &byte, 1);
}

/**
 * @brief Read a chunk of xmodemSize bytes from the input image file
 *
 * @param[in] context A FILE* handle to the input image file (casted as void*)
 * @param[in] xmodemBuffer A pointer to the memory to fill with the chunk read from the input file
 * @param xmodemSize The size of the chunk to read
**/
void myFetchChunk(void *context, void *xmodemBuffer, int xmodemSize) {
    
    FILE* fp = (FILE*)context;
    
    printf("Asked to read %d bytes from input file pointer %p\n", xmodemSize, fp);
    size_t bytes_read = fread(xmodemBuffer, 1, xmodemSize, fp);
    if (bytes_read != xmodemSize) {
        printf("Short read: %u bytes\n", bytes_read);
        if (feof(fp)) {
            printf("EOF reached\n");
        }
        printf("Read error\n");
        exit(1);
    }
}

#ifdef TEST_XMODEM_RECEIVE
/**
 * Note: this code is obsolete (it uses hardcoded memory areas, and should be adapter for Linux
 * Only yhe sending code has been adapted
**/
int main(void)
{
	int st;

	printf ("Send data using the xmodem protocol from your terminal emulator now...\n");
	/* the following should be changed for your environment:
	   0x30000 is the download address,
	   65536 is the maximum size to be written at this address
	 */
	st = XmodemReceive((char *)0x30000, 65536);
	if (st < 0) {
		printf ("Xmodem receive error: status: %d\n", st);
	}
	else  {
		printf ("Xmodem successfully received %d bytes\n", st);
	}

	return 0;
}
#endif
#ifdef TEST_XMODEM_SEND
int main(void)
{
	int st;
	int size;

	FILE* fp;

	sp = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (sp < 0) {
		printf ("error %d opening %s: %s", errno, portname, strerror (errno));
		return -1;
	}

	set_interface_attribs (sp, B115200, 0);  // set speed to 115200 bps, 8n1 (no parity)
	set_blocking (sp, 0);                // set no blocking

	char filename[] = "./firmware.gbl";
	fp = fopen(filename,"rb");
	fseek(fp, 0L, SEEK_END);
	long sz = ftell(fp);

	if (sz>INT_MAX)
		printf ("File is too large for transfer\n");
	else
		size = (int)sz;

	rewind(fp);
	printf ("Opening %s as handle %p\n", filename, fp);

	printf ("Prepare your terminal emulator to receive data now...\n");
	st = XmodemTransmit128b(myFetchChunk, (void *)fp, size);
	if (st < 0) {
		printf ("Xmodem transmit error: status: %d\n", st);
		return -1;
	}
	else  {
		printf ("Xmodem successfully transmitted %d bytes\n", st);
	}

	return 0;
}
#endif

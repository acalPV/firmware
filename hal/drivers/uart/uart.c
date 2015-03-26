//
// Copyright 2014 Per Vices Corporation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "uart.h"


// Maximum before a UART command is considered a fail
#define TIMEOUT 1000000	// us, 1.0 seconds

// Minimum time between UART commands
#define TIME_INTERVAL 35000 // us, 0.035 seconds

static struct timeval tprev;	// time since previous UART command
static struct timeval tstart;	// time since the beginning of a UART send attempt
static struct timeval tend;

// Options passed from server.c
static uint8_t _options = 0;

void set_uart_debug_opt(uint8_t options) {
	_options = options;
}

// return 1 if timeout, 0 if not
static uint8_t timeout(struct timeval* t, long int time) {
	gettimeofday(&tend, NULL);
	if ( ((tend.tv_usec + 1000000 * tend.tv_sec)
		- (t->tv_usec + 1000000 * t->tv_sec) - 26) > time)
		return 1;
	else
		return 0;
}

int set_uart_interface_attribs (int fd, int speed, int parity)
{
	gettimeofday(&tprev, NULL);	// on config, reset the prev timer

        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
		fprintf(stderr, "%s(): ERROR, %s\n", __func__, strerror(errno));
                return RETURN_ERROR;
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
		fprintf(stderr, "%s(): ERROR, %s\n", __func__, strerror(errno));
                return RETURN_ERROR;
        }
        return RETURN_SUCCESS;
}

void set_uart_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
		fprintf(stderr, "%s(): ERROR, %s\n", __func__, strerror(errno));
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
		fprintf(stderr, "%s(): ERROR, %s\n", __func__, strerror(errno));

        return;
}

int recv_uart(int fd, uint8_t* data, uint16_t* size, uint16_t max_size) {
	gettimeofday(&tstart, NULL);

	int rd_len = 0;
	while (!rd_len && !timeout(&tstart, TIMEOUT)) {
		rd_len += read(fd, data, max_size);
	}

	/*int i;
	printf("got %i characters, uart: ", rd_len);
	for (i = 0; i < rd_len; i++) printf("%c", data[i]);
	printf("\n");*/

	// if nothing to be read
	if (rd_len < 0) rd_len = 0;

	if (timeout(&tstart, TIMEOUT)) {
		printf("%s(): timedout\n", __func__);
		*size = rd_len;
		return RETURN_ERROR_UART_TIMEOUT;
	}

	*size = rd_len;
	return RETURN_SUCCESS;
}

int send_uart(int fd, uint8_t* data, uint16_t size) {
	gettimeofday(&tstart, NULL);

	// clear receive buffer first, for old data from previous commands
	flush_uart(fd);

	if (_options & SERVER_DEBUG_OPT)
		printf("%s(): %s\n", __func__, data);

	while (!timeout(&tprev, TIME_INTERVAL)){}

	int wr_len = 0;
	while (wr_len != size && !timeout(&tstart, TIMEOUT)) {
		wr_len += write(fd, data + wr_len, size - wr_len);
	}

	gettimeofday(&tprev, NULL);
	if (timeout(&tstart, TIMEOUT)) {
		printf("%s(): timedout\n", __func__);
		return RETURN_ERROR_UART_TIMEOUT;
	}

	return RETURN_SUCCESS;
}

int flush_uart(int fd) {
	// flushes UART on HPS side
	if( tcflush(fd, TCIOFLUSH) == 0)
		return RETURN_SUCCESS;
	else
		return RETURN_ERROR_UART_FLUSH;
}

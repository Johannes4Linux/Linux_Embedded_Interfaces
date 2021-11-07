#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

uint8_t mode = 0;
uint32_t speed = 500000;
uint8_t bits = 8;

/**
 * @brief function makes a SPI transfer
 *
 * @param fd		File descriptor to SPI bus
 * @param data		Data array with output (write) data
 *					will be overwritten with the received input data
 *	@param length	Length of the data array
 */
void spi_transfer(int fd, uint8_t *data, int length) {
	struct spi_ioc_transfer spi[length];
	int i;

	/* Setup transfer struct */
	for (i=0; i<length; i++) {
		memset(&spi[i], 0, sizeof(struct spi_ioc_transfer));
		spi[i].tx_buf = (unsigned long) (data+i);
		spi[i].rx_buf = (unsigned long) (data+i);
		spi[i].len = 1;
		spi[i].speed_hz = speed;
		spi[i].bits_per_word = bits;
	}

	/* Let's do the transfer */
	if(ioctl(fd, SPI_IOC_MESSAGE(length), spi) < 0) {
		perror("Error transfering data over SPI bus");
		close(fd);
		exit(-1);
	}
}

int main(int argc, char * argv[]) {
	int fd;
	uint8_t data[3];

	/* Open the SPI bus file descriptor */
	fd = open("/dev/spidev0.0", O_RDWR);
	if(fd < 0) {
		perror("Error opening SPI Bus");
		return -1;
	}

	/* Setup of the SPI Bus */
	if(ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
		perror("Error setting the SPI mode");
		close(fd);
		return -1;
	}
	if(ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
		perror("Error setting the SPI speed");
		close(fd);
		return -1;
	}
	if(ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
		perror("Error setting the SPI wordlength");
		close(fd);
		return -1;
	}

	/* Ready for SPI accesses */

	/* Setup GPIO's direction */
	data[0] = 0x40;
	data[1] = 0x0;
	data[2] = 0xfe;
	spi_transfer(fd, data, 3);


	/* Set LED */
	data[0] = 0x40;
	data[1] = 0xa;
	if(argc > 1) 
		data[2] = atoi(argv[1]) > 0;
	else
		data[2] = 0x0;
	spi_transfer(fd, data, 3);

	/* Read Button */
	data[0] = 0x41;
	data[1] = 0x9;
	data[2] = 0x00;
	spi_transfer(fd, data, 3);

	printf("Button is %s\n", ((data[2] & (1<<1)) > 0) ? "pressed" : "not pressed");

	close(fd);
	return 0;
}

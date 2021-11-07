#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int main(int argc, char *argv[]) {
	int i2c_bus;
	uint8_t data[2];

	/* Let's open the bus */
	i2c_bus = open("/dev/i2c-1", O_RDWR);
	if(i2c_bus < 0) {
		perror("Error opening I2C Bus");
		return -1;
	}

	/* Set the slave address */
	if(ioctl(i2c_bus, I2C_SLAVE, 0x20) < 0) {
		perror("Error setting slave address");
		close(i2c_bus);
		return -1;
	}

	/* Now let's access the bus */
	/* Set the GPIOA's direction */
	data[0] = 0x0;
	data[1] = 0xfe;

	if(write(i2c_bus, data, sizeof(data)) != 2) {
		perror("Error writing to I2C bus");
		close(i2c_bus);
		return -1;
	}

	/* Set the LED's state */
	data[0] = 0x14;
	if(argc > 1)
		data[1] = atoi(argv[1]) > 0;
	else
		data[1] = 0x0;
	
	if(write(i2c_bus, data, sizeof(data)) != 2) {
		perror("Error writing to I2C bus");
		close(i2c_bus);
		return -1;
	}

	/* Let's read the button's state */
	/* Frist write out the address */
	data[0] = 0x12;
	if(write(i2c_bus, data, 1) != 1) {
		perror("Error writing to I2C bus");
		close(i2c_bus);
		return -1;
	}

	/* Let's read the value back */
	if(read(i2c_bus, data, 1) != 1) {
		perror("Error reading from I2C bus");
		close(i2c_bus);
		return -1;
	}

	printf("Button is %s\n", (data[0] & (1<<1)) ? "Pressed" : "Not pressed");
	close(i2c_bus);
	return 0;
}





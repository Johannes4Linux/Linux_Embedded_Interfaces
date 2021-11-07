all: I2C_example SPI_example GPIO_example
	echo "All examples build"

I2C_example: I2C/mcp23017.c 
	gcc I2C/mcp23017.c -o I2C/mcp23017

SPI_example: SPI/mcp23s08.c 
	gcc SPI/mcp23s08.c -o SPI/mcp23s08

GPIO_example: GPIO/gpio_ctrl.c 
	gcc GPIO/gpio_ctrl.c -o GPIO/gpio_ctrl

clean:
	rm GPIO/gpio_ctrl SPI/mcp23s08 I2C/mcp23017


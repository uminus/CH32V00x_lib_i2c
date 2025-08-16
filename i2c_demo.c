/******************************************************************************
* Basic Example of using lib_i2c on the CH32V003 Microcontroller
*
*
* Demo Version 3.3    16 Aug 2025 
* See GitHub Repo for more information: 
* https://github.com/ADBeta/CH32V00x_lib_i2c
*
*
* Notes:
* When selecting a Clock Speed (on the CH32V003), Values between 10KHz and 1MHz
* work reliably and consistently. Values outside of this range can be sketchy
* 
* Alternatively, use one of the pre-defined Clock Speeds:
* I2C_CLK_10KHZ    I2C_CLK_50KHZ    I2C_CLK_100KHZ    I2C_CLK_400KHZ
* I2C_CLK_500KHZ   I2C_CLK_600KHZ   I2C_CLK_750KHZ    I2C_CLK_1MHZ
*
*
* To select the HW Pins used by the I2C Device is done via defining one of the
* following options - BEFORE THE HEADER IS INCLUDED.
* WARN: The best place to do this is in funconfig.h
*
* #define I2C_PINOUT_DEFAULT       SCL = PC2    SDA = PC1
* #define I2C_PINOUT_ALT_1         SCL = PD1    SDA = PD0
* #define I2C_PINOUT_ALT_2         SCL = PC5    SDA = PC6
*
*
* Read lib_i2c.h for documentation on each function and how to use them
*
* Released under the MIT Licence
* Copyright ADBeta (c) 2024 - 2025
******************************************************************************/
#include "ch32fun.h"
#include "lib_i2c.h"

#include <stdio.h>


// I2C Scan Callback example function. Prints the address which responded
void i2c_scan_callback(const uint8_t addr)
{
	printf("Address: 0x%02X Responded.\n", addr);
}


int main() 
{
	SystemInit();

	// Create an I2C Device Struct. This defines the I2C
	// * Clock Speed
	// * Address Type                 (7bit or 10bit)
	// * Address Value                (1 Byte for 7bit, 2 Bytes for 10bit)
	// * Register Bytes               (1, 2, 3 & 4 bytes supported)
	// * Timeout Count                (Number of clock cycles to wait for clock stretching)
	// NOTE: This demo example is using a DS3231 RTC
	i2c_device_t dev = {
		.clkr = I2C_CLK_400KHZ,
		.type = I2C_ADDR_7BIT,
		.addr = 0x68,
		.regb = 1,
		.tout = 2000,
	};

	// Initialise the I2C Interface
	if(i2c_init(&dev) != I2C_OK) printf("Failed to init the I2C Bus\n");

	// Initialising I2C causes the pins to transition from LOW to HIGH.
	// Wait 100ms to allow the I2C Device to timeout and ignore the transition.
	// Otherwise, an extra 1-bit will be added to the next transmission
	Delay_Ms(250);

	// Scan the I2C Bus, prints any devices that respond
	printf("----Scanning I2C Bus for Devices----\n");
	i2c_scan(i2c_scan_callback);
	printf("----Done Scanning----\n\n");

	
	// Keep track of the error status to debug issues.
	i2c_err_t i2c_stat;

	// Write to the -Seconds- Register (Reg 0x00, 0x00 Seconds)
	i2c_stat = i2c_write_reg(&dev, 0x00, (uint8_t[]){0x00}, 1);
	if(i2c_stat != I2C_OK) { printf("Error Using the I2C Bus\n"); return -1; }


	// Example of writing an array to a register.
	uint8_t array[3] = {0x00, 0x01, 0x02};
	i2c_stat = i2c_write_reg(&dev, 0x00, array, 3);
	if(i2c_stat != I2C_OK) { printf("Error Using the I2C Bus\n"); return -1; }


	// Example to read from the I2C Device
	uint8_t seconds = 0;    // Just Seconds (Read as Hex instead od Decimal)
	uint8_t time[3] = {0};  // Time in Sec, Min, Hrs (Hex not Decimal)
	while(1)
	{
		// Example reading just one byte
		i2c_stat = i2c_read_reg(&dev, 0x00, &seconds, 1);
		if(i2c_stat != I2C_OK) printf("Error Using the I2C Bus\n");
		// Print Seconds as a single hex byte
		printf("Seconds: %02X\n", seconds);

		
		// Example reading multiple bytes
		i2c_stat = i2c_read_reg(&dev, 0x00, time, 3);
		if(i2c_stat != I2C_OK) printf("Error Using the I2C Bus\n");
		// Print Time as Hrs Min Sec
		printf("Time: %02X:%02X:%02X\n\n", time[2], time[1], time[0]);


		// Wait 1 Second
		Delay_Ms(1000);
	}
}

/*
 * main.c
 *
 *  Created on: 12 мая 2018 г.
 *      Author: developer
 */
#include "humidity.h"

int main() {

	while (1) {
		float humidity, temperature;
		am2320_errors_t err = 0;
		err = am2320_get_data(&humidity, &temperature);
		printf("error: %d   h: %.1f   t: %.1f\n", err, humidity, temperature);
		sleep(1);
	}
	return 0;
}



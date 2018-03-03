#include "devctl.h"

#include "granum_common.h"

int8_t gr_dev_bmp280_reg_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t * buffer, uint8_t count) {
	gr_device_selector_t selector = {.pin = BMP280_SS};

	gr_dev_writereg(BMP280_IF, selector, reg_addr, buffer, count);
}

#ifndef DEVCTL_H_
#define DEVCTL_H_

//Уровень абстракции, маскирующий конкретные интерфейсы взаимодействия с устройствами на omnibus

#include "granum_common.h"

typedef enum {
	GR_DEVID_RASPBERRY,
	GR_DEVID_RADIO,
	GR_DEVID_GPS,
	GR_DEVID_MPU6000,
	GR_DEVID_LSM,
	GR_DEVID_BMP280,
	GR_DEVID_TSL0,
	GR_DEVID_TSL1,
	GR_DEVID_TSL2,
} gr_devid_t;

//Функции для работы с регистровыми устройствами (датчики)
gr_error_t gr_dev_readreg(gr_devid_t devid, size_t address, void * buffer, size_t count);
gr_error_t gr_dev_writereg(gr_devid_t devid, size_t address, void * buffer, size_t count);

//Функции для работы с потоковыми устройствами (uart raspberry, радиоканал)
gr_error_t gr_dev_receive(gr_devid_t devid, void * buffer, size_t count);
gr_error_t gr_dev_transmit(gr_devid_t devid, void * buffer, size_t count);

//Изменение состояния светодиода на плате
gr_error_t gr_dev_led(bool state);

#endif /* DEVCTL_H_ */

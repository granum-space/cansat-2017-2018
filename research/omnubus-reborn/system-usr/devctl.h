#ifndef DEVCTL_H_
#define DEVCTL_H_

//Уровень абстракции, маскирующий конкретные интерфейсы взаимодействия с устройствами на omnibus

#include "granum_common.h"

struct gr_interface_struct;
typedef struct gr_interface_struct gr_interface_t;

typedef struct {
	enum {
		GR_IF_PARAM_SPEED_SLOW,
		GR_IF_PARAM_SPEED_FAST,
	} speed;

} gr_interface_params_t;

extern gr_interface_t GR_IF_I2C_1;

//
gr_error_t gr_if_set_params(gr_interface_t * iface, gr_interface_params_t );

//Функции для работы с регистровыми устройствами (датчики)
gr_error_t gr_dev_readreg(gr_interface_t * iface, uint8_t dev_addr, uint8_t reg_addr, void * buffer, size_t count);
gr_error_t gr_dev_writereg(gr_interface_t * iface, uint8_t dev_addr, uint8_t reg_addr, void * buffer, size_t count);

//Функции для работы с потоковыми устройствами (uart raspberry, радиоканал)
gr_error_t gr_dev_receive(gr_interface_t * iface, void * buffer, size_t count);
gr_error_t gr_dev_transmit(gr_interface_t * iface, void * buffer, size_t count);

//Изменение состояния светодиода на плате
gr_error_t gr_dev_led(bool state);

#endif /* DEVCTL_H_ */

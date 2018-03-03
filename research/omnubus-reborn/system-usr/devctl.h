#ifndef DEVCTL_H_
#define DEVCTL_H_

//Уровень абстракции, маскирующий конкретные интерфейсы взаимодействия с устройствами на omnibus

#include "granum_common.h"

//
gr_error_t gr_if_set_params(gr_interface_t * iface, gr_interface_params_t * params);

//Функции для работы с регистровыми устройствами (датчики)
gr_error_t gr_dev_readreg(gr_interface_t * iface, gr_device_selector_t * selector, uint8_t reg_addr, void * buffer, size_t count);
gr_error_t gr_dev_writereg(gr_interface_t * iface, gr_device_selector_t * selector, uint8_t reg_addr, void * buffer, size_t count);

//Функции для работы с потоковыми устройствами (uart raspberry, радиоканал)
gr_error_t gr_dev_receive(gr_interface_t * iface, void * buffer, size_t count);
gr_error_t gr_dev_transmit(gr_interface_t * iface, void * buffer, size_t count);

//Изменение состояния светодиода на плате
gr_error_t gr_dev_led(bool state);

#endif /* DEVCTL_H_ */

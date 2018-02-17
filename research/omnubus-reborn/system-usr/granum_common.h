/*
 * granum_common.h
 *
 *  Created on: 10 февр. 2018 г.
 *      Author: developer
 */

#ifndef GRANUM_COMMON_H_
#define GRANUM_COMMON_H_

typedef enum {
	GR_ERROR_OK,
	GR_ERROR_WRONGARG,
} gr_error_t;

int gr_if_i2c_clockSpeeds[] = {100000, 100000, 100000};


#endif /* GRANUM_COMMON_H_ */

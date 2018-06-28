/*
 * common.h
 *
 *  Created on: 31 мая 2018 г.
 *      Author: developer
 */

#ifndef APPS_GRANUM_GRANUM_COMMON_H_
#define APPS_GRANUM_GRANUM_COMMON_H_

#include "router.h"

#define DEBUG (void)//printf

typedef enum {
	GR_STATE_BOOTING = 0,
	GR_STATE_READY,
	GR_STATE_LOADED,
	GR_STATE_FREE_FALL,
	GR_STATE_AWAITING_CHUTE,
	GR_STATE_PRE_LANDING,
	GR_STATE_LANDED,
} gr_state_t;

extern gr_state_t state;

typedef enum {
	GR_EVENT_ACTIVATION,
	GR_EVENT_EXIT,
	GR_EVENT_250m,
	GR_EVENT_CHUTE_GONE,
	GR_EVENT_EMERGENCY_LANDING,
	GR_EVENT_NEAR_GROUND,
	GR_EVENT_LANDED
} gr_event_t;

typedef enum {
	GR_SYSTEM_OMNIBUS = 1,
	GR_SYSTEM_RASPBERRY,
	GR_SYSTEM_GROUND,
} gr_system_t;

typedef enum {
	GR_COMPONENT_OMNIBUS_MAIN = 0,
	GR_COMPONENT_OMNIBUS_MADJWICK,
} gr_component_omnibus_t;

#endif /* APPS_GRANUM_GRANUM_COMMON_H_ */

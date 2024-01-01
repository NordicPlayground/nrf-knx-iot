/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef INCLUDE_OC_CONFIG_H_
#define INCLUDE_OC_CONFIG_H_

#include <stdint.h>
#include <time.h>

typedef uint64_t oc_clock_time_t;

#define OC_CLOCK_CONF_TICKS_PER_SECOND 1000
#define OC_MAX_NUM_DEVICES (1)
#define OC_MAX_APP_RESOURCES (4)
#define OC_MAX_NUM_CONCURRENT_REQUESTS (5)
#define OC_MAX_NUM_REP_OBJECTS (150)
#define OC_MAX_OBSERVE_SIZE 512
#define OC_MAX_NUM_ENDPOINTS (20)
#define OC_REQUEST_HISTORY
#define OC_DYNAMIC_ALLOCATION 1

// #define OC_DEBUG 1

#endif

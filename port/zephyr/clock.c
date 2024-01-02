/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <oc_clock.h>
#include <zephyr/kernel.h>

void
oc_clock_init(void)
{
    // Intentionally empty
}

oc_clock_time_t oc_clock_time(void)
{
    return k_uptime_get();
}
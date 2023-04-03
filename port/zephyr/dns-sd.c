/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdint.h>
#include <stdbool.h>

int
knx_publish_service(char *serial_no, uint32_t iid, uint32_t ia, bool pm)
{
    (void)serial_no;
    (void)iid;
    (void)ia;
    (void)pm;

    return 0;
}
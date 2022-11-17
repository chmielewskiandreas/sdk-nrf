/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <stdint.h>
#include <ztest.h>
#include <net/lwm2m_client_utils_fota.h>

int fota_update_counter_update(enum counter_type type, uint32_t new_value)
{
	ztest_check_expected_value(type);
	ztest_check_expected_value(new_value);
	return 0;
}

int fota_update_counter_read(struct update_counter *update_counter)
{
	update_counter->current = ztest_get_return_value();
	update_counter->update = ztest_get_return_value();
	return 0;
}
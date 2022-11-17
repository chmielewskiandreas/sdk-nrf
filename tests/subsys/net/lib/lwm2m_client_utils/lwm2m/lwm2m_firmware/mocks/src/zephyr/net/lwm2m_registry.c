/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <ztest.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/lwm2m.h>

LOG_MODULE_DECLARE(ztest, CONFIG_LOG_LEVEL_TEST);

int lwm2m_engine_set_u8(const char *pathstr, uint8_t value)
{
	ztest_check_expected_value(pathstr);
	ztest_check_expected_value(value);

	return 0;
}
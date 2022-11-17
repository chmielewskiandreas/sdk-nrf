/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <stdbool.h>
#include <ztest.h>

bool boot_is_img_confirmed(void)
{
	return ztest_get_return_value();
}

int boot_write_img_confirmed(void)
{
	return 0;
}
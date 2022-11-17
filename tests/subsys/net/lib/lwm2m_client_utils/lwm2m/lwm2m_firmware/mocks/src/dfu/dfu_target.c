/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <stddef.h>
#include <stdbool.h>
#include <ztest.h>
#include <dfu/dfu_target.h>

int dfu_target_init(int img_type, int img_num, size_t file_size, dfu_target_callback_t cb)
{
	return 0;
}

int dfu_target_img_type(const void *const buf, size_t len)
{
	return ztest_get_return_value();
}

int dfu_target_offset_get(size_t *offset)
{
	*offset = ztest_get_return_value();

	return ztest_get_return_value();
}

int dfu_target_write(const void *const buf, size_t len)
{
	return ztest_get_return_value();
}

int dfu_target_done(bool successful)
{
	return ztest_get_return_value();
}

int dfu_target_reset(void)
{
	return ztest_get_return_value();
}

int dfu_target_schedule_update(int img_num)
{
	return ztest_get_return_value();
}
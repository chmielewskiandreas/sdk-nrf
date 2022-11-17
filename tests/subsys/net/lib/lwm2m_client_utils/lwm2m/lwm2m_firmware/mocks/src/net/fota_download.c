/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <ztest.h>

#include <net/fota_download.h>

fota_download_callback_t test_client_callback = NULL;
bool fota_download_start_started = false;

int fota_download_init(fota_download_callback_t client_callback)
{
	test_client_callback = client_callback;
	return 0;
}

int fota_download_start(const char *host, const char *file, int sec_tag, uint8_t pdn_id,
			size_t fragment_size)
{
	fota_download_start_started = true;
	return 0;
}

int fota_download_target(void)
{
	return 0;
}

void test_fota_download_teardown()
{
	test_client_callback = NULL;
	fota_download_start_started = false;
}

bool test_fota_download_has_started()
{
	return fota_download_start_started;
}

void test_fota_download_throw_evt(enum fota_download_evt_id id)
{
	const struct fota_download_evt evt = { .id = id };
	test_client_callback(&evt);
}

void test_fota_download_throw_evt_error(enum fota_download_error_cause cause)
{
	const struct fota_download_evt evt = { .id = FOTA_DOWNLOAD_EVT_ERROR, .cause = cause };
	test_client_callback(&evt);
}
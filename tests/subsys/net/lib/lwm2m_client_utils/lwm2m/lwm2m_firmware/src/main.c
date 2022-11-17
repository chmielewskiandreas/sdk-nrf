/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/net/socket_offload.h>

#include <ztest.h>
#include <zephyr/logging/log.h>
#include <lwm2m_engine.h>
#include <net/fota_download.h>
#include <net/lwm2m_client_utils.h>
#include <net/lwm2m_client_utils_fota.h>

LOG_MODULE_REGISTER(ztest, CONFIG_LOG_LEVEL_TEST);

static char const FIRMWARE_UPDATE_PACKAGE_URI[] = "5/0/1";
static char const FIRMWARE_UPDATE_STATE[] = "5/0/3";

void client_acknowledge(void)
{
}

static void teardown()
{
	test_lwm2m_engine_teardown();
	test_fota_download_teardown();
}

static void reset_dfu_target()
{
	uint8_t state[1];

	state[0] = STATE_IDLE;

	teardown();

	ztest_returns_value(dfu_target_reset, 0);

	lwm2m_init_firmware();

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_STATE, 0, 0, 0, state, 0, 0, 0);
}

static void test_write_coap_uri_start_download(void)
{
	char const test_uri[] = "coap://10.1.0.10:5683/tmp/test.tmp";

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);

	lwm2m_init_firmware();

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));
}

static void test_write_coap_sec_uri_start_download(void)
{
	char const test_uri[] = "coaps://10.1.0.10:5684/tmp/test.tmp";

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);

	lwm2m_init_firmware();

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));
}

static void test_write_http_uri_start_download(void)
{
	char const test_uri[] = "http://10.1.0.10:80/tmp/test.tmp";

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);

	lwm2m_init_firmware();

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));
}

static void test_write_http_sec_uri_start_download(void)
{
	char const test_uri[] = "https://10.1.0.10:443/tmp/test.tmp";

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);

	lwm2m_init_firmware();

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));
}

static void test_write_uri_twice(void)
{
	char const test_uri[] = "coap://10.1.0.10:5683/tmp/test.tmp";

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_DOWNLOADING);

	lwm2m_init_firmware();

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));
}

static void test_write_invalid_uri_no_path(void)
{
	char const test_uri[] = "coap://10.1.0.10:5683";

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);

	lwm2m_init_firmware();

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));
}

static void test_write_invalid_uri_invalid_host(void)
{
	char const test_uri[] = "10.1.0.10:5683/tmp/test.tmp";

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);

	lwm2m_init_firmware();

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));
}

static void test_write_uri_reset_download(void)
{
	char const test_uri[] = "10.1.0.10:5683/tmp/test.tmp";

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_DOWNLOADED);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);

	lwm2m_init_firmware();

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri, 0, 1,
					     strlen(test_uri));
}

static void test_download_evt_progres(void)
{
	char const test_uri[] = "coap://10.1.0.10:5683/tmp/test.tmp";

	lwm2m_init_firmware();

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));

	test_fota_download_throw_evt(FOTA_DOWNLOAD_EVT_PROGRESS);
}

static void test_download_evt_cancelled(void)
{
	char const test_uri[] = "coap://10.1.0.10:5683/tmp/test.tmp";

	lwm2m_init_firmware();

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));

	test_fota_download_throw_evt(FOTA_DOWNLOAD_EVT_CANCELLED);
}

static void test_download_evt_finished(void)
{
	char const test_uri[] = "coap://10.1.0.10:5683/tmp/test.tmp";

	lwm2m_init_firmware();

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADED);

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));

	while (!test_fota_download_has_started()) {
		k_sleep(K_MSEC(1));
	}

	k_sleep(K_MSEC(10));

	test_fota_download_throw_evt(FOTA_DOWNLOAD_EVT_FINISHED);
}

static void test_download_evt_download_failed(void)
{
	char const test_uri[] = "coap://10.1.0.10:5683/tmp/test.tmp";

	lwm2m_init_firmware();

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_CONNECTION_LOST);

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));

	while (!test_fota_download_has_started()) {
		k_sleep(K_MSEC(1));
	}

	k_sleep(K_MSEC(10));

	test_fota_download_throw_evt_error(FOTA_DOWNLOAD_ERROR_CAUSE_DOWNLOAD_FAILED);
}

static void test_download_evt_download_invalid_update(void)
{
	char const test_uri[] = "coap://10.1.0.10:5683/tmp/test.tmp";

	lwm2m_init_firmware();

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_UNSUP_FW);

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));

	while (!test_fota_download_has_started()) {
		k_sleep(K_MSEC(1));
	}

	k_sleep(K_MSEC(10));

	test_fota_download_throw_evt_error(FOTA_DOWNLOAD_ERROR_CAUSE_INVALID_UPDATE);
}

static void test_download_evt_download_type_mismatch(void)
{
	char const test_uri[] = "coap://10.1.0.10:5683/tmp/test.tmp";

	lwm2m_init_firmware();

	ztest_returns_value(lwm2m_firmware_get_update_state, STATE_IDLE);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_DEFAULT);
	ztest_expect_value(lwm2m_firmware_set_update_state, state, STATE_DOWNLOADING);
	ztest_expect_value(lwm2m_firmware_set_update_result, result, RESULT_UNSUP_FW);

	test_lwm2m_engine_call_post_write_cb(FIRMWARE_UPDATE_PACKAGE_URI, 0, 0, 0, test_uri,
					     strlen(test_uri), 1, strlen(test_uri));

	while (!test_fota_download_has_started()) {
		k_sleep(K_MSEC(1));
	}

	k_sleep(K_MSEC(10));

	test_fota_download_throw_evt_error(FOTA_DOWNLOAD_ERROR_CAUSE_TYPE_MISMATCH);
}

static void test_lwm2m_init_image_successful(void)
{
	ztest_returns_value(fota_update_counter_read, COUNTER_UPDATE);
	ztest_returns_value(fota_update_counter_read, 1);
	ztest_returns_value(boot_is_img_confirmed, true);
	ztest_expect_value(lwm2m_engine_set_u8, pathstr, "5/0/5");
	ztest_expect_value(lwm2m_engine_set_u8, value, RESULT_SUCCESS);

	lwm2m_init_image();
}

static void test_lwm2m_init_image_failed(void)
{
	ztest_returns_value(fota_update_counter_read, COUNTER_UPDATE);
	ztest_returns_value(fota_update_counter_read, 2);
	ztest_returns_value(boot_is_img_confirmed, true);
	ztest_expect_value(lwm2m_engine_set_u8, pathstr, "5/0/5");
	ztest_expect_value(lwm2m_engine_set_u8, value, RESULT_UPDATE_FAILED);

	lwm2m_init_image();
}

static void test_apply_update(void)
{
	ztest_returns_value(lwm2m_firmware_get_update_state_inst, STATE_UPDATING);
	ztest_returns_value(fota_update_counter_read, COUNTER_CURRENT);
	ztest_returns_value(fota_update_counter_read, 0);
	ztest_expect_value(fota_update_counter_update, type, COUNTER_UPDATE);
	ztest_expect_value(fota_update_counter_update, new_value, 1);

	zassert_ok(lwm2m_firmware_apply_update(0), "applying firmware update should succeed");
}

static void test_apply_fw_update_partial(void)
{
	uint8_t data[20];

	ztest_returns_value(dfu_target_img_type, DFU_TARGET_IMAGE_TYPE_ANY);
	ztest_returns_value(dfu_target_offset_get, 0);
	ztest_returns_value(dfu_target_offset_get, 0);
	ztest_returns_value(dfu_target_write, 0);

	lwm2m_init_firmware();

	zassert_ok(test_lwm2m_firmware_firmware_block_received_cb(0, 0, 0, data, sizeof(data) / 2,
								  0, sizeof(data)),
		   "applying firmware update should succeed");
}

static void test_apply_fw_update_complete(void)
{
	uint8_t data[20];

	ztest_returns_value(dfu_target_img_type, DFU_TARGET_IMAGE_TYPE_ANY);
	ztest_returns_value(dfu_target_offset_get, 0);
	ztest_returns_value(dfu_target_offset_get, 0);
	ztest_returns_value(dfu_target_write, 0);

	ztest_returns_value(dfu_target_offset_get, sizeof(data) / 2);
	ztest_returns_value(dfu_target_offset_get, 0);
	ztest_returns_value(dfu_target_write, 0);
	ztest_returns_value(dfu_target_done, 0);
	ztest_returns_value(dfu_target_schedule_update, 0);

	lwm2m_init_firmware();

	zassert_ok(test_lwm2m_firmware_firmware_block_received_cb(0, 0, 0, data, sizeof(data) / 2,
								  0, sizeof(data)),
		   "apply first block of firmware update should succeed");
	zassert_ok(test_lwm2m_firmware_firmware_block_received_cb(0, 0, 0, data, sizeof(data) / 2,
								  1, sizeof(data)),
		   "apply last block of firmware update should succeed");
}

static void test_apply_fw_update_partial_invalid_dfu_offset(void)
{
	uint8_t data[20];

	ztest_returns_value(dfu_target_img_type, DFU_TARGET_IMAGE_TYPE_ANY);
	ztest_returns_value(dfu_target_offset_get, 10);
	ztest_returns_value(dfu_target_offset_get, 0);

	lwm2m_init_firmware();

	zassert_ok(test_lwm2m_firmware_firmware_block_received_cb(0, 0, 0, data, sizeof(data) / 2,
								  0, sizeof(data)),
		   "appyling block of firmware update to invalid dfu offset");
}

static void test_apply_fw_update_partial_invalid_dfu_write(void)
{
	uint8_t data[20];

	ztest_returns_value(dfu_target_img_type, DFU_TARGET_IMAGE_TYPE_ANY);
	ztest_returns_value(dfu_target_offset_get, 0);
	ztest_returns_value(dfu_target_offset_get, 0);
	ztest_returns_value(dfu_target_write, -1);
	ztest_returns_value(dfu_target_reset, 0);

	lwm2m_init_firmware();

	zassert_equal(test_lwm2m_firmware_firmware_block_received_cb(
			      0, 0, 0, data, sizeof(data) / 2, 0, sizeof(data)),
		      -1, "appyling block of firmware update, dfu write should fail");
}

static void test_apply_fw_update_partial_invalid_dfu_done(void)
{
	uint8_t data[20];

	ztest_returns_value(dfu_target_img_type, DFU_TARGET_IMAGE_TYPE_ANY);
	ztest_returns_value(dfu_target_offset_get, 0);
	ztest_returns_value(dfu_target_offset_get, 0);
	ztest_returns_value(dfu_target_write, 0);
	ztest_returns_value(dfu_target_done, -1);
	ztest_returns_value(dfu_target_reset, 0);

	lwm2m_init_firmware();

	zassert_equal(test_lwm2m_firmware_firmware_block_received_cb(0, 0, 0, data, sizeof(data), 1,
								     sizeof(data)),
		      -1, "appyling block of firmware update, dfu done should fail");
}

void test_main(void)
{
	ztest_test_suite(
		lwm2m_firmware_test,
		ztest_unit_test_setup_teardown(test_write_coap_uri_start_download, teardown,
					       teardown),
		ztest_unit_test_setup_teardown(test_write_coap_sec_uri_start_download,
					       unit_test_noop, teardown),
		ztest_unit_test_setup_teardown(test_write_http_uri_start_download, unit_test_noop,
					       teardown),
		ztest_unit_test_setup_teardown(test_write_http_sec_uri_start_download,
					       unit_test_noop, teardown),
		ztest_unit_test_setup_teardown(test_write_uri_twice, unit_test_noop, teardown),
		ztest_unit_test_setup_teardown(test_write_invalid_uri_no_path, unit_test_noop,
					       teardown),
		ztest_unit_test_setup_teardown(test_write_invalid_uri_invalid_host, unit_test_noop,
					       teardown),
		ztest_unit_test_setup_teardown(test_write_uri_reset_download, unit_test_noop,
					       teardown),
		ztest_unit_test_setup_teardown(test_download_evt_progres, unit_test_noop, teardown),
		ztest_unit_test_setup_teardown(test_download_evt_cancelled, unit_test_noop,
					       teardown),
		ztest_unit_test_setup_teardown(test_download_evt_finished, unit_test_noop,
					       teardown),
		ztest_unit_test_setup_teardown(test_download_evt_download_failed, unit_test_noop,
					       teardown),
		ztest_unit_test_setup_teardown(test_download_evt_download_invalid_update,
					       unit_test_noop, teardown),
		ztest_unit_test_setup_teardown(test_download_evt_download_type_mismatch,
					       unit_test_noop, teardown),
		ztest_unit_test_setup_teardown(test_lwm2m_init_image_successful, unit_test_noop,
					       unit_test_noop),
		ztest_unit_test_setup_teardown(test_lwm2m_init_image_failed, unit_test_noop,
					       unit_test_noop),
		ztest_unit_test_setup_teardown(test_apply_update, unit_test_noop, unit_test_noop),
		ztest_unit_test_setup_teardown(test_apply_fw_update_partial, unit_test_noop,
					       reset_dfu_target),
		ztest_unit_test_setup_teardown(test_apply_fw_update_complete, unit_test_noop,
					       reset_dfu_target),
		ztest_unit_test_setup_teardown(test_apply_fw_update_partial_invalid_dfu_offset,
					       unit_test_noop, reset_dfu_target),
		ztest_unit_test_setup_teardown(test_apply_fw_update_partial_invalid_dfu_write,
					       unit_test_noop, reset_dfu_target),
		ztest_unit_test_setup_teardown(test_apply_fw_update_partial_invalid_dfu_done,
					       unit_test_noop, reset_dfu_target)

	);

	ztest_run_test_suite(lwm2m_firmware_test);
}
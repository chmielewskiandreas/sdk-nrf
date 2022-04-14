/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <logging/log.h>
LOG_MODULE_REGISTER(download_client_test, LOG_LEVEL_DBG);

#include <zephyr.h>
#include "net_private.h"
#include <net/socket_offload.h>

#include <ztest.h>
#include <download_client.h>

#include "mock/socket.h"
#include "mock/dl_coap.h"

static struct download_client_cfg config = {
	.sec_tag = -1,
	.pdn_id = 0,
	.frag_size_override = 0,
};
static const char host[] = "coap://10.1.0.10";

static enum download_client_evt_id last_event = -1;

static int download_client_callback(const struct download_client_evt *event)
{
	if (event == NULL) {
		return -EINVAL;
	}

	switch (event->id) {
	case DOWNLOAD_CLIENT_EVT_FRAGMENT:
		last_event = DOWNLOAD_CLIENT_EVT_FRAGMENT;
		break;
	case DOWNLOAD_CLIENT_EVT_DONE:
		last_event = DOWNLOAD_CLIENT_EVT_DONE;
		break;
	case DOWNLOAD_CLIENT_EVT_ERROR:
		last_event = DOWNLOAD_CLIENT_EVT_ERROR;
		break;
	}

	return 0;
}

static enum download_client_evt_id get_last_download_client_event(bool reset)
{
	enum download_client_evt_id event;

	event = last_event;
	if (reset) {
		last_event = -1;
	}

	return event;
}

static void mock_return_values(const char *func, int32_t *val, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++) {
		z_ztest_returns_value(func, val[i]);
	}
}

static bool wait_for_event(enum download_client_evt_id event, uint8_t seconds)
{
	int i;

	for (i = 0; i < (20 * seconds); i++) {
		if (get_last_download_client_event(false) == event) {
			break;
		}

		k_sleep(K_MSEC(10));
	}

	return get_last_download_client_event(true) != event;
}

static void init(struct download_client *client)
{
	int err;

	memset(client, 0, sizeof(struct download_client));

	err = download_client_init(client, download_client_callback);
	zassert_ok(err, NULL);
	// FIXME: possible race condition; give time to client_init to start the dl thread
	k_sleep(K_SECONDS(1));

	err = download_client_connect(client, host, &config);
	zassert_ok(err, NULL);

	err = download_client_start(client, "no.file", 0);
	zassert_ok(err, NULL);
}

static void deinit(struct download_client *client)
{
	int err;

	err = download_client_disconnect(client);
	zassert_ok(err, NULL);
}

static void test_download_simple(void)
{
	struct download_client client;

	dl_coap_init(75, 20);

	int32_t recvfrom_parmas[] = { 25, 25, 25 };
	mock_return_values("mock_socket_offload_recvfrom", recvfrom_parmas,
			   sizeof(recvfrom_parmas) / sizeof(recvfrom_parmas[0]));
	int32_t sendto_parmas[] = { 20, 20, 20 };
	mock_return_values("mock_socket_offload_sendto", sendto_parmas,
			   sizeof(recvfrom_parmas) / sizeof(sendto_parmas[0]));

	init(&client);

	zassert_ok(wait_for_event(DOWNLOAD_CLIENT_EVT_DONE, 10), "Download must have finished");

	deinit(&client);
}

static void test_download_reconnect_on_socket_error(void)
{
	struct download_client client;

	dl_coap_init(75, 20);

	int32_t recvfrom_parmas[] = { 25, -1, 25, 25 };
	mock_return_values("mock_socket_offload_recvfrom", recvfrom_parmas,
			   sizeof(recvfrom_parmas) / sizeof(recvfrom_parmas[0]));
	int32_t sendto_parmas[] = { 20, 20, 20, 20 };
	mock_return_values("mock_socket_offload_sendto", sendto_parmas,
			   sizeof(recvfrom_parmas) / sizeof(sendto_parmas[0]));

	init(&client);

	zassert_ok(wait_for_event(DOWNLOAD_CLIENT_EVT_DONE, 10), "Download must have finished");

	deinit(&client);
}

static void test_download_reconnect_on_peer_close(void)
{
	struct download_client client;

	dl_coap_init(75, 20);

	int32_t recvfrom_parmas[] = { 25, 0, 25, 25 };
	mock_return_values("mock_socket_offload_recvfrom", recvfrom_parmas,
			   sizeof(recvfrom_parmas) / sizeof(recvfrom_parmas[0]));
	int32_t sendto_parmas[] = { 20, 20, 20, 20 };
	mock_return_values("mock_socket_offload_sendto", sendto_parmas,
			   sizeof(sendto_parmas) / sizeof(sendto_parmas[0]));

	init(&client);

	zassert_ok(wait_for_event(DOWNLOAD_CLIENT_EVT_DONE, 10), "Download must have finished");

	deinit(&client);
}

static void test_download_ignore_duplicate_block(void)
{
	struct download_client client;

	dl_coap_init(75, 20);

	int32_t recvfrom_parmas[] = { 25, 25, 25, 25 };
	mock_return_values("mock_socket_offload_recvfrom", recvfrom_parmas,
			   sizeof(recvfrom_parmas) / sizeof(recvfrom_parmas[0]));
	int32_t sendto_parmas[] = { 20, 20, 20 };
	mock_return_values("mock_socket_offload_sendto", sendto_parmas,
			   sizeof(sendto_parmas) / sizeof(sendto_parmas[0]));
	int32_t coap_parse_retv[] = { 0, 0, 1, 0 };
	mock_return_values("coap_parse", coap_parse_retv,
			   sizeof(coap_parse_retv) / sizeof(coap_parse_retv[0]));
	override_ret_values.func_coap_parse = true;

	init(&client);

	zassert_ok(wait_for_event(DOWNLOAD_CLIENT_EVT_DONE, 10), "Download must have finished");

	deinit(&client);
}

static void test_download_abort_on_invalid_block(void)
{
	struct download_client client;

	dl_coap_init(75, 20);

	int32_t recvfrom_parmas[] = { 25, 25, 25 };
	mock_return_values("mock_socket_offload_recvfrom", recvfrom_parmas,
			   sizeof(recvfrom_parmas) / sizeof(recvfrom_parmas[0]));
	int32_t sendto_parmas[] = { 20, 20, 20 };
	mock_return_values("mock_socket_offload_sendto", sendto_parmas,
			   sizeof(sendto_parmas) / sizeof(sendto_parmas[0]));
	int32_t coap_parse_retv[] = { 0, 0, -1 };
	mock_return_values("coap_parse", coap_parse_retv,
			   sizeof(coap_parse_retv) / sizeof(coap_parse_retv[0]));
	override_ret_values.func_coap_parse = true;

	init(&client);

	zassert_ok(wait_for_event(DOWNLOAD_CLIENT_EVT_ERROR, 10), "Download must be on error");

	deinit(&client);
}

static void test_download_continue_on_timeout(void)
{
	struct download_client client;

	dl_coap_init(75, 20);

	int32_t recvfrom_parmas[] = { 25, -1, 25, 25 };
	mock_return_values("mock_socket_offload_recvfrom", recvfrom_parmas,
			   sizeof(recvfrom_parmas) / sizeof(recvfrom_parmas[0]));
	int32_t sendto_parmas[] = { 20, 20, 20, 20 };
	mock_return_values("mock_socket_offload_sendto", sendto_parmas,
			   sizeof(sendto_parmas) / sizeof(sendto_parmas[0]));

	init(&client);

	zassert_ok(wait_for_event(DOWNLOAD_CLIENT_EVT_DONE, 10), "Download must have finished");

	deinit(&client);
}

void test_main(void)
{
	ztest_test_suite(lib_fota_download_test, ztest_unit_test(test_download_simple),
			 ztest_unit_test(test_download_reconnect_on_socket_error),
			 ztest_unit_test(test_download_reconnect_on_peer_close),
			 ztest_unit_test(test_download_ignore_duplicate_block),
			 ztest_unit_test(test_download_abort_on_invalid_block),
			 ztest_unit_test(test_download_continue_on_timeout));

	ztest_run_test_suite(lib_fota_download_test);
}

#define TEST_SOCKET_PRIO 40
NET_SOCKET_REGISTER(mock_socket, TEST_SOCKET_PRIO, AF_UNSPEC, mock_socket_is_supported,
		    mock_socket_create);
NET_DEVICE_OFFLOAD_INIT(mock_socket, "mock_socket", mock_nrf_modem_lib_socket_offload_init, NULL,
			&mock_socket_iface_data, NULL, 0, &mock_if_api, 1280);

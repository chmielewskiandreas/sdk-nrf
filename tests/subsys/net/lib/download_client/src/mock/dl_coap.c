/*
* Copyright (c) 2022 Nordic Semiconductor ASA
*
* SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
*/

#include <ztest.h>

#include <download_client.h>

#include "mock/dl_coap.h"

struct override_ret_values_t override_ret_values;
struct default_params_t default_params;

void dl_coap_init(size_t file_size, size_t coap_request_send_len)
{
	memset(&override_ret_values, 0, sizeof(override_ret_values));
	default_params.file_size = file_size;
	default_params.coap_request_send_len = coap_request_send_len;
	default_params.coap_request_send_timeout = 4000;
}

int socket_send(const struct download_client *client, size_t len, int timeout);

int coap_block_init(struct download_client *client, size_t from)
{
	if (override_ret_values.func_coap_block_init) {
		return ztest_get_return_value();
	}

	return 0;
}

int coap_get_recv_timeout(struct download_client *dl)
{
	if (override_ret_values.func_coap_get_recv_timeout) {
		return ztest_get_return_value();
	}

	return 4000;
}

int coap_initiate_retransmission(struct download_client *dl)
{
	if (override_ret_values.func_coap_initiate_retransmission) {
		return ztest_get_return_value();
	}

	return 0;
}

int coap_parse(struct download_client *client, size_t len)
{
	int ret = 0;

	if (client->file_size == 0) {
		client->file_size = default_params.file_size;
	}

	if (override_ret_values.func_coap_parse) {
		ret = ztest_get_return_value();
	}

	if (!ret) {
		client->progress += len;
	}

	return ret;
}

int coap_request_send(struct download_client *client)
{
	int err = 0;

	err = socket_send(client, default_params.coap_request_send_len,
			  default_params.coap_request_send_timeout);
	if (err) {
		return err;
	}

	return 0;
}
/*
* Copyright (c) 2022 grandcentrix GmbH
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef STUBS_H
#define STUBS_H

#include <zephyr/fff.h>
#include <zephyr/ztest.h>

#include <download_client.h>

DECLARE_FAKE_VALUE_FUNC(int, coap_block_init, struct download_client *, size_t);
DECLARE_FAKE_VALUE_FUNC(int, coap_get_recv_timeout, struct download_client *);
DECLARE_FAKE_VALUE_FUNC(int, coap_initiate_retransmission, struct download_client *);
DECLARE_FAKE_VALUE_FUNC(int, coap_parse, struct download_client *, size_t);
DECLARE_FAKE_VALUE_FUNC(int, coap_request_send, struct download_client *);
int coap_request_send_default(struct download_client *client);

DECLARE_FAKE_VALUE_FUNC(int, http_parse, struct download_client *, size_t);
DECLARE_FAKE_VALUE_FUNC(int, http_get_request_send, struct download_client *);

#define DO_FOREACH_FAKE(FUNC)                                                                      \
	do {                                                                                       \
		FUNC(coap_block_init)                                                              \
	} while (0)

#endif /* STUBS_H */

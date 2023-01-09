#include <zephyr/logging/log.h>

#include <stubs.h>

#include <zephyr/fff.h>
#include <zephyr/ztest.h>

LOG_MODULE_DECLARE(download_client_test);

DEFINE_FAKE_VALUE_FUNC(int, coap_block_init, struct download_client *, size_t);
DEFINE_FAKE_VALUE_FUNC(int, coap_get_recv_timeout, struct download_client *);
DEFINE_FAKE_VALUE_FUNC(int, coap_initiate_retransmission, struct download_client *);
DEFINE_FAKE_VALUE_FUNC(int, coap_parse, struct download_client *, size_t);
DEFINE_FAKE_VALUE_FUNC(int, coap_request_send, struct download_client *);
int coap_request_send_default(struct download_client *client)
{
	int err = 0;

	err = socket_send(client, 20, 4000);
	if (err) {
		return err;
	}

	return 20;
}

DEFINE_FAKE_VALUE_FUNC(int, http_parse, struct download_client *, size_t);
DEFINE_FAKE_VALUE_FUNC(int, http_get_request_send, struct download_client *);

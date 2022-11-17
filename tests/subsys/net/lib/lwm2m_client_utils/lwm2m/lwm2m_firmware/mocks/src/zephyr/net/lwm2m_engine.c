/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <zephyr/logging/log.h>
#include <zephyr/net/lwm2m.h>

LOG_MODULE_DECLARE(ztest, CONFIG_LOG_LEVEL_TEST);

#define MAX_CB_CNT 5
#define MAX_PATH_STR_LEN 10

uint8_t pre_write_cb_next = 0;
uint8_t post_write_cb_next = 0;

struct pre_write_cb {
	char path[MAX_PATH_STR_LEN];
	lwm2m_engine_get_data_cb_t cb;
};

struct post_write_cb {
	char path[MAX_PATH_STR_LEN];
	lwm2m_engine_set_data_cb_t cb;
};

struct pre_write_cb test_pre_write_cbs[MAX_CB_CNT];
struct post_write_cb test_post_write_cbs[MAX_CB_CNT];

int lwm2m_engine_register_pre_write_callback(const char *pathstr, lwm2m_engine_get_data_cb_t cb)
{
	if ((pre_write_cb_next + 1) == MAX_CB_CNT) {
		return -1;
	}

	strncpy(test_pre_write_cbs[pre_write_cb_next].path, pathstr, strlen(pathstr));
	test_pre_write_cbs[pre_write_cb_next].cb = cb;
	pre_write_cb_next++;

	return 0;
}

int lwm2m_engine_register_post_write_callback(const char *pathstr, lwm2m_engine_set_data_cb_t cb)
{
	if ((post_write_cb_next + 1) == MAX_CB_CNT) {
		return -1;
	}

	strncpy(test_post_write_cbs[post_write_cb_next].path, pathstr, strlen(pathstr));
	test_post_write_cbs[post_write_cb_next].cb = cb;
	post_write_cb_next++;

	return 0;
}

void test_lwm2m_engine_teardown()
{
	memset(test_pre_write_cbs, 0, sizeof(test_pre_write_cbs));
	pre_write_cb_next = 0;
	memset(test_post_write_cbs, 0, sizeof(test_post_write_cbs));
	post_write_cb_next = 0;
}

void test_lwm2m_engine_call_post_write_cb(const char *pathstr, uint16_t obj_inst_id,
					  uint16_t res_id, uint16_t res_inst_id, uint8_t *data,
					  uint16_t data_len, bool last_block, size_t total_size)
{
	uint8_t i = 0;

	for (i = 0; i < post_write_cb_next; i++) {
		if (!strncmp(pathstr, test_post_write_cbs[i].path, strlen(pathstr))) {
			test_post_write_cbs[i].cb(obj_inst_id, res_id, res_inst_id, data, data_len,
						  last_block, total_size);
		}
	}
}
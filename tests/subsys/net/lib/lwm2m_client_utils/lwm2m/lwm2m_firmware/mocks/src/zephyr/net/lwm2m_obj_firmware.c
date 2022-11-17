/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <ztest.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/lwm2m.h>

LOG_MODULE_DECLARE(ztest, CONFIG_LOG_LEVEL_TEST);

void lwm2m_firmware_set_update_result(uint8_t result)
{
	ztest_check_expected_value(result);
}

lwm2m_engine_set_data_cb_t firmware_block_received_cb;

void lwm2m_firmware_set_write_cb(lwm2m_engine_set_data_cb_t cb)
{
	firmware_block_received_cb = cb;
}

uint8_t lwm2m_firmware_get_update_state_inst(uint16_t obj_inst_id)
{
	return ztest_get_return_value();
}

void lwm2m_firmware_set_update_cb(lwm2m_engine_execute_cb_t cb)
{
}

void lwm2m_firmware_set_update_state(uint8_t state)
{
	ztest_check_expected_value(state);
}

uint8_t lwm2m_firmware_get_update_state(void)
{
	return ztest_get_return_value();
}

void lwm2m_firmware_set_update_state_inst(uint16_t obj_inst_id, uint8_t state)
{
	ztest_check_expected_value(state);
}

int test_lwm2m_firmware_firmware_block_received_cb(uint16_t obj_inst_id, uint16_t res_id,
						   uint16_t res_inst_id, uint8_t *data,
						   uint16_t data_len, bool last_block,
						   size_t total_size)
{
	return firmware_block_received_cb(obj_inst_id, res_id, res_inst_id, data, data_len,
					  last_block, total_size);
}
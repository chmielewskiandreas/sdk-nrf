#
# Copyright (c) 2022 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lwm2m_firmware)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-implicit-function-declaration -Wno-unused-function")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VAT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MOCK_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mocks/src)
set(MOCK_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mocks/inc)

# Add test sources
target_sources(app PRIVATE ${VAT_SRC_DIR}/main.c)
target_sources(app PRIVATE ${ZEPHYR_BASE}/../nrf/subsys/net/lib/lwm2m_client_utils/lwm2m/lwm2m_firmware.c)

# Add mock sources
target_sources(app PRIVATE ${MOCK_SRC_DIR}/dfu/dfu_target.c)
target_sources(app PRIVATE ${MOCK_SRC_DIR}/dfu/mcuboot.c)
target_sources(app PRIVATE ${MOCK_SRC_DIR}/net/fota_download.c)
target_sources(app PRIVATE ${MOCK_SRC_DIR}/net/lwm2m_client_utils_fota.c)
target_sources(app PRIVATE ${MOCK_SRC_DIR}/zephyr/net/lwm2m_engine.c)
target_sources(app PRIVATE ${MOCK_SRC_DIR}/zephyr/net/lwm2m_obj_firmware.c)
target_sources(app PRIVATE ${MOCK_SRC_DIR}/zephyr/net/lwm2m_registry.c)
target_sources(app PRIVATE ${MOCK_SRC_DIR}/zephyr/sys/reboot.c)

# Add includes directories
target_include_directories(app PRIVATE ${MOCK_INC_DIR})
target_include_directories(app PRIVATE ${ZEPHYR_BASE}/../nrf/include/)
target_include_directories(app PRIVATE ${ZEPHYR_BASE}/include/)
target_include_directories(app PRIVATE ${ZEPHYR_BASE}/../nrfxlib/nrf_modem/include/)
target_include_directories(app PRIVATE ${ZEPHYR_BASE}/subsys/net/lib/lwm2m/)

add_compile_definitions(CONFIG_LWM2M_ENGINE_MAX_PENDING=2)
add_compile_definitions(CONFIG_LWM2M_ENGINE_MAX_REPLIES=2)
add_compile_definitions(CONFIG_LWM2M_ENGINE_VALIDATION_BUFFER_SIZE=512)
add_compile_definitions(CONFIG_LWM2M_ENGINE_MESSAGE_HEADER_SIZE=512)
add_compile_definitions(CONFIG_DOWNLOAD_CLIENT_BUF_SIZE=512)
add_compile_definitions(CONFIG_DOWNLOAD_CLIENT_STACK_SIZE=512)
add_compile_definitions(CONFIG_LWM2M_COAP_BLOCK_SIZE=256)
add_compile_definitions(CONFIG_LWM2M_COAP_MAX_MSG_SIZE=512)
add_compile_definitions(CONFIG_LWM2M_CLIENT_UTILS_DOWNLOADER_SEC_TAG=-1)
add_compile_definitions(CONFIG_LWM2M_CLIENT_UTILS_FIRMWARE_UPDATE_OBJ_SUPPORT=y)
add_compile_definitions(CONFIG_LWM2M_CLIENT_UTILS_LOG_LEVEL=4)
add_compile_definitions(CONFIG_LOG_LEVEL_TEST=4)


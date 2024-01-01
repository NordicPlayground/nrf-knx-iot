#
#  Copyright (c) 2023 Nordic Semiconductor ASA
# 
#  SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

get_filename_component(BASE_DIR ../.. ABSOLUTE)

project(knx_sensor)

include_directories(app PUBLIC
${BASE_DIR}
${BASE_DIR}/deps
${BASE_DIR}/include 
${BASE_DIR}/port/ 
${BASE_DIR}/port/zephyr
)

add_compile_definitions(OC_CLIENT OC_SERVER OC_PUBLISHER_TABLE OC_BLOCK_WISE) 

target_sources(app PRIVATE
src/main.c
${BASE_DIR}/api/oc_server_api.c
${BASE_DIR}/api/oc_core_res.c
${BASE_DIR}/api/oc_rep.c
${BASE_DIR}/api/oc_buffer.c
${BASE_DIR}/api/oc_blockwise.c
${BASE_DIR}/api/oc_endpoint.c
${BASE_DIR}/api/oc_device_mode.c
${BASE_DIR}/api/oc_client_api.c
${BASE_DIR}/api/oc_base64.c
${BASE_DIR}/api/oc_ri.c
${BASE_DIR}/api/oc_helpers.c
${BASE_DIR}/api/oc_knx.c
${BASE_DIR}/api/oc_knx_client.c
${BASE_DIR}/api/oc_knx_swu.c
${BASE_DIR}/api/oc_knx_dev.c
${BASE_DIR}/api/oc_knx_fb.c
${BASE_DIR}/api/oc_knx_fp.c
${BASE_DIR}/api/oc_knx_helpers.c
${BASE_DIR}/api/oc_knx_sec.c
${BASE_DIR}/api/oc_knx_sub.c
${BASE_DIR}/api/oc_knx_p.c
${BASE_DIR}/api/oc_knx_gm.c
${BASE_DIR}/api/oc_main.c
${BASE_DIR}/api/oc_discovery.c
${BASE_DIR}/api/oc_network_events.c
${BASE_DIR}/api/oc_replay.c
${BASE_DIR}/util/oc_list.c
${BASE_DIR}/util/oc_process.c
${BASE_DIR}/util/oc_memb.c
${BASE_DIR}/util/oc_mmem.c
${BASE_DIR}/util/oc_etimer.c
${BASE_DIR}/util/oc_timer.c
${BASE_DIR}/messaging/coap/transactions.c
${BASE_DIR}/messaging/coap/observe.c
${BASE_DIR}/messaging/coap/engine.c
${BASE_DIR}/messaging/coap/separate.c
${BASE_DIR}/messaging/coap/coap.c
${BASE_DIR}/port/zephyr/connectivity.c
${BASE_DIR}/port/zephyr/abort.c
${BASE_DIR}/port/zephyr/random.c
${BASE_DIR}/port/zephyr/clock.c
${BASE_DIR}/port/zephyr/storage.c
${BASE_DIR}/port/zephyr/shell.c
${BASE_DIR}/port/zephyr/dns-sd.c
${BASE_DIR}/deps/tinycbor/src/cborencoder.c
${BASE_DIR}/deps/tinycbor/src/cborparser.c
${BASE_DIR}/deps/tinycbor/src/cborencoder_close_container_checked.c
)

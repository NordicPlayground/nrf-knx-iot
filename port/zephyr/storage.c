/*
* Copyright (c) 2023 Nordic Semiconductor ASA
*
* SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
*/

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <oc_log.h>
#include <oc_config.h>
#include <zephyr/settings/settings.h>

#define KNX_SETTINGS_ROOT_KEY "knx"
#define KNX_SETTINGS_MAX_PATH_LEN 32

struct knx_setting_read_ctx {
    /* Buffer for the setting. */
    uint8_t *value;

    /* Buffer length on input, setting length read on output. */
    size_t *length;

    /* Operation result. */
    int status;
};

int oc_storage_config(const char *store)
{
    int ret;

    (void)store;

    ret = settings_subsys_init();

    if (ret != 0) {
        PRINT("settings_subsys_init failed (ret %d)\n", ret);
    }

    return 0;
}

static int knx_setting_read_cb(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg,
                   void *param)
{
    int ret;
    struct knx_setting_read_ctx *ctx = (struct knx_setting_read_ctx *)param;

    if ((ctx->value == NULL) || (ctx->length == NULL)) {
        return 1;
    }

    if (*(ctx->length) < len) {
        len = *(ctx->length);
    }

    ret = read_cb(cb_arg, ctx->value, len);

    if (ret <= 0) {
        PRINT("Failed to read the setting, key: %s ret: %d\n", key, ret);
        ctx->status = -EIO;
        return 1;
    }

    if (ctx->length != NULL) {
        *(ctx->length) = len;
    }

    ctx->status = 0;
    return 0;
}

long oc_storage_read(const char *store, uint8_t *buf, size_t size)
{
    int ret;
    char path[KNX_SETTINGS_MAX_PATH_LEN];
    struct knx_setting_read_ctx read_ctx = {
        .value = buf,
        .length = (size_t *)&size,
        .status = -ENOENT,
    };

    #if OC_DEBUG
    PRINT("oc_storage_read(): %s, size: %u\n", store, size);
    #endif

    snprintf(path, sizeof(path), "%s/%s", KNX_SETTINGS_ROOT_KEY, store);

    ret = settings_load_subtree_direct(path, knx_setting_read_cb, &read_ctx);

    if (ret != 0 || read_ctx.status != 0) {
        if (read_ctx.status == -ENOENT) {
#if DEBUG
            PRINT("Key: %s not found\n", store);
#endif
        }
        else {
            PRINT("Failed to read the setting, path: %s, ret: %d, status: %d\n", path,
                  ret, read_ctx.status);
        }

        return 0;
    }

    return *read_ctx.length;
}

long oc_storage_write(const char *store, uint8_t *buf, size_t size)
{
    int ret;
    char path[KNX_SETTINGS_MAX_PATH_LEN];

#if OC_DEBUG
    PRINT("oc_storage_write(): %s, size: %u\n", store, size);
#endif

    snprintk(path, sizeof(path), "%s/%s", KNX_SETTINGS_ROOT_KEY, store);

    ret = settings_save_one(path, buf, size);

    if (ret) {
        PRINT("Failed to write the setting, ret: %d\n", ret);
        return 0;
    }

    return size;
}

int oc_storage_erase(const char *store)
{
    int ret;
    char path[KNX_SETTINGS_MAX_PATH_LEN];

#if OC_DEBUG
    PRINT("oc_storage_erase(): %s\n", store);
#endif

    snprintk(path, sizeof(path), "%s/%s", KNX_SETTINGS_ROOT_KEY, store);

    settings_delete(path);

    return 0;
}

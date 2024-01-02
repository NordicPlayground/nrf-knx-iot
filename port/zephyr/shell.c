/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <oc_core_res.h>
#include <api/oc_main.h>
#include <api/oc_knx_fp.h>
#include <api/oc_knx_dev.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  (void)arg
#endif

#define KNX_STORAGE_HOSTNAME "dev_knx_hostname"
#define KNX_STORAGE_PM "dev_knx_pm"
#define KNX_STORAGE_PORT "dev_knx_port"

#define MAX_GA_PER_ENTRY 8

static void print_help(const struct shell *shell, const char* arg)
{
    if (arg && (strcmp(arg, "add") == 0 || strcmp(arg, "edit") == 0))
    {
        shell_print(shell, "usage: knx got %s <id> <path> <cflags> [<ga>]", arg);
        shell_print(shell, "  <id>     - entry identifier");
        shell_print(shell, "  <path>   - path");
        shell_print(shell, "  <cflags> - flags");
        shell_print(shell, "  <ga>     - comma-separated group addresses list , e.g [1,4,12]");
    }
    else if (arg && strcmp(arg, "remove") == 0)
    {
        shell_print(shell, "usage: knx got remove <id> <path> <cflags> [<ga>]");
        shell_print(shell, "  <id> - entry identifier");
    }
    else if (arg && strcmp(arg, "print") == 0)
    {
        shell_print(shell, "print all the entries in the following format:");
        shell_print(shell, "  <entry index>: <id> <path> <cflags> <group addresses>");
    }
    else
    {
        shell_print(shell, "available commands:");
        shell_print(shell, "  add");
        shell_print(shell, "  edit");
        shell_print(shell, "  print");
        shell_print(shell, "  remove");
        shell_print(shell, " ");
        shell_print(shell, "command usage: knx got help <command>");
    }
}

static void knx_got_help(const struct shell *shell, size_t argc, char **argv)
{
    print_help(shell, argc > 1 ? argv[1] : NULL);
}


static void dump_group_addresses(char *buf, uint32_t *ga, int ga_len)
{
    int i;
    int buf_wr = 0;

    buf[0] = '[';
    buf_wr++;

    for (i = 0; i < ga_len - 1; ++i)
    {
        buf_wr += sprintf(&buf[buf_wr], "%u,", ga[i]);
    }

    sprintf(&buf[buf_wr], "%u]", ga[ga_len - 1]);
}

static int parse_group_addresses(const char *buf, int *ga, int max)
{
    int res = 0;
    int temp = 0;
    int idx = 1;
    bool after_comma = false;

    if (buf[0] != '[')
    {
        return 0;
    }

    while (buf[idx] && buf[idx] != ']')
    {
        if (buf[idx] >= '0' && buf[idx] <= '9')
        {
            temp = (temp * 10) + buf[idx] - '0';
            after_comma = false;
        }
        else if (buf[idx] == ',')
        {
            ga[res] = temp;
            temp = 0;
            res += 1;
            after_comma = true;
         
            if (res == max)
            {
                return res;
            }
        }
        else if (buf[idx] == ' ' && after_comma)
        {
            // Intentionally empty
        }
        else
        {
            return 0;
        }

        idx++;
    }

    if (buf[idx] == ']' && res < max)
    {
        ga[res] = temp;
        res += 1;
    }

    return res;
}

static int add_or_edit_got_entry(const struct shell *shell, bool add_only, 
                                  size_t argc, char **argv)
{
    oc_group_object_table_t entry;
    int index = 0;
    int temp_ga[MAX_GA_PER_ENTRY];
    int id;
    int ga_len;

    if (argc < 5)
    {
        print_help(shell, add_only ? "add" : "edit");
        return -EINVAL;
    }

    id = atoi(argv[1]);
    index = find_empty_slot_in_group_object_table(id);
    
    if (add_only &&
        oc_core_find_group_object_table_number_group_entries(index))
    {
        shell_error(shell, "entry id already in use, set a different one or use"
                           "the 'knx got edit' command");
        return -EINVAL;
    }

    entry.id = id;
    oc_new_string(&entry.href, argv[2], strlen(argv[2]));
    entry.cflags = atoi(argv[3]);

    ga_len = parse_group_addresses(argv[4], temp_ga, MAX_GA_PER_ENTRY);

    if (ga_len == 0)
    {
        shell_error(shell, "failed to parse group addresses");
        return -EINVAL;
    }

    entry.ga_len = ga_len;
    entry.ga = temp_ga;

    oc_core_set_group_object_table(index, entry);
    oc_dump_group_object_table_entry(index);
    oc_register_group_multicasts();

    oc_free_string(&entry.href);

    return 0;
}

static int knx_dev_sn(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    oc_device_info_t *device = oc_core_get_device_info(0);
    
    shell_print(shell, "%s", oc_string(device->serialnumber));

    return 0;
}

static int knx_dev_hwv(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    oc_device_info_t *device = oc_core_get_device_info(0);

    shell_print(shell, "%d.%d.%d", device->hwv.major, device->hwv.minor,
                device->hwv.patch);

    return 0;
}

static int knx_dev_hwt(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    oc_device_info_t *device = oc_core_get_device_info(0);
    
    shell_print(shell, "%s", oc_string(device->hwt));

    return 0;
}

static int knx_dev_model(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    oc_device_info_t *device = oc_core_get_device_info(0);
    
    shell_print(shell, "%s", oc_string(device->model));

    return 0;
}

static int knx_dev_sa(const struct shell *shell, size_t argc, char **argv)
{
    oc_device_info_t *device = oc_core_get_device_info(0);

    if (argc < 2)
    {
        shell_print(shell, "%u", (uint8_t)((device->ia) >> 8));
        return 0;
    }

    return 0;
}

static int knx_dev_da(const struct shell *shell, size_t argc, char **argv)
{
    oc_device_info_t *device = oc_core_get_device_info(0);
    
    if (argc < 2)
    {
        shell_print(shell, "%u", device->ia);
        return 0;
    }

    return 0;
}

static int knx_dev_hname(const struct shell *shell, size_t argc, char **argv)
{
    oc_device_info_t *device = oc_core_get_device_info(0);
    oc_hostname_t *hname_cb = oc_get_hostname_cb();
    
    if (argc < 2)
    {
        shell_print(shell, "%s", oc_string(device->hostname));
        return 0;
    }

    oc_core_set_device_hostname(0, argv[1]);
    oc_storage_write(KNX_STORAGE_HOSTNAME,
                         (uint8_t *)oc_string(device->hostname),
                         oc_string_len(device->hostname));
    
    if (hname_cb && hname_cb->cb) {
        hname_cb->cb(0, device->hostname, hname_cb->data);
    }

    return 0;
}

static int knx_dev_fid(const struct shell *shell, size_t argc, char **argv)
{
    uint64_t fid;
    oc_device_info_t *device = oc_core_get_device_info(0);

    if (argc < 2)
    {
        shell_print(shell, "%llu", device->fid);
        return 0;
    }

    fid = atoll(argv[1]);

    oc_core_set_device_fid(0, fid);
    oc_storage_write(KNX_STORAGE_FID, (uint8_t *)&fid, sizeof(fid));

    return 0;
}

static int knx_dev_iid(const struct shell *shell, size_t argc, char **argv)
{
    uint64_t iid;
    oc_device_info_t *device = oc_core_get_device_info(0);

    if (argc < 2)
    {
        shell_print(shell, "%llu", device->iid);
        return 0;
    }

    iid = atoll(argv[1]);

    oc_core_set_device_iid(0, iid);
    oc_storage_write(KNX_STORAGE_IID, (uint8_t *)&iid, sizeof(iid));

    return 0;
}

static int knx_dev_ia(const struct shell *shell, size_t argc, char **argv)
{
    uint32_t ia;
    oc_device_info_t *device = oc_core_get_device_info(0);

    if (argc < 2)
    {
        shell_print(shell, "%u", device->ia);
        return 0;
    }

    ia = atoi(argv[1]);

    oc_core_set_device_ia(0, ia);
    oc_storage_write(KNX_STORAGE_IA, (uint8_t *)&ia, sizeof(ia));

    return 0;
}

static int knx_dev_port(const struct shell *shell, size_t argc, char **argv)
{
    uint32_t port;
    oc_device_info_t *device = oc_core_get_device_info(0);

    if (argc < 2)
    {
        shell_print(shell, "%u", device->port);
        return 0;
    }

    device->port = atoi(argv[1]);
    oc_storage_write(KNX_STORAGE_PORT, (uint8_t *)&port, sizeof(port));

    return 0;
}

static int knx_got_print(const struct shell *shell, size_t argc, char **argv)
{
    oc_group_object_table_t *entry_ptr;
    char ga_buf[64];
    bool got_any = false;

    for (int i = 0; i < oc_core_get_group_object_table_total_size(); ++i) {
        entry_ptr = oc_core_get_group_object_table_entry(i);
        if (entry_ptr->ga_len > 0)
        {
            memset(ga_buf, 0, sizeof(ga_buf));
            dump_group_addresses(ga_buf, entry_ptr->ga, entry_ptr->ga_len);

            shell_print(shell, "[%2u]: %d %s %u %s", i, entry_ptr->id,
                        oc_string(entry_ptr->href), entry_ptr->cflags, ga_buf);
            got_any = true;
        }
    }

    if (!got_any)
    {
        shell_print(shell, "no entries found");
    }

    return 0;
}

static int knx_got_edit(const struct shell *shell, size_t argc, char **argv)
{
    return add_or_edit_got_entry(shell, /* add_only: */ false, argc, argv);
}

static int knx_got_add(const struct shell *shell, size_t argc, char **argv)
{
    return add_or_edit_got_entry(shell, /* add_only: */ true, argc, argv);
}

static int knx_got_remove(const struct shell *shell, size_t argc, char **argv)
{
    int id;
    int index;

    if (argc < 2)
    {
        print_help(shell, "remove");
        return 0;
    }

    id = atoi(argv[1]);
    index = find_empty_slot_in_group_object_table(id);

    if (index < 0)
    {
        shell_print(shell, "entry with id: %u not found", id);
        return -EINVAL;
    }

    oc_delete_group_object_table_entry(index);

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_dev,
    SHELL_CMD(sn, NULL, "Serial number", knx_dev_sn),
    SHELL_CMD(hwv, NULL, "Hardware version", knx_dev_hwv),
    SHELL_CMD(hwt, NULL, "Hardware type", knx_dev_hwt),
    SHELL_CMD(model, NULL, "Model", knx_dev_model),
    SHELL_CMD(sa, NULL, "Subnet address", knx_dev_sa),
    SHELL_CMD(da, NULL, "Device address", knx_dev_da),
    SHELL_CMD(hname, NULL, "Host name", knx_dev_hname),
    SHELL_CMD(fid, NULL, "Fabric ID", knx_dev_fid),
    SHELL_CMD(iid, NULL, "Installation ID", knx_dev_iid),
    SHELL_CMD(ia, NULL, "Individual address", knx_dev_ia),
    SHELL_CMD(port, NULL, "Port", knx_dev_port),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_got,
    SHELL_CMD(help, NULL, "Help", knx_got_help),
    SHELL_CMD(add, NULL, "Add entry", knx_got_add),
    SHELL_CMD(edit, NULL, "Edit entry", knx_got_edit),
    SHELL_CMD(remove, NULL, "Remove entry", knx_got_remove),
    SHELL_CMD(print, NULL, "Print entries", knx_got_print),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_knx,
    SHELL_CMD(dev, &sub_dev, "Device", NULL),
    SHELL_CMD(got, &sub_got, "Group object table", NULL),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_CMD_REGISTER(knx, &sub_knx, "KNX Commands", NULL);
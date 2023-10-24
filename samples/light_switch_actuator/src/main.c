/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Copyright (c) 2021-2022 Cascoda Ltd
Copyright (c) 2023 Nordic Semiconductor ASA
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include "oc_api.h"
#include "oc_core_res.h"
#include "api/oc_knx_fp.h"
#include "port/oc_clock.h"
#include "api/oc_knx_dev.h"

#include <zephyr/kernel.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <zephyr/drivers/gpio.h>

static struct k_mutex event_loop_mutex;
struct k_condvar event_cv;

#include <stdio.h> /* defines FILENAME_MAX */

#define MY_NAME "Actuator (LSAB) 417" /**< The name of the application */

#define btoa(x) ((x) ? "true" : "false")

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define LED0_DISABLED DT_GPIO_FLAGS(LED0_NODE, gpios) & GPIO_ACTIVE_LOW
#define LED1_DISABLED DT_GPIO_FLAGS(LED1_NODE, gpios) & GPIO_ACTIVE_LOW
#define LED2_DISABLED DT_GPIO_FLAGS(LED2_NODE, gpios) & GPIO_ACTIVE_LOW
#define LED3_DISABLED DT_GPIO_FLAGS(LED3_NODE, gpios) & GPIO_ACTIVE_LOW

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0});
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET_OR(LED1_NODE, gpios, {0});
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET_OR(LED2_NODE, gpios, {0});
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET_OR(LED3_NODE, gpios, {0});

static const struct gpio_dt_spec *leds[] = {&led0, &led1, &led2, &led3 };

static bool connected = false;

static void state_blink()
{
  bool state = true;

  if (!connected) {
    for (int i = 0; i < 7; ++i) {
      gpio_pin_set_raw(led2.port, led2.pin, !state ^ (led2.dt_flags & GPIO_ACTIVE_HIGH));
      state = !state;
      k_sleep(K_MSEC(1000));
    }
  } else {
    for (int i = 0; i < 7; ++i) {
      gpio_pin_set_raw(led1.port, led1.pin, !state ^ (led1.dt_flags & GPIO_ACTIVE_HIGH));
      state = !state;
      k_sleep(K_MSEC(1000));
    }
  }
}

int
app_init(void)  
{
  /* set the manufacturer name */
  int ret = oc_init_platform("Nordic Semiconductor", NULL, NULL);

  /* set the application name, version, base url, device serial number */
  ret |= oc_add_device(MY_NAME, "1.0.0", "//", "LSAB0", NULL, NULL);

  oc_device_info_t *device = oc_core_get_device_info(0);
  PRINT("Serial Number: %s\n", oc_string_checked(device->serialnumber));

  /* set the hardware version 1.0.0 */
  oc_core_set_device_hwv(0, 1, 0, 0);

  /* set the firmware version 1.0.0 */
  oc_core_set_device_fwv(0, 1, 0, 0);

  /* set the hardware type*/
  oc_core_set_device_hwt(0, "Pi");

  /* set the model */
  oc_core_set_device_model(0, "Example Actuator");

  return ret;
}

static void
handle_get_request(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data, size_t res_index)
{
  (void)user_data; /* variable not used */

  bool error_state = false; /* the error state, the generated code */
  int oc_status_code = OC_STATUS_OK;

  PRINT("-- Begin get_dpa_417_61: interface %d\n", interfaces);
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  // check the query parameter m with the various values
  char *m;
  char *m_key;
  size_t m_key_len;
  size_t m_len = (int)oc_get_query_value(request, "m", &m);
  if (m_len != -1) {
    PRINT("  Query param: %.*s", (int)m_len, m);
    oc_init_query_iterator();
    size_t device_index = request->resource->device;
    oc_device_info_t *device = oc_core_get_device_info(device_index);
    if (device != NULL) {
      oc_rep_begin_root_object();
      while (oc_iterate_query(request, &m_key, &m_key_len, &m, &m_len) != -1) {
        // unique identifier
        if ((strncmp(m, "id", m_len) == 0) | (strncmp(m, "*", m_len) == 0)) {
          char mystring[100];
          snprintf(mystring, 99, "urn:knx:sn:%s%s",
                   oc_string(device->serialnumber),
                   oc_string(request->resource->uri));
          oc_rep_i_set_text_string(root, 9, mystring);
        }
        // resource types
        if ((strncmp(m, "rt", m_len) == 0) | (strncmp(m, "*", m_len) == 0)) {
          oc_rep_set_text_string(root, rt, "urn:knx:dpa.417.61");
        }
        // interfaces
        if ((strncmp(m, "if", m_len) == 0) | (strncmp(m, "*", m_len) == 0)) {
          oc_rep_set_text_string(root, if, "if.s");
        }
        if ((strncmp(m, "dpt", m_len) == 0) | (strncmp(m, "*", m_len) == 0)) {
          oc_rep_set_text_string(root, dpt, oc_string(request->resource->dpt));
        }
        // ga
        if ((strncmp(m, "ga", m_len) == 0) | (strncmp(m, "*", m_len) == 0)) {
          int index = oc_core_find_group_object_table_url(
            oc_string(request->resource->uri));
          if (index > -1) {
            oc_group_object_table_t *got_table_entry =
              oc_core_get_group_object_table_entry(index);
            if (got_table_entry) {
              oc_rep_set_int_array(root, ga, got_table_entry->ga,
                                   got_table_entry->ga_len);
            }
          }
        }
      } /* query iterator */
      oc_rep_end_root_object();
    } else {
      /* device is NULL */
      oc_send_cbor_response(request, OC_STATUS_BAD_OPTION);
    }
    oc_send_cbor_response(request, OC_STATUS_OK);
    return;
  }

  CborError error;
  oc_rep_begin_root_object();
  if (res_index == 3)
  {
    oc_rep_i_set_boolean(root, 1, 1);
  }
  else
  {
    oc_rep_i_set_boolean(root, 1, 0);
  }
  oc_rep_end_root_object();

  error = g_err;

  if (error) {
    oc_status_code = true;
  }

  if (error_state == false) {
    oc_send_cbor_response(request, oc_status_code);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_dpa_417_61\n");
}

static void
handle_put_request(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data, size_t res_index)
{
  (void)interfaces;
  (void)user_data;

  bool state = false;

  PRINT("-- Begin put_dpa_417_61:\n");

  oc_rep_t *rep = NULL;
  // handle the different requests
  if (oc_is_redirected_request(request)) {
    PRINT(" S-MODE or /P\n");
  }
  rep = request->request_payload;
  while (rep != NULL) {
    if (rep->type == OC_REP_BOOL) {
      if (rep->iname == 1) {
        PRINT("  put_dpa_417_61 received : %d\n", rep->value.boolean);

        // Translate value to LED state depending on pin configuration
        state = !rep->value.boolean ^ (leds[res_index]->dt_flags & GPIO_ACTIVE_HIGH);

        gpio_pin_set_raw(leds[res_index]->port, leds[res_index]->pin, state);
        oc_send_cbor_response(request, OC_STATUS_CHANGED);
        PRINT("-- End put_dpa_417_61\n");
        return;
      }
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End put_dpa_417_61\n");
}

static void
get_p_1(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_get_request(request, interfaces, user_data, 0);
}

static void
get_p_2(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_get_request(request, interfaces, user_data, 1);
}

static void
get_p_3(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_get_request(request, interfaces, user_data, 2);
}

#if !CONFIG_LSAB_REDUCED_IO
static void
get_p_4(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_get_request(request, interfaces, user_data, 3);
}
#endif

static void
put_p_1(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_put_request(request, interfaces, user_data, 0);
}

static void
put_p_2(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_put_request(request, interfaces, user_data, 1);
}

static void
put_p_3(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_put_request(request, interfaces, user_data, 2);
}

#if !CONFIG_LSAB_REDUCED_IO
static void
put_p_4(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_put_request(request, interfaces, user_data, 3);
}
#endif

/**
 * register all the resources to the stack
 * this function registers all application level resources:
 * - each resource path is bind to a specific function for the supported methods
 * (GET, POST, PUT, DELETE)
 * - each resource is:
 *   - secure
 *   - observable
 *   - discoverable
 *   - used interfaces
 *
 * URL Table
 * | resource url | functional block/dpa  | GET | PUT | comment                  |
 * | ------------ | --------------------- | --- | --- | ------------------------ |
 * | p/1          | urn:knx:dpa.417.61    | Yes | Yes |                          |
 * | p/2          | urn:knx:dpa.417.61    | Yes | Yes | unavailable for Thingy53 |
 * | p/3          | urn:knx:dpa.417.61    | Yes | Yes | unavailable for Thingy53 |
 * | p/4          | urn:knx:dpa.417.61    | Yes | Yes | unavailable for Thingy53 |
 */
void
register_resource(const char *url, oc_request_callback_t get_cb, oc_request_callback_t put_cb)
{
  PRINT("Register Resource with local path \"%s\"\n", url);
  PRINT("Light Switching actuator 417 (LSAB) : SwitchOnOff \n");
  PRINT("Data point 417.61 (DPT_Switch) \n");
  oc_resource_t *res_light =
    oc_new_resource("light actuation", url, 2, 0);
  oc_resource_bind_resource_type(res_light, "urn:knx:dpa.417.61");
  oc_resource_bind_dpt(res_light, "urn:knx:dpt.Switch");
  oc_resource_bind_content_type(res_light, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_light, OC_IF_A); /* if.a */
  oc_resource_set_discoverable(res_light, true);
  
  if (get_cb)
  { // set the GET handler
    oc_resource_set_request_handler(res_light, OC_GET, get_cb, NULL);
  }
  
  if (put_cb)
  {
    // set the PUT handler
    oc_resource_set_request_handler(res_light, OC_PUT, put_cb, NULL);
  } 
  // register this resource,
  // this means that the resource will be listed in /.well-known/core
  oc_add_resource(res_light);
}

void register_resources(void)
{
  register_resource("/p/1", get_p_1, put_p_1);
  register_resource("/p/2", get_p_2, put_p_2);
  register_resource("/p/3", get_p_3, put_p_3);
#if !CONFIG_LSAB_REDUCED_IO
  register_resource("/p/4", get_p_4, put_p_4);
#endif
}

/**
 * @brief initiate preset for device
 * current implementation: device reset as command line argument
 * @param device the device identifier of the list of devices
 * @param data the supplied data.
 */
void
factory_presets_cb(size_t device, void *data)
{
  (void)device;
  (void)data;

  PRINT("factory_presets_cb: NOP\n");
}

/**
 * @brief application reset
 *
 * @param device_index the device identifier of the list of devices
 * @param reset_value the knx reset value
 * @param data the supplied data.
 */
void
reset_cb(size_t device_index, int reset_value, void *data)
{
  (void)device_index;
  (void)data;

  PRINT("reset_cb %d\n", reset_value);
}

/**
 * @brief restart the device (application depended)
 *
 * @param device_index the device identifier of the list of devices
 * @param data the supplied data.
 */
void
restart_cb(size_t device_index, void *data)
{
  (void)device_index;
  (void)data;

  PRINT("-----restart_cb -------\n");
  // exit(0);
}

/**
 * @brief set the host name on the device (application depended)
 *
 * @param device_index the device identifier of the list of devices
 * @param host_name the host name to be set on the device
 * @param data the supplied data.
 */
void
hostname_cb(size_t device_index, oc_string_t host_name, void *data)
{
  (void)device_index;
  (void)data;

  PRINT("-----host name ------- %s\n", oc_string_checked(host_name));
}

static void
signal_event_loop(void)
{
    k_mutex_lock(&event_loop_mutex, K_FOREVER);
    k_condvar_signal(&event_cv);
    k_mutex_unlock(&event_loop_mutex);
}

void
main()
{
  int init;
  oc_clock_time_t next_event;
  int ret;

  if (!device_is_ready(led0.port)) {
		return;
	}

  if (!device_is_ready(led1.port)) {
		return;
	}

  if (!device_is_ready(led2.port)) {
		return;
	}
#if !CONFIG_LSAB_REDUCED_IO
  if (!device_is_ready(led3.port)) {
		return;
	}
#endif

  ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT);
	if (ret != 0) {
		return;
	}

  ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT);
	if (ret != 0) {
		return;
	}

  ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT);
	if (ret != 0) {
		return;
	}

#if !CONFIG_LSAB_REDUCED_IO
  ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT);
	if (ret != 0) {
		return;
	}
#endif

  gpio_pin_set_raw(led0.port, led0.pin, LED0_DISABLED);
  gpio_pin_set_raw(led1.port, led1.pin, LED1_DISABLED);
  gpio_pin_set_raw(led2.port, led2.pin, LED2_DISABLED);
#if !CONFIG_LSAB_REDUCED_IO
  gpio_pin_set_raw(led3.port, led3.pin, LED3_DISABLED);
#endif

  state_blink();
  oc_storage_config(NULL);

  struct otInstance *instance = openthread_get_default_instance();

  while(otThreadGetDeviceRole(instance) < OT_DEVICE_ROLE_CHILD)
  {
    k_msleep(100);
  }

  connected = true;
  state_blink();

#ifdef OC_OSCORE
  PRINT("OSCORE - Enabled\n");
#else
  PRINT("OSCORE - Disabled\n");
#endif /* OC_OSCORE */

  PRINT("Server \"%s\" running, waiting for incoming connections.\n",
        MY_NAME);

  /* set the application callbacks */
  oc_set_hostname_cb(hostname_cb, NULL);
  oc_set_reset_cb(reset_cb, NULL);
  oc_set_restart_cb(restart_cb, NULL);
  oc_set_factory_presets_cb(factory_presets_cb, NULL);

  /* initializes the handlers structure */
  static const oc_handler_t handler = { .init = app_init,
                                        .signal_event_loop = signal_event_loop,
                                        .register_resources = register_resources
#ifdef OC_CLIENT
                                        ,
                                        .requests_entry = 0
#endif
  };

  /* start the stack */
  init = oc_main_init(&handler);

  if (init < 0) {
    PRINT("oc_main_init failed %d, exiting.\n", init);
    return;
  }

  oc_register_group_multicasts();

  oc_a_lsm_set_state(0, LSM_S_LOADED);

  k_mutex_init(&event_loop_mutex);
  k_condvar_init(&event_cv);

  while(1)
  {
    next_event = oc_main_poll();
    k_mutex_lock(&event_loop_mutex, K_FOREVER);

    if (next_event == 0)
    {
        k_condvar_wait(&event_cv, &event_loop_mutex, K_FOREVER);
    }
    else
    {
        k_condvar_wait(&event_cv, &event_loop_mutex, K_MSEC(next_event - oc_clock_time()));
    }
    k_mutex_unlock(&event_loop_mutex);
  }
}

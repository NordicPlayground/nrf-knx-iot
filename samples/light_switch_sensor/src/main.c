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
// test purpose only
#include "api/oc_knx_dev.h"
#ifdef INCLUDE_EXTERNAL
/* import external definitions from header file*/
/* this file should be externally supplied */
#include "external_header.h"
#endif

#include <zephyr/kernel.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <zephyr/drivers/gpio.h>

static struct k_mutex event_loop_mutex;
struct k_condvar event_cv;

#include <stdio.h> /* defines FILENAME_MAX */

#define MY_NAME "Sensor (LSSB) 417.61" /**< The name of the application */

#ifdef WIN32
/** windows specific code */
#include <windows.h>
static CONDITION_VARIABLE cv; /**< event loop variable */
static CRITICAL_SECTION cs;   /**< event loop variable */
#include <direct.h>
#define GetCurrentDir _getcwd
#elif defined(__linux) || defined(__linux__) || defined(linux)
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define btoa(x) ((x) ? "true" : "false")

#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)
#define SW2_NODE DT_ALIAS(sw2)
#define SW3_NODE DT_ALIAS(sw3)

static bool current_states[] = {false, false, false, false};

static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
							      {0});
#if !CONFIG_LSSB_REDUCED_IO
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios,
							      {0});
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET_OR(SW2_NODE, gpios,
							      {0});
static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET_OR(SW3_NODE, gpios,
							      {0});
#endif

static struct gpio_callback button0_cb_data;
#if !CONFIG_LSSB_REDUCED_IO
static struct gpio_callback button1_cb_data;
static struct gpio_callback button2_cb_data;
static struct gpio_callback button3_cb_data;
#endif

void button0_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
  current_states[0] = !current_states[0];
  oc_do_s_mode_with_scope_no_check(2, "/p/1", "w");
  oc_do_s_mode_with_scope_no_check(5, "/p/1", "w");
}

#if !CONFIG_LSSB_REDUCED_IO
void button1_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
  current_states[1] = !current_states[1];
  oc_do_s_mode_with_scope_no_check(2, "/p/2", "w");
  oc_do_s_mode_with_scope_no_check(5, "/p/2", "w");
}

void button2_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{ 
  current_states[2] = !current_states[2];
  oc_do_s_mode_with_scope_no_check(2, "/p/3", "w");
  oc_do_s_mode_with_scope_no_check(5, "/p/3", "w");
}

void button3_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
  current_states[3] = !current_states[3];
  oc_do_s_mode_with_scope_no_check(2, "/p/4", "w");
  oc_do_s_mode_with_scope_no_check(5, "/p/4", "w");
}
#endif

int
app_init(void)
{
  /* set the manufacturer name */
  int ret = oc_init_platform("Nordic Semiconductor", NULL, NULL);

  /* set the application name, version, base url, device serial number */
  ret |= oc_add_device(MY_NAME, "1.0.0", "//", "LSSB0", NULL, NULL);

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
  oc_rep_i_set_boolean(root, 1, current_states[res_index]);
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
get_p_1(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_get_request(request, interfaces, user_data, 0);
}

#if !CONFIG_LSSB_REDUCED_IO
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

static void
get_p_4(oc_request_t *request, oc_interface_mask_t interfaces,
          void *user_data)
{
  handle_get_request(request, interfaces, user_data, 3);
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
 * | p/1          | urn:knx:dpa.417.61    | Yes | NO  |                          |
 * | p/2          | urn:knx:dpa.417.61    | Yes | NO  | unavailable for Thingy53 |
 * | p/3          | urn:knx:dpa.417.61    | Yes | NO  | unavailable for Thingy53 |
 * | p/4          | urn:knx:dpa.417.61    | Yes | NO  | unavailable for Thingy53 |
 */
void
register_resource(const char *url, oc_request_callback_t get_cb, oc_request_callback_t put_cb)
{
  PRINT("Register Resource with local path \"%s\"\n", url);
  PRINT("Light Switching Sensor 421.61 (LSSB) : SwitchOnOff \n");
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
  register_resource("/p/1", get_p_1, NULL);
#if !CONFIG_LSSB_REDUCED_IO
  register_resource("/p/2", get_p_2, NULL);
  register_resource("/p/3", get_p_3, NULL);
  register_resource("/p/4", get_p_4, NULL);
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

/**
 * @brief initializes the global variables
 * registers and starts the handler
 */
void
initialize_variables(void)
{
  /* initialize global variables for resources */
}

static void
signal_event_loop(void)
{
  k_mutex_lock(&event_loop_mutex, K_FOREVER);
  k_condvar_signal(&event_cv);
  k_mutex_unlock(&event_loop_mutex);
}

/**
 * @brief main application.
 * initializes the global variables
 * registers and starts the handler
 * handles (in a loop) the next event.
 * shuts down the stack
 */
void
main()
{
  int init;
  oc_clock_time_t next_event;

  int ret;

	if (!device_is_ready(button0.port)) {
		return;
	}

#if !CONFIG_LSSB_REDUCED_IO
  if (!device_is_ready(button1.port)) {
		return;
	}

  if (!device_is_ready(button2.port)) {
		return;
	}

  	if (!device_is_ready(button3.port)) {
		return;
	}
#endif

	ret = gpio_pin_configure_dt(&button0, GPIO_INPUT);
	if (ret != 0) {
		return;
	}

#if !CONFIG_LSSB_REDUCED_IO
  ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
	if (ret != 0) {
		return;
	}

  ret = gpio_pin_configure_dt(&button2, GPIO_INPUT);
	if (ret != 0) {
		return;
	}

  ret = gpio_pin_configure_dt(&button3, GPIO_INPUT);
	if (ret != 0) {
		return;
	}
#endif

	ret = gpio_pin_interrupt_configure_dt(&button0,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		return;
	}

#if !CONFIG_LSSB_REDUCED_IO
  ret = gpio_pin_interrupt_configure_dt(&button1,
					      GPIO_INT_EDGE_TO_ACTIVE);
  if (ret != 0) {
		return;
	}

  ret = gpio_pin_interrupt_configure_dt(&button2,
					      GPIO_INT_EDGE_TO_ACTIVE);
  if (ret != 0) {
		return;
	}

  ret = gpio_pin_interrupt_configure_dt(&button3,
					      GPIO_INT_EDGE_TO_ACTIVE);
  if (ret != 0) {
		return;
	}
#endif

	gpio_init_callback(&button0_cb_data, button0_pressed, BIT(button0.pin));
#if !CONFIG_LSSB_REDUCED_IO
	gpio_init_callback(&button1_cb_data, button1_pressed, BIT(button1.pin));
	gpio_init_callback(&button2_cb_data, button2_pressed, BIT(button2.pin));
	gpio_init_callback(&button3_cb_data, button3_pressed, BIT(button3.pin));
#endif

	gpio_add_callback(button0.port, &button0_cb_data);
#if !CONFIG_LSSB_REDUCED_IO 
  gpio_add_callback(button1.port, &button1_cb_data);
  gpio_add_callback(button2.port, &button2_cb_data);
  gpio_add_callback(button3.port, &button3_cb_data);
#endif

  oc_storage_config(NULL);

  /*initialize the variables */
  initialize_variables();

  /* initializes the handlers structure */
  static const oc_handler_t handler = { .init = app_init,
                                        .signal_event_loop = signal_event_loop,
                                        .register_resources = register_resources
#ifdef OC_CLIENT
                                        ,
                                        .requests_entry = 0
#endif
  };

  /* set the application callbacks */
  oc_set_hostname_cb(hostname_cb, NULL);
  oc_set_reset_cb(reset_cb, NULL);
  oc_set_restart_cb(restart_cb, NULL);
  oc_set_factory_presets_cb(factory_presets_cb, NULL);

  /* start the stack */
  init = oc_main_init(&handler);

  oc_a_lsm_set_state(0, LSM_S_LOADED);

  if (init < 0) {
    PRINT("oc_main_init failed %d, exiting.\n", init);
    return;
  }

#ifdef OC_OSCORE
  PRINT("OSCORE - Enabled\n");
#else
  PRINT("OSCORE - Disabled\n");
#endif /* OC_OSCORE */

  PRINT("Server \"%s\" running, waiting for incoming connections.\n",
        MY_NAME);

  struct otInstance *instance = openthread_get_default_instance();

  while(otThreadGetDeviceRole(instance) < OT_DEVICE_ROLE_CHILD)
  {
    k_msleep(100);
  }

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

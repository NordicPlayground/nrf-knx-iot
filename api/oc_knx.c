/*
 // Copyright (c) 2021-2022 Cascoda Ltd
 //
 // Licensed under the Apache License, Version 2.0 (the "License");
 // you may not use this file except in compliance with the License.
 // You may obtain a copy of the License at
 //
 //      http://www.apache.org/licenses/LICENSE-2.0
 //
 // Unless required by applicable law or agreed to in writing, software
 // distributed under the License is distributed on an "AS IS" BASIS,
 // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 // See the License for the specific language governing permissions and
 // limitations under the License.
 */

#include "oc_api.h"
#include "oc_core_res.h"
#include "oc_knx.h"
#include "oc_knx_client.h"
#include "oc_knx_dev.h"
#include "oc_knx_fp.h"
#include "oc_knx_gm.h"
#include "oc_knx_sec.h"
#include "oc_main.h"
#include <stdio.h>
#include "oc_rep.h" // should not be needed

#include "port/dns-sd.h"

#ifdef OC_SPAKE
#include "security/oc_spake2plus.h"
#endif

#define TAGS_AS_STRINGS

#define LSM_STORE "LSM_STORE"
#define FINGERPRINT_STORE "dev_knx_fingerprint"
#define OSN_STORE "dev_knx_osn"

// ---------------------------Variables --------------------------------------

oc_group_object_notification_t g_received_notification;

uint64_t g_fingerprint = 0;
uint64_t g_osn = 0;

oc_pase_t g_pase;

oc_string_t g_idevid;
oc_string_t g_ldevid;

// ----------------------------------------------------------------------------

enum SpakeKeys {
  SPAKE_ID = 0,
  SPAKE_SALT = 5,
  SPAKE_PA_SHARE_P = 10,
  SPAKE_PB_SHARE_V = 11,
  SPAKE_PBKDF2 = 12,
  SPAKE_CB_CONFIRM_V = 13,
  SPAKE_CA_CONFIRM_P = 14,
  SPAKE_RND = 15,
  SPAKE_IT = 16,
};

#define RESTART_DEVICE 2
#define RESET_DEVICE 1

static int
convert_cmd(char *cmd)
{
  if (strncmp(cmd, "reset", strlen("reset")) == 0) {
    return RESET_DEVICE;
  }
  if (strncmp(cmd, "restart", strlen("restart")) == 0) {
    return RESTART_DEVICE;
  }

  OC_DBG("convert_cmd command not recognized: %s", cmd);
  return 0;
}

int
restart_device(size_t device_index)
{
  PRINT("restart device\n");

  oc_restart_t *my_restart = oc_get_restart_cb();
  if (my_restart && my_restart->cb) {
    // do a reboot on application level
    my_restart->cb(device_index, my_restart->data);
  }

  return 0;
}

int
oc_reset_device(size_t device_index, int value)
{
  PRINT("reset device: %d\n", value);

  oc_knx_device_storage_reset(device_index, value);

  return 0;
}

// ----------------------------------------------------------------------------

static void
oc_core_knx_get_handler(oc_request_t *request, oc_interface_mask_t iface_mask,
                        void *data)
{
  (void)data;
  (void)iface_mask;
  size_t response_length = 0;

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_JSON &&
      request->accept != APPLICATION_CBOR) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }
  if (request->accept == APPLICATION_JSON) {

    int length = oc_rep_add_line_to_buffer("{");
    response_length += length;

    length = oc_rep_add_line_to_buffer("\"api\": { \"version\": \"1.0.0\",");
    response_length += length;

    length = oc_rep_add_line_to_buffer("\"base\": \"/ \"}");
    response_length += length;

    length = oc_rep_add_line_to_buffer("}");
    response_length += length;

    oc_send_json_response(request, OC_STATUS_OK);
    request->response->response_buffer->response_length = response_length;
  } else {

    oc_rep_begin_root_object();

    oc_rep_set_text_string(root, base, "/");

    oc_rep_set_key(&root_map, "api");
    oc_rep_begin_object(&root_map, api);
    oc_rep_set_text_string(api, version, "1.0.0");
    oc_rep_end_object(&root_map, api);

    oc_rep_end_root_object();

    oc_send_cbor_response(request, OC_STATUS_OK);
  }
}

/*
  value = 1   Unsigned
  cmd = 2  Text string
  status = 3  Unsigned
  code = "code" Unsigned
  time = "time" Unsigned
*/
static void
oc_core_knx_post_handler(oc_request_t *request, oc_interface_mask_t iface_mask,
                         void *data)
{
  (void)data;
  (void)iface_mask;
  // size_t response_length = 0;

  int value = -1;
  int cmd = -1;
  // int time;
  // int code;

  PRINT("oc_core_knx_post_handler\n");

  char buffer[200];
  memset(buffer, 200, 1);
  oc_rep_to_json(request->request_payload, (char *)&buffer, 200, true);
  PRINT("%s", buffer);

  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    switch (rep->type) {
    case OC_REP_INT: {
      if (rep->iname == 1) {
        {
          value = (int)rep->value.integer;
        }
      }
    } break;
    case OC_REP_STRING: {
      if (rep->iname == 2) {
        {
          // the command
          cmd = convert_cmd(oc_string(rep->value.string));
        }
      }
    } break;
    case OC_REP_NIL:
      break;
    default:
      break;
    }
    rep = rep->next;
  }

  PRINT("  cmd   : %d\n", cmd);
  PRINT("  value : %d\n", (int)value);

  bool error = true;
  size_t device_index = request->resource->device;

  if (cmd == RESTART_DEVICE) {
    restart_device(device_index);
    error = false;
  } else if (cmd == RESET_DEVICE) {
    oc_reset_device(device_index, value);
    error = false;
  }

  // Before executing the reset function,
  // the KNX IoT device MUST return a response with CoAP response 690 code 2.04
  // CHANGED and with payload containing Error Code and Process Time in seconds
  // as defined 691 for the Response to a Master Reset Request for KNX Classic
  // devices, see [10].
  if (error == false) {

    // oc_rep_begin_root_object ();
    oc_rep_begin_root_object();

    // TODO note need to figure out how to fill in the correct response values
    oc_rep_set_int(root, code, 5);
    oc_rep_set_int(root, time, 2);
    oc_rep_end_root_object();

    PRINT("oc_core_knx_post_handler %d - end\n",
          oc_rep_get_encoded_payload_size());
    oc_send_cbor_response(request, OC_STATUS_CHANGED);
    return;
  }

  PRINT(" invalid command\n");
  oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
  return;
}

void
oc_create_knx_resource(int resource_idx, size_t device)
{
  OC_DBG("oc_create_knx_resource\n");
  oc_core_populate_resource(resource_idx, device, "/.well-known/knx",
                            OC_IF_LI | OC_IF_SEC, APPLICATION_LINK_FORMAT,
                            OC_DISCOVERABLE, oc_core_knx_get_handler, 0,
                            oc_core_knx_post_handler, 0, 0, "");
}

// ----------------------------------------------------------------------------

oc_lsm_state_t
oc_knx_lsm_state(size_t device_index)
{
  oc_device_info_t *device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    OC_ERR("device not found %d", (int)device_index);
    return LSM_S_UNLOADED;
  }

  return device->lsm_s;
}

/*
 * function will store the new state
 */
int
oc_knx_lsm_set_state(size_t device_index, oc_lsm_event_t new_state)
{
  oc_device_info_t *device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    OC_ERR("device not found %d", (int)device_index);
    return -1;
  }
  device->lsm_s = new_state;

  oc_storage_write(LSM_STORE, (uint8_t *)&device->lsm_s, sizeof(device->lsm_s));

  return 0;
}

const char *
oc_core_get_lsm_state_as_string(oc_lsm_state_t lsm)
{
  // states
  if (lsm == LSM_S_UNLOADED) {
    return "unloaded";
  }
  if (lsm == LSM_S_LOADED) {
    return "loaded";
  }
  if (lsm == LSM_S_LOADING) {
    return "loading";
  }
  if (lsm == LSM_S_UNLOADING) {
    return "unloading";
  }
  if (lsm == LSM_S_LOADCOMPLETING) {
    return "load completing";
  }

  return "";
}

const char *
oc_core_get_lsm_event_as_string(oc_lsm_event_t lsm)
{
  // commands
  if (lsm == LSM_E_NOP) {
    return "nop";
  }
  if (lsm == LSM_E_STARTLOADING) {
    return "startLoading";
  }
  if (lsm == LSM_E_LOADCOMPLETE) {
    return "loadComplete";
  }
  if (lsm == LSM_E_UNLOAD) {
    return "unload";
  }

  return "";
}

/*
 * function will store the new state
 */
bool
oc_lsm_event_to_state(oc_lsm_event_t lsm_e, size_t device_index)
{
  if (lsm_e == LSM_E_NOP) {
    // do nothing
    return true;
  }
  if (lsm_e == LSM_E_STARTLOADING) {
    oc_knx_lsm_set_state(device_index, LSM_S_LOADING);
    return true;
  }
  if (lsm_e == LSM_E_LOADCOMPLETE) {
    oc_knx_lsm_set_state(device_index, LSM_S_LOADED);
    return true;
  }
  if (lsm_e == LSM_E_UNLOAD) {
    // do a reset
    oc_delete_group_rp_table();
    oc_delete_group_object_table();
    oc_knx_lsm_set_state(device_index, LSM_S_UNLOADED);
    return true;
  }
  return false;
}

static void
oc_core_knx_lsm_get_handler(oc_request_t *request,
                            oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;

  PRINT("oc_core_knx_lsm_get_handler\n");

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_CBOR) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }

  size_t device_index = request->resource->device;
  oc_device_info_t *device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
    return;
  }
  oc_lsm_state_t lsm = oc_knx_lsm_state(device_index);

  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 3, lsm);
  oc_rep_end_root_object();

  oc_send_cbor_response(request, OC_STATUS_OK);

  PRINT("oc_core_knx_lsm_get_handler - done\n");
}

static void
oc_core_knx_lsm_post_handler(oc_request_t *request,
                             oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;
  oc_rep_t *rep = NULL;

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_CBOR) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }

  size_t device_index = request->resource->device;
  oc_device_info_t *device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
    return;
  }

  bool changed = false;
  int event = LSM_E_NOP;
  /* loop over the request document */
  rep = request->request_payload;
  while (rep != NULL) {
    if (rep->type == OC_REP_INT) {
      if (rep->iname == 2) {
        event = (int)rep->value.integer;
        changed = true;
        break;
      }
    }
    rep = rep->next;
  } /* while */

  PRINT("  load event %d [%s]\n", event,
        oc_core_get_lsm_event_as_string((oc_lsm_event_t)event));
  // check the input and change the state
  changed = oc_lsm_event_to_state((oc_lsm_event_t)event, device_index);

  /* input was set, so create the response*/
  if (changed == true) {
    oc_rep_begin_root_object();
    oc_rep_i_set_int(root, 3, (int)oc_knx_lsm_state(device_index));
    oc_rep_end_root_object();
    oc_send_cbor_response(request, OC_STATUS_CHANGED);

    if (oc_is_device_in_runtime(device_index)) {
      oc_register_group_multicasts();
      oc_init_datapoints_at_initialization();
      oc_device_info_t *device = oc_core_get_device_info(device_index);
      knx_publish_service(oc_string(device->serialnumber), device->iid, device->ia);
    }

    return;
  }

  oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
}

void
oc_create_knx_lsm_resource(int resource_idx, size_t device)
{
  OC_DBG("oc_create_knx_lsm_resource\n");
  // "/a/lsm"
  oc_core_populate_resource(
    resource_idx, device, "/a/lsm", OC_IF_C, APPLICATION_CBOR, OC_DISCOVERABLE,
    oc_core_knx_lsm_get_handler, 0, oc_core_knx_lsm_post_handler, 0, 0, "");
}

// ----------------------------------------------------------------------------

static void
oc_core_knx_knx_get_handler(oc_request_t *request,
                            oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;

  PRINT("oc_core_knx_knx_get_handler\n");

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_CBOR) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }

  size_t device_index = request->resource->device;
  oc_device_info_t *device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
    return;
  }

  // g_received_notification
  // { 4: "sia", 5: { 6: "st", 7: "ga", 1: "value" } }

  oc_rep_begin_root_object();
  // sia
  oc_rep_i_set_int(root, 4, g_received_notification.sia);

  oc_rep_i_set_key(&root_map, 5);
  CborEncoder value_map;
  cbor_encoder_create_map(&root_map, &value_map, CborIndefiniteLength);

  // ga
  oc_rep_i_set_int(value, 7, g_received_notification.ga);
  // st M Service type code(write = w, read = r, response = rp) Enum : w, r, rp
  oc_rep_i_set_text_string(value, 6, oc_string(g_received_notification.st));
  // missing value

  cbor_encoder_close_container_checked(&root_map, &value_map);

  oc_rep_end_root_object();

  oc_send_cbor_response(request, OC_STATUS_OK);

  PRINT("oc_core_knx_knx_get_handler - done\n");
}

// ----------------------------------------------------------------------------

bool
oc_s_mode_notification_to_json(char *buffer, size_t buffer_size,
                               oc_group_object_notification_t notification)
{
  // { 5: { 6: <st>, 7: <ga>, 1: <value> } }
  // { "s": { "st": <st>,  "ga": <ga>, "value": <value> } }
  int size = snprintf(
    buffer, buffer_size,
    "{ \"sia\": %d \"s\":{ \"st\": \"%s\" , \"ga\":%d, \"value\": %s }  }",
    notification.sia, oc_string(notification.st), notification.ga,
    oc_string(notification.value));
  if (size > buffer_size) {
    return false;
  }
  return true;
}

void
oc_reset_g_received_notification()
{
  g_received_notification.sia = -1;
  g_received_notification.ga = -1;
  // g_received_notification.value =

  oc_free_string(&g_received_notification.st);
  oc_new_string(&g_received_notification.st, "", strlen(""));
}

/*
 {sia: 5678, es: {st: write, ga: 1, value: 100 }}
*/
static void
oc_core_knx_knx_post_handler(oc_request_t *request,
                             oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;
  oc_rep_t *rep = NULL;
  oc_rep_t *rep_value = NULL;

  PRINT("KNX KNX Post Handler");
  char buffer[200];
  memset(buffer, 200, 1);
  oc_rep_to_json(request->request_payload, (char *)&buffer, 200, true);
  PRINT("Decoded Payload: %s\n", buffer);
  PRINT("Full Payload Size: %d\n", (int)request->_payload_len);
  OC_LOGbytes_OSCORE(request->_payload, (int)request->_payload_len);

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_CBOR) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }

  size_t device_index = request->resource->device;
  oc_device_info_t *device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
    return;
  }
  oc_reset_g_received_notification();

  /* loop over the request document to parse all the data */
  rep = request->request_payload;
  while (rep != NULL) {
    switch (rep->type) {
    case OC_REP_INT: {
      // sia
      if (rep->iname == 4) {
        g_received_notification.sia = (int)rep->value.integer;
      }
    } break;
    case OC_REP_OBJECT: {
      // find the storage index, e.g. for this object
      oc_rep_t *object = rep->value.object;

      object = rep->value.object;
      while (object != NULL) {
        switch (object->type) {
        case OC_REP_STRING: {
#ifdef TAGS_AS_STRINGS
          if (oc_string_len(object->name) == 2 &&
              memcmp(oc_string(object->name), "st", 2) == 0) {
            oc_free_string(&g_received_notification.st);
            oc_new_string(&g_received_notification.st,
                          oc_string(object->value.string),
                          oc_string_len(object->value.string));
          }
#endif
          if (object->iname == 6) {
            oc_free_string(&g_received_notification.st);
            oc_new_string(&g_received_notification.st,
                          oc_string(object->value.string),
                          oc_string_len(object->value.string));
          }
        } break;

        case OC_REP_INT: {
#ifdef TAGS_AS_STRINGS
          if (oc_string_len(object->name) == 3 &&
              memcmp(oc_string(object->name), "sia", 3) == 0) {
            g_received_notification.sia = (int)object->value.integer;
          }
          if (oc_string_len(object->name) == 2 &&
              memcmp(oc_string(object->name), "ga", 2) == 0) {
            g_received_notification.ga = (int)object->value.integer;
          }
#endif
          // sia
          if (object->iname == 4) {
            g_received_notification.sia = (int)object->value.integer;
          }
          // ga
          if (object->iname == 7) {
            g_received_notification.ga = (int)object->value.integer;
          }
        } break;
        case OC_REP_NIL:
          break;
        default:
          break;
        }
        object = object->next;
      }
    }
    case OC_REP_NIL:
      break;
    default:
      break;
    }
    rep = rep->next;
  }

  // gateway functionality: call back for all s-mode calls
  oc_gateway_t *my_gw = oc_get_gateway_cb();
  if (my_gw != NULL && my_gw->cb) {
    // call the gateway function
    my_gw->cb(device_index, &g_received_notification, my_gw->data);
  }

  bool do_write = false;
  bool do_read = false;
  // handle the request
  // loop over the group addresses of the /fp/r
  PRINT(" .knx : sia   %d\n", g_received_notification.sia);
  PRINT(" .knx : ga    %d\n", g_received_notification.ga);
  PRINT(" .knx : st    %s\n", oc_string(g_received_notification.st));
  if (strcmp(oc_string(g_received_notification.st), "w") == 0) {
    // case_1 :
    // Received from bus: -st w, any ga ==> @receiver:
    // clags = w -> overwrite object value
    do_write = true;
  } else if (strcmp(oc_string(g_received_notification.st), "rp") == 0) {
    // Case 2)
    // Received from bus: -st rp, any ga
    //@receiver: clags = u -> overwrite object value
    do_write = true;
  } else if (strcmp(oc_string(g_received_notification.st), "r") == 0) {
    // Case 4)
    // @sender: cflags = r
    // Received from bus: -st r
    // Sent: -st rp, sending association (1st assigned ga)
    do_read = true;
  }

  int index = oc_core_find_group_object_table_index(g_received_notification.ga);
  PRINT(" .knx : index %d\n", index);
  if (index == -1) {
    // if nothing is found (initially) then return a bad request.
    oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
    return;
  }

  while (index != -1) {
    oc_string_t myurl = oc_core_find_group_object_table_url_from_index(index);
    PRINT(" .knx : url  %s\n", oc_string(myurl));
    if (oc_string_len(myurl) > 0) {
      // get the resource to do the fake post on
      oc_resource_t *my_resource = oc_ri_get_app_resource_by_uri(
        oc_string(myurl), oc_string_len(myurl), device_index);
      if (my_resource == NULL) {
        return;
      }

      // check if the data is allowed to write or update
      oc_cflag_mask_t cflags = oc_core_group_object_table_cflag_entries(index);
      if (((cflags & OC_CFLAG_WRITE) > 0) && (do_write)) {
        PRINT(" (case1) index %d handled due to flags %d\n", index, cflags);
        // CASE 1:
        // Received from bus: -st w, any ga
        // @receiver : clags = w->overwrite object value
        my_resource->post_handler.cb(request, iface_mask, data);
        if ((cflags & OC_CFLAG_TRANSMISSION) > 0) {
          // Case 3) part 1
          // @sender : updated object value + cflags = t
          // Sent : -st w, sending association(1st assigned ga)
          PRINT("  (case3) sending \n");
          oc_do_s_mode_with_scope(2, oc_string(myurl), "w");
          oc_do_s_mode_with_scope(5, oc_string(myurl), "w");
        }
      }
      if (((cflags & OC_CFLAG_UPDATE) > 0) && (do_write)) {
        PRINT(" (case2) index %d handled due to flags %d\n", index, cflags);
        // Case 2)
        // Received from bus: -st rp , any ga
        // @receiver : clags = u->overwrite object value
        my_resource->post_handler.cb(request, iface_mask, data);
        if ((cflags & OC_CFLAG_TRANSMISSION) > 0) {
          PRINT("  (case3) sending \n");
          // Case 3) part 2
          // @sender : updated object value + cflags = t
          // Sent : -st w, sending association(1st assigned ga)
          oc_do_s_mode_with_scope(2, oc_string(myurl), "w");
          oc_do_s_mode_with_scope(5, oc_string(myurl), "w");
        }
      }
      if (((cflags & OC_CFLAG_READ) > 0) && (do_read)) {
        PRINT(" (case4) index %d handled due to flags %d\n", index, cflags);
        // Case 4)
        // @sender: cflags = r
        // Received from bus: -st r
        // Sent: -st rp, sending association (1st assigned ga)
        // oc_do_s_mode(oc_string(myurl), "rp");
        oc_do_s_mode_with_scope(2, oc_string(myurl), "rp");
        oc_do_s_mode_with_scope(5, oc_string(myurl), "rp");
      }
    }
    // get the next index in the table to get the url from.
    // this stops when the returned index == -1
    int new_index = oc_core_find_next_group_object_table_index(
      g_received_notification.ga, index);
    index = new_index;
  }

  // don't send anything back on a multi cast message
  if (request->origin && (request->origin->flags & MULTICAST)) {
    PRINT(" .knx : Multicast - not sending response\n");
    oc_send_cbor_response(request, OC_IGNORE);
    return;
  }

  PRINT(" .knx : Unicast - sending response\n");
  // send the response
  oc_send_cbor_response(request, OC_STATUS_OK);
}

void
oc_create_knx_knx_resource(int resource_idx, size_t device)
{
  OC_DBG("oc_create_knx_knx_resource\n");

  oc_core_populate_resource(resource_idx, device, "/.knx", OC_IF_LI | OC_IF_G,
                            APPLICATION_CBOR, OC_DISCOVERABLE,
                            oc_core_knx_knx_get_handler, 0,
                            oc_core_knx_knx_post_handler, 0, 1, "urn:knx:g.s");
}

// ----------------------------------------------------------------------------

static void
oc_core_knx_fingerprint_get_handler(oc_request_t *request,
                                    oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;
  PRINT("oc_core_knx_fingerprint_get_handler\n");

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_CBOR) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }
  // cbor_encode_uint(&g_encoder, g_fingerprint);
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_fingerprint);
  oc_rep_end_root_object();

  PRINT("oc_core_knx_fingerprint_get_handler - done\n");
  oc_send_cbor_response(request, OC_STATUS_OK);
}

void
oc_create_knx_fingerprint_resource(int resource_idx, size_t device)
{
  OC_DBG("oc_create_knx_fingerprint_resource\n");
  oc_core_populate_resource(
    resource_idx, device, "/.well-known/knx/f", OC_IF_C, APPLICATION_CBOR,
    OC_DISCOVERABLE, oc_core_knx_fingerprint_get_handler, 0, 0, 0, 0, "");
}

// ----------------------------------------------------------------------------

static void
oc_core_knx_osn_get_handler(oc_request_t *request,
                            oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;
  PRINT("oc_core_knx_osn_get_handler\n");

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_CBOR) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }
  // cbor_encode_uint(&g_encoder, g_osn);
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_osn);
  oc_rep_end_root_object();

  PRINT("oc_core_knx_osn_get_handler - done\n");
  oc_send_cbor_response(request, OC_STATUS_OK);
}

void
oc_create_knx_osn_resource(int resource_idx, size_t device)
{
  OC_DBG("oc_create_knx_osn_resource\n");
  oc_core_populate_resource(resource_idx, device, "/.well-known/knx/osn",
                            OC_IF_NONE, APPLICATION_CBOR, OC_DISCOVERABLE,
                            oc_core_knx_osn_get_handler, 0, 0, 0, 0, "");
}

// ----------------------------------------------------------------------------

static void
oc_core_knx_ldevid_get_handler(oc_request_t *request,
                               oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;
  size_t response_length = 0;

  PRINT("oc_core_knx_ldevid_get_handler\n");

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_PKCS7_CMC_REQUEST) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }
  response_length = oc_string_len(g_ldevid);
  oc_rep_encode_raw((const uint8_t *)oc_string(g_ldevid),
                    (size_t)response_length);

  request->response->response_buffer->content_format =
    APPLICATION_PKCS7_CMC_RESPONSE;
  request->response->response_buffer->code = oc_status_code(OC_STATUS_OK);
  request->response->response_buffer->response_length = response_length;

  PRINT("oc_core_knx_ldevid_get_handler- done\n");
}

/* optional resource */
void
oc_create_knx_ldevid_resource(int resource_idx, size_t device)
{
  OC_DBG("oc_create_knx_ldevid_resource\n");
  oc_core_populate_resource(resource_idx, device, "/.well-known/knx/ldevid",
                            OC_IF_D, APPLICATION_PKCS7_CMC_REQUEST,
                            OC_DISCOVERABLE, oc_core_knx_ldevid_get_handler, 0,
                            0, 0, 0, 1, ":dpt.a[n]");
}

// ----------------------------------------------------------------------------

static void
oc_core_knx_idevid_get_handler(oc_request_t *request,
                               oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;
  size_t response_length = 0;

  PRINT("oc_core_knx_idevid_get_handler\n");

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_PKCS7_CMC_REQUEST) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }
  response_length = oc_string_len(g_idevid);
  oc_rep_encode_raw((const uint8_t *)oc_string(g_idevid),
                    (size_t)response_length);

  request->response->response_buffer->content_format =
    APPLICATION_PKCS7_CMC_RESPONSE;
  request->response->response_buffer->code = oc_status_code(OC_STATUS_OK);
  request->response->response_buffer->response_length = response_length;

  PRINT("oc_core_knx_idevid_get_handler- done\n");
}

void
oc_create_knx_idevid_resource(int resource_idx, size_t device)
{
  OC_DBG("oc_create_knx_idevid_resource\n");
  oc_core_populate_resource(resource_idx, device, "/.well-known/knx/idevid",
                            OC_IF_D, APPLICATION_PKCS7_CMC_REQUEST,
                            OC_DISCOVERABLE, oc_core_knx_idevid_get_handler, 0,
                            0, 0, 0, 1, ":dpt.a[n]");
}

// ----------------------------------------------------------------------------

#ifdef OC_SPAKE
static spake_data_t spake_data;
static int failed_handshake_count = 0;

static bool is_blocking = false;

static oc_event_callback_retval_t
decrement_counter(void *data)
{
  if (failed_handshake_count > 0) {
    --failed_handshake_count;
  }

  if (is_blocking && failed_handshake_count == 0) {
    is_blocking = false;
  }
  return OC_EVENT_CONTINUE;
}

static void
increment_counter()
{
  ++failed_handshake_count;
}

static bool
is_handshake_blocked()
{
  if (is_blocking) {
    return true;
  }

  // after 10 failed attempts per minute, block the client for the
  // next minute
  if (failed_handshake_count > 10) {
    is_blocking = true;
    return true;
  }

  return false;
}

static int
get_seconds_until_unblocked()
{
  return failed_handshake_count * 10;
}

#endif /* OC_SPAKE */

static void
oc_core_knx_spake_post_handler(oc_request_t *request,
                               oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;
  PRINT("oc_core_knx_spake_post_handler\n");

  /* check if the accept header is cbor-format */
  if (request->accept != APPLICATION_CBOR) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }
  if (request->content_format != APPLICATION_CBOR) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }
  // check if the state is unloaded
  size_t device_index = request->resource->device;
  if (oc_knx_lsm_state(device_index) != LSM_S_UNLOADED) {
    PRINT(" not in unloading state\n");
    oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
    return;
  }

#ifdef OC_SPAKE
  if (is_handshake_blocked()) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_SERVICE_UNAVAILABLE);

    request->response->response_buffer->max_age = get_seconds_until_unblocked();
    return;
  }
#endif /* OC_SPAKE */

  oc_rep_t *rep = request->request_payload;
  int valid_request = 0;
  // check input
  // note: no check if there are multiple byte strings in the request payload
  while (rep != NULL) {
    switch (rep->type) {
    case OC_REP_BYTE_STRING: {
      if (rep->iname == SPAKE_PA_SHARE_P) {
        valid_request = SPAKE_PA_SHARE_P;
      }
      if (rep->iname == SPAKE_CA_CONFIRM_P) {
        valid_request = SPAKE_CA_CONFIRM_P;
      }
      if (rep->iname == SPAKE_RND) {
        valid_request = SPAKE_RND;
      }
    } break;
    default:
      break;
    }
    rep = rep->next;
  }

  if (valid_request == 0) {
    oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
  }
  rep = request->request_payload;

  if (valid_request == SPAKE_RND) {
    // set the default id, in preparation for the response
    // this gets overwritten if the ID is present in the
    // request payload handled below
    oc_free_string(&g_pase.id);
    oc_new_string(&g_pase.id, "responderkey", strlen("responderkey"));
  }
  // handle input
  while (rep != NULL) {
    switch (rep->type) {
    case OC_REP_BYTE_STRING: {
      if (rep->iname == SPAKE_CA_CONFIRM_P) {
        oc_free_string(&g_pase.ca);
        oc_new_string(&g_pase.ca, oc_string(rep->value.string),
                      oc_byte_string_len(rep->value.string));
      }
      if (rep->iname == SPAKE_PA_SHARE_P) {
        oc_free_string(&g_pase.pa);
        oc_new_string(&g_pase.pa, oc_string(rep->value.string),
                      oc_byte_string_len(rep->value.string));
      }
      if (rep->iname == SPAKE_RND) {
        oc_free_string(&g_pase.rnd);
        oc_new_string(&g_pase.rnd, oc_string(rep->value.string),
                      oc_byte_string_len(rep->value.string));
      }
      if (rep->iname == SPAKE_ID) {
        // if the ID is present, overwrite the default
        oc_free_string(&g_pase.id);
        oc_new_string(&g_pase.id, oc_string(rep->value.string),
                      oc_byte_string_len(rep->value.string));
      }
    } break;
    default:
      break;
    }
    rep = rep->next;
  }

  PRINT("oc_core_knx_spake_post_handler valid_request: %d\n", valid_request);

  if (valid_request == SPAKE_RND) {
#ifdef OC_SPAKE
    // generate random numbers for rnd, salt & it (# of iterations)
    oc_spake_parameter_exchange(&g_pase.rnd, &g_pase.salt, &g_pase.it);
#endif /* OC_SPAKE */
    oc_rep_begin_root_object();
    // id (0)
    oc_rep_i_set_byte_string(root, SPAKE_ID, oc_cast(g_pase.id, uint8_t),
                             oc_byte_string_len(g_pase.id));
    // rnd (15)
    oc_rep_i_set_byte_string(root, SPAKE_RND, oc_cast(g_pase.rnd, uint8_t),
                             oc_byte_string_len(g_pase.rnd));
    // pbkdf2
    oc_rep_i_set_key(&root_map, SPAKE_PBKDF2);
    oc_rep_begin_object(&root_map, pbkdf2);
    // it 16
    oc_rep_i_set_int(pbkdf2, SPAKE_IT, g_pase.it);
    // salt 5
    oc_rep_i_set_byte_string(pbkdf2, SPAKE_SALT, oc_cast(g_pase.salt, uint8_t),
                             oc_byte_string_len(g_pase.salt));
    oc_rep_end_object(&root_map, pbkdf2);
    oc_rep_end_root_object();
    oc_send_cbor_response(request, OC_STATUS_CHANGED);
    return;
  }
#ifdef OC_SPAKE
  if (valid_request == SPAKE_PA_SHARE_P) {
    // return changed, frame pb (11) & cb (13)

    const char *password = oc_spake_get_password();
    int ret;
    mbedtls_mpi_free(&spake_data.w0);
    mbedtls_ecp_point_free(&spake_data.L);
    mbedtls_mpi_free(&spake_data.y);
    mbedtls_ecp_point_free(&spake_data.pub_y);

    mbedtls_mpi_init(&spake_data.w0);
    mbedtls_ecp_point_init(&spake_data.L);
    mbedtls_mpi_init(&spake_data.y);
    mbedtls_ecp_point_init(&spake_data.pub_y);

    ret = oc_spake_calc_w0_L(password, oc_byte_string_len(g_pase.salt),
                             oc_cast(g_pase.salt, uint8_t), g_pase.it,
                             &spake_data.w0, &spake_data.L);
    if (ret != 0) {
      OC_ERR("oc_spake_calc_w0_L failed with code %d", ret);
      goto error;
    }

    ret = oc_spake_gen_keypair(&spake_data.y, &spake_data.pub_y);
    if (ret != 0) {
      OC_ERR("oc_spake_gen_keypair failed with code %d", ret);
      goto error;
    }

    // next step: calculate pB, encode it into the struct
    mbedtls_ecp_point pB;
    mbedtls_ecp_point_init(&pB);
    if (oc_spake_calc_pB(&pB, &spake_data.pub_y, &spake_data.w0)) {
      mbedtls_ecp_point_free(&pB);
      goto error;
    }

    oc_free_string(&g_pase.pb);
    oc_alloc_string(&g_pase.pb, kPubKeySize);
    oc_free_string(&g_pase.cb);
    oc_alloc_string(&g_pase.cb, kPubKeySize);

    if (oc_spake_encode_pubkey(&pB, oc_cast(g_pase.pb, uint8_t))) {
      mbedtls_ecp_point_free(&pB);
      goto error;
    }

    if (oc_spake_calc_transcript_responder(&spake_data,
                                           oc_cast(g_pase.pa, uint8_t), &pB)) {
      mbedtls_ecp_point_free(&pB);
      goto error;
    }

    oc_spake_calc_cB(spake_data.Ka_Ke, oc_cast(g_pase.cb, uint8_t),
                     oc_cast(g_pase.pa, uint8_t));
    mbedtls_ecp_point_free(&pB);

    oc_rep_begin_root_object();
    // pb (11)
    oc_rep_i_set_byte_string(root, SPAKE_PB_SHARE_V, oc_string(g_pase.pb),
                             oc_byte_string_len(g_pase.pb));
    // cb (13)
    oc_rep_i_set_byte_string(root, SPAKE_CB_CONFIRM_V, oc_string(g_pase.cb),
                             oc_byte_string_len(g_pase.cb));
    oc_rep_end_root_object();
    oc_send_cbor_response(request, OC_STATUS_CHANGED);
    return;
  }
  if (valid_request == SPAKE_CA_CONFIRM_P) {
    // calculate expected cA
    uint8_t expected_ca[32];
    if (g_pase.pb.ptr == NULL)
      goto error;
    oc_spake_calc_cA(spake_data.Ka_Ke, expected_ca,
                     oc_cast(g_pase.pb, uint8_t));

    if (memcmp(expected_ca, oc_cast(g_pase.ca, uint8_t), 32) != 0) {
      OC_ERR("oc_spake_calc_cA failed");
      goto error;
    }

    // TODO initialize OSCORE and create auth token using this key
    // shared_key is 16-byte array - NOT NULL TERMINATED
    uint8_t *shared_key = spake_data.Ka_Ke + 16;
    size_t shared_key_len = 16;

    // set thet /auth/at entry with the calculated shared key.
    oc_oscore_set_auth(shared_key, shared_key_len);

    oc_send_cbor_response(request, OC_STATUS_CHANGED);
    // handshake completed successfully - clear state
    memset(spake_data.Ka_Ke, 0, sizeof(spake_data.Ka_Ke));
    mbedtls_ecp_point_free(&spake_data.L);
    mbedtls_ecp_point_free(&spake_data.pub_y);
    mbedtls_mpi_free(&spake_data.w0);
    mbedtls_mpi_free(&spake_data.y);

    mbedtls_ecp_point_init(&spake_data.L);
    mbedtls_ecp_point_init(&spake_data.pub_y);
    mbedtls_mpi_init(&spake_data.w0);
    mbedtls_mpi_init(&spake_data.y);

    oc_free_string(&g_pase.pa);
    oc_free_string(&g_pase.pb);
    oc_free_string(&g_pase.ca);
    oc_free_string(&g_pase.cb);
    oc_free_string(&g_pase.rnd);
    oc_free_string(&g_pase.salt);
    g_pase.it = 100000;
    return;
  }
error:
  // be paranoid: wipe all global data after an error
  memset(spake_data.Ka_Ke, 0, sizeof(spake_data.Ka_Ke));
  mbedtls_ecp_point_free(&spake_data.L);
  mbedtls_ecp_point_free(&spake_data.pub_y);
  mbedtls_mpi_free(&spake_data.w0);
  mbedtls_mpi_free(&spake_data.y);

  mbedtls_ecp_point_init(&spake_data.L);
  mbedtls_ecp_point_init(&spake_data.pub_y);
  mbedtls_mpi_init(&spake_data.w0);
  mbedtls_mpi_init(&spake_data.y);
#endif /* OC_SPAKE */

  oc_free_string(&g_pase.pa);
  oc_free_string(&g_pase.pb);
  oc_free_string(&g_pase.ca);
  oc_free_string(&g_pase.cb);
  oc_free_string(&g_pase.rnd);
  oc_free_string(&g_pase.salt);
  g_pase.it = 100000;

#ifdef OC_SPAKE
  increment_counter();
#endif /* OC_SPAKE */
  oc_send_cbor_response(request, OC_STATUS_BAD_REQUEST);
}

void
oc_create_knx_spake_resource(int resource_idx, size_t device)
{
  OC_DBG("oc_create_knx_spake_resource\n");
  oc_core_populate_resource(resource_idx, device, "/.well-known/knx/spake",
                            OC_IF_NONE, APPLICATION_CBOR, OC_DISCOVERABLE, 0, 0,
                            oc_core_knx_spake_post_handler, 0, 0, "");

#ifdef OC_SPAKE
  // can fail if initialization of the RNG does not work
  assert(oc_spake_init() == 0);
  mbedtls_mpi_init(&spake_data.w0);
  mbedtls_ecp_point_init(&spake_data.L);
  mbedtls_mpi_init(&spake_data.y);
  mbedtls_ecp_point_init(&spake_data.pub_y);
  // start SPAKE brute force protection timer
  oc_set_delayed_callback(NULL, decrement_counter, 10);
#endif /* OC_SPAKE */
}

// ----------------------------------------------------------------------------

void
oc_knx_set_idevid(const char *idevid, int len)
{
  oc_free_string(&g_idevid);
  oc_new_string(&g_idevid, idevid, len);
}

void
oc_knx_set_ldevid(char *idevid, int len)
{
  oc_free_string(&g_ldevid);
  oc_new_string(&g_ldevid, idevid, len);
}

// ----------------------------------------------------------------------------

void
oc_knx_load_osn()
{
  g_osn = 0;
  oc_storage_read(OSN_STORE, (uint8_t *)&g_osn, sizeof(g_osn));
}

void
oc_knx_dump_osn()
{
  oc_storage_write(OSN_STORE, (uint8_t *)&g_osn, sizeof(g_osn));
}

void
oc_knx_set_osn(uint64_t osn)
{
  g_osn = osn;
  oc_knx_dump_osn();
}

uint64_t
oc_knx_get_osn()
{
  return g_osn;
}

// ----------------------------------------------------------------------------

void
oc_knx_load_fingerprint()
{
  g_fingerprint = 0;
  oc_storage_read(FINGERPRINT_STORE, (uint8_t *)&g_fingerprint,
                  sizeof(g_fingerprint));
}

void
oc_knx_dump_fingerprint()
{
  oc_storage_write(FINGERPRINT_STORE, (uint8_t *)&g_fingerprint,
                   sizeof(g_fingerprint));
}

void
oc_knx_set_fingerprint(uint64_t fingerprint)
{
  g_fingerprint = fingerprint;
}

void
oc_knx_increase_fingerprint()
{
  g_fingerprint++;
  oc_knx_dump_fingerprint();
}

// ----------------------------------------------------------------------------

void
oc_knx_load_state(size_t device_index)
{
  int temp_size;

  oc_lsm_state_t lsm;
  PRINT("oc_knx_load_state: Loading Device Config from Persistent storage\n");

  oc_device_info_t *device = oc_core_get_device_info(device_index);
  if (device == NULL) {
    OC_ERR(" could not get device %d\n", (int)device_index);
    return;
  }

  temp_size = oc_storage_read(LSM_STORE, (uint8_t *)&lsm, sizeof(lsm));
  if (temp_size > 0) {
    device->lsm_s = lsm;
    PRINT("  load state (storage) %ld [%s]\n", (long)lsm,
          oc_core_get_lsm_state_as_string((oc_lsm_state_t)lsm));
  }

  oc_knx_load_fingerprint();
  oc_knx_load_osn();
}

void
oc_create_knx_resources(size_t device_index)
{
  OC_DBG("oc_create_knx_resources");

  oc_create_knx_lsm_resource(OC_KNX_LSM, device_index);
  oc_create_knx_knx_resource(OC_KNX_DOT_KNX, device_index);
  oc_create_knx_fingerprint_resource(OC_KNX_FINGERPRINT, device_index);
  oc_create_knx_osn_resource(OC_KNX_OSN, device_index);
  oc_create_knx_ldevid_resource(OC_KNX_LDEVID, device_index);
  oc_create_knx_idevid_resource(OC_KNX_IDEVID, device_index);
  oc_create_knx_spake_resource(OC_KNX_SPAKE, device_index);
  oc_create_knx_resource(OC_KNX, device_index);
}

// ----------------------------------------------------------------------------

bool
oc_is_device_in_runtime(size_t device_index)
{
  oc_device_info_t *device = oc_core_get_device_info(device_index);
  if (device->ia <= 0) {
    return false;
  }
  if (device->iid <= 0) {
    return false;
  }
  if (device->lsm_s != LSM_S_LOADED) {
    return false;
  }

  return true;
}
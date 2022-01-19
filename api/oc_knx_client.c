/*
 // Copyright (c) 2021 Cascoda Ltd
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
#include "api/oc_knx_client.h"
#include "api/oc_knx_fp.h"

#include "oc_core_res.h"
#include "oc_discovery.h"
#include <stdio.h>


// ----------------------------------------------------------------------------

typedef struct ia_userdata
{
  int ia;
  oc_discover_ia_cb_t cb_func;
} ia_userdata;




// ----------------------------------------------------------------------------

oc_s_mode_response_cb_t m_s_mode_cb = NULL;


// ----------------------------------------------------------------------------

static oc_discovery_flags_t
discovery_ia_cb(const char *payload, int len, oc_endpoint_t *endpoint,
             void *user_data)
{
  //(void)anchor;
  (void)payload;
  (void)len;

  PRINT("discovery_ia_cb\n");
  oc_endpoint_print(endpoint);

  ia_userdata *cb_data = (ia_userdata *)user_data;
  if (cb_data && cb_data->cb_func) {
    PRINT("discovery_ia_cb: calling function\n");
    cb_data->cb_func(cb_data->ia, endpoint);
  }
  if (cb_data) {
    free(user_data);
  }

  return OC_STOP_DISCOVERY;
}


int
oc_knx_client_get_endpoint_from_ia(int ia, oc_discover_ia_cb_t my_func)
{
  char query[20];
  snprintf(query, 20, "if=urn:knx:ia.%d", ia);
  
  ia_userdata * cb_data = (ia_userdata *)malloc(sizeof(ia_userdata));
  cb_data->ia = ia;
  cb_data->cb_func = my_func;

  oc_do_wk_discovery_all(query, 2, discovery_ia_cb, cb_data);
  oc_do_wk_discovery_all(query, 3, discovery_ia_cb, cb_data);
  oc_do_wk_discovery_all(query, 5, discovery_ia_cb, cb_data);
  return 0;
}



bool
oc_is_s_mode_request(oc_request_t *request)
{
  if (request == NULL) {
    return false;
  }

  // TODO: add code to check if the request is part of the same setup
  // e.g. check idd of the device vs the data in the request

  PRINT("  oc_is_s_mode_request %.*s\n", (int)request->uri_path_len,
        request->uri_path);
  if (strncmp(".knx", request->uri_path, request->uri_path_len) == 0) {
    return true;
  }
  return false;
}

oc_rep_t *
oc_s_mode_get_value(oc_request_t *request)
{

  /* loop over the request document to parse all the data */
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    switch (rep->type) {
    case OC_REP_OBJECT: {
      // find the storage index, e.g. for this object
      oc_rep_t *object = rep->value.object;

      object = rep->value.object;
      while (object != NULL) {
        // search for "value" (1)
        if (object->iname == 1) {
          return object;
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
  return NULL;
}

static void
oc_issue_s_mode(int sia_value, int group_address, char *rp, uint8_t *value_data,
                int value_size)
{
  int scope = 5;
  //(void)sia_value; /* variable not used */

  PRINT("  oc_issue_s_mode : scope %d\n", scope);

  oc_make_ipv6_endpoint(mcast, IPV6 | DISCOVERY | MULTICAST, 5683, 0xff, scope,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0xfd);

  if (oc_init_post("/.knx", &mcast, NULL, NULL, LOW_QOS, NULL)) {

    /*
    { 4: <sia>, 5: { 6: <st>, 7: <ga>, 1: <value> } }
    */

    PRINT("before subtract and encode: %d\n",
          oc_rep_get_encoded_payload_size());
    // oc_rep_new(response_buffer.buffer, response_buffer.buffer_size);
    // oc_rep_subtract_length(oc_rep_get_encoded_payload_size());
    PRINT("after subtract, before encode: %d\n",
          oc_rep_get_encoded_payload_size());

    oc_rep_begin_root_object();

    oc_rep_i_set_int(root, 4, sia_value);

    oc_rep_i_set_key(&root_map, 5);
    CborEncoder value_map;
    cbor_encoder_create_map(&root_map, &value_map, CborIndefiniteLength);

    // ga
    oc_rep_i_set_int(value, 7, group_address);
    // st M Service type code(write = w, read = r, response = rp) Enum : w, r,
    // rp
    oc_rep_i_set_text_string(value, 6, rp);

    // set the "value" key
    oc_rep_i_set_key(&value_map, 5);
    // copy the data, this is already in cbor from the fake response of the
    // resource GET function
    oc_rep_encode_raw_encoder(&value_map, value_data, value_size);

    cbor_encoder_close_container_checked(&root_map, &value_map);

    oc_rep_end_root_object();

    PRINT("S-MODE Payload Size: %d\n", oc_rep_get_encoded_payload_size());

    if (oc_do_post_ex(APPLICATION_CBOR, APPLICATION_CBOR)) {
      PRINT("  Sent POST request\n");
    } else {
      PRINT("  Could not send POST request\n");
    }
  }
}

int
oc_do_s_mode_internal(char *resource_url, char *rp, uint8_t *buf, int buf_size)
{
  (void)rp;

  if (resource_url == NULL) {
    return 0;
  }

  oc_resource_t *my_resource =
    oc_ri_get_app_resource_by_uri(resource_url, strlen(resource_url), 0);
  if (my_resource == NULL) {
    PRINT(" oc_do_s_mode : error no URL found %s\n", resource_url);
    return 0;
  }

  // get the sender ia
  // size_t device_index = 0;
  // oc_device_info_t *device = oc_core_get_device_info(device_index);
  // int sia_value = device->ia;

  uint8_t *buffer = malloc(100);
  if (!buffer) {
    OC_WRN("oc_do_s_mode: out of memory allocating buffer");
  } //! buffer

  oc_request_t request = { 0 };
  oc_response_t response = { 0 };
  response.separate_response = 0;
  oc_response_buffer_t response_buffer;
  // if (!response_buf && resource) {
  //  OC_DBG("coap_notify_observers: Issue GET request to resource %s\n\n",
  //         oc_string(resource->uri));
  response_buffer.buffer = buffer;
  response_buffer.buffer_size = 100;

  // same initialization as oc_ri.c
  response_buffer.code = 0;
  response_buffer.response_length = 0;
  response_buffer.content_format = 0;

  response.separate_response = NULL;
  response.response_buffer = &response_buffer;

  request.response = &response;
  request.request_payload = NULL;
  request.query = NULL;
  request.query_len = 0;
  request.resource = NULL;
  request.origin = NULL;
  request._payload = NULL;
  request._payload_len = 0;

  request.content_format = APPLICATION_CBOR;
  request.accept = APPLICATION_CBOR;
  request.uri_path = resource_url;
  request.uri_path_len = strlen(resource_url);

  oc_rep_new(response_buffer.buffer, response_buffer.buffer_size);

  // get the value...oc_request_t request_obj;
  oc_interface_mask_t iface_mask = OC_IF_NONE;
  // void *data;
  my_resource->get_handler.cb(&request, iface_mask, NULL);

  // get the data
  // int value_size = request.response->response_buffer->buffer_size;
  int value_size = oc_rep_get_encoded_payload_size();
  uint8_t *value_data = request.response->response_buffer->buffer;

  // Cache value data, as it gets overwritten in oc_issue_do_s_mode
  // uint8_t buf[100];
  if (value_size < buf_size) {
    memcpy(buf, value_data, value_size);
    return value_size;
  }
  OC_ERR(" allocated buf too small to contain s-mode value");
  return 0;
}

void
oc_do_s_mode(char *resource_url, char *rp)
{
  int value_size;
  if (resource_url == NULL) {
    return;
  }

  uint8_t *buffer = malloc(100);
  if (!buffer) {
    OC_WRN("oc_do_s_mode: out of memory allocating buffer");
  } //! buffer

  value_size = oc_do_s_mode_internal(resource_url, rp, buffer, 100);

  oc_resource_t *my_resource =
    oc_ri_get_app_resource_by_uri(resource_url, strlen(resource_url), 0);
  if (my_resource == NULL) {
    PRINT(" oc_do_s_mode : error no URL found %s\n", resource_url);
    return;
  }

  // get the sender ia
  size_t device_index = 0;
  oc_device_info_t *device = oc_core_get_device_info(device_index);
  int sia_value = device->ia;

  int group_address = 0;

  // loop over all group addresses and issue the s-mode command
  int index = oc_core_find_group_object_table_url(resource_url);
  while (index != -1) {
    PRINT("  index : %d\n", index);

    int ga_len = oc_core_find_group_object_table_number_group_entries(index);
    for (int j = 0; j < ga_len; j++) {
      group_address = oc_core_find_group_object_table_group_entry(index, j);
      PRINT("   ga : %d\n", group_address);
      // issue the s-mode command
      oc_issue_s_mode(sia_value, group_address, rp, buffer, value_size);

      // loop over the full recipient table and send a message if the group is
      // there
      for (int j = 0; j < oc_core_get_recipient_table_size(); j++) {
        // oc_core_send_message_recipient_table_index(index, j,
        //                                           buffer, value_size);
      }
    }
    index = oc_core_find_next_group_object_table_url(resource_url, index);
  }
}

// ----------------------------------------------------------------------------

bool
oc_set_s_mode_response_cb(oc_s_mode_response_cb_t my_func)
{
  m_s_mode_cb = my_func;
  return true;
}

oc_s_mode_response_cb_t oc_get_s_mode_response_cb() {
  return m_s_mode_cb;
}
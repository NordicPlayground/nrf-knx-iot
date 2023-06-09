/*
// Copyright (c) 2016 Intel Corporation
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

#include "oc_client_state.h"

#include "messaging/coap/oc_coap.h"
#include "oc_api.h"
#include "oc_discovery.h"
#include "oc_knx_fb.h"
#include "oc_knx_fp.h"

#include "oc_core_res.h"
#include "oc_endpoint.h"

#ifdef OC_SECURITY
#include "security/oc_pstat.h"
#include "security/oc_sdi.h"
#include "security/oc_tls.h"
#endif
#ifdef OC_OSCORE
#include "security/oc_tls.h"
#endif

bool
oc_add_resource_to_wk(oc_resource_t *resource, oc_request_t *request,
                      size_t device_index, size_t *response_length, int matches)
{
  (void)device_index; /* variable not used */
  int length;

  if (resource == NULL) {
    return false;
  }

  if ((oc_string_len(resource->uri) == 0)) {
    return false;
  }

  if (matches > 0) {
    length = oc_rep_add_line_to_buffer(",\n");
    *response_length += length;
  }

  length = oc_rep_add_line_to_buffer("<");
  *response_length += length;

  oc_endpoint_t *eps = oc_connectivity_get_endpoints(request->resource->device);
  oc_string_t ep, uri;
  memset(&uri, 0, sizeof(oc_string_t));
  while (eps != NULL) {
//#ifdef OC_SECURITY
#ifdef OC_OSCORE
    if (eps->flags & SECURED) {
#else
    if ((eps->flags & SECURED) == 0) {
#endif /* OC_SECURITY */
      if (oc_endpoint_to_string(eps, &ep) == 0) {
        length = oc_rep_add_line_to_buffer(oc_string(ep));
        *response_length += length;
        oc_free_string(&ep);
        break;
      }
    }
    eps = eps->next;
  }

  length = oc_rep_add_line_to_buffer(oc_string(resource->uri));
  *response_length += length;

  length = oc_rep_add_line_to_buffer(">;");
  *response_length += length;

  int i;
  int numberofresourcetypes =
    (int)oc_string_array_get_allocated_size(resource->types);

  if (numberofresourcetypes > 0) {
    length = oc_rep_add_line_to_buffer("rt=\"");
    *response_length += length;

    for (i = 0; i < numberofresourcetypes; i++) {
      size_t size = oc_string_array_get_item_size(resource->types, i);
      const char *t =
        (const char *)oc_string_array_get_item(resource->types, i);
      if (size > 0) {

        if (i > 0) {
          // white space as separator of the rt values
          length = oc_rep_add_line_to_buffer(" ");
          *response_length += length;
        }

        length = oc_rep_add_line_size_to_buffer(t, (int)size);
        *response_length += length;
      }
    }

    length = oc_rep_add_line_to_buffer("\";");
    *response_length += length;
  }

  if (resource->interfaces > 0) {
    length = oc_rep_add_line_to_buffer("if=");
    *response_length += length;
    length = oc_frame_interfaces_mask_in_response(resource->interfaces);
    *response_length += length;

    length = oc_rep_add_line_to_buffer(";");
    *response_length += length;
  }

  if (resource->content_type) {
    length = oc_rep_add_line_to_buffer("ct=");
    *response_length += length;
    char my_ct_value[5];
    sprintf((char *)&my_ct_value, "%d", resource->content_type);
    length = oc_rep_add_line_to_buffer(my_ct_value);
    *response_length += length;
  }

  return true;
}

bool
oc_filter_resource(oc_resource_t *resource, oc_request_t *request,
                   size_t device_index, size_t *response_length, int matches)
{
  (void)device_index; /* variable not used */

  if (!oc_filter_resource_by_rt(resource, request)) {
    return false;
  }

  if (!oc_filter_resource_by_if(resource, request)) {
    return false;
  }

  if (!(resource->properties & OC_DISCOVERABLE)) {
    return false;
  }

  return oc_add_resource_to_wk(resource, request, device_index, response_length,
                               matches);
}

int
oc_process_resources(oc_request_t *request, size_t device_index,
                     size_t *response_length)
{
  int matches = 0;

  if (*response_length > 0) {
    /* frame the trailing comma */
    matches = 1;
  }

  oc_resource_t *resource = oc_ri_get_app_resources();
  for (; resource; resource = resource->next) {
    if (resource->device != device_index ||
        !(resource->properties & OC_DISCOVERABLE))
      continue;

    if (oc_filter_resource(resource, request, device_index, response_length,
                           matches)) {
      matches++;
    }
  }

  return matches;
}

static void
oc_wkcore_discovery_handler(oc_request_t *request,
                            oc_interface_mask_t iface_mask, void *data)
{
  (void)data;
  (void)iface_mask;
  size_t response_length = 0;
  int matches = 0;

  /* check if the accept header is link-format */
  if (request->accept != APPLICATION_LINK_FORMAT &&
      request->accept != APPLICATION_JSON) {
    /* handle bad request..
    note below layer ignores this message if it is a multi cast request */
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
    return;
  }

  char *value = NULL;
  size_t value_len;
  char *key;
  size_t key_len;
  char *rt_request = 0;
  int rt_len = 0;
  char *ep_request = 0;
  int ep_len = 0;
  char *if_request = 0;
  int if_len = 0;
  char *d_request = 0;
  int d_len = 0;

  value_len = -1;
  oc_init_query_iterator();
  while (oc_iterate_query(request, &key, &key_len, &value, &value_len) > 0) {
    if (strncmp(key, "rt", key_len) == 0) {
      rt_request = value;
      rt_len = (int)value_len;
    }
    if (strncmp(key, "ep", key_len) == 0) {
      ep_request = value;
      ep_len = (int)value_len;
    }
    if (strncmp(key, "if", key_len) == 0) {
      if_request = value;
      if_len = (int)value_len;
    }
    if (strncmp(key, "d", key_len) == 0) {
      d_request = value;
      d_len = (int)value_len;
    }
  }

  // get the device structure from the request.
  size_t device_index = request->resource->device;
  oc_device_info_t *device = oc_core_get_device_info(device_index);

  /* if device is belongs to a group address
     ?d=urn:knx:g.s.[ga]
     list the data points to which the group address applies to
  */
  if (d_len > 12 && strncmp(d_request, "urn:knx:g.s.", 12) == 0) {
    int group_address = atoi(&d_request[12]);
    PRINT(" group address: %d\n", group_address);
    // if not loaded the we can just return
    oc_lsm_state_t lsm = oc_knx_lsm_state(device_index);
    if (lsm != LSM_S_LOADED) {
      /* handle bad request..
      note below layer ignores this message if it is a multi cast request */
      PRINT(" not loaded!");
      request->response->response_buffer->code =
        oc_status_code(OC_STATUS_BAD_REQUEST);
      return;
    }
    // create the response
    bool added = oc_add_points_in_group_object_table_to_response(
      request, device_index, group_address, &response_length, matches);
    if (added) {
      request->response->response_buffer->content_format =
        APPLICATION_LINK_FORMAT;
      request->response->response_buffer->response_length = response_length;
      request->response->response_buffer->code = oc_status_code(OC_STATUS_OK);
    } else {
      request->response->response_buffer->code =
        oc_status_code(OC_STATUS_BAD_REQUEST);
    }

    return;
  }

  if (if_len == 13 && strncmp(if_request, "urn:knx:if.pm", 13) == 0) {

    if (oc_is_device_mode_in_programming(device_index)) {
      /* device is in programming mode so create the response */
      /* add only the serial number when the interface is if.pm && device is
         in programming mode return <>; ep="urn:knx:sn.<serial-number>" return
         immediately so we do not process any other query params note: we ignore
         the ep=urn:knx:sn.* and if=urn:knx:if.pm concatenation. since that only
         needs to respond when the device is in programming mode
      */
      int size = oc_rep_add_line_to_buffer("<>;ep=\"urn:knx:sn.");
      response_length = response_length + size;
      size = oc_rep_add_line_to_buffer(oc_string(device->serialnumber));
      response_length = response_length + size;
      size = oc_rep_add_line_to_buffer("\"");
      response_length = response_length + size;
      matches = 1;

      request->response->response_buffer->response_length = response_length;
      request->response->response_buffer->content_format =
        APPLICATION_LINK_FORMAT;
      request->response->response_buffer->code = oc_status_code(OC_STATUS_OK);

      PRINT(" oc_wkcore_discovery_handler PM HANDLING: OK\n");
    } else {
      /* device is not in programming mode so ignore this request*/
      request->response->response_buffer->content_format =
        APPLICATION_LINK_FORMAT;
      if (request->origin && (request->origin->flags & MULTICAST) == 0) {
        request->response->response_buffer->code =
          oc_status_code(OC_STATUS_BAD_REQUEST);
      } else {
        request->response->response_buffer->code = OC_IGNORE;
      }
    }
    return;
  }

  /* handle individual address */
  if (if_request != 0 && if_len > 11 &&
      strncmp(if_request, "urn:knx:ia.", 11) == 0) {
    char *if_ia_s = if_request + 11;
    int if_ia_i = atoi(if_ia_s);
    if (if_ia_i == device->ia) {
      /* return the ll entry: </dev/ia>;rt="dpt.value2Ucount";ct=50 */
      int size = oc_rep_add_line_to_buffer("</dev/sa>;rt=\"dpa.0.57\";ct=50,");
      response_length = response_length + size;
      size = oc_rep_add_line_to_buffer("</dev/dp>;rt=\"dpa.0.58\";ct=50,");
      response_length = response_length + size;
      size =
        oc_rep_add_line_to_buffer("</dev/ia>;rt=\"dpt.value2Ucount\";ct=50");
      response_length = response_length + size;

      request->response->response_buffer->response_length = response_length;
      request->response->response_buffer->content_format =
        APPLICATION_LINK_FORMAT;
      request->response->response_buffer->code = oc_status_code(OC_STATUS_OK);

      PRINT(" oc_wkcore_discovery_handler IA HANDLING: OK\n");
      return;
    } else {
      /* should ignore this request*/
      // PRINT(" oc_wkcore_discovery_handler IA HANDLING: IGNORE\n");
      request->response->response_buffer->code = OC_IGNORE;
      return;
    }
  }

  /* handle serial number*/
  if (ep_request != 0 && ep_len > 11 &&
      strncmp(ep_request, "urn:knx:sn.", 11) == 0) {
    /* request for all devices via serial number wild card*/
    char *ep_serialnumber = ep_request + 11;
    bool frame_ep = false;

    if (strncmp(ep_serialnumber, "*", 1) == 0) {
      /* matches wild card*/
      frame_ep = true;
    }
    if (strncmp(oc_string(device->serialnumber), ep_serialnumber,
                strlen(oc_string(device->serialnumber))) == 0) {
      frame_ep = true;
    }
    if (frame_ep) {
      /* return <>; ep="urn:knx:sn.<serial-number>"*/
      int size = oc_rep_add_line_to_buffer("<>;ep=\"urn:knx:sn.");
      response_length = response_length + size;
      size = oc_rep_add_line_to_buffer(oc_string(device->serialnumber));
      response_length = response_length + size;
      size = oc_rep_add_line_to_buffer("\"");
      response_length = response_length + size;
      matches = 1;
    }
  }

  if (rt_len > 0 || if_len > 0) {
    size_t device = request->resource->device;
    PRINT("  oc_wkcore_discovery_handler rt='%.*s'\n", rt_len, rt_request);
    PRINT("  oc_wkcore_discovery_handler if='%.*s'\n", if_len, if_request);
    matches = oc_process_resources(request, device, &response_length);
  }

  oc_add_resource_to_wk(oc_core_get_resource_by_index(OC_DEV, device_index),
                        request, device_index, &response_length, matches);

  oc_add_resource_to_wk(
    oc_core_get_resource_by_index(OC_KNX_AUTH, device_index), request,
    device_index, &response_length, matches);

  oc_add_resource_to_wk(oc_core_get_resource_by_index(OC_KNX_SWU, device_index),
                        request, device_index, &response_length, matches);

  // optional, not yet implemented
  // oc_add_resource_to_wk(oc_core_get_resource_by_index(OC_SUB, device_index),
  //                      request, device_index, &response_length, matches);

  if (request->origin && (request->origin->flags & MULTICAST) == 0) {
    // only for unicast
    bool added = oc_add_function_blocks_to_response(request, device_index,
                                                    &response_length, matches);
    if (added) {
      matches++;
    }
  }

  request->response->response_buffer->content_format = APPLICATION_LINK_FORMAT;

  if (matches > 0 && response_length > 0) {
    PRINT("  oc_wkcore_discovery_handler response_length %d'\n",
          (int)response_length);
    request->response->response_buffer->response_length = response_length;
    request->response->response_buffer->code = oc_status_code(OC_STATUS_OK);
  } else if (request->origin && (request->origin->flags & MULTICAST) == 0) {
    request->response->response_buffer->code =
      oc_status_code(OC_STATUS_BAD_REQUEST);
  } else {
    request->response->response_buffer->code = OC_IGNORE;
  }
}

void
oc_create_discovery_resource(int resource_idx, size_t device)
{
  if (resource_idx == WELLKNOWNCORE) {
    oc_core_populate_resource(resource_idx, device, "/.well-known/core",
                              OC_IF_NONE, APPLICATION_LINK_FORMAT,
                              OC_DISCOVERABLE, oc_wkcore_discovery_handler, 0,
                              0, 0, 1, "wk");
  }
}

oc_discovery_flags_t
oc_ri_process_discovery_payload(uint8_t *payload, int len,
                                oc_client_handler_t client_handler,
                                oc_endpoint_t *endpoint,
                                oc_content_format_t content, void *user_data)
{
  oc_discovery_all_handler_t all_handler = client_handler.discovery_all;

  oc_discovery_flags_t ret = OC_CONTINUE_DISCOVERY;

  if (content == APPLICATION_LINK_FORMAT) {

    PRINT("oc_ri_process_discovery_payload: calling handler all\n");
    if (all_handler) {
      all_handler((const char *)payload, len, endpoint, user_data);
    }
  }

  return ret;
}

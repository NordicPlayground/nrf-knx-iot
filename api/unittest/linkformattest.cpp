/******************************************************************
 *
 * Copyright 2021 Cascoda Ltd All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <cstdlib>
#include <gtest/gtest.h>
#include <stdio.h>
#include <string>

#include "oc_api.h"
#include "oc_core_res.h"
#include "oc_helpers.h"

class TestLinkFormat : public testing::Test {
protected:
  virtual void SetUp()
  {
    // oc_core_init();
    // oc_random_init();
  }
  virtual void TearDown()
  {
    // oc_core_shutdown();
    // oc_random_destroy();
  }
};

int
check_string(const char *input1, const char *input2, int input2_len)
{
  oc_string_t compare;

  oc_new_string(&compare, input2, input2_len);

  EXPECT_STREQ(input1, oc_string(compare))
    << input1 << "  " << oc_string(compare);

  oc_free_string(&compare);

  return 0;
}

TEST_F(TestLinkFormat, LF_full)
{
  const char payload[] =
    "<coap://[fe80::8d4c:632a:c5e7:ae09]:60054/p/a>;rt=\"urn:knx:dpa.352.51\";if=if.a;ct=60,\
    <coap://[fe80::8d4c:632a:c5e7:ae09]:60054/p/b>;if=if.s;rt=\"urn:knx:dpa.352.52\";ct=60,\
    <coap://[fe80::8d4c:632a:c5e7:ae09]:60054/p/c>;ct=60;rt=\"urn:knx:dpa.353.52\";if=if.s,\
    <coap://[fe80::8d4c:632a:c5e7:ae09]:60054/dev>;rt=\"urn:knx:fb.0\";ct=40,\
    <coap://[fe80::8d4c:632a:c5e7:ae09]:60054/swu>;rt=\"urn:knx:fbswu\";ct=40";
  int len = strlen(payload);

  int nr_entries = oc_lf_number_of_entries(payload, len);

  const char *uri;
  int uri_len;
  const char *param;
  int param_len;

  PRINT(" entries %d\n", nr_entries);
  EXPECT_EQ(5, nr_entries);

  int i = 0;
  oc_lf_get_entry_uri(payload, len, i, &uri, &uri_len);
  check_string("coap://[fe80::8d4c:632a:c5e7:ae09]:60054/p/a", uri, uri_len);

  oc_lf_get_entry_param(payload, len, i, "rt", &param, &param_len);
  check_string("\"urn:knx:dpa.352.51\"", param, param_len);

  oc_lf_get_entry_param(payload, len, i, "if", &param, &param_len);
  check_string("if.a", param, param_len);

  oc_lf_get_entry_param(payload, len, i, "ct", &param, &param_len);
  check_string("60", param, param_len);

  i = 1;
  oc_lf_get_entry_uri(payload, len, i, &uri, &uri_len);
  check_string("coap://[fe80::8d4c:632a:c5e7:ae09]:60054/p/b", uri, uri_len);

  oc_lf_get_entry_param(payload, len, i, "rt", &param, &param_len);
  check_string("\"urn:knx:dpa.352.52\"", param, param_len);

  oc_lf_get_entry_param(payload, len, i, "if", &param, &param_len);
  check_string("if.s", param, param_len);

  oc_lf_get_entry_param(payload, len, i, "ct", &param, &param_len);
  check_string("60", param, param_len);

  i = 2;
  oc_lf_get_entry_uri(payload, len, i, &uri, &uri_len);
  check_string("coap://[fe80::8d4c:632a:c5e7:ae09]:60054/p/c", uri, uri_len);

  oc_lf_get_entry_param(payload, len, i, "rt", &param, &param_len);
  check_string("\"urn:knx:dpa.353.52\"", param, param_len);

  oc_lf_get_entry_param(payload, len, i, "if", &param, &param_len);
  check_string("if.s", param, param_len);

  oc_lf_get_entry_param(payload, len, i, "ct", &param, &param_len);
  check_string("60", param, param_len);

  i = 3;
  oc_lf_get_entry_uri(payload, len, i, &uri, &uri_len);
  check_string("coap://[fe80::8d4c:632a:c5e7:ae09]:60054/dev", uri, uri_len);

  oc_lf_get_entry_param(payload, len, i, "rt", &param, &param_len);
  check_string("\"urn:knx:fb.0\"", param, param_len);

  int retval = oc_lf_get_entry_param(payload, len, i, "if", &param, &param_len);
  EXPECT_EQ(0, retval);
  // check_string("if.s", param, param_len);

  oc_lf_get_entry_param(payload, len, i, "ct", &param, &param_len);
  check_string("40", param, param_len);

  i = 4;
  oc_lf_get_entry_uri(payload, len, i, &uri, &uri_len);
  check_string("coap://[fe80::8d4c:632a:c5e7:ae09]:60054/swu", uri, uri_len);

  oc_lf_get_entry_param(payload, len, i, "rt", &param, &param_len);
  check_string("\"urn:knx:fbswu\"", param, param_len);

  retval = oc_lf_get_entry_param(payload, len, i, "if", &param, &param_len);
  EXPECT_EQ(0, retval);

  // check_string("if.s", param, param_len);

  oc_lf_get_entry_param(payload, len, i, "ct", &param, &param_len);
  check_string("40", param, param_len);
}

TEST_F(TestLinkFormat, LF_zero)
{
  const char payload[] =
    "<coap://[fe80::8d4c:632a:c5e7:ae09]:60054/p/a>;rt=\"urn:knx:dpa.352.51\";if=if.a;ct=60,\
    <coap://[fe80::8d4c:632a:c5e7:ae09]:60054/p/b>;if=if.s;rt=\"urn:knx:dpa.352.52\";ct=60,\
    <coap://[fe80::8d4c:632a:c5e7:ae09]:60054/p/c>;ct=60;rt=\"urn:knx:dpa.353.52\";if=if.s,\
    <coap://[fe80::8d4c:632a:c5e7:ae09]:60054/dev>;rt=\"urn:knx:fb.0\";ct=40,\
    <coap://[fe80::8d4c:632a:c5e7:ae09]:60054/swu>;rt=\"urn:knx:fbswu\";ct=40";
  int len = strlen(payload);

  int nr_entries = oc_lf_number_of_entries(NULL, len);
  EXPECT_EQ(0, nr_entries);

  nr_entries = oc_lf_number_of_entries(payload, 0);
  EXPECT_EQ(0, nr_entries);
}

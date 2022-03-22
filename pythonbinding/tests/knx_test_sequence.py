#!/usr/bin/env python
#############################
#
#    copyright 2021 Cascoda Ltd.
#    Redistribution and use in source and binary forms, with or without modification,
#    are permitted provided that the following conditions are met:
#    1.  Redistributions of source code must retain the above copyright notice,
#        this list of conditions and the following disclaimer.
#    2.  Redistributions in binary form must reproduce the above copyright notice,
#        this list of conditions and the following disclaimer in the documentation
#        and/or other materials provided with the distribution.
#
#    THIS SOFTWARE IS PROVIDED "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE OR
#    WARRANTIES OF NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL THE
#    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
#    OR CONSEQUENTIAL DAMAGES
#    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#    LOSS OF USE, DATA, OR PROFITS;OR BUSINESS INTERRUPTION)
#    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#    EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#############################
# pylint: disable=C0103
# pylint: disable=C0114
# pylint: disable=C0115
# pylint: disable=C0116
# pylint: disable=C0201
# pylint: disable=C0209
# pylint: disable=C0413
# pylint: disable=R0801
# pylint: disable=R0902
# pylint: disable=R0913
# pylint: disable=R0915
# pylint: disable=R1732
# pylint: disable=W0702
# pylint: disable=W1514
# pylint: disable=W0212

import argparse
import os
import signal
import sys
import time
import traceback

#add parent directory to path so we can import from there
currentdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(currentdir)
sys.path.append(parentdir)

#from knx_stack import KNXIOTStack
import knx_stack

def print_fail(msg = None):
    print(f"Failure {sys._getframe().f_back.f_lineno}: {msg if msg is not None else ''}")

def safe_print(response):
    if response is not None:
        response.print_payload()
    else:
        print("no response")

def compare_dict(dict1, dict2): # pylint: disable=R0911
    if dict1 is None:
        return False
    if dict2 is None:
        return False
    dict1_set = set(dict1.keys())
    dict2_set = set(dict2.keys())
    # compare the keys
    diff1 = dict1_set.difference(dict2_set)
    if len(diff1) > 0:
        print("compare_dict diff1:", diff1)
        return False
    diff2 = dict2_set.difference(dict1_set)
    if len(diff2) > 0:
        print("compare_dict diff2:", diff2)
        return False
    # compare the values
    dictv1_set = set(dict1.values())
    dictv2_set = set(dict2.values())
    # compare the keys
    diffv1 = dictv1_set.difference(dictv2_set)
    if len(diffv1) > 0:
        print("compare_dict diffv1:", diffv1)
        return False
    diffv2 = dictv2_set.difference(dictv1_set)
    if len(diffv2) > 0:
        print("compare_dict difvf2:", diffv2)
        return False
    return True

def test_discover(my_stack):
    time.sleep(1)
    my_stack.discover_devices()
    if my_stack.get_nr_devices() > 0:
        print ("SN :", my_stack.device_array[0].sn)

def get_sn(my_stack):
    print("Get SN :")
    sn = my_stack.device_array[0].sn
    response = my_stack.issue_cbor_get(sn, "/dev/sn")
    print ("response:", response)
    if response is not None:
        safe_print(response)
    my_stack.purge_response(response)

# no json tags as strings
def do_sequence_dev(my_stack):
    sn = my_stack.device_array[0].sn
    print("========dev get=========")
    print("Get HWT :")
    response = my_stack.issue_cbor_get(sn, "/dev/hwt")
    safe_print(response)
    safe_print(response)
    my_stack.purge_response(response)

    #print("===================")
    #print("Get HWV :");
    #response =  my_stack.issue_cbor_get(sn, "/dev/hwv")
    #safe_print(response)
    #my_stack.purge_response(response)

    #print("===================")
    #print("Get FWV :");
    #response =  my_stack.issue_cbor_get(sn, "/dev/fwv")
    #safe_print(response)
    #my_stack.purge_response(response)

    print("===================")
    print("-------------------")
    print("Get Model :")
    response =  my_stack.issue_cbor_get(sn,"/dev/model")
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    response =  my_stack.issue_cbor_get(sn,"/dev/pm")
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/ia")
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    response =  my_stack.issue_cbor_get(sn,"/dev/hostname")
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    response =  my_stack.issue_cbor_get(sn,"/dev/iid")
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    response =  my_stack.issue_cbor_get(sn,"/dev/ipv6")
    safe_print(response)
    my_stack.purge_response(response)

# no json tags as strings
def do_sequence_dev_programming_mode(my_stack):
    sn = my_stack.device_array[0].sn
    print("========dev programming=========")

    print("-------------------")
    content = True
    print("set PM :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/pm",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/pm")
    safe_print(response)
    safe_print(response)
    if content == response.get_payload_boolean():
        print("PASS : /dev/pm ", content)
    my_stack.purge_response(response)

    print("-------------------")
    content = 44
    print("set IA :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/ia",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/ia")
    safe_print(response)
    if content == response.get_payload_int():
        print("PASS : /dev/ia ", content)
    else:
        print_fail(msg="/dev/ia")
    my_stack.purge_response(response)

    print("-------------------")
    content = "my host name"
    print("set hostname :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/hostname",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/hostname")
    safe_print(response)
    if content == response.get_payload_string():
        print("PASS : /dev/hostname ", content)
    else:
        print_fail(msg="/dev/hostname")
    my_stack.purge_response(response)

    print("-------------------")
    content = 555
    print("set iid :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/iid", content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/iid")
    safe_print(response)
    if content == response.get_payload_int():
        print("PASS : /dev/iid ", content)
    else:
        print_fail(msg="/dev/iid")
    my_stack.purge_response(response)
    print("-------------------")

    content = 7777
    print("set sa :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/sa", content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/sa")
    safe_print(response)
    if content == response.get_payload_int():
        print("PASS : /dev/sa ", content)
    else:
        print_fail(msg="/dev/sa")
    my_stack.purge_response(response)
    print("-------------------")

    content = 777788
    print("set da :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/da", content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/da")
    safe_print(response)
    if content == response.get_payload_int():
        print("PASS : /dev/da ", content)
    else:
        print_fail(msg="/dev/da")
    my_stack.purge_response(response)
    print("-------------------")

    content = 9777788
    print("set port :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/port", content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/port")
    safe_print(response)
    if content == response.get_payload_int():
        print("PASS : /dev/port", content)
    else:
        print_fail(msg="/dev/port")
    my_stack.purge_response(response)
    print("-------------------")


    # reset programming mode
    content = False
    response =  my_stack.issue_cbor_put(sn,"/dev/pm",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/pm")
    safe_print(response)
    safe_print(response)
    if content == response.get_payload_boolean():
        print("PASS : /dev/pm ", content)
    else:
        print_fail(msg="/dev/pm")
    my_stack.purge_response(response)
    print("-------------------")

# no json tags as strings
def do_sequence_dev_programming_mode_fail(my_stack):
    sn = my_stack.device_array[0].sn
    print("========dev programming fail=========")

    print("-------------------")
    content = False
    print("set PM :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/pm",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/pm")
    safe_print(response)
    safe_print(response)
    if content == response.get_payload_boolean():
        print("PASS : /dev/pm ", content)
    else:
        print_fail(msg="/dev/pm")
    my_stack.purge_response(response)

    print("-------------------")
    content = 4444
    print("set IA :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/ia",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/ia")
    safe_print(response)
    safe_print(response)
    if content != response.get_payload_int():
        print("PASS (expected failure): /dev/ia ", content, " != ", response.get_payload_int())
    else:
        print_fail(msg="/dev/ia")
    my_stack.purge_response(response)

    print("-------------------")
    content = "my new host name"
    print("set hostname :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/hostname",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/hostname")
    safe_print(response)
    if content != response.get_payload_string():
        print("PASS (expected failure): /dev/hostname ",
            content, " != ", response.get_payload_string())
    else:
        print_fail(msg="/dev/hostname")
    my_stack.purge_response(response)

    print("-------------------")
    content = 3333
    print("set iid :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/iid",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn,"/dev/iid")
    safe_print(response)
    if content != response.get_payload_int():
        print("PASS (expected failure): /dev/iid ", content, " != ", response.get_payload_int())
    else:
        print_fail(msg="/dev/iid")
    my_stack.purge_response(response)
    print("-------------------")


# cmd ==> 2
def do_sequence_lsm(my_stack):
    print("========lsm=========")
    sn = my_stack.device_array[0].sn

    print("-------------------")
    response =  my_stack.issue_cbor_get(sn, "/a/lsm")
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    #content = {2 : "startLoading"}
    #expect = { "3": "loading"}
    content = { 2: 1}  # startLoading
    expect = { "3" : 2 } #  LSM_S_LOADING = 2,
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn, "/a/lsm")
    safe_print(response)

    if compare_dict(expect, response.get_payload_dict()):
        print("PASS : /a/lsm ", expect, response.get_payload_dict())
    else:
        print_fail(msg="/a/lsm")
    my_stack.purge_response(response)

    print("-------------------")
    content = {2 : 2 } # loadComplete
    expect  = { "3": 1} # loaded
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn, "/a/lsm")
    safe_print(response)
    if compare_dict(expect, response.get_payload_dict()):
        print("PASS : /a/lsm ", expect, response.get_payload_dict())
    else:
        print_fail(msg="/a/lsm")
    my_stack.purge_response(response)

    print("-------------------")
    content = {2 : "unload"}
    expect  = { "3": "unloaded"}
    content = {2 : 4 }
    expect  = { "3": 0 }
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    safe_print(response)
    my_stack.purge_response(response)
    response =  my_stack.issue_cbor_get(sn, "/a/lsm")
    safe_print(response)
    if compare_dict(expect, response.get_payload_dict()):
        print("PASS : /a/lsm ", expect, response.get_payload_dict())
    else:
        print_fail(msg="/a/lsm")
    my_stack.purge_response(response)
    print("-------------------")

def do_check_table(my_stack, sn, table_url, content):
    response =  my_stack.issue_cbor_post(sn,table_url,content)
    safe_print(response)
    my_stack.purge_response(response)

    response =  my_stack.issue_linkformat_get(sn,table_url)
    safe_print(response)
    lf = knx_stack.LinkFormat(response.payload)
    print(" lines:", lf.get_nr_lines())
    # list the response
    for line in lf.get_lines():
        print(" -get------------------------", line)
        print(" url :", lf.get_url(line))
        print(" ct  :", lf.get_ct(line))
        print(" rt  :", lf.get_rt(line))
        # print the cbor data from the entry
        if lf.get_ct(line) == "60" :
            response2 =  my_stack.issue_cbor_get(sn, lf.get_url(line))
            response2.print_payload()
            print ("response2:",response2)
            my_stack.purge_response(response2)
    # delete the entries
    for line in lf.get_lines():
        if lf.get_ct(line) == "60" :
            print(" -delete------------------------", line)
            response2 =  my_stack.issue_cbor_delete(sn, lf.get_url(line))
            response2.print_payload()
            print ("response2:",response2)
            my_stack.purge_response(response2)
    my_stack.purge_response(response)

    response =  my_stack.issue_linkformat_get(sn,table_url)
    safe_print(response)
    lf = knx_stack.LinkFormat(response.payload)
    print(" lines:", lf.get_nr_lines())
    for line in lf.get_lines():
        print(" line :", line)
    if lf.get_nr_lines() == 0:
        print("PASS : (after delete)", table_url)
    else:
        message = table_url + " delete failed"
        print_fail(msg=message)

# cmd ==> 2
def do_sequence_fp_programming(my_stack):
    print("========fp programming=========")
    sn = my_stack.device_array[0].sn

    print("-------------------")
    content = { 2: "reset"}
    print("reset :", content)
    response =  my_stack.issue_cbor_post(sn,"/.well-known/knx",content)
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    content = True
    print("set PM :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/pm",content)
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    content = 455
    print("set IA :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/ia",content)
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    content = 555
    response =  my_stack.issue_cbor_put(sn,"/dev/iid",content)
    safe_print(response)
    my_stack.purge_response(response)

    print("-------------------")
    content = { 2: "startLoading"}
    content = { 2: 1}  # startLoading
    print("lsm :", content)
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    safe_print(response)
    my_stack.purge_response(response)

    response =  my_stack.issue_cbor_get(sn,"/a/lsm")
    safe_print(response)
    my_stack.purge_response(response)

    # group object table
    # id (0)= 1
    # href (11)= /p/light
    # ga (7 )=
    # cflags (8) = ["r" ] ; read = 8, write=16, init=32,transmit=64, update=128
    content = [ {0: 5, 11: "/p/a", 7:[1], 8: 16 } ,
                {0: 2, 11: "/p/b", 7:[2], 8: 16+64 },
                {0: 225, 11: "/p/c", 7:[2], 8: 16+64 }]
    do_check_table(my_stack, sn, "/fp/g",content)

    # too long
    content = [ {0: 5, 11: "p/123456789012345678901234567890", 7:[1], 8: 16 }]
    do_check_table(my_stack, sn, "/fp/g",content)

    content = [ {0: 1, 10: "/p/push1", 7:[1], 12 :3 },
                {0: 5, 112: "/p/push5", 7:[1], 12 : 4 },
                {0: 255, 10: "/p/push255", 7:[1], 12 :5 } ]
    do_check_table(my_stack, sn, "/fp/r",content)

    content = [ {0: 1, 10: "/p/pushpp", 7:[1], 12 :6 },
                {0: 5, 112: "/p/push5", 7:[1], 12 : 7 },
                {0: 235, 10: "/p/push235", 7:[1], 12 :8 } ]
    do_check_table(my_stack, sn, "/fp/p",content)

    # path 112
    content = [ {0: 1, 112 : "/p/123456789012345678901234567890", 7:[1], 12 :6 } ]
    do_check_table(my_stack, sn, "/fp/p",content)
    # url 10
    content = [ {0: 1, 10 : "/p/123456789012345678901234567890", 7:[1], 12 :6 } ]
    do_check_table(my_stack, sn, "/fp/p",content)

    content = False
    print("set PM :", content)
    response =  my_stack.issue_cbor_put(sn,"/dev/pm",content)
    safe_print(response)
    my_stack.purge_response(response)

    content = { 2: 2 } # loadComplete
    print("lsm :", content)
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    print ("response:", response)
    my_stack.purge_response(response)

    response =  my_stack.issue_cbor_get(sn,"/a/lsm")
    print ("response:", response)
    my_stack.purge_response(response)


# ./knx resource
# sia ==> 4
# ga ==> 7
# st ==> 6
def do_sequence_knx_knx_s_mode(my_stack):
    sn = my_stack.device_array[0].sn
    print("========.knx=s-mode========")
    print("-------------------")
    content = { 2: "startLoading"}
    content = { 2: 1}  # startLoading
    print("lsm :", content)
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    safe_print(response)
    my_stack.purge_response(response)

    # group object table
    # id (0)= 1
    # url (11)= /p/light
    # ga (7 )= 1
    # cflags (8) = ["r" ] ; read = 8, write=16, init=32,transmit=64, update=128
    #
    content = [ {0: 5, 11: "p/push5", 7:[1], 8: 8 } ,
                {0: 2, 11: "p/light2", 7:[2], 8: 8+32 },
                {0: 225, 11: "p/light255", 7:[2], 8: 8+64 }]
    response =  my_stack.issue_cbor_post(sn,"/fp/g",content)
    print ("response:",response.get_payload())
    my_stack.purge_response(response)
    #do_check_table(my_stack, sn, "/fp/g",content)

    content = {2 : 2 } # loadComplete
    print("lsm :", content)
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    print ("response:", response)
    my_stack.purge_response(response)

    response =  my_stack.issue_cbor_get(sn, "/.knx")
    safe_print(response)
    my_stack.purge_response(response)

    content = {"value": { 4 : 5, 7: 7777 , 6 : "w"}}
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    safe_print(response)
    my_stack.purge_response(response)

    response =  my_stack.issue_cbor_get(sn, "/.knx")
    safe_print(response)
    my_stack.purge_response(response)

    content = {"value": { 4 : 5, 7: 7777 , 6 : "r"}}
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    safe_print(response)
    my_stack.purge_response(response)

    response =  my_stack.issue_cbor_get(sn, "/.knx")
    safe_print(response)
    my_stack.purge_response(response)

# ./knx resource
# sia ==> 4
# ga ==> 7
# st ==> 6
def do_sequence_knx_knx_recipient(my_stack):
    sn = my_stack.device_array[0].sn
    print("========.knx=recipient========")
    print("-------------------")
    content = { 2: "startLoading"}
    content = { 2: 1}  # startLoading
    print("lsm :", content)
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    safe_print(response)
    my_stack.purge_response(response)

    # recipient table, setting the destination
    # id (0)= 1
    # ia (12) = 1 + path (112)= .knx
    # url (10)= /p/light
    # ga (7 )= 1
    content = [ {0: 5, 12: 1, 112: ".knx", 7:[1] } ,
                {0: 2, 12: 1, 112: ".knx-temp", 7:[2] },
                {0: 10, 12: 1, 7:[2] },
                {0: 225, 10: "coap://p/light255", 7:[2] }]
    response =  my_stack.issue_cbor_post(sn,"/fp/r",content)
    if response is not None:
        print ("response:",response.get_payload())
    my_stack.purge_response(response)
    #do_check_table(my_stack, sn, "/fp/g",content)

    content = {2 : 2 } # loadComplete
    print("lsm :", content)
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    print ("response:", response)
    my_stack.purge_response(response)

    response =  my_stack.issue_cbor_get(sn, "/.knx")
    safe_print(response)
    my_stack.purge_response(response)

    content = {"value": { 4 : 5, 7: 7777 , 6 : "rp"}}
    response =  my_stack.issue_cbor_post(sn,"/a/lsm",content)
    safe_print(response)
    my_stack.purge_response(response)

    response =  my_stack.issue_cbor_get(sn, "/.knx")
    safe_print(response)
    my_stack.purge_response(response)

def do_sequence_knx_osn(my_stack):
    sn = my_stack.device_array[0].sn
    print("========knx/osn=========")
    response =  my_stack.issue_cbor_get(sn, "/.well-known/knx/osn")
    print("response:",response)
    print("   value:", response.get_payload_int())
    my_stack.purge_response(response)

def do_sequence_knx_crc(my_stack):
    #  url, content, accept, contents
    sn = my_stack.device_array[0].sn
    print("========knx/fingerprint=========")
    response =  my_stack.issue_cbor_get(sn, "/.well-known/knx/f")
    safe_print(response)
    print("   value:", response.get_payload_int())
    my_stack.purge_response(response)

def do_sequence_core_knx(my_stack):
    sn = my_stack.device_array[0].sn
    print("=get=======.well-known/knx=========")
    response =  my_stack.issue_cbor_get(sn, "/.well-known/knx")
    safe_print(response)
    print ("    value:", response.get_payload_dict())
    my_stack.purge_response(response)

def do_sequence_a_sen(my_stack):
    sn = my_stack.device_array[0].sn
    print("========/a/sen=========")
    content = {2: "reset"}
    response =  my_stack.issue_cbor_post(sn,"/a/sen",content)
    print ("response:", response)
    my_stack.purge_response(response)

def do_sequence_f(my_stack):
    sn = my_stack.device_array[0].sn
    print("========/f=========")
    response =  my_stack.issue_linkformat_get(sn, "/f")
    print ("response:", response)
    lf = knx_stack.LinkFormat(response.payload)
    print(" lines:", lf.get_nr_lines())
    for line in lf.get_lines():
        print(line)
    for line in lf.get_lines():
        print(" -------------------------")
        print(" url :", lf.get_url(line))
        print(" ct  :", lf.get_ct(line))
        print(" rt  :", lf.get_rt(line))
        if lf.get_ct(line) == "40" :
            response2 =  my_stack.issue_linkformat_get(sn, lf.get_url(line))
            lf2 = knx_stack.LinkFormat(response2.payload)
            print ("response2:",response2)
            for line2 in lf2.get_lines():
                print("    -------------------------",lf.get_url(line) )
                print("    url :", lf2.get_url(line2))
                print("    ct  :", lf2.get_ct(line2))
                print("    rt  :", lf2.get_rt(line2))
            my_stack.purge_response(response2)
    my_stack.purge_response(response)


def do_auth_at(my_stack):
    sn = my_stack.device_array[0].sn
    print("========/auth/at=========")
    response =  my_stack.issue_linkformat_get(sn, "/auth/at")
    print ("response:", response)
    lf = knx_stack.LinkFormat(response.payload)
    print(" lines:", lf.get_nr_lines())
    my_stack.purge_response(response)
    # DTLS
    #Req : Content-Format: "application/cbor"
    #Payload:
    #[{
    #"id": "BC6BLLhk56...",          (0)
    #"scope": ["<if.scope>"],                 (9)
    #"ace_profile": "coap_dtls",              (19)
    #"sub":  "<.inst1.local>",                (2)
    #"cnf": {                                 (8)
    #"kid": "<trust list [X.509]fingerprint>" (2)
    #}}]
    #
    # 0 = id (token)
    # 9 = scope
    # 8 = cnf (configuration)
    #
    # OSCORE payload
    # {
    #"id": "23451.",                 (0)
    #"profile": 2,       (19)
    #"scope": ["if.sec"],             (9)
    #"cnf": {                         (8)
    #"osc": {                         (4)
    #"alg": "AES-CCM-16-64-128",     (4) - string
    #"id": "<kid>",                  (6)
    #"ms": "ca5….4dd43e4c"           (2)
    #}}}
    #
    # DTLS payload
    #{
    #"0": "OC5BLLhkAG...",
    #"2": "<asn-system-id.ldevid>",
    # "8": {
    # "3": "<kid>"
    # },
    # "9": ["if.sec"],
    # "19": 1
    #}
    content = [ { 0: "my_dtls_token", 2: "<asn>", 9 : ["if.a", "if.c", "if.sec"], 19: 1,
                  8 : {3: "mykid" } },
                { 0: "my_oscore_token", 9 : ["if.a", "if.pm"], 19:2,
                  8 : { 4: { 6: "mykid", 2: "my_ms", 4:"AES-CCM-16-64-128" } } },
                { 0: "my_oscore_token_ga_1", 9 : [100], 19:2,
                  8 : { 4: { 6: "mykid", 2: "my_ms", 4:"AES-CCM-16-64-128" } } },
                { 0: "my_oscore_token_2", 3: "myaudience", 9 : [500, 600, 700], 19:2,
                  8 : { 4: { 6: "mykid", 2: "my_ms", 4:"AES-CCM-16-64-128" } } }
              ]
    response =  my_stack.issue_cbor_post(sn,"/auth/at",content)
    my_stack.purge_response(response)
    response =  my_stack.issue_linkformat_get(sn, "/auth/at")
    print ("response:", response)
    lf = knx_stack.LinkFormat(response.payload)
    print(" lines:", lf.get_nr_lines())
    for line in lf.get_lines():
        print(line)
    for line in lf.get_lines():
        print(" -------------------------")
        print(" url :", lf.get_url(line))
        print(" ct  :", lf.get_ct(line))
        print(" rt  :", lf.get_rt(line))
        response3 =  my_stack.issue_cbor_get(sn, lf.get_url(line))
        print ("response:",response3)
        print ("    value:", response3.get_payload_dict())
        my_stack.purge_response(response3)
    for line in lf.get_lines():
        print(" ---------DELETE----------------")
        print(" url :", lf.get_url(line))
        response3 =  my_stack.issue_cbor_delete(sn, lf.get_url(line))
        print ("response:",response3)
        print ("    value:", response3.get_payload_dict())
        my_stack.purge_response(response3)
    print(" --------EMPTY-----------------")
    response =  my_stack.issue_linkformat_get(sn, "/auth/at")
    print ("response:", response)
    lf = knx_stack.LinkFormat(response.payload)
    print(" lines:", lf.get_nr_lines())
    for line in lf.get_lines():
        print(line)


def do_discovery_tests(my_stack):
    print("========discovery=========")
    # group address
    data = my_stack.discover_devices_with_query_data("d=urn:knx:g.s.1")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # all devices
    data = my_stack.discover_devices_with_query_data("rt=urn:knx:dpa.*")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # devices with 353
    data = my_stack.discover_devices_with_query_data("rt=urn:knx:dpa.353*")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # devices with 352
    data = my_stack.discover_devices_with_query_data("rt=urn:knx:dpa.352*")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # devices with ia = 5
    data = my_stack.discover_devices_with_query_data("if=urn:knx:ia.5")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # devices with all interfaces
    data = my_stack.discover_devices_with_query_data("if=urn:knx:if.*")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # devices with sensor interfaces
    data = my_stack.discover_devices_with_query_data("if=urn:knx:if.s")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # devices with actuator interfaces
    data = my_stack.discover_devices_with_query_data("if=urn:knx:if.a")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # devices programming mode filtering
    data = my_stack.discover_devices_with_query_data("if=urn:knx:if.pm")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # compound filtering actuator datapoint
    data = my_stack.discover_devices_with_query_data("if=urn:knx:if.a&rt=urn:knx:dpa.353*")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # compound filtering sensor and data point
    data = my_stack.discover_devices_with_query_data("if=urn:knx:if.s&rt=urn:knx:dpa.352*")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # serial number wild card
    data = my_stack.discover_devices_with_query_data("ep=urn:knx:sn.*")
    print(" -------------------------")
    print (data)
    print(" -------------------------")
    # serial specific value
    data = my_stack.discover_devices_with_query_data("ep=urn:knx:sn.012346")
    print(" -------------------------")
    print (data)
    print(" -------------------------")


def do_sequence(my_stack):
    if my_stack.get_nr_devices() > 0:
        get_sn(my_stack)
        #do_discovery_tests(my_stack)
        #return
        #do_sequence_knx_knx_s_mode(my_stack)
        do_sequence_knx_knx_recipient(my_stack)
        #return
        do_sequence_dev(my_stack)
        do_sequence_dev_programming_mode(my_stack)
        do_sequence_dev_programming_mode_fail(my_stack)
        do_sequence_f(my_stack)
        do_sequence_lsm(my_stack)
        do_sequence_fp_programming(my_stack)
        #return
        do_sequence_knx_crc(my_stack)
        do_sequence_knx_osn(my_stack)
        do_sequence_core_knx(my_stack)
        do_auth_at(my_stack)
        return
        # .knx
        #do_sequence_a_sen(my_stack)

def do_discovery(my_stack):
    """
    fb discovery recursive
    """
    if my_stack.get_nr_devices() > 0:
        data = my_stack.discover_devices_with_query_data("rt=urn:knx:dpa.*")
        print(" -------------------------")
        print (data)
        print(" -------------------------")
        do_sequence_f(my_stack)

def do_security(my_stack):
    """
    do security tests
    """
    if my_stack.get_nr_devices() > 0:
        do_auth_at(my_stack)


def do_spake(my_stack):
    """
    do spake handshake
    """
    if my_stack.get_nr_devices() > 0:
        sn = my_stack.device_array[0].sn
        print("========spake=========", sn)
        my_stack.initate_spake(sn, "LETTUCE")

def do_fp(my_stack):
    """
    do fp tests
    """
    if my_stack.get_nr_devices() > 0:
        do_sequence_fp_programming(my_stack)

def do_all(my_stack):
    if my_stack.get_nr_devices() > 0:
        do_sequence(my_stack)
        do_discovery_tests(my_stack)
        do_sequence_knx_knx_s_mode(my_stack)
        do_sequence_knx_knx_recipient(my_stack)
        return
        # .knx
        #do_sequence_a_sen(my_stack)

if __name__ == '__main__':  # pragma: no cover

    parser = argparse.ArgumentParser()

    # input (files etc.)
    #parser.add_argument("-sn", "--serialnumber",
    #                help="serial number of the device", nargs='?',
    #                const=1, required=True)
    parser.add_argument("-scope", "--scope",
                    help="scope of the multicast request [2,5]", nargs='?',
                    default=2, const=1, required=False)
    parser.add_argument("-sleep", "--sleep",
                    help="sleep in seconds", nargs='?',
                    default=0, const=1, required=False)
    parser.add_argument("-all", "--all",
                    help="do all tests", nargs='?',
                    default=False, const=1, required=False)
    parser.add_argument("-disc", "--discovery",
                    help="do discovery", nargs='?',
                    default=False, const=1, required=False)
    parser.add_argument("-sec", "--security",
                    help="do security", nargs='?',
                    default=False, const=1, required=False)
    parser.add_argument("-fp", "--fp",
                    help="do function point programming", nargs='?',
                    default=False, const=1, required=False)
    parser.add_argument("-spake", "--spake",
                    help="do spake", nargs='?',
                    default=False, const=1, required=False)
    print(sys.argv)
    args = parser.parse_args()
    print("scope         :" + str(args.scope))
    print("sleep         :" + str(args.sleep))
    print("all           :" + str(args.all))
    print("discovery     :" + str(args.discovery))
    print("security      :" + str(args.security))
    print("fp            :" + str(args.fp))
    print("spake         :" + str(args.spake))
    time.sleep(int(args.sleep))

    the_stack = knx_stack.KNXIOTStack()
    signal.signal(signal.SIGINT, the_stack.sig_handler)

    try:
        test_discover(the_stack)
        if args.all:
            do_all(the_stack)
        elif args.discovery:
            do_discovery(the_stack)
        elif args.security:
            do_security(the_stack)
        elif args.fp:
            do_fp(the_stack)
        elif args.spake:
            do_spake(the_stack)
        else:
            do_sequence(the_stack)
    except:
        traceback.print_exc()

    time.sleep(2)
    the_stack.quit()
    sys.exit()

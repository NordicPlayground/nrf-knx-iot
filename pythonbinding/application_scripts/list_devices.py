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

import sys
import time
import traceback
import os
import signal
import argparse

#add parent directory to path so we can import from there
currentdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(currentdir)
sys.path.append(parentdir)

import knx_stack

def do_discover_all(my_stack, scope = 2):
    my_stack.discover_devices( int(scope))
    if my_stack.get_nr_devices() > 0:
        print ("SN :", my_stack.device_array[0].sn)

if __name__ == '__main__':  # pragma: no cover

    parser = argparse.ArgumentParser()

    # input (files etc.)
    parser.add_argument("-scope", "--scope",
                    help="scope of the multicast request [2,5]", nargs='?',
                    default=2, const=1, required=False)
    # (args) supports batch scripts providing arguments
    print(sys.argv)
    args = parser.parse_args()

    print("scope         :" + str(args.scope))

    the_stack = knx_stack.KNXIOTStack()
    signal.signal(signal.SIGINT, the_stack.sig_handler)
    # wait until the stack is initialized
    time.sleep(1)

    try:
        do_discover_all(the_stack, args.scope)
    except:
        traceback.print_exc()
    the_stack.quit()
    sys.exit()
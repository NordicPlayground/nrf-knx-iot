.. _light_switch_sensor:

Light Switch Sensor
###################

This sample demonstrates KNX IoT Point API device with one or four Light Switch Sensor basic Functional Blocks.
Each of them is built with a single Switch On/Off input Datapoint (Datapointype ID: 1.001, Name: DPT_Switch).

Supported devices
*****************

The sample supports the following development kits:

+--------------------+----------+--------------------------+------------------------------+
| Hardware platforms | PCA      | Board name               | Build target                 |
+====================+==========+==========================+==============================+
| nRF52840 DK        | PCA10056 | nrf52840dk_nrf52840      | ``nrf52840dk_nrf52840``      |
+--------------------+----------+--------------------------+------------------------------+
| nRF5340 DK         | PCA10095 | nrf5340dk_nrf5340_cpuapp | ``nrf5340dk_nrf5340_cpuapp`` |
+--------------------+----------+--------------------------+------------------------------+
| Thingy:53          | PCA20053 | thingy53_nrf5340_cpuapp  | ``thingy53_nrf5340_cpuapp``  |
+--------------------+----------+--------------------------+------------------------------+

Building and running
********************

   Build the sample by following the steps in the `Building and programming an application`_ page.

.. note::
   This sample can be found in the :file:`knx/samples/light_switch_sensor` directory.

Testing
*******

Complete the following steps to test and debug the sample in the nRF Connect SDK:

1. Follow `Testing and debugging an application`_ guidelines.
   After successfully building the sample and flashing manufacturing data, the sample is ready to use.

#. To refresh the logs, restart the program by pressing the :guilabel:`RESET` button on your development kit.
   You should see the output similar to the following:

.. code-block:: console

   *** Booting Zephyr OS build v3.2.99-ncs1 ***
   Loading Group Object Table from Persistent storage
   Loading Group Recipient Table from Persistent storage
   Loading Group Publisher Table from Persistent storage
   Loading AT Table from Persistent storage
   Serial Number: LSSB0
   Loading Device Config from Persistent storage
     idd (storage) 0
     sa (storage) 0
     da (storage) 0
   oc_knx_load_state: Loading Device Config from Persistent storage
   Register Resource with local path "/p/1"
   Light Switching Sensor 421.61 (LSSB) : SwitchOnOff 
   Data point 417.61 (DPT_Switch) 
   Register Resource with local path "/p/2"
   Light Switching Sensor 421.61 (LSSB) : SwitchOnOff 
   Data point 417.61 (DPT_Switch) 
   Register Resource with local path "/p/3"
   Light Switching Sensor 421.61 (LSSB) : SwitchOnOff 
   Data point 417.61 (DPT_Switch) 
   Register Resource with local path "/p/4"
   Light Switching Sensor 421.61 (LSSB) : SwitchOnOff 
   Data point 417.61 (DPT_Switch) 
   factory_presets_cb: NOP
   oc_register_group_multicasts: port 0 
   oc_init_datapoints_at_initialization
   OSCORE - Disabled
   Server "Sensor (LSSB) 417.61" running, waiting for incoming connections.
   uart:~$

OpenThread configuration
========================

The application starts the Thread interface automatically, and either joins an existing network or creates a new one.

A mesh network uses the following configuration:

+------------------+-------------------------------------+
| Parameter        | Value                               |
+==================+=====================================+
| PAN ID           | 0xabcd                              |
+------------------+-------------------------------------+
| Channel          | 11                                  |
+------------------+-------------------------------------+
| Network name     | KNX                                 |
+------------------+-------------------------------------+
| Extended PAN ID  | dead00beef00cafe                    |
+------------------+-------------------------------------+
| Network key      | 00112233445566778899aabbccddeeff    |
+------------------+-------------------------------------+

.. _Building and programming an application: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started/programming.html#gs-programming
.. _Testing and debugging an application: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started/testing.html#gs-testing

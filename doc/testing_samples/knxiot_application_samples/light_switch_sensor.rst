.. _light_switch_sensor:

Light Switch Sensor
###################

This sample demonstrates KNX IoT Point API device with four Light Switch Sensor basic Functional Blocks.
Each of them is built with a single Switch On/Off input Datapoint (Datapointype ID: 1.001, Name: DPT_Switch).

Check the `KNX Interworking Datapoint Types`_ specification for details.

Requirements
************

The sample supports the following development kits:

+--------------------+----------+----------------------+-------------------------+
| Hardware platforms | PCA      | Board name           | Build target            |
+====================+==========+======================+=========================+
| nRF52840 DK        | PCA10056 | nrf52840dk_nrf52840  | ``nrf52840dk_nrf52840`` |
+--------------------+----------+----------------------+-------------------------+

Building and running
********************
This sample can be found in the :file:`samples/light_switch_sensor` directory.

.. note::
      Zephyr SDK is installed in NCS directory, thus the following additional environment variables must be set:
      
      .. code-block:: bash
      
        export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
        export ZEPHYR_SDK_INSTALL_DIR=<NCS_INSTALL_DIR>/toolchains/v2.3.0/opt/zephyr-sdk/

      And run this command:

      .. code-block:: bash
      
        source <NCS_INSTALL_DIR>/v2.3.0/zephyr/zephyr-env.sh
      
      where :guilabel:`<NCS_INSTALL_DIR>`` is NCS's directory path.

To build the sample, follow the steps in the `Building and programming an application`_ page.

Testing
*******

See `Testing and debugging an application`_ for information on testing and debugging in the nRF Connect SDK.

After successfully building the sample and flashing manufacturing data, the sample is ready to use.
To refresh the logs, restart the program by pressing the :guilabel:`RESET` button on your development kit.
You should see output similar to the following:

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

The application starts the Thread interface automatically, and either joins or creates a new network.
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
.. _KNX Interworking Datapoint Types: https://www.knx.org/wAssets/docs/downloads/Certification/Interworking-Datapoint-types/03_07_02-Datapoint-Types-v02.02.01-AS.pdf
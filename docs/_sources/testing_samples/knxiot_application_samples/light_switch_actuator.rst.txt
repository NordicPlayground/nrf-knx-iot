.. _light_switch_actuator:

Light Switch Actuator
#####################

This sample demonstrates KNX IoT Point API device with four Light Switch Actuator basic Functional Blocks.
Each of them is built with a single Switch On/Off input Datapoint (Datapointype ID: 1.001, Name: DPT_Switch).

Check the `KNX Interworking Datapoint Types`_ specification for details.

Supported devices
*****************

The sample supports the following development kit:

.. warning::

   Nowy hardware: nRF5340 DK i Thingy53

+--------------------+----------+--------------------------+------------------------------+
| Hardware platforms | PCA      | Board name               | Build target                 |
+====================+==========+==========================+==============================+
| nRF52840 DK        | PCA10056 | nrf52840dk_nrf52840      | ``nrf52840dk_nrf52840``      |
+--------------------+----------+--------------------------+------------------------------+
| nRF5340 DK         | PCA10095 | nrf5340dk_nrf5340_cpuapp | ``nrf5340dk_nrf5340_cpuapp`` |
+--------------------+----------+--------------------------+------------------------------+
| Thingy53           | PCA20053 | thingy53_nrf5340_cpuapp  | ``thingy53_nrf5340_cpuapp``  |
+--------------------+----------+--------------------------+------------------------------+

Building and running
********************

.. note::
   Zephyr SDK is installed in NCS directory.

1. Set additional environment variables as follows:

   .. code-block:: bash

      export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
      export ZEPHYR_SDK_INSTALL_DIR=<NCS_INSTALL_DIR>/toolchains/v2.3.0/opt/zephyr-sdk/

#. Activate the Zephyr environment as follows:

   .. code-block:: bash

      source <NCS_INSTALL_DIR>/v2.3.0/zephyr/zephyr-env.sh

   where :guilabel:`<NCS_INSTALL_DIR>` is an NCS directory path.

#. Build the sample by following the steps in the `Building and programming an application`_ page.

.. note::
   This sample can be found in the :file:`samples/light_switch_actuator` directory.

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
   Serial Number: LSAB0
   Loading Device Config from Persistent storage
     idd (storage) 0
     sa (storage) 0
     da (storage) 0
   oc_knx_load_state: Loading Device Config from Persistent storage
   Register Resource with local path "/p/1"
   Light Switching actuator 417 (LSAB) : SwitchOnOff 
   Data point 417.61 (DPT_Switch) 
   Register Resource with local path "/p/2"
   Light Switching actuator 417 (LSAB) : SwitchOnOff 
   Data point 417.61 (DPT_Switch) 
   Register Resource with local path "/p/3"
   Light Switching actuator 417 (LSAB) : SwitchOnOff 
   Data point 417.61 (DPT_Switch) 
   Register Resource with local path "/p/4"
   Light Switching actuator 417 (LSAB) : SwitchOnOff 
   Data point 417.61 (DPT_Switch) 
   factory_presets_cb: NOP
   oc_register_group_multicasts: port 0 
   oc_init_datapoints_at_initialization
   OSCORE - Disabled
   Server "Actuator (LSAB) 417" running, waiting for incoming connections.
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
.. _KNX Interworking Datapoint Types: https://www.knx.org/wAssets/docs/downloads/Certification/Interworking-Datapoint-types/03_07_02-Datapoint-Types-v02.02.01-AS.pdf
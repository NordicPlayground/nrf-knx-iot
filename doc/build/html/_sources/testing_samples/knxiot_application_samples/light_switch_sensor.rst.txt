.. _light_switch_sensor:

Light Switch Sensor
###################

This sample demonstrates KNX IoT Point API device with four **Light Switch Sensor Basic** Functional Blocks where each is built 
with a single **Switch On/Off** input Datapoint (Datapointype ID: 1.001, Name: DPT_Switch).

For details check `KNX Interworking Datapoint Types`_ specification.

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
This sample can be found under :file:`samples/light_switch_sensor`.

To build the sample, follow the steps in the `Building and programming an application`_ documentation.

Testing
*******

See `Testing and debugging an application`_ for information on testing and debugging in the nRF Connect SDK.

After successfully building the sample and flashing manufacturing data, the sample is ready to use.
To refresh the logs, restart the program by pressing the :guilabel:`RESET` button on your development kit.
You should see the following output:

  .. note:: 
     PLACEHOLDER PLACEHOLDER PLACEHOLDER PLACEHOLDER PLACEHOLDER PLACEHOLDER PLACEHOLDER PLACEHOLDER PLACEHOLDER 

OpenThread configuration
========================

Application starts automatically Thread interface and either join or create a new network. Mesh network uses the following configuration:

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
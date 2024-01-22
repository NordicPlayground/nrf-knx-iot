.. _case_3:

Case 3 - mixed configuration
############################

Having Thread network configured and operational, you can start KNX IoT device configuration and testing.
One device must be configured as an Actuator and one as a Sensor.

Perform configuration/mapping of buttons built in to KNX IoT sensor to control LEDs built in to the KNX IoT Actuator - one button to multiple LEDs.
It is achieved by setting KNX Actuator `nRF52840 DK`_ to receive events from multple groups.

#. Configure Actuator device by adding entries to Actuator's Group Object Table.

   .. code-block:: console

      uart:~$ knx got add 1 /p/1 22 [1,2,3,4]
      uart:~$ knx got add 2 /p/2 22 [2,3,4]
      uart:~$ knx got add 3 /p/3 22 [3,4]
      uart:~$ knx got add 4 /p/4 22 [4]

#. Set a device installation identifier and a device individual address for the Actuator device.

   .. code-block:: console

      uart:~$ knx dev iid 1
      uart:~$ knx dev ia 1

#. Configure Sensor device by adding entries to Sensor's Group Object Table.

   .. code-block:: console

      uart:~$ knx got add 1 /p/1 22 [1]
      uart:~$ knx got add 2 /p/2 22 [2]
      uart:~$ knx got add 3 /p/3 22 [3]
      uart:~$ knx got add 4 /p/4 22 [4]

#. Set a device installation identifier and a device individual address for the Sensor device.

   .. code-block:: console

      uart:~$ knx dev iid 1
      uart:~$ knx dev ia 2

#. Devices are configured. Use one of the Sensor's buttons to control one or many of the assigned Actuator's LEDs. 
   The interconnections between buttons and LEDs are presented in the following table:

   +----------------+------------------------+
   | Control button | Controlled LEDs        |
   | (Sensor)       | (Actuator)             |
   +================+========================+
   | BUTTON1        | LED1                   |
   +----------------+------------------------+
   | BUTTON2        | LED1, LED2             |
   +----------------+------------------------+
   | BUTTON3        | LED1, LED2, LED3       |
   +----------------+------------------------+
   | BUTTON4        | LED1, LED2, LED3, LED4 |
   +----------------+------------------------+

.. _nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK

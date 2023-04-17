.. _case_2:

Case 2 - one button mapped to one LED
#####################################

Having Thread network configured and operational, you can start KNX IoT device configuration and testing.
One device must be configured as an Actuator and one as a Sensor.

Perform configuration/mapping of buttons built in to KNX IoT Sensor `nRF52840 DK`_ to control LEDs built in to the KNX IoT Actuator `nRF52840 DK`_ - one button to one LED.
It is achieved by dedicated on KNX group to every input-output pair.

#. Configure the Actuator device by adding entries to Actuator's Group Object Table.

   .. code-block:: console

      uart:~$ knx got add 1 /p/1 22 [1]
      uart:~$ knx got add 2 /p/2 22 [2]
      uart:~$ knx got add 3 /p/3 22 [3]
      uart:~$ knx got add 4 /p/4 22 [4]

#. Set device installation identifier and device individual address for Actuator device.

   .. code-block:: console

      uart:~$ knx dev iid 1
      uart:~$ knx dev ia 1

#. Configure the Sensor device by adding entries to Sensor's Group Object Table.

   .. code-block:: console

      uart:~$ knx got add 1 /p/1 22 [1]
      uart:~$ knx got add 2 /p/2 22 [2]
      uart:~$ knx got add 3 /p/3 22 [3]
      uart:~$ knx got add 4 /p/4 22 [4]

#. Set device installation identifier and device individual address for Sensor device.

   .. code-block:: console

      uart:~$ knx dev iid 1
      uart:~$ knx dev ia 2

#. Devices are configured.
   Use one of the sensor's buttons to control one of the assigned actuator's LEDs.
   The interconnections between buttons and LEDs are presented in the following table:

   +----------------+----------------+
   | Control button | Controlled LED |
   | (Sensor)       | (Actuator)     |
   +================+================+
   | BUTTON1        | LED1           |
   +----------------+----------------+
   | BUTTON2        | LED2           |
   +----------------+----------------+
   | BUTTON3        | LED3           |
   +----------------+----------------+
   | BUTTON4        | LED4           |
   +----------------+----------------+

.. _nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK

.. _case_1:

Case 1 - one button mapped to four LEDs
#######################################

Having Thread network configured and operational, you can start KNX IoT device configuration and testing.
One device must be configured as the Actuator and one as the Sensor.

One button of KNX IoT Sensor `nRF52840 DK`_ device will control 4 LEDs built in to KNX IoT Actuator `nRF52840 DK`_ device.

#. Configure the Actuator device by adding entries to Actuator's Group Object Table.

   .. code-block:: console

      uart:~$ knx got add 1 /p/1 22 [1]
      uart:~$ knx got add 2 /p/2 22 [1]
      uart:~$ knx got add 3 /p/3 22 [1]
      uart:~$ knx got add 4 /p/4 22 [1]

#. Set device installation identifier and device individual address for Actuator device.

   .. code-block:: console

      uart:~$ knx dev iid 1
      uart:~$ knx dev ia 1

#. Configure the Sensor device by adding entries to Sensor's Group Object Table.

   .. code-block:: console

      uart:~$ knx got add 1 /p/1 22 [1]

#. Set device installation identifier and device individual address for Sensor device.

   .. code-block:: console

      uart:~$ knx dev iid 1
      uart:~$ knx dev ia 2

#. Devices are configured.
   Use sensor's BUTTON1 to disable or enable all of the assigned actuator's LEDs.
   Usage of the other buttons has no effect.
   The interconnections between buttons and LEDs are presented in the following table:

   +----------------+------------------------+
   | Control button | Controlled LEDs        |
   | (Sensor)       | (Actuator)             |
   +================+========================+
   | BUTTON1        | LED1, LED2, LED3, LED4 |
   +----------------+------------------------+

.. _nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK

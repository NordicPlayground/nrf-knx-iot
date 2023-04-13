..  _knxiot_testing:

KNX IoT testing
###############

With the :ref:`light_switch_actuator` and :ref:`light_switch_sensor` installed, you will be able to test a simple KNX IoT system consisted of two Thread-based devices.

.. _starting_thread_network:

Starting Thread network
***********************

The devices enable the Thread interface during startup automatically.
They are configured to create and join Thread network with the following parameters:

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

To verify Thread devices status, do the following:

1. Check active data.

   .. code-block:: console

      uart:~$ ot dataset active
      Active Timestamp: 1
      Channel: 15
      Channel Mask: 0x07fff800
      Ext PAN ID: 3b78ce629cbc8e23
      Mesh Local Prefix: fdd7:9fa0:468:ba1b::/64
      Network Key: 00112233445566778899aabbccddeeff
      Network Name: OpenThread-7cf7
      PAN ID: 0x7cf7
      PSKc: b2e04e90cef89d25981a7dd8f3c16db5
      Security Policy: 672 onrc
      Done

#. Verify devices' roles.

  * Check if one of devices is set as a Thread leader.

    .. code-block:: console
 
       uart:~$ ot state
       leader
       Done
    
  * Check if another device is either a Thread child or leader:
	
    .. code-block:: console

       uart:~$ ot state
       child
       Done

    .. code-block:: console

       uart:~$ ot state
       leader
       Done	

	  
Connecting Light Switch Sensor to Light Switch Actuator
*******************************************************

KNX IoT devices, just as the classic KNX devices, consist of one or many Functional Blocks.
The Functional Block contains one or more Datapoints which are its inputs, outputs, and parameters.

The most important feature of Datapoint's description is its type.
The type specifies how the Datapoint is encoded, its supported range, units, an so on.
All Datapoints of given Functional Block and featured types are described in the `KNX Specification`_ page.

Both Light Switch Actuator and Sensor are built with 4 Functional Blocks with a single Switch On/Off Datapoint (type name: DPT_Switch, id: 1.001).
Every Datapoint controls a single `Nordic nRF52840 DK`_ LED (actuator) or is controlled by a single `Nordic nRF52840 DK`_ button (sensor).
For communication between KNX IoT Point API, devices send s-mode messages over the CoAP protocol.
A recipient of the message checks its Group Object Table to verify that it is capable of processing it.

Tables below present configuration of Datapoints for Actuator and Sensor devices.

* Light Switch Actuator Datapoints
 
  +--------+-------+-----+-----+
  | LED    | Path  | GET | SET |
  +========+=======+=====+=====+
  | LED1   | /p/1  | Yes | Yes |
  +--------+-------+-----+-----+
  | LED2   | /p/2  | Yes | Yes |
  +--------+-------+-----+-----+
  | LED3   | /p/3  | Yes | Yes |
  +--------+-------+-----+-----+
  | LED4   | /p/4  | Yes | Yes |
  +--------+-------+-----+-----+

* Light Switch Sensor Datapoints
 
  +----------+-------+-----+-----+
  | Button   | Path  | GET | SET |
  +==========+=======+=====+=====+
  | BUTTON1  | /p/1  | Yes | No  |
  +----------+-------+-----+-----+
  | BUTTON2  | /p/2  | Yes | No  |
  +----------+-------+-----+-----+
  | BUTTON3  | /p/3  | Yes | No  |
  +----------+-------+-----+-----+
  | BUTTON4  | /p/4  | Yes | No  |
  +----------+-------+-----+-----+

LEDs and buttons location
*************************

All samples run on the `Nordic nRF52840 DK`_ board.
The picture below presents a location of LEDs (yellow rectangle) and buttons (green rectangle) used in examples.

   .. figure:: /images/dk_leds_and_buttons.png
      :alt: Location of DKs LEDs (yellow rectangle) and buttons (green rectangle)

Example 1 - one button mapped to four LEDs
******************************************

Having Thread network configured and operational, you can start KNX IoT device configuration and testing.
One device must be configured as the Actuator and one as the Sensor.

One button of KNX IoT Actuator `Nordic nRF52840 DK`_ device will control four LEDs built in to KNX IoT Sensor `Nordic nRF52840 DK`_ device.

#. Configure the Actuator device by adding entries to Actuator's Group Object Table with the ``knx got`` command.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1]
      uart:~$ knx got 2 /p/2 22 [1]
      uart:~$ knx got 3 /p/3 22 [1]
      uart:~$ knx got 4 /p/4 22 [1]

#. Configure the Sensor device by adding entries to Sensor's Group Object Table with the ``knx got`` command.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1]

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

Example 2 - one button mapped to one LED
****************************************

Having Thread network configured and operational, you can start KNX IoT device configuration and testing.
One device must be configured as an Actuator and one as a Sensor.

We need to perform configuration/mapping of buttons built in to KNX IoT Sensor `Nordic nRF52840 DK`_ to control LEDs built in to the KNX IoT Actuator `Nordic nRF52840 DK`_ - one button to one LED.
It is achieved by dedicated on KNX group to every input-output pair.

1. Configure the Actuator device by adding entries to Actuator's Group Object Table with the ``knx got`` command.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1]
      uart:~$ knx got 2 /p/2 22 [2]
      uart:~$ knx got 3 /p/3 22 [3]
      uart:~$ knx got 4 /p/4 22 [4]

#. Configure the Sensor device by adding entries to Sensor's Group Object Table with the ``knx got`` command.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1]
      uart:~$ knx got 2 /p/2 22 [2]
      uart:~$ knx got 3 /p/3 22 [3]
      uart:~$ knx got 4 /p/4 22 [4]

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

Example 3 - Mixed configuration
*******************************

Having Thread network configured and operational, you can start KNX IoT device configuration and testing.
One device must be configured as an Actuator and one as a Sensor.

We need to perform configuration/mapping of buttons built in to KNX IoT sensor to control LEDs built in to the KNX IoT Actuator - one button to one LED.
It is achieved by setting KNX Actuator `Nordic nRF52840 DK`_ to receive events from multple groups.

#. Configure Actuator device by adding entries to Actuator's Group Object Table - command ``knx got``.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1,2,3,4]
      uart:~$ knx got 2 /p/2 22 [2,3,4]
      uart:~$ knx got 3 /p/3 22 [3,4]
      uart:~$ knx got 4 /p/4 22 [4]

#. Configure Sensor device by adding entries to Sensor's Group Object Table - command ``knx got``.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1]
      uart:~$ knx got 2 /p/2 22 [2]
      uart:~$ knx got 3 /p/3 22 [3]
      uart:~$ knx got 4 /p/4 22 [4]
	  
#. Devices are configured. Use one of the sensor's buttons to control one or many of the assigned actuator's LEDs. 
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

Sniffing KNX IoT Point API communication
****************************************

In order to inspect the packets exchanged between KNX devices 802.15.4 sniffer can be used.
After setting up the Wireshark as described in :ref:`sniffer configuration <sniffer_configuration>` with regard to default configuration specified in :ref:`Starting Thread Network <starting_thread_network>` section we can capture CoAP messages sent through a default port (5683).

.. figure:: /images/example_capture.png

   Figure: Capture of CoAP messages

.. figure:: /images/example_coap_packet.png
   
   Figure: Content of CoAP message

.. _Nordic nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK
.. _KNX Specification: https://www.knx.org/wAssets/docs/downloads/Certification/Interworking-Datapoint-types/03_07_02-Datapoint-Types-v02.02.01-AS.pdf
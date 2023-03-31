..  _knxiot_testing:

KNX IoT testing
###############

With the sample applications - **Light Switch Actuator** and **Light Switch Sensor**, you will be apble to test simple KNX IoT system consisted of two Thread based devices.

Starting Thread network
***********************

Devices enable Thread interface during startup automatically - they are configured to create and join Thread network with parameters as below.

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

#. Check Active Data using :file:`ot dataset active` command.

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

#. Check devices roles using :file:`ot state` command.

  * one of device must be a Thread leader:

     .. code-block:: console
  
        uart:~$ ot state
        leader
        Done
    
  * another device either Thread child:
	
     .. code-block:: console

        uart:~$ ot state
        child
        Done
      
    or Thread leader:
	  
     .. code-block:: console

        uart:~$ ot state
        leader
        Done	

	  
Connecting Light Switch Sensor to Light Switch Actuator
*******************************************************

KNX IoT Point API devices just as the classic KNX devices consist of one or many Functional Blocks. The Functional Block contain one or more Datapoints which are its
inputs, outputs, and parameters. The most important feature of Datapoint's description is its type. The type specifies how the Datapoint is encoded,
its supported range, units, etc. All Datapoints of given Functional Block and featured types are described in KNX Specification.

Both Light Switch Actuator and Light  Switch Sensor are built with 4 Functional Blocks with a single Datapoint - "Switch On Off" (type name: DPT_Switch, id: 1.001). 
Every Datapoint controls a single `Nordic nRF52840 DK`_ kit LED (actuator) or is controlled by a single `Nordic nRF52840 DK`_ button (sensor). 
For communication between KNX IoT Point API devices send s-mode messages over CoAP protocol. 
Recipient of the message checks its Group Object Table in order to verify that it is capable of processing it. 

Tables below presents actuator and sensor devices Datapoints configuration.

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

All examples run on `Nordic nRF52840 DK`_ board. Picture below presents location of the LEDs (yellow rectangle) and buttons (green rectangle) used in examples.

   .. figure:: /images/dk_leds_and_buttons.png
      :alt: Location of DKs LEDs (yellow rectangle) and buttons (green rectangle)


Example 1 - One button mapped to four LEDs
******************************************

Having Thread network configured and operational you can start KNX IoT device configuration and testing.
One device must be configured as Actuator and one as Sensor.

One button of KNX IoT Actuator `Nordic nRF52840 DK`_ device will control four LEDs built in to KNX IoT Sensor `Nordic nRF52840 DK`_ device.

#. Configure Actuator device by adding entries to Actuator's Group Object Table - command :file:`knx got`.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1]
      uart:~$ knx got 2 /p/2 22 [1]
      uart:~$ knx got 3 /p/3 22 [1]
      uart:~$ knx got 4 /p/4 22 [1]

#. Configure Sensor device by adding entries to Sensor's Group Object Table - command :file:`knx got`.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1]

#. Devices are configured. Use sensor's BUTTON1 to disable or enable all of the assigned actuator's LEDs. Usage of the other buttons has no effect. 
   The interconnections between buttons and LEDs are presented in the following table.

   +----------------+------------------------+
   | Control button | Controlled LEDs        |
   | (Sensor)       | (Actuator)             |
   +================+========================+
   | BUTTON1        | LED1, LED2, LED3, LED4 |
   +----------------+------------------------+


Example 2 - One button mapped to one LED
****************************************

Having Thread network configured and operational you can start KNX IoT device configuration and testing.
One device must be configured as Actuator and one as Sensor.

We need to perform configuration/mapping of buttons built in to KNX IoT Sensor `Nordic nRF52840 DK`_ to control LEDs built in to the KNX IoT Actuator `Nordic nRF52840 DK`_ - one button to one LED.
It is achieved by dedicated on KNX group to every input-output pair.

#. Configure Actuator device by adding entries to Actuator's Group Object Table - command :file:`knx got`.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1]
      uart:~$ knx got 2 /p/2 22 [2]
      uart:~$ knx got 3 /p/3 22 [3]
      uart:~$ knx got 4 /p/4 22 [4]

#. Configure Sensor device by adding entries to Sensor's Group Object Table - command :file:`knx got`.


   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1]
      uart:~$ knx got 2 /p/2 22 [2]
      uart:~$ knx got 3 /p/3 22 [3]
      uart:~$ knx got 4 /p/4 22 [4]

#. Devices are configured. Use one of the sensor's buttons to control one of the assigned actuator's LEDs. 
   The interconnections between buttons and LEDs are presented in the following table.

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

Having Thread network configured and operational you can start KNX IoT device configuration and testing.
One device must be configured as Actuator and one as Sensor.

We need to perform configuration/mapping of buttons built in to KNX IoT sensor to control LEDs built in to the KNX IoT Actuator - one button to one LED.
It is achieved by setting KNX Actuator `Nordic nRF52840 DK`_ to receive events from multple groups.

#. Configure Actuator device by adding entries to Actuator's Group Object Table - command :file:`knx got`.

   .. code-block:: console

      uart:~$ knx got 1 /p/1 22 [1,2,3,4]
      uart:~$ knx got 2 /p/2 22 [2,3,4]
      uart:~$ knx got 3 /p/3 22 [3,4]
      uart:~$ knx got 4 /p/4 22 [4]

#. Configure Sensor device by adding entries to Sensor's Group Object Table - command :file:`knx got`.

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

.. _Nordic nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK
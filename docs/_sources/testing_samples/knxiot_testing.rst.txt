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
Functional Block contains one or more Datapoints which are its inputs, outputs, and parameters.

The most important feature of Datapoint's description is its type.
The type specifies how the Datapoint is encoded, its supported range, units, an so on.
All Datapoints of a given Functional Block and featured types are described in the `KNX Specification`_ page.

.. warning::
   Zmiana 4 functional bloków na 1, 3, albo 4

Both Light Switch Actuator and Sensor are built with 1, 3, or 4 Functional Blocks with a single Switch On/Off Datapoint
(type name: DPT_Switch, id: 1.001). Every Datapoint controls a single LED (Actuator) or is controlled
by a single button (Sensor). For communication between KNX IoT Point API, devices send s-mode messages
over the CoAP protocol. A recipient of the message checks its Group Object Table to verify that it is capable of
processing it.

The following tables present configuration of Datapoints for Light Switch Actuator and Light Switch Sensor devices:

.. warning::
   Dodanie komentarza "not supported by Thingy:53"

* Light Switch Actuator Datapoints:

  +--------+-------+-----+-----+--------------------------+
  | LED    | Path  | GET | SET | Comment                  |
  +========+=======+=====+=====+==========================+
  | LED1   | /p/1  | Yes | Yes |                          |
  +--------+-------+-----+-----+--------------------------+
  | LED2   | /p/2  | Yes | Yes |                          |
  +--------+-------+-----+-----+--------------------------+
  | LED3   | /p/3  | Yes | Yes |                          |
  +--------+-------+-----+-----+--------------------------+
  | LED4   | /p/4  | Yes | Yes | Unsupported by Thingy:53 |
  +--------+-------+-----+-----+--------------------------+

* Light Switch Sensor Datapoints:

  +----------+-------+-----+-----+--------------------------+
  | Button   | Path  | GET | SET | Comment                  |
  +==========+=======+=====+=====+==========================+
  | BUTTON1  | /p/1  | Yes | No  |                          |
  +----------+-------+-----+-----+--------------------------+
  | BUTTON2  | /p/2  | Yes | No  | Unsupported by Thingy:53 |
  +----------+-------+-----+-----+--------------------------+
  | BUTTON3  | /p/3  | Yes | No  | Unsupported by Thingy:53 |
  +----------+-------+-----+-----+--------------------------+
  | BUTTON4  | /p/4  | Yes | No  | Unsupported by Thingy:53 |
  +----------+-------+-----+-----+--------------------------+

Location of LEDs and buttons
****************************

.. warning::

   Usunięte info 'All samples run on the `nRF52840 DK`_ board.', zmiana liczby pojedynczej na mnogie
   Dodane obrazki z nRF5340 DK i Thingy 53

Pictures below present locations of LEDs (yellow rectangles) and buttons (green rectangles) used in samples.

   .. figure:: /images/dk_leds_and_buttons_nrf52840dk.png

      Image: Location of DKs LEDs (yellow rectangle) and buttons (green rectangle) of nRF52840 DK.
   .. figure:: /images/dk_leds_and_buttons_nrf5340dk.png

      Image: Location of DKs LEDs (yellow rectangle) and buttons (green rectangle) of nRF5340 DK.
   .. figure:: /images/dk_leds_and_buttons_thingy53.png

      Image: Location of DKs LEDs (yellow rectangle) and buttons (green rectangle) of Thingy:53.

Sniffing KNX IoT Point API communication
****************************************

In order to inspect the packets exchanged between KNX devices, the 802.15.4 sniffer can be used.
To capture CoAP messages sent through a default port (5683), set up the Wireshark as described in :ref:`sniffer configuration <sniffer_configuration>` with regard to default configuration specified in the :ref:`Starting Thread Network <starting_thread_network>` section.

.. figure:: /images/example_capture.png

   Figure: Capture of CoAP messages.

.. figure:: /images/example_coap_packet.png

   Figure: Content of CoAP message.

Sample cases
************

Configure Actuators and Sensors based on the following cases:

.. toctree::
   :maxdepth: 1
   :glob:
   :caption: Subpages:

   testing_cases/case_1.rst
   testing_cases/case_2.rst
   testing_cases/case_3.rst

.. _nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK
.. _KNX Specification: https://www.knx.org/wAssets/docs/downloads/Certification/Interworking-Datapoint-types/03_07_02-Datapoint-Types-v02.02.01-AS.pdf

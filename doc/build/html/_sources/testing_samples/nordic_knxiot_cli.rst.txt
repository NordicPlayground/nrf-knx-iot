..  _nordic_knxiot_cli:

KNX IoT Point API stack configuration
#####################################

Each KNX IoT device must be configured to make it working as a part of KNX network.

For demo purposes, Nordic Semiconductor provides the dedicated KNX IoT CLI to be used to configure KNX IoT device.
In the future, KNX IoT devices will be configured using the `ETS tool`_.

KNX IoT CLI
***********

+---------------------+-------------------------------------------+
| Command             | Description                               |
+=====================+===========================================+
| ``knx dev sn``      | Read device serial number                 |
+---------------------+-------------------------------------------+
| ``knx dev hwv``     | Read device hardware version              |
+---------------------+-------------------------------------------+
| ``knx dev hwt``     | Read device hardware type                 |
+---------------------+-------------------------------------------+
| ``knx dev model``   | Read device mode                          |
+---------------------+-------------------------------------------+
| ``knx dev sa``      | Read/write device subnet address          |
+---------------------+-------------------------------------------+
| ``knx dev da``      | Read/write device address                 |
+---------------------+-------------------------------------------+
| ``knx dev hname``   | Read/write device hostname                |
+---------------------+-------------------------------------------+
| ``knx dev fid``     | Read/write device fabric identifier       |
+---------------------+-------------------------------------------+
| ``knx dev iid``     | Read/write device installation identifier |
+---------------------+-------------------------------------------+
| ``knx dev ia``      | Read/write device individual address      |
+---------------------+-------------------------------------------+
| ``knx dev port``    | Read/write device port                    |
+---------------------+-------------------------------------------+
| ``knx got``         | Print/configure Group Object Table        |
+---------------------+-------------------------------------------+

Group Object Table configuration
********************************

Group Object Table is used to control interconnections (data sharing) between KNX Datapoint types.
Every entry in the table consists of the properties described below:

+---------------------+--------------------------------------------------------------------------------------+
| Property            | Description                                                                          |
+=====================+======================================================================================+
| Identifier          | Unique identifier of the entry, used for fetching and altering entry's content.      |
+---------------------+--------------------------------------------------------------------------------------+
| Path                | Datapoint's path, for example, `/p/rts/roomtemp`.                                    |
+---------------------+--------------------------------------------------------------------------------------+
| Group addresses     | Addresses of groups the resource is a part of.                                       |
+---------------------+--------------------------------------------------------------------------------------+
| Configuration flags | Bitmap used for controlling the communication. More information about flags `here`_. |
+---------------------+--------------------------------------------------------------------------------------+

Configuration flags
===================

For CLI purposes, `Configuration flags`_ are represented by assigned bits.

+------------------------------+----------+
| Configuration flag           | Bit no.  |
+==============================+==========+
| C: the Communication flag    | 2nd      |
+------------------------------+----------+
| R: the Read flag             | 3rd      |
+------------------------------+----------+
| T: the Transmit flag         | 4th      |
+------------------------------+----------+
| W: the Write flag            | 5th      |
+------------------------------+----------+
| U: the Update flag           | 6th      |
+------------------------------+----------+
| I: the Initialization flag   | 7th      |
+------------------------------+----------+

Printing Group Object Table entries
===================================

In order to read the Group Object Table content, use the ``knx got print`` command.

.. code-block:: console
  
   uart:~$ knx got print
   [ 0]: 124 /p/ls/room 252 [1]
   [ 1]: 256 /p/ls/kitchen 252 [2,3]
   [ 2]: 512 /p/ls/garage 252 [4,5,6]

The ``knx got print`` command output prints three entries in the Group Object Table.
Entries are described in details in the table below:
  
+---------+--------------+---------------+---------------------+-----------------------------------------------------------------------------+
| Entry   | Identifier   | Path          | Flags               | Group addresses                                                             |
+=========+==============+===============+=====================+=============================================================================+
| 0       | 124          | /p/ls/room    | 252 - all flags set | Property is a member of group with address equal 1.                         |
+---------+--------------+---------------+---------------------+-----------------------------------------------------------------------------+
| 1       | 256          | /p/ls/kitchen | 252 - all flags set | Property is a member of group with address equal 2 and 3 respectively.      |
+---------+--------------+---------------+---------------------+-----------------------------------------------------------------------------+
| 2       | 512          | /p/ls/garage  | 252 - all flags set | Property is a member of group with address equal 4, 5 and 6 respectively.   |
+---------+--------------+---------------+---------------------+-----------------------------------------------------------------------------+

  .. note::
  
     Identifiers are used to distinct entries within a device's table.
     They are not used for the communication purposes.

Group addresses meaning
=======================

When a property is a member group with a given address, it can send and receive messages within this group.
Multiple groups can be assigned to a single group.
It lets actuator react to events from multiple sources and sensors to publish events to multiple groups.
When a group is configured, a device joins the corresponding multicast group with an IPv6 address based on the group's address.

Sample Group Object Table configuration
=======================================

The following tables show sample sets of configuration for the Light Actuator and Sensor.

Light Actuator with a single input
""""""""""""""""""""""""""""""""""

The following entry sample allows the Light Actuator to receive notifications from a group with address equal to `1` for a property with the :file:`/p/light/room` path.

+---------+--------------+---------------------+--------+------------------+
| Entry   | Identifier   | Path                | Flags  | Group addresses  |
+=========+==============+=====================+========+==================+
| 0       | 1            | /p/light/room       | 0x7C   | 1                |
+---------+--------------+---------------------+--------+------------------+

Light Sensor with a single output
"""""""""""""""""""""""""""""""""

The following entry sample allows the Light Sensor to send notifications to a group with an address equal to `24` for a property with the :file:`/p/light/livingroom` path.

+---------+--------------+---------------------+--------+------------------+
| Entry   | Identifier   | Path                | Flags  | Group addresses  |
+=========+==============+=====================+========+==================+
| 0       | 2            | /p/light/livingroom | 0x7C   | 24               |
+---------+--------------+---------------------+--------+------------------+

Light Actuator with multiple inputs
"""""""""""""""""""""""""""""""""""

The following entry sample allows the Light Actuator to receive notifications from groups with addresses equal to `16` and `17` for a property with the :file:`/p/light/basement` path.

+---------+--------------+---------------------+--------+------------------+
| Entry   | Identifier   | Path                | Flags  | Group addresses  |
+=========+==============+=====================+========+==================+
| 0       | 4            | /p/light/basement   | 0x7C   | 16 and 17        |
+---------+--------------+---------------------+--------+------------------+

Light Sensor with multiple outputs
""""""""""""""""""""""""""""""""""

The following entry sample allows the Light Sensor to send notifications to groups with addresses equal to `32` and `64` for property with the :file:`/p/light/kitchen` path.

+---------+--------------+---------------------+--------+------------------+
| Entry   | Identifier   | Path                | Flags  | Group addresses  |
+=========+==============+=====================+========+==================+
| 0       | 8            | /p/light/kitchen    | 0x7C   | 32 and 64        |
+---------+--------------+---------------------+--------+------------------+

Altering Group Object Table entry
=================================

In order to add, change, or remove an entry, the following commands can be used:

+--------------------+-------------------------------------------------+
| Action             | Description                                     |
+====================+=================================================+
| Adding a new entry | ``knx got add <id> <path> [<ga>] <flags>``      |
+--------------------+-------------------------------------------------+
| Changing an entry  | ``knx got edit <id> <path> [<ga>] <flags>``     |
+--------------------+-------------------------------------------------+
| Removing an entry  | ``knx got remove <id>``                         |
+--------------------+-------------------------------------------------+

The command's parameters usage is described in the following table:

+-------------+-------------------------------------------+----------------------------------+
| Parameter   | Description                               | Data type                        |
+=============+===========================================+==================================+
| <id>        | Identifier                                | integer                          |
+-------------+-------------------------------------------+----------------------------------+
| <path>      | Entry paths                               | string                           |
+-------------+-------------------------------------------+----------------------------------+
| <ga>        | Comma separated list of Group Addresses   | comma-separated integer list     |
+-------------+-------------------------------------------+----------------------------------+
| <flags>     | Configuration flags described above       | integer                          |
+-------------+-------------------------------------------+----------------------------------+

Samples below:

.. code-block:: console
  
   uart:~$ knx got add 11 /p/light/gamingroom 252 [1]

.. code-block:: console
  
   uart:~$ knx got edit 11 /p/light/gamingroom 252 [1,2,3]

.. code-block:: console
  
   uart:~$ knx got remove 11

.. _ETS tool: https://www.knx.org/knx-en/for-professionals/software/ets-professional/
.. _here: https://support.knx.org/hc/en-us/articles/115003188089
.. _Configuration flags: https://support.knx.org/hc/en-us/articles/115003188089
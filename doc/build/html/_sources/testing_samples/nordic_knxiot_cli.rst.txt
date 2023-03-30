..  _nordic_knxiot_cli:

KNX IoT Point API stack configuration
#####################################


Each KNX IoT device must be configured to make it working as a part of KNX network.
In the future, KNX IoT devices will be configured using `ETS tool`_. 

For demo purposes Nordic Semiconductor, as a part of SDK, provides dedicated KNX IoT CLI to be used to configure KNX IoT device.

KNX IoT CLI 
***********

+---------------------+-------------------------------------------+
| Command             | Description                               |
+=====================+===========================================+
| knx dev sn          | Read device serial number                 |
+---------------------+-------------------------------------------+
| knx dev hwv         | Read device hardware version              |
+---------------------+-------------------------------------------+
| knx dev hwvt        | Read device hardware type                 |
+---------------------+-------------------------------------------+
| knx dev model       | Read device mode                          |
+---------------------+-------------------------------------------+
| knx dev sa          | Read/write device subnet address          |
+---------------------+-------------------------------------------+
| knx dev da          | Read/write device address                 |
+---------------------+-------------------------------------------+
| knx dev hname       | Read/write device hostname                |
+---------------------+-------------------------------------------+
| knx dev fid         | Read/write device fabric identifier       |
+---------------------+-------------------------------------------+
| knx dev iid         | Read/write device installation identifier |
+---------------------+-------------------------------------------+
| knx dev ia          | Read/write device individual address      |
+---------------------+-------------------------------------------+
| knx dev port        | Read/write device port                    |
+---------------------+-------------------------------------------+
| knx dev pm          | Read/write  device programming mode       |
+---------------------+-------------------------------------------+
| knx got             | Print/configure Group Objet Table         |
+---------------------+-------------------------------------------+
| knx grt             | Print/configure Group Recipient Table     |
+---------------------+-------------------------------------------+
| knx gpt             | Print/configure Group Publisher Table     |
+---------------------+-------------------------------------------+
| knx lsm             | Read/write Load State Machine state       |
+---------------------+-------------------------------------------+


Group Object Table configuration
********************************

Group Object Table is used to control interconnections (data sharing) between KNX Datapoints. Every entry in the table consists of the properties described below.

+---------------------+--------------------------------------------------------------------------------------+
| Property            | Description                                                                          |
+=====================+======================================================================================+
| Indentifier         | Unique identifier of the entry, used for fetching and altering entry's content.      |
+---------------------+--------------------------------------------------------------------------------------+
| Path                | Datapoint's path, e.g.: /p/rts/roomtemp .                                            |
+---------------------+--------------------------------------------------------------------------------------+
| Group addresses     | Addresses of groups the resource is part of                                          |
+---------------------+--------------------------------------------------------------------------------------+
| Configuration flags | Bitmap used for controlling the communication. More information about flags `here`_. |
+---------------------+--------------------------------------------------------------------------------------+

Configuration flags
===================

For CLI purposes `Configuration flags`_ are represented by assigned bits.

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

In order to read Group Object Table content, you can use :file:`knx got` command.

  .. code-block:: console
  
     uart:~$ knx got
     [ 0]: 124 /p/ls/room 252 [1]
     [ 1]: 256 /p/ls/kitchen 252 [2,3]
     [ 2]: 512 /p/ls/garage 252 [4,5,6]

:file:`knx got` command output showw three entries in Group Object Table. Entries are desribed in details in table below.
  
+---------+--------------+---------------+---------------------+-----------------------------------------------------------------------------+
| Entry   | Identifier   | Path          | Flags               | Group addresses                                                             |
+=========+==============+===============+=====================+=============================================================================+
| 0       | 124          | /p/ls/room    | 252 - all flags set | Property is a member of group with address equal 1                          |
+---------+--------------+---------------+---------------------+-----------------------------------------------------------------------------+
| 1       | 256          | /p/ls/kitchen | 252 - all flags set | Property is a member of group with address equal 2 and 3 respectively       |
+---------+--------------+---------------+---------------------+-----------------------------------------------------------------------------+
| 2       | 512          | /p/ls/garage  | 252 - all flags set | Property is a member of group with address equal 4, 5 and 6 respectively    |
+---------+--------------+---------------+---------------------+-----------------------------------------------------------------------------+

  .. note::
  
     Identifiers are used to distinct entries within a device's table and are not used for the communication purposes

Group adresses meaning
======================

When the property is a member group with a given address it can send and receive messages within this group. Multiple groups can be assigned to a single groups. It lets actuator react to events from multiple sources and sensors to publish events to multiple groups. When the group is configured the device joins corresponding multicast group with an IPv6 address based on the group's address.

Example Group Object Table Configuration
========================================

The following configuration is an example how to configure:

1. Light Actuator which is a recipient for events from a single group
2. Light Sensor that publish events to a single group
3. Light Actuator which is a recipient for events from multiple groups
4. Light Sensor that publish events to multiple groups

Light Actuator with a single input
""""""""""""""""""""""""""""""""""""

The following entry example allows the Light Actuator to receive notifications from group with address equal to 1 for property with path :file:`/p/light/room`.

+---------+--------------+---------------------+--------+------------------+
| Entry   | Identifier   | Path                | Flags  | Group addresses  |
+=========+==============+=====================+========+==================+
| 0       | 1            | /p/light/room       | 0x7C   | 1                |
+---------+--------------+---------------------+--------+------------------+

Light Sensor with a single output
""""""""""""""""""""""""""""""""""""

The following entry example allows the Light Sensor to send notifications to group with address equal to 24 for property with path :file:`/p/light/livingroom`. 

+---------+--------------+---------------------+--------+------------------+
| Entry   | Identifier   | Path                | Flags  | Group addresses  |
+=========+==============+=====================+========+==================+
| 0       | 2            | /p/light/livingroom | 0x7C   | 1                |
+---------+--------------+---------------------+--------+------------------+

Light Actuator with a multiple inputs
"""""""""""""""""""""""""""""""""""""

The following entry example allows the Light Actuator to receive notifications from groups with addresses equal to 16 an 17 for property with path :file:`/p/light/basement`. 

+---------+--------------+---------------------+--------+------------------+
| Entry   | Identifier   | Path                | Flags  | Group addresses  |
+=========+==============+=====================+========+==================+
| 0       | 4            | /p/light/basement   | 0x7C   | 16,17            |
+---------+--------------+---------------------+--------+------------------+

Light Sensor with a multiple outputs
""""""""""""""""""""""""""""""""""""

+---------+--------------+---------------------+--------+------------------+
| Entry   | Identifier   | Path                | Flags  | Group addresses  |
+=========+==============+=====================+========+==================+
| 0       | 8            | /p/light/kitchen    | 0x7C   | 32, 64           |
+---------+--------------+---------------------+--------+------------------+

The following entry example allows the Light Sensor to send notifications to groups with addresses equal to 32 an 64 for property with path :file:`/p/light/kitchen`.

Altering Group Object Table entry
=================================

In order to add new entry or edit the existing one :file:`knx got <id> <path> [<ga>] <flags>` can be used. 

When entry identifier matches any other in the table its content is updated. Otherwise, new entry is added to the table. The command's parameters usage is described in the following table.

+-------------+-------------------------------------------+----------------------------------+
| Parameter   | Description                               | Data type                        | 
+=============+===========================================+==================================+
| <id>        | Identifier                                | integer                          | 
+-------------+-------------------------------------------+----------------------------------+
| <path>      | Entry paths                               | string                           | 
+-------------+-------------------------------------------+----------------------------------+
| [<ga]       | Comma separated list of Group Addresses   | comma separated integer list     | 
+-------------+-------------------------------------------+----------------------------------+
| <flags>     | Configuration flags described above       | integer                          | 
+-------------+-------------------------------------------+----------------------------------+

Example below:

  .. code-block:: console
  
     uart:~$ knx got 11 /p/light/gamingroom 252

 .. _ETS tool: https://www.knx.org/knx-en/for-professionals/software/ets-professional/
 .. _here: https://support.knx.org/hc/en-us/articles/115003188089
 .. _Configuration flags: https://support.knx.org/hc/en-us/articles/115003188089
.. _environment_setup:

Requirements and setup
######################

This page outlines the requirements that you need to meet before you start working with the KNX IoT Point API environment.
Once completed, you will be able to run and test a sample KNX IoT application.

Hardware requirements
*********************

To meet the hardware requirements, ensure you have two Nordic's boards from the list of three supported models:

  * `nRF52840 DK`_
  * `nRF5340 DK`_
  * `Nordic Thingy\:53`_

To start working with the devices, refer to the following guidelines:

  * `Getting started with nRF52 Series`_
  * `Getting started with nRF53 Series`_
  * `Getting started with Thingy\:53`_

.. _sniffer_configuration:

Optionally, you can use `nRF52840 Dongle`_ as a Thread sniffer.
To start working with the `nRF52840 Dongle`_ as a sniffer, refer to the `nRF Sniffer for 802.15.4`_ page.

Software requirements
*********************

To meet the software requirements, install `nRF KNX IoT repository`_ and `nRF Command Line Tools`_.

nRF KNX IoT repository
======================

The KNX IoT Point API solution from Nordic Semiconductor is based on the `nRF Connect SDK v2.5.0`_.

Use the Nordic's `nRF52840 DK`_, `nRF5340 DK`_, or `Nordic Thingy\:53`_ to create a KNX IoT Point API prototype for :ref:`light_switch_actuator` and :ref:`light_switch_sensor` samples.

Follow these steps to download the KNX IoT application for the nRF Connect SDK:

1. Clone the nRF KNX IoT repository from the `nRF KNX IoT repository`_:

   .. code-block:: console

      $ git clone --branch <revision> https://github.com/NordicPlayground/nrf-knx-iot.git nrf-knx-iot

   For example:

   .. code-block:: console

      $ git clone --branch v0.3.0 https://github.com/NordicPlayground/nrf-knx-iot.git nrf-knx-iot

#. Update all repositories:

   .. code-block:: console

      $ west update

   Depending on your connection, the update might take some time.

nRF Command Line Tools
======================

Download the nRF Command Line from the `nRF Command Line Tools`_ page.

Wireshark
=========

Optionally, for tracing and sniffing purposes, install `Wireshark`_.
To configure Wireshark for Thread packets sniffing, go to the `Configuring Wireshark for Thread`_ page.

.. _nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK
.. _nRF5340 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF5340-DK
.. _Nordic Thingy\:53: https://www.nordicsemi.com/Products/Development-hardware/Nordic-Thingy-53
.. _nRF52840 Dongle: https://www.nordicsemi.com/Products/Development-hardware/nRF52840-Dongle
.. _nRF Command Line Tools: https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Command-Line-Tools/Download#infotabs
.. _Wireshark: https://www.wireshark.org/download.html
.. _Getting started with nRF52 Series: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/working_with_nrf/nrf52/gs.html
.. _Getting started with nRF53 Series: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/device_guides/working_with_nrf/nrf53/nrf5340_gs.html
.. _Getting started with Thingy\:53: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/working_with_nrf/nrf53/thingy53_gs.html
.. _nRF Sniffer for 802.15.4: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_sniffer_802154%2FUG%2Fsniffer_802154%2Finstalling_sniffer_802154.html
.. _nRF Connect SDK Getting started guide: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started.html
.. _Installing automatically: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/gs_assistant.html#installing-automatically
.. _Installing manually: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/gs_installing.html#install-the-required-tools
.. _Configuring Wireshark for Thread: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_sniffer_802154%2FUG%2Fsniffer_802154%2Fconfiguring_sniffer_802154.html
.. _nRF KNX IoT repository: https://github.com/NordicPlayground/nrf-knx-iot/
.. _nRF Connect SDK v2.5.0: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.5.0/nrf/index.html
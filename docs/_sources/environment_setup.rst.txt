.. _environment_setup:

Requirements and setup
######################

This page outlines the requirements that you need to meet before you start working with the KNX IoT Point API environment.
Once completed, you will be able to run and test a sample KNX IoT application.

Hardware requirements
*********************

To meet the hardware requirements, ensure you have two `nRF52840 DK`_ boards.
To start working with the devices, refer to the `Getting started with nRF52 Series`_ page.

.. _sniffer_configuration:

Optionally, you can use `nRF52840 Dongle`_ as a Thread sniffer.
To start working with the `nRF52840 Dongle`_ as a sniffer, refer to the `nRF Sniffer for 802.15.4`_ page.

Software requirements
*********************

To meet the software requirements, install `nRF Connect SDK v2.3.0`_ and `nRF Command Line Tools`_.

nRF Connect SDK
===============

Perform the following steps to install `nRF Connect SDK v2.3.0`_ and setup `nRF52840 DK`_ boards:

#. Set up your development environment by choosing one of the installation methods below:

   * Follow `Installing automatically`_ guildelines to perform an automatic installation through the Toolchain Manager.
   * Follow `Installing manually`_ guidelines to perform a manual installation.

   .. note::
      For additional information on setting up the device as well as Nordic’s development environment and tools, see the `nRF Connect SDK Getting started guide`_.

#. Clone the KNX IoT Point API repository.

   .. code-block:: console

      $ git clone https://github.com/NordicPlayground/nrf-knx-iot.git

#. Download dependencies.

   .. code-block:: console

      $ git submodule init
      $ git submodule update

nRF Command Line Tools
======================

Download the nRF Command Line from the `nRF Command Line Tools`_ page.

Wireshark
=========

Optionally, for tracing and sniffing purposes, install `Wireshark`_.
To configure Wireshark for Thread packets sniffing, go to the `Configuring Wireshark for Thread`_ page.

.. _nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK
.. _nRF52840 Dongle: https://www.nordicsemi.com/Products/Development-hardware/nRF52840-Dongle
.. _nRF Connect SDK v2.3.0: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/index.html
.. _nRF Command Line Tools: https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Command-Line-Tools/Download#infotabs
.. _Wireshark: https://www.wireshark.org/download.html
.. _Getting started with nRF52 Series: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/working_with_nrf/nrf52/gs.html
.. _nRF Sniffer for 802.15.4: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_sniffer_802154%2FUG%2Fsniffer_802154%2Finstalling_sniffer_802154.html
.. _nRF Connect SDK Getting started guide: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started.html
.. _Installing automatically: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/gs_assistant.html#installing-automatically
.. _Installing manually: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/gs_installing.html#install-the-required-tools
.. _Configuring Wireshark for Thread: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_sniffer_802154%2FUG%2Fsniffer_802154%2Fconfiguring_sniffer_802154.html

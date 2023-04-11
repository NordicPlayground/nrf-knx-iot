.. _setting_up_sdk:

SDK setup
#########

The KNX IoT Point API solution from Nordic Semiconductor is based on the `nRF Connect SDK v2.3.0`_.

Use the Nordic's `nRF52840 DK`_ to create a KNX IoT Point API prototype for Light Switch Actuator and Light Switch Sensor samples.

1. Set up your development environment by following one of the installation methods below:

   .. tabs::

      .. tab:: Automatic installation (Toolchain Manager)

         Follow the steps in `Installing automatically`_ to perform automatic installation using the Toolchain Manager.

      .. tab:: Manual installation

         Follow the steps in `Installing manually`_ to perform a manual installation.

   .. note::
      For additional information on setting up the device as well as Nordic’s development environment and tools, see the `nRF Connect SDK Getting started guide`_.

#. Clone the KNX IoT Point API private repository.

     .. code-block:: console

       $ git clone https://github.com/konradderda/nordic_knx.git

#. Download dependencies.

     .. code-block:: console

        $ git submodule init
        $ git submodule update

#. Download the nRF Command Line from the `nRF Command Line Tools`_ page.

#. Optionally, to configure Wireshark for Thread packets sniffing, go to the `Configuring Wireshark for Thread`_ page.

.. _nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK
.. _nRF Connect SDK Getting started guide: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started.html
.. _nRF Connect SDK v2.3.0: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/index.html
.. _nRF Command Line Tools: https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Command-Line-Tools/Download#infotabs
.. _Installing automatically: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/gs_assistant.html#installing-automatically
.. _Installing manually: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/gs_installing.html#install-the-required-tools
.. _Configuring Wireshark for Thread: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_sniffer_802154%2FUG%2Fsniffer_802154%2Fconfiguring_sniffer_802154.html
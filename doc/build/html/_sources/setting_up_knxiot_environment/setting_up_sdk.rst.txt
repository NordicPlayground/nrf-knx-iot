.. _setting_up_sdk:

Setting up SDK
##############



The KNX IoT Point API solution from Nordic Semiconductor is based on the `nRF Connect SDK v2.3.0`_. 

In order to create a KNX IoT Point API prototype, you need to use the following devices:

 * `Nordic nRF52840 DK`_ - for Light Switch Actuator and Light Switch Sensor samples.

You can set up your development environment by following one of the installation methods below:

.. tabs::

   .. tab:: Automatic installation (Toolchain Manager)

      Follow the steps in `Installing automatically`_ to perform automatic installation using the Toolchain Manager.

   .. tab:: Manual installation

      Follow the steps in `Installing manually`_ to perform a manual installation.

For additional information on setting up the device and Nordic’s development environment and tools, see the `nRF Connect SDK Getting started guide`_.



Downloading KNX IoT Point API repository
****************************************

1. Clone the KNX IoT Point API repository 

  .. note::
   currently it's a private repository, we need to change it to a public one later on):
  
  .. code-block:: console
     
	 $ git clone https://github.com/konradderda/nordic_knx.git

2. Download dependencies

  .. code-block:: console

     $ git submodule init
     $ git submodule update

Downloading nRF Command Line Tools
**********************************

To download the nRF Command Line, go to the `nRF Command Line Tools`_ page.

Configuring Wireshark for Thread
********************************

To configure Wireshark for Thread packets sniffing, go to the `Configuring Wireshark for Thread`_ page.

.. _Nordic nRF52840 DK: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK
.. _nRF Connect SDK Getting started guide: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started.html
.. _nRF Connect SDK v2.3.0: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/index.html
.. _nRF Command Line Tools: https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Command-Line-Tools/Download#infotabs
.. _Installing automatically: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/gs_assistant.html#installing-automatically
.. _Installing manually: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/gs_installing.html#install-the-required-tools
.. _Configuring Wireshark for Thread: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_sniffer_802154%2FUG%2Fsniffer_802154%2Fconfiguring_sniffer_802154.html
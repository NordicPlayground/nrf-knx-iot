.. _index:

Welcome to nRF Connect SDK – KNX IoT Point API
#################################################

KNX IoT extends the capabilities of the KNX standard with the power and flexibility of the Internet of Things (IoT). It allows KNX devices to be integrated with other IoT devices and cloud-based services, enabling the creation of advanced smart building systems.

With KNX IoT, devices can communicate over IP networks, allowing for greater flexibility in how devices are connected and controlled.

KNX IoT can be divided in 3 parts:

  * Standarized KNX IoT 3rd Party API that enables data interoperatbility by introducing an abstracion layer between KNX-specific knowledge nad 3rd party applications that could utilize data produced by KNX installations
  * Semantic layer that is added to the KNX Information model. KNX installations description created during system design with ETS is available via 3rd Party - supports devices and KNX systems interoperability
  * KNX IoT Point API that enabled IPv6 and Thread based connectivity to the KNX devices

This documentation provides instructions on how to correctly setup KNX Point API stack from the perspective of the Nordic SoCs and the `nRF Connect SDK`_. You can browse the content by referring to the following pages:

.. toctree::
   :maxdepth: 1
   :glob:
   :caption: Subpages:

   about_knxiot.rst
   setting_up_knxiot_environment/setting_up_knxiot_environment.rst
   testing_samples/samples.rst
   knxiot_release_notes.rst
   knxiot_known_issues.rst
   additional_resources.rst

.. _nRF Connect SDK: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/index.html

.. _index:

Welcome to nRF Connect SDK – KNX IoT Point API
##############################################

`KNX IoT`_ extends the capabilities of the `KNX`_ standard with the power and flexibility of the Internet of Things (IoT).
As a result, it allows KNX devices to be integrated with IoT devices and cloud-based services, enabling the creation of advanced smart building automation systems.
With the KNX IoT, devices can communicate over IP networks, allowing for greater flexibility in how devices are connected and controlled.

KNX IoT consists of 3 main elements responsible for data interoperability:

* KNX IoT 3rd Party API
   A standardized API using an abstraction layer between KNX-specific knowledge and 3rd party applications that could utilize data produced by KNX installations.
* Semantic Project Export
   A semantic layer that is added to the KNX Information Model (KIM).
   KNX installations description created during the system design in ETS is available through the KNX IoT 3rd Party API.
* KNX IoT Point API
   An API that enables connectivity between KNX devices through IPv6 and Thread.

This documentation provides instructions on how to correctly setup KNX Point API stack for the `nRF Connect SDK`_.

.. toctree::
   :maxdepth: 1
   :glob:
   :caption: Subpages:

   about_knxiot.rst
   environment_setup.rst
   testing_samples/samples.rst
   knxiot_release_notes.rst
   knxiot_known_issues.rst
   additional_resources.rst

.. _nRF Connect SDK: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.5.0/nrf/index.html
.. _KNX IoT: https://www.knx-iotech.org/
.. _KNX: https://www.knx.org/knx-en/for-professionals/index.php

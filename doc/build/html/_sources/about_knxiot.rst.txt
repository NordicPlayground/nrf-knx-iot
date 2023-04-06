.. _about_knxiot:

About KNX IoT Point API
#########################

KNX IoT Point API is described as a part of new specification (chapter 3.10.5) in the KNX system specification group.

The specification covers:

  * IPv6 based transport layer, eg. suitable for Thread-based networks
  * A new communication/message protocol using CoAP and CBOR
  * Usage of the same functional blocks and messages as the other KNX transport layers
  * Usage of the same s-mode message semantics as the other KNX transport layers
  * Usage of the same configuration data to configure which device is talking to which device

`KNX IoT Point API stack`_ is an open-source and reference implementation of KNX IoT standard.
This implementation has been integrated with nRF Connect SDK and can be used to build KNX-IoT device based on Thread networking including all the benefits it has to offer.

KNX Point API devices
*********************
Products that implement KNX IoT Point API stack (KNX IoT Point API devices) use an IPv6-based physical layer such as Thread for communication with the rest of the KNX installation. They can be easily used with exiting KNX products as a part of the single system (eg. combined with products that utilized Twisted Pair, KNXnet/IP as transportation layer).

KNX IoT Point API devices are using the same concept of interoperability as present in classic KNX  TP (Twisted Pair) and RF (Radio Frequency) based devices - the same semantic data are shared on the tranport layer as in all other KNX devices. But the lower layers of the protocol stack are not KNX-specific - Thread protocol is used instead.
That means middleware is required for sharing the data between KNX classic and KNX IoT parts of the system.

Middleware functionality could be implemented as separate device - KNX IoT Router, that will be responsible for transmitting data from KNX IoT Point API devices via Thread Border Router to classic part of the KNX system and from KNX classic devices to KNX IoT Point API devices.
This functionality could be also integrated with devices that is providing TBR (Thread Border Router) functionalities.

   .. figure:: /images/knx_iot_sys_example.png
      :alt: Example of KNX installation consisting of KNX TP and IoT devices.



To get more details on the KNX IoT and KNX IoT Point API, see the sources of the :ref:`Additional_resources` page.

.. _KNX IoT Point API stack: https://www.knx.org/knx-en/for-manufacturers/get-started/knx-iot-stack/
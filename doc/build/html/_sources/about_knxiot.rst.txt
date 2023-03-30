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

KNX IoT Point API specification is covered by open-source `KNX IoT Point API stack`_ reference implementation.  

This implementation has been integrated with nRF Connect SDK and can be used to build KNX-IoT device based on Thread networking including all the benefits it has to offer.

Products that implement KNX IoT Point API stack are connected via the Thread protocol with the rest of the network and KNX installation – they can be easily used with exiting KNX products as a part of the single system (eg. combined with products that utilized Twisted Pair, KNXnet/IP as transportation layer).

To get more details on the KNX IoT and KNX IoT Point API, see the sources of the :ref:`Additional_resources` page.

.. _KNX IoT Point API stack: https://www.knx.org/knx-en/for-manufacturers/get-started/knx-iot-stack/
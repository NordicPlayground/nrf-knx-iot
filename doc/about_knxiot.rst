.. _about_knxiot:

About KNX IoT Point API
#######################

The KNX IoT Point API is described as a part of a new specification (chapter 3/10/5) in the KNX system specification.

The specification covers:

  * The IPv6-based transport layer that is suitable, for example, for Thread-based networks.
  * A new communication/message protocol using the CoAP protocol and the CBOR format.
  * Usage of the following data already used by other KNX transport layers:
     
     * Functional blocks and messages.
     * S-mode message semantics.
     * Configuration data to configure which device is talking to which device.

`KNX IoT Point API stack`_ is an open-source and reference implementation of the KNX IoT standard.
This implementation has been integrated with the nRF Connect SDK, hence it can be used to build a KNX-IoT device based on Thread networking including all the benefits it has to offer.

KNX Point API devices
*********************
Products that implement KNX IoT Point API stack use an IPv6-based physical layer, such as Thread, for communication with the rest of the KNX installation.
KNX IoT Point API devices and other already existing KNX products (using other transmission methods like TP, PL, RF) can be easily interconnected within the same system.

KNX IoT Point API devices are based on the same interoperability concept as classic KNX TP (Twisted Pair), KNX PL (Power Line) and KNX RF (Radio Frequency) methods.
The same semantic data are shared through the transport layer.
Lower layers of the protocol stack are not KNX-specific - the Thread protocol is used instead.
It means that a middleware is required for sharing the data between KNX classic and KNX IoT parts of the system.

 .. note::
    The middleware functionality could be implemented as a separate device called KNX IoT router.
    It is connected to the same IP network as the KNX IP router.

    The KNX IoT router is responsible for transmitting data in both directions:
    
    * From KNX IoT Point API devices through the Thread Border router to the classic part of the KNX system through the KNX IP router.
    * From KNX classic devices through the KNX IP, KNX IoT and Thread Border routers to KNX IoT Point API devices.
 
    Moreover, if it is needed and there are enough hardware resources, this feature can be implemented as an additional service on Thread Border router device.

The diagram below presents a sample of the KNX installation that combines KNX classic and KNX IoT devices within a single system.

 .. figure:: /images/knx_iot_sys_sample.svg
    :alt: Sample KNX installation consisting of KNX classic and IoT devices.

To get more details on the KNX IoT and KNX IoT Point API, see the sources listed on the :ref:`Additional_resources` page.

.. _KNX IoT Point API stack: https://www.knx.org/knx-en/for-manufacturers/get-started/knx-iot-stack/
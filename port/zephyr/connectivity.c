/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stddef.h>
#include <oc_endpoint.h>
#include <oc_connectivity.h>
#include <oc_log.h>
#include <oc_buffer.h>

#include <zephyr/kernel.h>

#include <openthread/udp.h>
#include <openthread/message.h>
#include <openthread/instance.h>

#define RX_BUFFER_SIZE 1280

static struct otUdpSocket socket;
static struct oc_endpoint_t endpoint;
static struct k_mutex net_event_mutex;

static void handle_udp_received(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    oc_message_t *message = oc_allocate_message();

    message->endpoint.device = 0;
    message->endpoint.flags = IPV6;
    message->length = otMessageRead(aMessage, otMessageGetOffset(aMessage), message->data, sizeof(message->data));
    message->endpoint.addr.ipv6.port = aMessageInfo->mPeerPort;

    memcpy(message->endpoint.addr.ipv6.address, aMessageInfo->mPeerAddr.mFields.m8, 16);

    PRINT("Incoming message of size %zd bytes from ", message->length);
    PRINTipaddr(message->endpoint);
    PRINT("\n\n");

    oc_network_event(message);
}

int oc_connectivity_init(size_t device)
{
    otError           error;
    struct otInstance *instance = openthread_get_default_instance();
    otSockAddr        sockaddr;

    memset(&socket, 0, sizeof(otUdpSocket));
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.mPort = 5683; // CoAP port

    if (!otUdpIsOpen(instance, &socket))
    {
        error = otUdpOpen(instance, &socket, handle_udp_received, NULL);
        error = otUdpBind(instance, &socket, &sockaddr, OT_NETIF_THREAD);
    }
}

int
oc_send_buffer(oc_message_t *message)
{
    otError            error   = OT_ERROR_NONE;
    otMessage *        msg = NULL;
    otMessageInfo      msgInfo;
    otMessageSettings  msgSettings = {true, OT_MESSAGE_PRIORITY_NORMAL};
    struct otInstance *instance = openthread_get_default_instance();


    memcpy(msgInfo.mPeerAddr.mFields.m8, message->endpoint.addr.ipv6.address, 16);
    memset(msgInfo.mSockAddr.mFields.m8, 0, 16);

    msgInfo.mSockPort = socket.mSockName.mPort;
    msgInfo.mPeerPort = message->endpoint.addr.ipv6.port;

    msg = otUdpNewMessage(instance, &msgSettings);

    if (msg == NULL)
    {
        PRINT("oc_send_buffer: Failed to allocate OT buffers\n");
        return 0;
    }

    error = otMessageAppend(msg, message->data, message->length);

    if (error != 0)
    {
        PRINT("oc_send_buffer: Failed to append bytes\n");
    }

    error = otUdpSend(instance, &socket, msg, &msgInfo);

    PRINT("oc_send_buffer: Result %u\n", error);

    return 0;
}

void
oc_send_discovery_request(oc_message_t *message)
{
    PRINT("oc_send_discovery_request: Outgoing message of size %zd bytes to ",
        message->length);
    PRINTipaddr(message->endpoint);
    PRINT("\n");

    oc_send_buffer(message);
}

oc_endpoint_t *oc_connectivity_get_endpoints(size_t device)
{
    return NULL;
}

void
oc_connectivity_shutdown(size_t device)
{
    // Intentionally empty
}

void
oc_network_event_handler_mutex_init(void)
{
    k_mutex_init(&net_event_mutex);
}

void
oc_network_event_handler_mutex_lock(void)
{
    k_mutex_lock(&net_event_mutex, K_FOREVER);
}

void
oc_network_event_handler_mutex_unlock(void)
{
    k_mutex_unlock(&net_event_mutex);
}

void
oc_network_event_handler_mutex_destroy(void)
{
    // Intentionally empty
}

void
oc_connectivity_subscribe_mcast_ipv6(oc_endpoint_t *address)
{
    otIp6SubscribeMulticastAddress(openthread_get_default_instance(), 
                                   (const otIp6Address *) address->addr.ipv6.address);
}

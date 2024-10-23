/**
 * Hook functions
 *
 * Declares hook functions called by lwip.
 * Also declared API for configuring hook functions.
 *
 * The name of this file is specified as LWIP_HOOK_FILENAME in lwipopts.h.
 */

#ifndef LWIP_HOOKS_H
#define LWIP_HOOKS_H

#include "lwip/netif.h"

/**
 * LWIP_HOOK_UNKNOWN_ETH_PROTOCOL
 *
 * Called from ethernet_input() when an unknown eth type is encountered.
 *
 * By default, this will do nothing and return ERR_IF.
 * If a hook function has been set in lwip_set_hook_for_unknown_eth_protocol(),
 * then that function will be called.
 *
 * \param pbuf  Payload points to ethernet header!
 * \param netif Network interface.
 * \return ERR_OK if packet is accepted and freed,
 *         ERR_IF otherwise.
 */
err_t lwip_hook_unknown_eth_protocol(struct pbuf *pbuf, struct netif *netif);

/**
 * Configure function to be called by lwip_hook_unknown_eth_protocol()
 *
 *\param netif Network interface.
 *\param hook  Hook function to be called when frame with unknown eth type
 *             is encountered. Should return ERR_OK for accepted and freed
 *             frames, ERR_IF otherwise.
 */
void lwip_set_hook_for_unknown_eth_protocol(struct netif *netif, netif_input_fn hook);

#endif /* LWIP_HOOKS_H */

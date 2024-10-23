#include "lwip/lwip_hooks.h"

#ifdef LWIP_HOOK_UNKNOWN_ETH_PROTOCOL
static netif_input_fn lwip_hook_for_unknown_eth_protocol;

err_t lwip_hook_unknown_eth_protocol(struct pbuf *pbuf, struct netif *netif)
{
  if(lwip_hook_for_unknown_eth_protocol == NULL)
  {
    /* Not handled. User needs to free pbuf */
    return ERR_IF;
  }
  else
  {
    return lwip_hook_for_unknown_eth_protocol(pbuf, netif);
  }
}

void lwip_set_hook_for_unknown_eth_protocol(struct netif *netif, netif_input_fn hook)
{
  lwip_hook_for_unknown_eth_protocol = hook;
}
#endif /* LWIP_HOOK_UNKNOWN_ETH_PROTOCOL */

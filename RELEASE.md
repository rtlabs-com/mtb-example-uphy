# U-Phy Sample Application 

#### v0.6.1
* Fixes for network initialization problems
* Profinet SNMP support added

  **NOTE** : When building application the lwip stack will be automatically patched with <mtb_shared>/mtb-mw-uphy/<release_tag>/src/lwip/src.


##### Known issues:
* Only Eclipse IDE for ModusToolbox supported.

* Number of ports in the GSDML file is not correct
  GSDML declares two ports, but the EVK only supports one port.

* Limited EtherNet/IP support
  TCP/IP Interface, Ethernet Link and QoS objects are not yet implemented.

* The XMC72_EVK board is built with different serial flash memories
  The application flash memory configuration is a build time
  configuration and no detection or autoconfiguration is done
  in this sample.  
  Workaraound: Change the flash configuration using the Modus Toolbox
  QSPI Configurator tool and rebuild the application
  Identify mounted flash on your EVK using the text printed on the
  QSPI Flash circuit  
  Tested with S25FL512S and S25HL512T(Uniform).

* Previous version of ECM (ethernet connection manager) allowed setting mac address in cy_ecm_ethif_init(). The latest version of ECM (v2.1.0) have this configuration done in device configurator. No alternative option of setting mac address in runtime has been found.

* Crash during network initialization (82)  
  Workaround: Power cycle EVK

* Filesystem lost during stability test (81)  
  Workaround: Format filesystem using `format_fs` command and restart device.

* Error message when starting profinet first time (72)  
  Workaround: Reboot device


#### v0.6.0

* First release of U-Phy for Modus Toolbox supporting Profinet and EthernetIP.

##### Known issues:

* Profinet SNMP integration is not implemented. 
  Profinet is working but the device is behaving a Class A device without SNMP. 
  The attributes in the generated GSDML are not correct.

* Number of ports in the GSDML file is not correct
  GSDML declares two ports, but the EVK only supports on port.

* Limited EtherNet/IP support
  TCP/IP Interface, Ethernet Link and QoS objects are not yet implemented.

* The XMC72_EVK board is built with different serial flash memories
  The application flash memory configuration is a build time
  configuration and no detection or autoconfiguration is done
  in this sample.

  Change the flash configuration using the Modus Toolbox
  QSPI Configurator tool and rebuild the application
  Identify mounted flash on your EVK using the text printed on the
  QSPI Flash circuit

  Tested with S25FL512S and S25HL512T(Uniform).

* Only Eclipse IDE for ModusToolbox supported.

* Logging in uphy library is not aligned with cy_log.h log macros.

* Previous version of ECM (ethernet connection manager) allowed setting mac address in cy_ecm_ethif_init(). The latest version of ECM (v2.1.0) have this configuration done in device configurator. No alternative option of setting mac address in runtime has been found.

* Example project & middleware repos not fully aligned with name convention as outlined in 'AN235691  ModusToolbox™ & Friends'.

* **The versions of middleware libraries used and tested in code examples, as well as those utilized by the RT-Labs' middleware:**

  | Library                                 |       Version        |
  | --------------------------------------- | :------------------: |
  | **ethernet-core-freertos-lwip-mbedtls** |    release-v2.2.0    |
  | **ethernet-phy-driver**                 |    release-v1.1.0    |
  | **littlefs**                            |        v2.4.0        |
  | **retarget-io**                         |    release-v1.6.0    |
  | **serial-flash**                        |    release-v1.4.2    |
  | **abstraction-rtos**                    |    release-v1.8.2    |
  | **cat1cm0p**                            |    release-v1.7.0    |
  | **clib-support**                        |    release-v1.5.0    |
  | **cmsis**                               |    release-v5.8.1    |
  | **SEARCH_connectivity-utilities**       |    release-v4.5.0    |
  | **core-lib**                            |    release-v1.4.2    |
  | **core-make**                           |    release-v3.3.1    |
  | **ethernet-connection-manager**         |    release-v2.1.0    |
  | **freertos**                            |  release-v10.5.002   |
  | **SEARCH_lwip-freertos-integration**    |    release-v1.0.0    |
  | **lwip-network-interface-integration**  |    release-v1.4.1    |
  | **lwip**                                | STABLE-2_1_2_RELEASE |
  | **mbedtls**                             |    mbedtls-3.4.0     |
  | **mtb-hal-cat1**                        |    release-v2.6.1    |
  | **mtb-pdl-cat1**                        |   release-v3.12.0    |
  | **recipe-make-cat1c**                   |    release-v1.3.0    |
  | **secure-sockets**                      |    release-v3.7.0    |
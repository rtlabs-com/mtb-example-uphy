# U-Phy Sample Application 

#### v0.6.1
* Fixes for network initialization problems
* Profinet SNMP support added

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


# U-Phy Sample Application 

<img src="img/u-phy.png" alt="Alt text" width="100" style="float:right; margin-right:50px;margin-bottom: 50px">

This user example shows how to implement Industrial Ethernet connectivity using the
U-Phy Middleware. Currently Profinet and Ethernet/IP is supported.

The application implements a basic I/O device connecting inputs and outputs to to the EVK buttons and LEDs.
Profinet GSDML and EtherNet/IP EDS files for integration in an engineering tool are found in the `generated/` folder.

After building and programming the example, the EVK can be connected to a Profinet or Ethernet/IP network for evaluation.

> ### About U-Phy
>  U-Phy from RT-Labs provides a single and unified API for your favorite Industrial Ethernet protocols. 
>  
>  Checkout the README and documentation for the U-Phy Middleware for information 
>  on how to develop Industrial Ethernet devices using U-Phy. Other resources:
>  - [U-Phy Middleware](https://github.com/rtlabs-com/mtb-mw-uphy) - U-Phy Middleware repo on Github
>  - [U-Phy](https://rt-labs.com/u-phy/) - General introduction to U-Phy concept


## Features
- Industrial Ethernet
    - Profinet Device
    - EtherNet/IP Adapter
- Command Line Interface (CLI)
    - Configuration of active protocol
    - Get/Set I/O data
    - Watch status
- Device I/O data 
    - Input data mapped to EVK buttons
    - Output data mapped to EVK LEDs
- U-Phy Middleware (Advanced)
    - Device Design 
    - Redefine Device I/O data using U-Phy Device Builder
    - Regenerate description files (GSDML, EDS) and code using U-Phy Generator (upgen)

## Getting started

### Building and program application using Modus Toolbox
1. Start modus toolbox 3.2 (Eclipse IDE)
2. New application -> KIT_XMC72_EVK -> Getting Started -> Industrial Ethernet
3. Build project
4. Launch Industrial Ethernet onto XMC72_EVK using 'Industrial_Ethernet Program (KitProg3_MiniProg4)' launch target.
5. Verify that USER_LED3 flashes at 0,5Hz

### Mode LED
USER_LED3 indicated the current mode of operation and is referred to as the Mode LED.
When the application is started Mode LED flash at 0,5Hz.
When connected to a PLC (Profinet or EtherNet/IP) the Mode LED is steady ON.

The Profinet LED signalling feature is also mapped to the Mode LED. 
For Profinet users a natural first step of evaluation is to flash the LED using Proneta or your favorite engineering tool :)

### Command Line Interface
The Command Line Interface (CLI) is available on the EVK USB serial interface. It is used to configure the active protocol and also to watch and set the current status of the I/O data. It is also possible to store the active protocol to nonvolatile memory which is useful for demos and testing.

```
Default UART setting :
    - 115200 baud
    - 8 bits
    - No parity
    - 1 stop bit
```

A list of supported commands are displayed using the help command:

```
----------------------------------------------------------------------
 Industrial Ethernet Demo
 Configure communication protocol using this shell. Usage:
  'help'       - list of available commands
  'help <cmd>' - show command details
  'about'      - for information on this application

 Built Oct  8 2024 at 10:07:02
----------------------------------------------------------------------

Autostart disabled, start U-Phy using console command 'up_start'
> help
about                - about this application
up_alarm             - up_alarm <add/remove> <slot_ix> <level> <error_type>
up_autostart         - configure u-phy device autostart
format_fs            - format the filesystem
help                 - show help
mac                  - read/write MAC address
reboot               - reboot the device
up_device            - show static device configuration
up_signal            - get or set signal value and status
up_start             - start u-phy protocol
up_status            - show device status and signal values
netcfg               - configure network parameters

> about

Industrial Ethernet Demo

This user example shows how to implement Industrial Ethernet
connectivity using the U-Phy Middleware. Currently Profinet and
Ethernet/IP is supported.

The application implements a basic I/O device connecting inputs
and outputs to the EVK buttons and LEDs.

Profinet GSDML and EtherNet/IP EDS files for integration in an
engineering tool are found in the `generated/` folder.

Start communication using 'up_start' command.

```

### Device I/O Data
The default device supports the following I/O data modules:
- **I8** - 8 Bits Input (Sent to PLC)
    - Bit 0 is mapped to EVK user button 1
    - Bit 1 is mapped to EVK user button 2
- **O8** - 8 Bits Output (Received from PLC)
    - Bit 0 is mapped to EVK user LED 1
    - Bit 1 is mapped to EVK user LED 2
- **IO8** - 8 Bits Input/Output
    - No used  

In the U-Phy concept devices are defined by model in json format and device description files (Profinet GSDML, EtherNet/IP EDS) and device specific code is generated using the device generator tool. The default device model is found in ``model/model.json`` and the device specific files are found in ``generated/`` folder.

Note that CLI can be used to watch or set the IO-data.
  
### Find locally assigned IP address
The IP address of your device / EVk is found in the serial log. Note that the IP address is set by engineering tools and may change when switching the active protocol.

IP address may also be shown via shell command 'netcfg'

```
> netcfg

[en0] :
  mac address : 00:03:19:45:00:01
  ipaddress   : 192.168.2.25
  netmask     : 255.255.255.0
  gateway     : 192.168.2.1
  hostname    : not set
  dhcp        : enabled
```

### Configuring network

Out of the box, this sample app will configure DHCP for Ethernet/IP and static ip address when selecting Profinet.
Network configuration may be set in runtime using the 'netcfg' console command.

The default static IP is configured in mtb_shared/rtlabs-uphy-lib/latest-v1.X/src/network/network.h

```
#define APP_STATIC_IP_ADDR MAKE_IPV4_ADDRESS (192, 168, 0, 50)
#define APP_NETMASK        MAKE_IPV4_ADDRESS (255, 255, 255, 0)
#define APP_STATIC_GATEWAY MAKE_IPV4_ADDRESS (192, 168, 0, 1)
```

### Connect to PLC
Device description files are available in the ``generated\`` folder
If you are new to the protocol you want to evaluate information on how to get started is found in the the U-Phy Middleware documentation.

## Advanced

The U-Phy framework from RT-Labs includes a software library with Industrial Ethernet protocol and tools for designing the device data model. The device is defined by a model in json format. The model is designed a web service. Device specific artifacts are generated from model using the U-Phy device generator. This section is short description of how to do this for the Modus Toolbox context. Please refer to documentation on [U-Phy Documentation](https://docs.rt-labs.com/u-phy/) for reference.

### Redefining device I/O data
The device IO data is defined using the [U-Phy Device Builder](https://devicebuilder.rt-labs.com/).
Clicking the ``New`` button in the upper left corner will start a wizard that help you define an initial model. After completing the wizard, use the ``Save`` button and save the resulting json-file in the project ``model`` folder.

### Regenerating device specific artifacts
The U-Phy Device Generator (upgen) is part part of the U-Phy Middleware. The executables are located in the ``bin`` folder. Both linux and windows version are available. 

The script ``run-uphy-device-generator.sh`` in the project root folder is a helper utility for running upgen tool.
It can be run using the (modus) shell. Pass your device model as argument to the script to generate its artifacts.

```
  $ ./uphy-device-generator.sh model/digio.json
  Run U-Phy Generator
  +++ ../mtb_shared/rtlabs-uphy-lib/latest-v1.x/bin/upgen.exe export -d generated --generator Code model/digio.json
  +++ ../mtb_shared/rtlabs-uphy-lib/latest-v1.x/bin/upgen.exe export -d generated --generator Profinet model/digio.json
  +++ ../mtb_shared/rtlabs-uphy-lib/latest-v1.x/bin/upgen.exe export -d generated --generator EtherNetIP model/digio.json
```

Note the content in the generated folder is overwritten. The script itself contains some comments that may be useful.




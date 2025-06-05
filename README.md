
# U-Phy Industrial Ethernet Connectivity Example

This user example shows how to implement Industrial Ethernet connectivity using the U-Phy Middleware.
Currently, Profinet and EtherNet/IP are supported.

The application implements a basic I/O device connecting inputs and outputs to the EVK buttons and LEDs.
Profinet GSDML and EtherNet/IP EDS files for integration in an engineering tool are found in the `generated/` folder.

After building and programming the example, the EVK can be connected to a Profinet, EtherNet/IP or ModBus TCP network for evaluation.

Using U-Phy, the process data of the sample application can easily be redefined.
The inputs, outputs, and configuration parameters of the device are modeled using the U-Phy Device Builder.
Device description files such as Profinet GSDML and EtherNet/IP EDS are generated from the model.
See the advanced section below for a step-by-step guide.

## About U-Phy

U-Phy from RT-Labs provides a single and unified API for your favorite Industrial Ethernet protocols.
Check out the README and documentation for the U-Phy Middleware for information on how to develop
Industrial Ethernet devices using U-Phy. Other resources:

- [U-Phy](https://rt-labs.com/u-phy/) - General introduction to the U-Phy concept
- [U-Phy Middleware](https://github.com/rtlabs-com/mtb-mw-uphy) - U-Phy Modus Toolbox Middleware on GitHub

## Features
- Industrial Ethernet
    - Profinet Device
    - EtherNet/IP Adapter
    - Modbus TCP Adapter
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

**Note** : The runtime of U-Phy stack is limited to 2 hours. To obtain the full version, please contact your regional sales representative of Infineon Technologies AG.

## Getting started

### Building and program application using Modus Toolbox
1. Start modus toolbox 3.3 (Eclipse IDE)
2. New application -> KIT_XMC72_EVK -> Getting Started -> Industrial Ethernet
3. Build project
4. Launch Industrial Ethernet onto XMC72_EVK using 'Industrial_Ethernet Program (KitProg3_MiniProg4)' launch target.
5. Verify that USER_LED3 flashes at 0,5Hz

### Mode LED

USER_LED3 indicates the current mode of operation and is referred to as the Mode LED.

- When the application starts, the Mode LED flashes at 0.5 Hz.
- When connected to a PLC (Profinet or EtherNet/IP), the Mode LED is steady ON.

The Profinet LED signaling feature is also mapped to the Mode LED. For Profinet users, a natural first step of evaluation is to flash the LED using Proneta or your favorite engineering tool.

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
  - Not used

In the U-Phy concept, devices are defined by a model in JSON format. Device description files (Profinet GSDML, EtherNet/IP EDS) and device-specific code are generated using the device generator tool. The default device model is found in the `model/model.json` file, and the device-specific files are located in the `generated/` folder.

Note that the CLI can be used to watch or set the I/O data.
  
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

The default static IP is configured in mtb_shared/rtlabs-uphy-lib/latest-v0.X/src/network/network.h

```
#define APP_STATIC_IP_ADDR MAKE_IPV4_ADDRESS (192, 168, 0, 50)
#define APP_NETMASK        MAKE_IPV4_ADDRESS (255, 255, 255, 0)
#define APP_STATIC_GATEWAY MAKE_IPV4_ADDRESS (192, 168, 0, 1)
```

### Connect to PLC
Device description files are available in the ``generated\`` folder
If you are new to the protocol you want to evaluate information on how to get started is found in the the U-Phy Middleware documentation.

## Advanced

The U-Phy framework from RT-Labs includes a software library with Industrial Ethernet protocols and tools for designing the device data model.
The device is defined by a model in JSON format, which is designed using a web service. 
Device-specific artifacts are generated from the model using the U-Phy device generator. 
This section provides a brief description of how to do this in the Modus Toolbox context. 
Please refer to the [U-Phy Documentation](https://docs.rt-labs.com/u-phy/) for more details.

### Redefining Device I/O Data
The device I/O data is defined using the [U-Phy Device Builder](https://devicebuilder.rt-labs.com/). Clicking the `New` button in the upper left corner will start a wizard that helps you define an initial model. After completing the wizard, use the `Save` button to save the resulting json file in the project's `model` folder.

### Regenerating Device-Specific Artifacts
The U-Phy Device Generator (upgen) is part of the U-Phy Middleware. The executables are located in the `bin` folder, with versions available for both Linux and Windows.

The script `run-uphy-device-generator.sh` in the project root folder is a helper utility for running the upgen tool. It can be run using the Modus shell. Pass your device model as an argument to the script to generate its artifacts.


```
  $ ./uphy-device-generator.sh model/digio.json
  Run U-Phy Generator
  +++ ../mtb_shared/rtlabs-uphy-lib/latest-v1.x/bin/upgen.exe export -d generated --generator Code model/digio.json
  +++ ../mtb_shared/rtlabs-uphy-lib/latest-v1.x/bin/upgen.exe export -d generated --generator Profinet model/digio.json
  +++ ../mtb_shared/rtlabs-uphy-lib/latest-v1.x/bin/upgen.exe export -d generated --generator EtherNetIP model/digio.json
```

Note the content in the generated folder is overwritten. The script itself contains some comments that may be useful.

## Requirements

- [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) v3.2 or later (tested with v3.4)
- Board support package (BSP) minimum required version for :
   - KIT_XMC72_EVK: v1.0.0
- Programming language: C

## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; Embedded Compiler v11.3.1 (`GCC_ARM`) - Default value of `TOOLCHAIN`

## Supported kits (make variable 'TARGET')

- [XMC7200 Evaluation Kit](https://www.infineon.com/KIT_XMC72_EVK) (`KIT_XMC72_EVK`)

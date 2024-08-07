# VSCODE Devcontainer for Arduino

This is a self-contained environment that allows compiling and uploading
Arduino source code without having to install anything on your computer, beside VSCode and Docker. It uses the VSCode's devcontainer built-in feature.

## Licensing
The included Docker file, the devcontainer.json file and the hello-world sample are licensed under the MIT License. All other files are governed by their respective licenses.

## How it works
This devcontainer is based on Microsoft's CPP devcontainer using Debian and contains the required extensions to facilitate Arduino development.  
The devcontainer is configured to launch the container with `--privileged` options. This is required to access the serial ports from inside the Docker container. If your feel unsafe about that, you can remove this option and add an UDEV rule to configure the file permissions on the Arduino device automatically.

## Environment
This devcontainer has been tested with the following environment:
- Ubuntu 22.04 running on WSL 2.2.4.0 (on Win11 23H2)
- Docker 27.1.1
- VSCode 1.92.0

## How to use the devcontainer
1) Open the project in VSCode: `File->Open Folder`
2) Reopen it in devcontainer:
   1)  `CTRL+SHIFT+P` to open the command pallet
   2)  `Remote-Containers: Reopen in container`

## Inside the devcontainer
Most of the option below are also available from the status bar
### Selecting the Board
From the command pallet, type `Arduino: Board config`
### Opening the serial monitor
From the command pallet, type `Arduino: Open serial monitor`
### Opening the library manager
From the command pallet, type `Arduino: Library manager`
### Working with the code
`ALT-CTRL-R`: Compile only  
`ALT-CTRL-U`: Compile and upload to the Arduino device

For more information about the Arduino extension for VSCode, consult [this page](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino).

## External librairies
If you need to install external librairies, use the `arduino-cli lib install` command. See the end of the Docker files for more information.

## Troubleshooting

### Pre-requisites on Windows Subsystem for Linux (WSL2)
Check out the [Microsoft Documentation](https://learn.microsoft.com/en-us/windows/wsl/connect-usb) on how to connect USB devices and perform the installation steps for *USBIPD-WIN*.

After every restart of WSL perform the following steps (little bit anoying...):
1. Open your WSL VM with `wsl` in a *PowerShell* window and keep the WSL running.
2. List all USB devices connected to Windows by opening a new *PowerShell* window in *administrator* mode.
   ```bash
   usbipd list
   ```
   Make sure the USB device is shared. Otherwise use (admin priviledges needed): 
   ```bash
   usbipd bind --busid <busid_e.g:_4-4>
   ```
2. Attach the USB device to WSL (no admin priviledges needed)
   ```bash
   usbipd attach --wsl --busid <busid_e.g:_4-4>
   ```
3. Perform the following steps in your running WSL shell. List the attached USB devices by running:
   ```bash
   lsusb
   ```
   You shoud see the device you just attached and be able to interact with it using normal Linux tools.

### Find the device path on your Linux machine and make it accessible from within your devcontainer
In Linux serial ports are */dev/tty** (where "*" can be a number e.g. 0-3 or something like *USB0*) not *COMn* which is the notation used in Windows.

1. Check where your device is attached to:
   ```bash
   dmseg | grep tty
   ```
   ... which outputs something like: *usb 1-1: ch3411-uart convertor now attached to ttyUSB0*. This means your device path is */dev/ttyUSB0*

2. Create a mount point in the *devcontainer.json* file to have access to this device within your devcontainer.
   ```JSON
   "mounts": [
		"source=/dev/ttyUSB0,target=/dev/ttyUSB0,type=bind"
   ],
   ```
   For this option the devcontainer needs to run in *priviledged* mode. Add the following to the configuration:
   ```JSON
   "runArgs": [
		"--privileged",
		"--cap-add=SYS_PTRACE",
		"--security-opt",
		"seccomp=unconfined"
	],
   ```
   For further information consult the [Docker documentation](https://docs.docker.com/reference/cli/docker/container/run/) for options (*runArgs*) of the `docker container run` command.
   > Note: This may be not the best option in regards of security. I use it in my home lab environment for developing. BUT there are other possibilities (cgroup method). Checkout [this discussion on stackoverflow](https://stackoverflow.com/questions/24225647/docker-a-way-to-give-access-to-a-host-usb-or-serial-device#comment55565965_30205490).

3. For your Arduino VS Code extension you can now configure the serial port (e.g. */dev/ttyUSB0*) in the `.vscode/arduino.json` file as follows:
   ```JSON
   {
      "port": "/dev/ttyUSB0",
   }
   ```

### Add additional Boards Manager URLs to use e.g. an ESP8266
1. In the *devcontainer.json* file configure the additional URL in *settings* (here for the ESP8266):
   ```JSON
   "settings": { 
		"arduino.additionalUrls": [
			"http://arduino.esp8266.com/stable/package_esp8266com_index.json"
		]
	},
   ```
2. In VS Code use the `>Arduino: Board Manager` command, search for e.g. `ESP8266` and install the packages 
3. Use the `>Arduino: Board Config` command to configure the board
4. Optional: Another option is to configure the settings in the *.vscode/ardunio.json* file as follows:
   ```JSON
   {
    "board": "esp8266:esp8266:nodemcuv2",
    "configuration": "xtal=80,vt=flash,exception=disabled,stacksmash=disabled,ssl=all,mmu=3232,non32xfer=fast,eesz=4M2M,led=2,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=115200"
   }
   ```

### Further ressources
- Checkout [this article](https://blog.golioth.io/program-mcu-from-wsl2-with-usb-support/) for more information on how to set up the WSL
- *Udev* is the Linux subsystem that supplies your computer with device events... get amiliar with it. Check out [this article](https://opensource.com/article/18/11/udev).
  - Use `sudo udevadm monitor -u` to monitor *udev* events
- Checkout [this article](https://circuitjournal.com/esp8266-with-arduino-ide) for the general steps in the Arduino IDE to configure a ESP-12E / ESP-12F / NodeMCU.
  - Configure the Board Manager URL
  - Flash boards
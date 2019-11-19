# lunchlauncher

## Features

In my old office environment, people that went to lunch together were scattered around different rooms and floors. I built this button, which sends emails to a list of people after it is pressed and tells them that lunch is launched.

It consists of four parts:

 1. The button. This is an emergency stop button, which is filled with a microcontroller, a LED for visual feedback, and a serial-to-usb adapter to access it via usb from a computer.
 2. A linux daemon. There is a C, a bash script, and a systemd version. It manages communication with the button via a serial terminal. If a pressed button is detected, the php script below is called.
 3. A php script. This script can be stored on the web server. It manages a mailing list and allows sending mails to the list (do not expose this script to the internet! It is meant to run on a local LAN only!). If the daemon notices a pushed button, mails can be sent via this script.
 2. An optional bash script. This is a linux bash script, which should be executed via cron job. It checks the above php script for new lunch launches and can display more alarms (besides emails).

## Installation

### 1. The button

Schematics are in the `button/eagle` folder:

![Schematics of button](/button/eagle/schematic.png "Schematics")

The heart is an Atmega Tiny13-20PU, which consumes little power and still offers Soft-UART communication. I added a cheap serial-to-usb adapter to the button, such that the button has only an usb cable (power for the microcontroller is also drawn from the 5V usb power line). Furthermore, I drilled a hole into the front of the button and inserted an LED, which gives visual feedback to the user.

Source code can be found in the folder `button/source/atmega`.

### 2. Linux daemon

The daemon can be found in the folder:

 - The C version: `button/source/daemon`,
 - The bash version: `button/source/daemonbash`,
 - The systemd version: `button/source/systemd-service`.

 I recommend using the bash version. It connects to the php script and ran stable for a couple of years on my productive machine. If your serial-to-usb adapter does not show as `/dev/ttyUSB0`, you will need to adapt it in the code. Furthermore, you will need to update server addresses.

### 3. php script

This script manages the mailing list. All files can be found in `www`.

Any user with html access to this page can send mails to users on the list (be careful!). I used ssmtp for mail sending, but any sendmail service will do. You will need to update the `$serverAddress` variable in the script.

A list of all lunch launches is stored in the file `launchlist.dat`, the mailing list is stored locally in the file `maillist.dat`.

Warning: This script is meant for small LAN environments, where no privacy concerns exist (all mail addresses and names are known to everyone, no mail abuse may happen, ...).

### 4. bash script

This script is optional. It can be found in `script`.

It checks the above mentioned php script for new launches and shows additional alarms (e.g. sound, new browser tab, notification, ...). Installation notices are below the header of the script.

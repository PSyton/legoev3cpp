legoev3cpp
==========

This is a light-weight cross-platform C++11 API for remote control of the LEGO EV3. The current platform specific  implementation files are only iOS bluetooth. I use this library to remote-control my EV3 from an iOS app. If you find this project useful, please propose any additions/changes.

The following are on the TODO list...
* Local variables in opcodes and variable dereferencing in opcodes.
* Implement all opcode structures.
* System commands.

This project is dependent on the ev3 sources with the minor header changes made in the dsjove fork (pull request has been made). I have a fork of both ev3 source trees with the changes.

The "Jove's Landing" sample app demonstrates usage. "AppDelegate.mm" shows how to create the link to the EV3. "RailSwitch.mm" shows how to send a command to EV3.

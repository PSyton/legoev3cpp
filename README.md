legoev3cpp
==========

This is a light-weight cross-platform C++14 API for remote control of the LEGO EV3. The current platform specific  implementation files are only iOS bluetooth. I use this library to remote-control my EV3 from an iOS app. If you find this project useful, please propose any additions/changes.

The following are on the TODO list...
1) Variable dereferencing in opcodes.
	- There is a sizeof() problem with DirecOpcode
	- Optionally pushing consexpr string size limits to params
2) Finish implementing all direct opcode structures.
3) Finish implementing all system opcode structures.
	- There is a crash/deadlock with FileUploader
4) Fetching extended brick info timeouts
5) Finish WIFI connections
6) Create more platform specific connection classes

This project is dependent on the ev3 sources with the minor header changes made in the dsjove fork (pull request has been made). I have a fork of both ev3 source trees with the changes. You do NOT need to change the firmware on the EV3. The header files are only for desktop compilation.

The "Jove's Landing" sample app demonstrates usage. 
- “AppDelegate.mm" shows how to create the link to the EV3. 
- “RailSwitch.mm" shows how to send a command to EV3.
- “DirectoryListingViewController.mm” and “FileViewController.mm” shows how to interact with files

legoev3cpp
==========

This is a light-weight cross-platform C++11 API for remote control of the LEGO EV3. The current platform specific  implementation files are only iOS bluetooth. I use this library to remote-control my EV3 from an iOS app. If you find this project useful, please propose any additions/changes.

The following are on the TODO list...
* Variable size opcodes (string parameters). This will require an std::string similar strategy for allocation.
* Local variables in opcodes. ValueStore and DirectInstructions will need some refactoring.
* Implement all opcode structures. Still looking for decent documentation!
* System commands. Once Direct command is entirely impemented look at code re-use.

This project is dependent on the ev3 sources with the minor header changes made in the dsjove fork (pull request has been made).



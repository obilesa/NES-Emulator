NES-Emulator - WIP
===========

Description
-------------
An NES emulator powered by C++, Emscripten and SDL2. Supports all ROMs using the iNES 001 mapper.

Installation
--------------
1. Download the latest release from the Releases tab on GitHub.
2. Unzip the downloaded file
3. Open the unzipped folder
4. Launch a live server from the open folder, you can do this for example with python:

       py -m http.server <port>
   
6. Navigate to `localhost:<port>`

#### Known issues
* If the game does not refresh after changing the game file, try deleting the site data inside the browser.

Usage
------
To change games replace the included .nes file with the new game file.

Future plans
----------------------
* Complete the intgration of the Emulator into the web fronted
* Support more mappers
* Support second controller

Attribution
---------------
[NES Dev Wiki](https://www.nesdev.org/)

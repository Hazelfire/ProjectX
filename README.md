# ProjectX
## Installation
Please download the installer from <hazelfire.ddns.net/ProjectXInstall.exe> and run it. If you wish to build ProjectX from source, or help develop the game. Read on

Compiling from source in not reccomended for begginners in programming.

First clone this repository using git, then run `git submodule update --init --recursive` to get all the external sources required to run the game.

go into the cocos2d-x directory and run download-deps.py. After finishing downloading, look in the ProjectFiles directory for your platform and extract it's contents to the main directory.

Install boost, if you are on linux, installing boost with your package manager should be sufficient. If you are on windows, it's a bit more involved. Download boost from <https://sourceforge.net/projects/boost/files/boost/1.61.0/> and extract it somewhere you can remember. I have it extracted to Program Files.

### Windows
If you are on windows. You will have a ProjectX.sln file in the proj.win32 directory. Open this with Visual Studio. If everything loads correctly, right click the ProjectX Project on the right and click properties -> C++ -> Additional include directories. Change these to something meaningful. By default they are set to my absolute directories so change them around so that they point to websockets, lua, yaml-cpp and boost. Then you should be able to compile ProjectX

### Linux
Inside proj.linux, run `cmake ..` then `make` and you should have the exceutable in the proj.linux/bin directory 

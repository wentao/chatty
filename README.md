Chatty: A Simple TCP-Socket Chat Server Written In C++/QT
=========================================================

Server: Build & Run
-------------------

The server is implemented in C++ with
[QT](http://qt-project.org). Although I only tested the compilation
under Mac OS X and Linux, it should compile under all desktop
environments supported by QT.

In order to build the server binary, you have to download and config
the QT SDK for your target OS. Then enter the project folder, execute
the following commands:

`qmake`

`make`

You would expect a sucessful building, except a few compiler
warnings. Now run the binary with:

`./chatty`

The server will start and listen on port 8237.

Client: How-To
--------------

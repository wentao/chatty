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
the following commands

`qmake`

`make`

You would expect a sucessful build, except a few compiler
warnings. Now run the binary with

`./chatty`

The server will start and listen on port 8237.

Client: How-To
--------------

It's encouraged to use `telnet` to communicate with the server. You
can initiate the connection by running

`telnet <ip> 8237`

If the connection succeed, you will see a welcome message sent back
from the server, something like

`> Welcome to chatty, please tell us your name:`

Please note that all server messages sent to you are starting with the
prefix `> `. Now the server is asking for your login name (must be
unique among all logged in users). Let's say we enter the name

`wensa`

`> Welcome wensa!`

Now you are connected to the server. However, you aren't able to talk
to anyone until you enter a chat room. You can either use the command
`/rooms` to list existing chat rooms, or the command `/create` to
create a new chat room. There is a limit on the maximum number of chat
rooms, defaulting to 20.

`/create playchatty`

`> Room playchatty created.`

`/rooms`

`>  * default`

`>  * playchatty`

To join a chat room, you can use

`/join playchatty`

`> Joining room: playchatty`

`>  * wensa (** this is you)`

It looks like we are the only one in the new chat room. Imagine we
invite a friend `asnew` to join this room, following the same
procedure. Once he joins the chat room, we will see something like

`> asnew has joined.`

`/who`

`>  * asnew`

`>  * wensa (** this is you)`

Now we have two people in this room. We can start chatting! When you
are in a chat room, any text you entered will be broadcasted to
everyone in the same chat room. If you enter

`hi there`

`> wensa: hi there`

Then if `asnew` replied with `hey`, you will get a message like

`> asnew: hey`

If you want to send a message to someone privately, you can use

`/whisper asnew where are you?`

`> To asnew: where are you?`

What `asnew` will see on his end is something like

`> From wensa: where are you?`

If you are done with the chat, you can leave the chat room by using

`/leave`

`> You left the room: playchatty`

Now if you use the command

`/quit`

`Connection closed by foreign host.`

You will be disconnected from the server.

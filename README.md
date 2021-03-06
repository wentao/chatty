Chatty: A Simple TCP-Socket Chat Server Written In C++/QT
=========================================================

## Server: Build & Run

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

## Client: How-To

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
to anyone until you enter a chat room. You can use the command
`/rooms` to list existing chat rooms

`/rooms`

`>  * default`

Then use `/join` to join a chat room

`/join default`

`> Joining room: default`

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

If you are done with the chat, you can leave the chat room by using

`/leave`

`> You left the room: default`

Now if you use the command

`/quit`

`Connection closed by foreign host.`

You will be disconnected from the server.

## Extras & Hints

### Private messages

If you want to send a message to someone in a chat room privately, you
can use

`/whisper asnew where are you?`

`> To asnew: where are you?`

What `asnew` will see on his end is something like

`> From wensa: where are you?`

### Optional command arguments

To run commands followed by arguments, such as `/join`, `/whisper`,
you don't have to complete all arguments in a single shot. Instead,
you can just type the command name itself, then the server will walk
you through a wizard dialog to allow you enter individual arguments
separately. For example

`/whisper`

`> Please enter the name of the person to whisper, or type /abort to cancel the action`

`wensa`

`> Please enter the message to send, or type /abort to cancel the action`

`hey there`

`> To wensa: hey there`

### Creating chat rooms

By default, there is only one chat room `default`. You can create more
chat rooms by using

`/create <roomname> [pin]`

You can skip the optional pin argument if you want your chat room to
be public (anyone can join). Otherwise

`/create private pincode`

`> Room private created.`

`/rooms`

`>  * default`

`>  * private (** pin required)`

You will see that a `private` room was created. When a user wants to
join the room protected by a pin, he is required to enter the pin.

`/join private`

`> Please enter the pin for the room, or enter /abort the cancel.`

`guess`

`> Incorrect pin!`

`> Please enter the pin for the room, or enter /abort the cancel.`

`pincode`

`> Joining room: private`

`>  * wensa (** this is you)`

Please note the server only supports 20 chat rooms at maximum.

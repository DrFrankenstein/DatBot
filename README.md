# Hello!

You are looking at DatBot, yet another IRC bot for doing things that IRC bots do.

Right now it's at a pretty embryonic stage, but hey, you can always checkout the source and build it (standard cmake pipeline) if
you're so inclined.

# Features
Will it be better than all those other IRC bots out there? Eh, probably not. ¯\\\_(ツ)_/¯ But two of its goals are:
* Some Twitch integrations to make some overlays light up and stuff
* Eventually have a mature-enough IRC library that can stand on its own, and build a client and other goodies from it

Aside from that, this is just a spare time project to get my mind off of JavaScript and into C++ for a change.

# Design
The central point of DatBot's architecture is _reactive programming_, and it uses RxCpp as a framework for that.
IRC messages and events are passed between components as observable streams of processed data that can be subscribed to.

Networking is handled portably through the Asio library.

Some other components from Boost are used for various purposes; for example, some components from Spirit.Qi are used as helpers for
parsing (however, we're not currently using the whole parsing framework it provides).

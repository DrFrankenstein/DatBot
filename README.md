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

# Requirements
You'll need
* A C++17 compiler
  * Visual C++ 19.11 or better (that's what the project is developed on)
  * GCC 7 or better
  * clang 4 or better
* CMake 3.8 or better
* Boost 1.69 or better
* RxCpp 4.0 or better
* yaml-cpp 0.6.0 or better

You can use whatever dev environment that supports CMake (or the other way around, whatever semantics you prefer). Popular ones are
Visual Studio, VS Code, CLion, Qt Creator, Eclipse, X-Code... or just good ol' vim amd make, in which case `cmake && make` should
get stuff going.

There's currently no CI set up, so my apologies if things get broken on GCC and clang. Do some investigation, and file an issue,
or even better, a PR with a fix. Hopefully it's just the aforementioned version numbers not being accurate anymore. Later on, I'll
get CI going. When I get around to it™.

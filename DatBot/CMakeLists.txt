cmake_minimum_required (VERSION 3.8)

add_executable (DatBot 
    "DatBot.cpp" 
    "Net/AsioDevice.cpp" 
    "Irc/Session.cpp"
    "Irc/Parsers/MessageParser.cpp"
)

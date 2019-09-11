cmake_minimum_required (VERSION 3.8)

add_executable (DatBot 
    "DatBot.cpp" 
    "Net/AsioDevice.cpp" 
    "Irc/Session.cpp"
    "Irc/Models/Message.cpp"
    "Irc/Parsers/MessageParser.cpp"
)

target_link_libraries(DatBot PRIVATE yaml-cpp)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/datbot.conf"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

cmake_minimum_required (VERSION 3.8)

add_executable (DatBot 
    "DatBot.cpp" 
	"Bot/IrcBot.cpp"
    "Net/AsioDevice.cpp" 
	"Irc/ChannelManager.cpp"
    "Irc/Session.cpp"
    "Irc/Models/Message.cpp"
    "Irc/Parsers/MessageParser.cpp"
)

target_link_libraries(DatBot PRIVATE Boost::program_options)
target_link_libraries(DatBot PRIVATE rxcpp)
target_link_libraries(DatBot PRIVATE yaml-cpp)
target_link_libraries(DatBot PRIVATE Threads::Threads)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/datbot.conf"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

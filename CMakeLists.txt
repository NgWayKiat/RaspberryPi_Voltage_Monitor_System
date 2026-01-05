cmake_minimum_required(VERSION 3.10.0)
project(MCC118 VERSION 0.1.0 LANGUAGES C)

add_library(MCC118 CMakeLists.txt ClientSocket.cpp Global.cpp Logger.cpp MCC_DAQHATS.cpp Main.cpp Misc.cpp ServerSocket.cpp Versions.cpp)

include(CTest)
enable_testing()


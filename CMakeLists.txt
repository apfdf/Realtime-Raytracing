cmake_minimum_required(VERSION 3.0.0)
project(Realtime-Raytracing VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(Realtime-Raytracing main.cpp)
target_link_libraries(Realtime-Raytracing SDL2main SDL2)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

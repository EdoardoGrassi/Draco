cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

add_library(device-system STATIC)
if(WIN32)
    target_sources(device-system PRIVATE "src/device_system_win32.cpp")
    find_library(XINPUT_FOUND "xinput" REQUIRED DOC "XInput library")
    if(XINPUT_FOUND)
        message(STATUS "XInput found at ${XINPUT_FOUND}")
        target_compile_definitions(device-system PRIVATE "WIN32_LEAN_AND_MEAN" "NOMINMAX")
        target_link_libraries(device-system PRIVATE ${XINPUT_FOUND})
    else()
        message(SEND_ERROR "Cannot locate XInput library")
    endif()
endif()
add_library(drako::devices ALIAS device-system)


add_library(input-system STATIC
    "src/input_system.cpp"
    "src/input_system_types.cpp")
target_link_libraries(input-system
    PRIVATE drako::devices
    PUBLIC yaml-cpp
)
add_library(drako::input ALIAS input-system)


add_executable(device_system_testrun "test/device_system_testrun.cpp")
target_link_libraries(device_system_testrun PRIVATE drako::devices drako::input)

add_executable(input_system_testrun "test/input_system_testrun.cpp")
target_link_libraries(input_system_testrun PRIVATE drako::input)
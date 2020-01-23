﻿cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project("drako" VERSION 0.1.0 LANGUAGES CXX)
enable_testing()

# Set required c++ standard.
# Enable an error report if the standard isn't supported.
# Disable compiler specific extensions.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output folders for binaries, executables and libraries.
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)


if(MSVC)
    add_compile_options(/WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
if(WIN32)
    # Undefine ****ing min() and max() macros.
    add_compile_definitions(NOMINMAX)
endif()

add_subdirectory("include/drako/core")
add_subdirectory("include/drako/concurrency")
add_subdirectory("include/drako/plugins")
add_subdirectory("include/drako/input")
add_subdirectory("include/drako/physics")

add_executable(${PROJECT_NAME}_threading_test "test/main_thread_test.cpp")

add_executable(${PROJECT_NAME}_audio_test
    "test/main_audio_test.cpp"
    "include/drako/audio/audio_input_device_win.cpp")

add_executable(drako_mouse_test WIN32 "test/main_mouse_test.cpp")
target_link_libraries(drako_mouse_test PRIVATE drako::native_sys)

add_executable(drako_keyboard_test WIN32 "test/keyboard_app_test.cpp")
target_link_libraries(drako_keyboard_test PRIVATE drako::native_sys)

add_executable(drako_hid_test WIN32 "test/hid_app_test.cpp")
target_link_libraries(drako_hid_test PRIVATE drako::native_sys)



add_subdirectory("include/drako/graphics")

# Experimental GUI render
add_executable(vulkan_gui_render_test WIN32 "src/vulkan_gui_render_win32.cpp")
target_link_libraries(vulkan_gui_render_test PRIVATE drako::gui_renderer)

# Experimental Drako engine with forward renderer
add_executable(vulkan_engine WIN32 "src/vulkan_engine_win32.cpp")
target_link_libraries(vulkan_engine PRIVATE drako::forward_renderer)


# Compiler for runtime resources consumed by the Drako engine
add_executable(drako_resource_compiler "src/drako_resource_compiler.cpp")
set_target_properties(drako_resource_compiler PROPERTIES OUTPUT_NAME "drako-builder")

add_executable(obj_importer "test/obj_to_mesh.cpp")
target_link_libraries(obj_importer PRIVATE drako::obj)

# === TESTING ===
# work in progress...

file(GLOB test_files LIST_DIRECTORIES false CONFIGURE_DEPENDS "test_*.cpp")

foreach(file ${test_files})
    # Strip last extension from filename
    get_filename_component(file_without_ext ${file} NAME_WLE)
	add_executable(${file_without_ext} ${file})
	target_link_libraries(${file_without_ext} ${PROJECT_LIBS})
	add_test(NAME ${file_without_ext}
        COMMAND ${CMAKE_CTEST_COMMAND}
    )
	set_tests_properties(${file_without_ext}
		PROPERTIES
		PASS_REGULAR_EXPRESSION "Test passed"
    )
	set_tests_properties(${file_without_ext}
		PROPERTIES
		FAIL_REGULAR_EXPRESSION "(Exception|Test failed)"
    )
	set_tests_properties(${file_without_ext}
		PROPERTIES
		TIMEOUT 120
    )
endforeach()

# === PACKAGING ===
# work in progress...




# === FORMATTING ===
# Format with clang-format tool.

# work in progress...

find_program(DRAKO_CLANG_FORMAT_FOUND NAMES clang-format DOC "Path to clang-format executable")
mark_as_advanced(DRAKO_CLANG_FORMAT_FOUND)
if(NOT DRAKO_CLANG_FORMAT_FOUND)
    message(STATUS "clang-format not found. Target <format> won't be available.")
else()
    message(STATUS "clang-format found at ${DRAKO_CLANG_FORMAT_FOUND}.")
    # set(DO_CLANG_FORMAT "${CLANG_FORMAT_EXE}" "-i -style=file")

    foreach(local_build_target IN ${BUILDSYSTEM_TARGETS})
        get_target_property(local_sources local_build_target SOURCES)
        list(APPEND DRAKO_ALL_TARGETS_SOURCES ${local_sources})
    endforeach()

    add_custom_target(
        TARGET format
        COMMAND clang-format -i -style=LLVM ${DRAKO_ALL_TARGETS_SOURCES}
    )
endif()


# === DOCUMENTATION ===
# work in progress...

find_program(DOXYGEN_EXE "doxygen" DOC "Path to doxygen executable")
if(NOT DOXYGEN_EXE)
    message(STATUS "doxygen not found. Target <doc> won't be available.")
else()
    message(STATUS "doxygen found at ${DOXYGEN_EXE}.")
    add_custom_target(TARGET doc COMMAND doxygen)
endif()
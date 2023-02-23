cmake_minimum_required 	(VERSION 3.15)
get_filename_component 	(PROJECT_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
project 		(${PROJECT_NAME} LANGUAGES C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-O3 -D_GNU_SOURCE")

find_package(PkgConfig REQUIRED)

pkg_check_modules(GST_VIDEO REQUIRED IMPORTED_TARGET gstreamer-video-1.0)
set(LIBS ${LIBS} PkgConfig::GST_VIDEO)

pkg_check_modules(GST_RTSP REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0)
set(LIBS ${LIBS} PkgConfig::GST_RTSP )


add_executable		( gst-rec gst-rec.c )
target_link_libraries 	( gst-rec ${LIBS} )

add_executable		( gst-rtsp gst-rtsp.c )
target_link_libraries 	( gst-rtsp ${LIBS} )

install(TARGETS gst-rec gst-rtsp
        RUNTIME DESTINATION bin)

message(STATUS)
message(STATUS "General configuration for ${PROJECT_NAME}")
message(STATUS "======================================================================")
message(STATUS "Platform:")
message(STATUS "  Arch / OS:            ${CMAKE_HOST_SYSTEM_PROCESSOR} / ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION}")
message(STATUS "  C Compiler:           ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  C Standard:           ${CMAKE_C_STANDARD} ${CMAKE_C_STANDARD}")
message(STATUS "  CMake Version:        ${CMAKE_VERSION}")
message(STATUS "  Build type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "======================================================================")
message(STATUS)

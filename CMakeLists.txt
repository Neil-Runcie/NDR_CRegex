cmake_minimum_required(VERSION 3.22)

project(NDR_LAP
    VERSION 0.1.0
    DESCRIPTION "NDR C Regex"
    LANGUAGES C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(ndr_cregex STATIC src/ndr_regex.c)

configure_file(${CMAKE_SOURCE_DIR}/src/ndr_cregex.h ${CMAKE_SOURCE_DIR}/include/ndr_cregex.h)

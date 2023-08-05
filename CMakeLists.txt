cmake_minimum_required(VERSION 3.22)

project(NDR_CRegex
    VERSION 0.1.1
    DESCRIPTION "NDR C Regex"
    LANGUAGES C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(ndr_cregex STATIC src/ndr_regex.c src/ndr_regextracker.c src/ndr_regexnode.c)

configure_file(${CMAKE_SOURCE_DIR}/src/ndr_regex.h ${CMAKE_SOURCE_DIR}/include/ndr_regex.h)

install (TARGETS ndr_cregex
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)

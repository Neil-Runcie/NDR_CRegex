CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
PROJECT(PCRE2 C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

INCLUDE(GNUInstallDirs)

project(NDR_CRegex
    VERSION 0.1.1
    DESCRIPTION "NDR C Regex"
    LANGUAGES C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(ndr_cregex STATIC src/ndr_regex.c src/ndr_regextracker.c src/ndr_regexnode.c)

configure_file(${CMAKE_SOURCE_DIR}/src/ndr_regex.h ${CMAKE_SOURCE_DIR}/include/ndr_regex.h)

install (TARGETS ndr_cregex
         ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
         LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
         RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)

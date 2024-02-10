cmake_minimum_required(VERSION 3.12)
project(LLIB)

# Windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Release
if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox /OPT:REF")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

# Add a postfix for debug library
set(CMAKE_DEBUG_POSTFIX d)

# Source / include
set(LLIB_SRC "src")
set(LLIB_INCLUDE "include")

# Source files
file(GLOB LLIB_SOURCES
    ${LLIB_SRC}/*.cpp
)

# Source files
file(GLOB LLIB_INCLUDES
    ${LLIB_INCLUDE}/*.h
)

# As lib
add_library(${PROJECT_NAME} STATIC ${LLIB_SOURCES})

# Set headers
target_include_directories(${PROJECT_NAME} PUBLIC ${LLIB_INCLUDE})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${LLIB_INCLUDES}")

# Export variables
set(LLIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/${LLIB_INCLUDE}" CACHE STRING "Share this variable with other submodules")
set(LLIB_LIBRARIES $<TARGET_FILE:${PROJECT_NAME}> CACHE STRING "Share this variable with other submodules")

# Install
install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    FILES ${PROJECT_SOURCE_DIR}/FindLLIB.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}
)
cmake_minimum_required(VERSION 3.12)
project(L)

if (NOT DEFINED LLIB_LIBRARIES OR NOT DEFINED LLIB_INCLUDE_DIR)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/")
    find_package(LLIB REQUIRED)
endif()

# Add a postfix for debug library
set(CMAKE_DEBUG_POSTFIX d)

# Source / include
set(L_SRC "src")
set(L_INCLUDE "include")

# Source files
file(GLOB L_SOURCES
    ${L_SRC}/*.cpp
)

# Source files
file(GLOB L_INCLUDES
    ${L_INCLUDE}/*.h
)

# As exe
add_executable(${PROJECT_NAME} ${L_SOURCES})

# Add d if debug mode
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Add include
target_include_directories(${PROJECT_NAME} PUBLIC ${LLIB_INCLUDE_DIR})

# Link LLIB
target_link_libraries(${PROJECT_NAME} ${LLIB_LIBRARIES})

# Install
install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

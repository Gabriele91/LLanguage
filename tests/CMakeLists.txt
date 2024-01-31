cmake_minimum_required(VERSION 3.12)
project(tests)

if (NOT DEFINED LLIB_LIBRARIES OR NOT DEFINED LLIB_INCLUDE_DIR)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/")
    find_package(LLIB REQUIRED)
endif()

# Add a postfix for debug library
set(CMAKE_DEBUG_POSTFIX d)

# Source / include
set(TEST_SRC "src")
set(TEST_SCRIPT "scripts")
set(TEST_INCLUDE "include")

# Source files
file(GLOB TEST_SOURCES
    ${TEST_SRC}/*.cpp
)

# Script files
file(GLOB TEST_SCRIPTS
    ${TEST_SCRIPT}/*.ll
)

# Source files
file(GLOB TEST_INCLUDES
    ${TEST_INCLUDE}/*.h
)

# As exe
add_executable(${PROJECT_NAME} ${TEST_SOURCES})
add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME} "${pathword}" "totoro.txt"
)

# Add d if debug mode
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Add include
target_include_directories(${PROJECT_NAME} PUBLIC ${LLIB_INCLUDE_DIR})

# Link LLIB
target_link_libraries(${PROJECT_NAME} ${LLIB_LIBRARIES})

# Install
install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/tests"
)
install(
    FILES ${TEST_SCRIPTS} DESTINATION "${CMAKE_INSTALL_PREFIX}/tests"
)
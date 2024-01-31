cmake_minimum_required(VERSION 3.12)
project(L_LANGUAGE)

# Compile VM & Main
add_subdirectory(llib)
add_subdirectory(l)
add_subdirectory(tests)

# LLIB before L
add_dependencies(L LLIB)


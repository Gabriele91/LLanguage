# Dirs
set(LLIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
set(LLIB_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/lib")
set(LLIB_DYNAMIC_DIR "${CMAKE_CURRENT_LIST_DIR}/bin")

# Include files
file(GLOB LLIB_INCLUDES ${LLIB_INCLUDE_DIR}/*.h)

# Lib
file(GLOB LLIB_LIBRARIES ${LLIB_LIB_DIR}/*.lib)

# Check if both the header and the library have been found
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LLIB DEFAULT_MSG LLIB_INCLUDES LLIB_LIBRARIES)

# Set found
set(LLIB_FOUND ON)

# Mask as lib
mark_as_advanced(LLIB_FOUND LLIB_INCLUDE_DIR LLIB_LIB_DIR LLIB_DYNAMIC_DIR LLIB_LIBRARIES LLIB_INCLUDES)

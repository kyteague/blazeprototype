#
# Copyright (C) 2011-12, Dynamic NDArray Developers
# BSD 2-Clause License, see LICENSE.txt
#

project(blaze)

cmake_minimum_required(VERSION 2.6)

# Force the default build type to be Release, because a Debug
# build doesn't work properly with the default Python build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
      "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
      FORCE)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(Boost REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(PythonLibsNew REQUIRED)
find_package(NumPy REQUIRED)
include(UseCython)

# Default install location for Python packages
if (NOT PYTHON_PACKAGE_INSTALL_PREFIX)
    set(PYTHON_PACKAGE_INSTALL_PREFIX "${PYTHON_SITE_PACKAGES}" CACHE STRING
      "Choose the Python module directory (default site-packages)" FORCE)
endif()

# Require version >= 1.5
if(NUMPY_VERSION_DECIMAL LESS 10500)
    message(FATAL_ERROR,
        "Blaze requires NumPy >= 1.5")
endif()

if (NOT EXISTS "${PROJECT_SOURCE_DIR}/libraries/dynamicndarray/include/dynd/ndarray.hpp")
message(FATAL_ERROR
    "The dynamicndarray C++ library must be placed in\n"
    "libraries/dynamicndarray.\nRemove any temporary CMake\n"
    "files, then if you're using git, run 'git submodule init'\n"
    "and 'git submodule update' to clone it as a submodule.\n"
    "See INSTALL.txt for more details.")
endif()

# For now, we include dynamicndarray as a submodule.
add_subdirectory(libraries/dynamicndarray)

include_directories(
    ${PYTHON_INCLUDE_DIRS}
    ${NUMPY_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    libraries/dynamicndarray/include
    include
    )

set(pydynd_CPP_SRC
    include/codegen_cache_functions.hpp
    include/ctypes_interop.hpp
    include/do_import_array.hpp
    include/placement_wrappers.hpp
    include/dtype_functions.hpp
    include/exception_translation.hpp
    include/ndarray_functions.hpp
    include/ndarray_from_py.hpp
    include/ndarray_as_py.hpp
    include/numpy_interop.hpp
    include/elwise_gfunc_functions.hpp
    include/elwise_reduce_gfunc_functions.hpp
    include/utility_functions.hpp
    include/vm_elwise_program_functions.hpp
    src/codegen_cache_functions.cpp
    src/ctypes_interop.cpp
    src/dtype_functions.cpp
    src/exception_translation.cpp
    src/ndarray_functions.cpp
    src/ndarray_from_py.cpp
    src/ndarray_as_py.cpp
    src/numpy_interop.cpp
    src/elwise_gfunc_functions.cpp
    src/elwise_reduce_gfunc_functions.cpp
    src/utility_functions.cpp
    src/vm_elwise_program_functions.cpp
    )
set(pydynd_CYTHON_SRC
    src/_pydynd.pyx
    include/codegen_cache.pxd
    include/dynd.pxd
    include/dtype.pxd
    include/ndarray.pxd
    include/elwise_gfunc.pxd
    include/elwise_reduce_gfunc.pxd
    include/vm_elwise_program.pxd
    )
set_source_files_properties(${pydynd_CYTHON_SRC} PROPERTIES CYTHON_IS_CXX 1)

source_group("Cython Source" REGULAR_EXPRESSION ".*pyx$")
source_group("Cython Headers" REGULAR_EXPRESSION ".*pxd$")

cython_add_module(_pydynd ${pydynd_CPP_SRC} ${pydynd_CYTHON_SRC})

set_target_properties(_pydynd PROPERTIES LINKER_LANGUAGE CXX)
if(WIN32)
elseif(APPLE)
    # We compile with -Werror, and Cython's code generates warnings, so
    # disable that specifically for this project.
    set_target_properties(
        _pydynd
        PROPERTIES COMPILE_FLAGS
        "-Wno-unused-parameter -Wno-self-assign -Wno-unused-function"
        )
else()
    # strict aliasing is disabled because Cython generated code
    # which made g++ 4.6.1 complain
    set_target_properties(
        _pydynd
        PROPERTIES COMPILE_FLAGS
        "-Wno-unused-parameter -Wno-unused-function -fno-strict-aliasing"
        )
endif()

target_link_libraries(_pydynd dynamicndarray)

# Install all the Python scripts
install(DIRECTORY blaze DESTINATION "${PYTHON_PACKAGE_INSTALL_PREFIX}"
    FILES_MATCHING PATTERN "*.py")
# Install the module
install(TARGETS _pydynd DESTINATION "${PYTHON_PACKAGE_INSTALL_PREFIX}/blaze/nd")
# Install the basic_kernels .dll/.so
get_property(basic_kernels_loc TARGET basic_kernels PROPERTY LOCATION)
# NOTE: This only uses one configuration for the whole Visual Studio
#       solution, so CMAKE_BUILD_TYPE must match the build target
#       you want to install with.
string(REPLACE "$(Configuration)" ${CMAKE_BUILD_TYPE} basic_kernels_loc ${basic_kernels_loc})
string(REPLACE "$(OutDir)" ${CMAKE_BUILD_TYPE} basic_kernels_loc ${basic_kernels_loc})
install(FILES ${basic_kernels_loc} DESTINATION "${PYTHON_PACKAGE_INSTALL_PREFIX}/blaze/nd")
# Install the dynamicndarray lib
if(NOT WIN32)
    get_property(dynamicndarray_loc TARGET dynamicndarray PROPERTY LOCATION)
    install(FILES ${dynamicndarray_loc} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()

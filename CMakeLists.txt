cmake_minimum_required(VERSION 3.14) # I like pie

project(PocketSphinx VERSION 4.9.999.0
  DESCRIPTION "A small speech recognizer"
  HOMEPAGE_URL "https://github.com/cmusphinx/pocketsphinx")
include(CMakePrintHelpers)
set(PACKAGE_NAME ${PROJECT_NAME})
string(TOLOWER ${PROJECT_NAME} PROJECT_SHORTNAME)
set(PACKAGE_VERSION ${PROJECT_VERSION})
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
set(PACKAGE_TARNAME "${PROJECT_SHORTNAME}-${PROJECT_VERSION}")
set(PACKAGE_URL ${PROJECT_HOMEPAGE_URL})
set(PACKAGE_BUGREPORT dhdaines@gmail.com)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
  enable_testing()
endif()

include(CheckTypeSize)
include(CheckSymbolExists)
include(CheckLibraryExists)
include(TestBigEndian)
include(GNUInstallDirs)

# Statically link the Python package
if(CALL_FROM_SETUP_PY)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE(sys/stat.h HAVE_SYS_STAT_H)
CHECK_SYMBOL_EXISTS(snprintf stdio.h HAVE_SNPRINTF)
CHECK_SYMBOL_EXISTS(popen stdio.h HAVE_POPEN)
CHECK_TYPE_SIZE(long LONG)
CHECK_TYPE_SIZE("long long" LONG_LONG)
# OMG CMake is so incredibly awful
set(SIZEOF_LONG ${LONG})
set(SIZEOF_LONG_LONG ${LONG_LONG})
cmake_print_variables(SIZEOF_LONG SIZEOF_LONG_LONG)
test_big_endian(WORDS_BIGENDIAN)
cmake_print_variables(WORDS_BIGENDIAN)

# Don't do this
#if(CMAKE_BUILD_TYPE STREQUAL Debug)
#  set(SPHINX_DEBUG 1)
#endif()

# Compiles some code as the wrong endianness in order to ensure that
# it works properly
if(DEBUG_ENDIAN)
  add_definitions(-DDEBUG_ENDIAN)
endif()
cmake_print_variables(SPHINX_DEBUG DEBUG_ENDIAN)

if(MSVC)
  add_compile_options(/W3)
else()
  add_compile_options(-Wall -Wextra)
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
if(BUILD_SHARED_LIBS)
  add_definitions(-DSPHINX_DLL)
endif()
option(FIXED_POINT "Build using fixed-point math" OFF)
if(NOT DEFAULT_RADIX)
  set(DEFAULT_RADIX 12)
endif()
cmake_print_variables(FIXED_POINT DEFAULT_RADIX)

# Maybe not a great idea, but it does work on both Windows and Linux
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

configure_file(config.h.in config.h)
configure_file(sphinx_config.h.in include/sphinxbase/sphinx_config.h)
add_definitions(-DHAVE_CONFIG_H)

add_subdirectory(src)
# Only build SWIG and Python if we are building the package
if(CALL_FROM_SETUP_PY)
  add_subdirectory(swig)
else()
  # Don't build or install these in Python
  add_subdirectory(model)
  add_subdirectory(doc)
  add_subdirectory(include)
  add_subdirectory(programs)
  add_subdirectory(test)
  configure_file(pocketsphinx.pc.in pocketsphinx.pc @ONLY)
  install(FILES ${CMAKE_BINARY_DIR}/pocketsphinx.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()


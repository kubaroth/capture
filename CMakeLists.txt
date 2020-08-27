cmake_minimum_required(VERSION 3.10)
project(capture)

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(ExternalProject)

# Get source from already checked out submodule
ExternalProject_Add(pdfhummus
  URL ${CMAKE_SOURCE_DIR}/pdfwriter
  INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install_dir
  CMAKE_ARGS
  "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/install_dir"
  )

add_executable(capture src/main.cpp)
add_dependencies(capture pdfhummus)

set(CMAKE_CXX_FLAGS "-fpermissive -g -O0")

set (PDFHUMMUS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install_dir CACHE STRING "Path to PDF-Writer libraries" )

target_link_libraries(capture
  ${PDFHUMMUS_PATH}/lib/libPDFWriter.a
  ${PDFHUMMUS_PATH}/lib/libFreeType.a
  ${PDFHUMMUS_PATH}/lib/libLibAesgm.a
  ${PDFHUMMUS_PATH}/lib/libLibPng.a
  ${PDFHUMMUS_PATH}/lib/libLibTiff.a
  ${PDFHUMMUS_PATH}/lib/libLibJpeg.a
  ${PDFHUMMUS_PATH}/lib/libZlib.a
  X11
  )

add_executable(rgb tests/mainimport.cpp tests/rgb.cpp)
add_test(NAME "rgb" COMMAND "rgb")


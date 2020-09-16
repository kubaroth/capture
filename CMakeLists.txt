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
add_executable(image tests/mainimport.cpp tests/image.cpp)
add_test(NAME "image" COMMAND "image")

# The image tests depends on png shipped in PDFWriter library
add_dependencies(image pdfhummus) # png.h

target_link_libraries(image
  ${PDFHUMMUS_PATH}/lib/libLibPng.a
  ${PDFHUMMUS_PATH}/lib/libZlib.a
  )


### Documentation

if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
  find_package(Doxygen)

  if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM )
  else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif (DOXYGEN_FOUND)

  install(TARGETS capture DESTINATION bin)
  install(DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/doc_doxygen DESTINATION docs)
endif()


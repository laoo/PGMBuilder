cmake_minimum_required (VERSION 3.25)


project ( PGMBuilder )

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/libextern/zip )

set( CMAKE_CXX_STANDARD 23 )

add_executable( PGMBuilder
  main.cpp
  ProgramOptions.cpp
  ProgramOptions.hpp
  bulkConvert.cpp
  convert.cpp
  MameDB.cpp
  MameDBImport.cpp
  Builder.cpp
  Log.cpp
  Image.cpp
  TextEncoding.cpp
  WriteUtils.cpp
  manualBuild.cpp
  MameImage.cpp
  RomAssembly.cpp
  crypt.cpp
  crypt/kov/kov_deprotect.cpp
  crypt/orlegend/orlegend_deprotect.cpp
  crypt/simple_decrypt.cpp)

target_include_directories( PGMBuilder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libextern/zip/src )
target_link_libraries( PGMBuilder PRIVATE zip )

if (WIN32)

  target_compile_definitions( PGMBuilder PRIVATE -D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_DEPRECATE )

else()

  target_compile_options( PGMBuilder PRIVATE
    -Wpedantic
    -Wall
    -Wextra
    -O2
  )

endif()

target_precompile_headers( PGMBuilder PRIVATE
  <algorithm> <array>
  <bit>
  <cassert> <charconv> <cstdint>
  <filesystem> <format> <fstream> <functional>
  <iostream>
  <optional>
  <span> <string>
  <unordered_map>
  <vector>
)



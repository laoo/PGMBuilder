﻿cmake_minimum_required (VERSION 3.25)


project ( PGMBuilder )

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/libextern/zip )

set( CMAKE_CXX_STANDARD 20 )

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
  MameImage.cpp
)

target_include_directories( PGMBuilder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libextern/zip/src )
target_link_libraries( PGMBuilder PRIVATE zip )

if (WIN32)

  target_compile_definitions( PGMBuilder PRIVATE -D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_DEPRECATE )
  #set_source_files_properties(MameDBImport.cpp PROPERTIES COMPILE_FLAGS "/P")

endif()


target_precompile_headers( PGMBuilder PRIVATE
  <array>
  <algorithm>
  <string>
  <bit>
  <charconv>
  <cassert>
  <cstdint>
  <vector>
  <format>
  <span>
  <iostream>
  <filesystem>
  <fstream>
  <optional>
  <unordered_map>
)



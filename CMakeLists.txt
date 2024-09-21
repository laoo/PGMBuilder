cmake_minimum_required (VERSION 3.25)


project ( PGMBuilder )

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/libextern/zip )

set( CMAKE_CXX_STANDARD 20 )

add_executable( PGMBuilder
  main.cpp
)

target_include_directories( PGMBuilder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libextern/zip/src )
target_link_libraries( PGMBuilder PRIVATE zip )

if (WIN32)

  target_compile_definitions( PGMBuilder PRIVATE -D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_DEPRECATE )

endif()


target_precompile_headers( PGMBuilder PRIVATE
  <array>
  <algorithm>
  <string>
  <bit>
  <cstdint>
  <vector>
  <format>
  <span>
  <iostream>
  <filesystem>
  <fstream>
  <optional>
)



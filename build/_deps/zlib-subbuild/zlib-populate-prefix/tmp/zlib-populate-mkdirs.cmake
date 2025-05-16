# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/zlib-src"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/zlib-build"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/zlib-subbuild/zlib-populate-prefix"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/zlib-subbuild/zlib-populate-prefix/tmp"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/zlib-subbuild/zlib-populate-prefix/src"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/crow-src"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/crow-build"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/crow-subbuild/crow-populate-prefix"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/crow-subbuild/crow-populate-prefix/tmp"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/crow-subbuild/crow-populate-prefix/src/crow-populate-stamp"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/crow-subbuild/crow-populate-prefix/src"
  "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/crow-subbuild/crow-populate-prefix/src/crow-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/crow-subbuild/crow-populate-prefix/src/crow-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/Nitheesh/project/ERA-Efficient-Resource-allocation/build/_deps/crow-subbuild/crow-populate-prefix/src/crow-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

#!/bin/sh


echo "All done."; ccache -V && ccache --show-stats && ccache --zero-stats;
echo "using gcc : : ccache $CXX : <cflags>-std=c11 <cxxflags>-std=c++11 ;" > ~/user-config.jam ;
echo "using clang : : ccache $CXX : <cflags>-std=c11 <cxxflags>-std=c++11 ;" >> ~/user-config.jam; 


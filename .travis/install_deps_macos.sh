#!/bin/bash

#
# Note: gmp and boost already installed
#
brew update > brew.log
#brew install qt5 doxygen homebrew/science/hdf5 graphviz graphicsmagick fftw eigen
brew install qt5 graphicsmagick fftw eigen
# Explicit install of libqglviewer
brew tap DGtal-team/homebrew-dgtal
brew install libqglviewer



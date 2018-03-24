#!/bin/bash

#
# Note: gmp and boost already installed
#
brew update
#brew install qt5 doxygen homebrew/science/hdf5 graphviz graphicsmagick fftw eigen
brew install qt5 graphicsmagick fftw eigen
# Explicit install of libqglviewer
brew install http://liris.cnrs.fr/david.coeurjolly/misc/libqglviewer.rb

brew install opencv3

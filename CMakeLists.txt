#Bertrand Kerautret <bertrand.kerautret@loria.fr>
# -----------------------------------------------------------------------------
# Project definition
# -----------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.20) 
PROJECT(DGtalTools-contrib)

cmake_policy(SET CMP0057 NEW) # the new interpretation of IN_LIST is by default not set (at least until  3.21.1).
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
option(NO_ADD_STBIMAGE_IMPLEMENT "To avoid duplicated linking errors (like LNK2005 in MSVC)" OFF)
option(DISABLE_POLYSCOPE "Disable polyscope based tools" OFF)
list(APPEND CMAKE_PREFIX_PATH "${DGtal_DIR}")




# -----------------------------------------------------------------------------
# CPP20
# -----------------------------------------------------------------------------
set(DGTALTOOLS_CMAKE_CXX_STANDARD_MIN_REQUIRED 20)
 if(NOT CMAKE_CXX_STANDARD)
   set(CMAKE_CXX_STANDARD ${DGTALTOOLS_CMAKE_CXX_STANDARD_MIN_REQUIRED})
 else()
  # Throw if CMAKE_CXX_STANDARD is 98
  if(${CMAKE_CXX_STANDARD} EQUAL 98)
    message(FATAL_ERROR "CMAKE_CXX_STANDARD is set to ${CMAKE_CXX_STANDARD}, "
      "but DGtalTools requires at least ${DGTALTOOLS_CMAKE_CXX_STANDARD_MIN_REQUIRED}.")
  endif()
endif()
if(NOT CMAKE_CXX_STANDARD_REQUIRED)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
message(STATUS "C++ standard set to ${CMAKE_CXX_STANDARD}")


# -----------------------------------------------------------------------------
# DGtal dependencies
# -----------------------------------------------------------------------------
message(STATUS "-------------------------------------------------------------------------------")
message(STATUS "Checking if DGtal is installed:")
FIND_PACKAGE(DGtal 2.0.0 REQUIRED)
INCLUDE_DIRECTORIES(${DGTAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DGTAL_LIBRARY_DIRS})
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
message(STATUS "DGtal found.")




# -------------------------------------------------------------------------
# This test is for instance used for ITK v3.x. ITK forces a limited
# template depth. We remove this option.
# --------------------------------------------------------------------------
if (CMAKE_CXX_FLAGS MATCHES "-ftemplate-depth-[0-9]*")
    message( "Warning: some package has enabled a limited template depth for the C++ compiler." )
    message( "         Disabling option -ftemplate-depth-xx in CMAKE_CXX_FLAGS." )
    set( CMAKE_CXX_FLAGS_TMP ${CMAKE_CXX_FLAGS} )
    STRING( REGEX REPLACE "-ftemplate-depth-[0-9]*" ""
    CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_TMP}" )
     message ("         CMAKE_CXX_FLAGS=" ${CMAKE_CXX_FLAGS} )
endif (CMAKE_CXX_FLAGS MATCHES "-ftemplate-depth-[0-9]*")



## or USE_OPENCV
if(WITH_OPENCV)
   find_package( OpenCV REQUIRED )
   include_directories(${OpenCV_INCLUDE_DIRS})

endif()



# -----------------------------------------------------------------------------
# CLI11
# -----------------------------------------------------------------------------
include_directories( "${PROJECT_SOURCE_DIR}/ext/" )



# -----------------------------------------------------------------------------
# Define variables and cmake parameters, and display some information
# -----------------------------------------------------------------------------
# SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
SET(DGtalToolsContrib_VERSION_MAJOR 1)
SET(DGtalToolsContrib_VERSION_MINOR 5)
SET(DGtalToolsContrib_VERSION_PATCH beta)
SET(DGTALTOOLSContrib_VERSION "${DGtalToolsContrib_VERSION_MAJOR}.${DGtalToolsContrib_VERSION_MINOR}.${DGtalToolsContrib_VERSION_PATCH}")
SET(PROJECT_VERSION "${DGtalToolsContrib_VERSION_MAJOR}.${DGtalToolsContrib_VERSION_MINOR}.${DGtalToolsContrib_VERSION_PATCH}")
SET(VERSION ${DGtalToolsContrib_VERSION_MAJOR}.${DGtalToolsContrib_VERSION_MINOR}.${DGtalToolsContrib_VERSION_PATCH})





#-----------------------------------------------------------------------------
# ITK setting
#-----------------------------------------------------------------------------
if (  DGTAL_WITH_ITK )
    include(${ITK_USE_FILE})
ENDIF( DGTAL_WITH_ITK )



#-----------------------------------------------------------------------------
# polyscope
#-----------------------------------------------------------------------------
if (NOT(DISABLE_POLYSCOPE))
  message(STATUS "Enabling polyscope tools")
  include(CPM)
  include(polyscope)
else()
    message(STATUS "Disabling polyscope tools")
endif()







#------Tools subdirectories ------
add_subdirectory(geometry2d)
add_subdirectory(geometry3d)
add_subdirectory(visualisation)



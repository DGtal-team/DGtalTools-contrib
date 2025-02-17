#Bertrand Kerautret <bertrand.kerautret@loria.fr>
# -----------------------------------------------------------------------------
# Project definition
# -----------------------------------------------------------------------------
PROJECT(DGtalTools-contrib)

cmake_minimum_required (VERSION 3.11) 
cmake_policy(SET CMP0057 NEW)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
include(polyscope)




 # -----------------------------------------------------------------------------
 # CPP17
 # -----------------------------------------------------------------------------
set(DGTALTOOLS_CONTRIB_CMAKE_CXX_STANDARD_MIN_REQUIRED 17)
 if(NOT CMAKE_CXX_STANDARD)
   set(CMAKE_CXX_STANDARD ${DGTALTOOLS_CONTRIB_CMAKE_CXX_STANDARD_MIN_REQUIRED})
 else()
  # Throw if CMAKE_CXX_STANDARD is 98
  if(${CMAKE_CXX_STANDARD} EQUAL 98)
    message(FATAL_ERROR "CMAKE_CXX_STANDARD is set to ${CMAKE_CXX_STANDARD}, "
      "but DGtalTools requires at least ${DGTALTOOLS_CONTRIB_CMAKE_CXX_STANDARD_MIN_REQUIRED}.")
  endif()
endif()
if(NOT CMAKE_CXX_STANDARD_REQUIRED)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()


FIND_PACKAGE(DGtal 1.4 REQUIRED)
INCLUDE_DIRECTORIES(${DGTAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DGTAL_LIBRARY_DIRS})

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



# CLI11
include_directories( "${PROJECT_SOURCE_DIR}/ext/" )
include_directories(${PROJECT_SOURCE_DIR})



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









#------Tools subdirectories ------
add_subdirectory(geometry2d)
add_subdirectory(geometry3d)
add_subdirectory(visualisation)



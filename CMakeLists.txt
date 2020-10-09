#Bertrand Kerautret <bertrand.kerautret@loria.fr>
# -----------------------------------------------------------------------------
# Project definition
# -----------------------------------------------------------------------------
PROJECT(DGtalTools-contrib)

cmake_minimum_required (VERSION 3.1) 

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# -----------------------------------------------------------------------------
# CPP11 
# -----------------------------------------------------------------------------
 set (CMAKE_CXX_STANDARD 11)
 set (CMAKE_CXX_STANDARD_REQUIRED TRUE)



FIND_PACKAGE(DGtal 1.0.0 REQUIRED)
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



# -----------------------------------------------------------------------------
# Define variables and cmake parameters, and display some information
# -----------------------------------------------------------------------------
# SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
SET(DGtalToolsContrib_VERSION_MAJOR 1)
SET(DGtalToolsContrib_VERSION_MINOR 1)
SET(DGtalToolsContrib_VERSION_PATCH 0)
SET(DGTALTOOLSContrib_VERSION "${DGtalToolsContrib_VERSION_MAJOR}.${DGtalToolsContrib_VERSION_MINOR}.${DGtalToolsContrib_VERSION_PATCH}")
SET(PROJECT_VERSION "${DGtalToolsContrib_VERSION_MAJOR}.${DGtalToolsContrib_VERSION_MINOR}.${DGtalToolsContrib_VERSION_PATCH}")
SET(VERSION ${DGtalToolsContrib_VERSION_MAJOR}.${DGtalToolsContrib_VERSION_MINOR}.${DGtalToolsContrib_VERSION_PATCH})






 # -----------------------------------------------------------------------------
 # Looking for boost
 # -----------------------------------------------------------------------------
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_FOUND FALSE)
FIND_PACKAGE(Boost 1.50.0 REQUIRED)
if ( Boost_FOUND )
  ADD_DEFINITIONS(${BOOST_DEFINITIONS} -DBOOST_ALL_NO_LIB)
  # SYSTEM to avoid warnings from boost.
  include_directories(SYSTEM ${Boost_INCLUDE_DIRS} )
  SET(DGtalToolsContribLibDependencies ${DGtalToolsContribLibDependencies}
     ${Boost_LIBRAIRIES})
   SET(DGtalLibInc ${Boost_INCLUDE_DIRS})  
endif( Boost_FOUND )




#------Tools subdirectories ------
add_subdirectory(geometry2d)
add_subdirectory(geometry3d)
add_subdirectory(visualisation)



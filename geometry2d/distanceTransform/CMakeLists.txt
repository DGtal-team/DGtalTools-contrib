if (WITH_QT5)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Xml_EXECUTABLE_COMPILE_FLAGS}")
endif (WITH_QT5)

add_executable(RationalBeattySequenceTest RationalBeattySequenceTest.cpp)

add_executable(CumulativeSequenceTest CumulativeSequence.cpp CumulativeSequenceTest.cpp)
#target_link_libraries(CumulativeSequenceTest sequence)

add_executable(LUTBasedNSDistanceTransform LUTBasedNSDistanceTransform.cpp ImageFilter.cpp ImageWriter.cpp ImageReader.cpp NeighborhoodSequenceDistance.cpp D4DistanceDT.cpp D8DistanceDT.cpp RatioNSDistanceDT.cpp PeriodicNSDistanceDT.cpp CumulativeSequence.cpp PBMImageReader.cpp PGMImageWriter.cpp)

target_link_libraries (LUTBasedNSDistanceTransform ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies})

if (WITH_PNG)
    find_package(PNG REQUIRED)
    if (PNG_FOUND)
  add_definitions(-DWITH_PNG)
  include_directories(${PNG_INCLUDE_DIRS})
  add_library(pngio PNGImageReader.cpp PNGImageWriter.cpp)
  target_link_libraries(LUTBasedNSDistanceTransform ${PNG_LIBRARIES})
  target_link_libraries(LUTBasedNSDistanceTransform pngio)
  message(STATUS "      PNG_FOUND       true")
    endif (PNG_FOUND)
endif (WITH_PNG)

install(TARGETS LUTBasedNSDistanceTransform
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)




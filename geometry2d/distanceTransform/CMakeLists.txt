add_executable(RationalBeattySequenceTest RationalBeattySequenceTest.cpp)

add_executable(CumulativeSequenceTest CumulativeSequence.cpp CumulativeSequenceTest.cpp)
#target_link_libraries(CumulativeSequenceTest sequence)

set(LUTBasedNSDistanceTransform_LIBRARIES)
set(LUTBasedNSDistanceTransform_SOURCES LUTBasedNSDistanceTransform.cpp ImageFilter.cpp ImageWriter.cpp ImageReader.cpp NeighborhoodSequenceDistance.cpp D4DistanceDT.cpp D8DistanceDT.cpp RatioNSDistanceDT.cpp PeriodicNSDistanceDT.cpp CumulativeSequence.cpp PBMImageReader.cpp PGMImageWriter.cpp)

if (WITH_PNG)
    find_package(PNG REQUIRED)
    if (PNG_FOUND)
      add_definitions(-DWITH_PNG)
      include_directories(${PNG_INCLUDE_DIRS})
      list(APPEND LUTBasedNSDistanceTransform_SOURCES PNGImageReader.cpp PNGImageWriter.cpp)
      list(APPEND LUTBasedNSDistanceTransform_LIBRARIES ${PNG_LIBRARIES})
      message(STATUS "      PNG_FOUND       true")
    endif (PNG_FOUND)
endif (WITH_PNG)

add_executable(LUTBasedNSDistanceTransform ${LUTBasedNSDistanceTransform_SOURCES})

target_link_libraries (LUTBasedNSDistanceTransform ${LUTBasedNSDistanceTransform_LIBRARIES} ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies})

install(TARGETS LUTBasedNSDistanceTransform
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)




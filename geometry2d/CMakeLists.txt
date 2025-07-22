SET(DGTAL_TOOLS_DEVEL_SRC
  meaningFullThickness
  rosinThreshold
  rotNmap
)


FOREACH(FILE ${DGTAL_TOOLS_DEVEL_SRC})
  add_executable(${FILE} ${FILE}.cpp)
  target_link_libraries (${FILE} ${DGTAL_LIBRARIES} ${DGtalLibDependencies} ${DGtalToolsContribLibDependencies} )
  install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)

if(WITH_OPENCV)
    add_executable(houghLineDetect houghLineDetect)
    target_link_libraries (houghLineDetect ${DGTAL_LIBRARIES} ${DGtalLibDependencies}  ${DGtalToolsContribLibDependencies} ${OpenCV_LIBS}  )
  install(TARGETS houghLineDetect 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

endif()

add_subdirectory(distanceTransform)

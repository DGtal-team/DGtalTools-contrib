SET(DGTAL_TOOLS_DEVEL_SRC
  meaningFullThickness
  rosinThreshold
  rotNmap
)


FOREACH(FILE ${DGTAL_TOOLS_DEVEL_SRC})
  add_executable(${FILE} ${FILE})
  target_link_libraries (${FILE} ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies} )
  install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)

if(WITH_OPENCV)
    add_executable(houghLineDetect houghLineDetect)
    target_link_libraries (houghLineDetect ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies} ${OpenCV_LIBS}  )
  install(TARGETS houghLineDetect 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

endif()

add_subdirectory(distanceTransform)

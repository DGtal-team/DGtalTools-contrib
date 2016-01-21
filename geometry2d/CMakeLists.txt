SET(DGTAL_TOOLS_DEVEL_SRC
  meaningFullThickness
)


FOREACH(FILE ${DGTAL_TOOLS_DEVEL_SRC})
  add_executable(${FILE} ${FILE})
  target_link_libraries (${FILE} ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies} )
  install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)

add_subdirectory(distanceTransform)

SET(DGTAL_TOOLS_CONTRIB
   displayTgtCoverAlphaTS.cpp
)


FOREACH(FILE ${DGTAL_TOOLS_CONTRIB})
  add_executable(${FILE} ${FILE})
  target_link_libraries (${FILE} ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies})
  install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)




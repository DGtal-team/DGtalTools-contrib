SET(DGTAL_TOOLS_CONTRIB
   displayTgtCoverAlphaTS
   displayLineSegments
   displaySet2dPts
)

SET(DGTAL_TOOLS_CONTRIB_POLY
   polyMeshEdit
   polyMeshColorize
)





FOREACH(FILE ${DGTAL_TOOLS_CONTRIB})
  add_executable(${FILE} ${FILE}.cpp)
  target_link_libraries (${FILE} ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies})
  install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)


FOREACH(FILE ${DGTAL_TOOLS_CONTRIB_POLY})
  add_executable(${FILE} ${FILE}.cpp)
  target_link_libraries (${FILE}  polyscope ${DGTAL_LIBRARIES} ${DGtalLibDependencies}  ${DGtalToolsContribLibDependencies})
  install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)



SET(DGTAL_TOOLS_DEVEL_SRC
    meshBasicEdit
    computeMeshDistances
    volLocalMax
    basicMorphoFilter
    xyzScale
    off2obj
    obj2off
    off2sdp
    volFillCCSize
    vol2meshAndNormals
    meshAxisCutter
    graph2vol
    trunkMeshTransform
    splitMeshFromCol
)


FOREACH(FILE ${DGTAL_TOOLS_DEVEL_SRC})
  add_executable(${FILE} ${FILE}.cpp)
  target_link_libraries (${FILE} ${DGTAL_LIBRARIES} ${DGtalLibDependencies}  ${DGtalToolsContribLibDependencies})
  install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)


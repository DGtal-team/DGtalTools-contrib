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
)


FOREACH(FILE ${DGTAL_TOOLS_DEVEL_SRC})
  add_executable(${FILE} ${FILE})
  target_link_libraries (${FILE} ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies})
  install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)

if (  WITH_VISU3D_QGLVIEWER )
  SET(DGTAL_TOOLS_DEVEL_SRC
    )
  FOREACH(FILE ${DGTAL_TOOLS_DEVEL_SRC}) 
    add_executable(${FILE} ${FILE})
    target_link_libraries ( ${FILE} ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies})   
    install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
  ENDFOREACH(FILE)

endif ( WITH_VISU3D_QGLVIEWER   )

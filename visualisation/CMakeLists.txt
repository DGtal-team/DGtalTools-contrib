SET(DGTAL_TOOLS_CONTRIB
   displayTgtCoverAlphaTS
)

FIND_PACKAGE(DGtal REQUIRED)
INCLUDE_DIRECTORIES(${DGTAL_INCLUDE_DIRS})
LINK_DIRECTORIES(${DGTAL_LIBRARY_DIRS})

FIND_PACKAGE(Boost 1.46.0 REQUIRED COMPONENTS program_options)
if ( Boost_FOUND )
  message(STATUS "Boost and boost_program_options found.")
  include_directories( ${Boost_INCLUDE_DIRS} )
      SET(DGtalToolsLibDependencies ${DGtalToolsLibDependencies} 
     ${Boost_LIBRAIRIES}  
     ${Boost_PROGRAM_OPTIONS_LIBRARY})
   SET(DGtalLibInc ${Boost_INCLUDE_DIRS})
endif( Boost_FOUND) 


FOREACH(FILE ${DGTAL_TOOLS_CONTRIB})
  add_executable(${FILE} ${FILE})
  target_link_libraries (${FILE} ${DGTAL_LIBRARIES} ${DGtalToolsContribLibDependencies})
  install(TARGETS ${FILE} 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
ENDFOREACH(FILE)



 
add_executable(meshViewerEdit meshViewerEdit compClass/ViewerMesh)
target_link_libraries(meshViewerEdit ${DGTAL_LIBRARIES} ${DGtalToolsLibDependencies})
install(TARGETS meshViewerEdit RUNTIME DESTINATION bin
                                LIBRARY DESTINATION lib
                                ARCHIVE DESTINATION lib)

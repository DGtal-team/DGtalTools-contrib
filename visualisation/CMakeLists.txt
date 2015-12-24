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




 


if (  WITH_VISU3D_QGLVIEWER )

  if (WITH_QT4)
    QT4_WRAP_UI(ui_meshViewerEdit_FORMS_HEADERS meshViewerEdit.ui)
    QT4_WRAP_CPP(meshViewerEdit_HEADERS_MOC meshViewerEdit.h  OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

    add_executable(meshViewerEdit meshViewerEdit.cpp compClass/ViewerMesh ${sliceViewer_HEADERS_MOC}
                                               ${ui_sliceViewer_FORMS_HEADERS})
    target_link_libraries (meshViewerEdit ${DGTAL_LIBRARIES}  ${DGtalToolsLibDependencies})

  elseif (WITH_QT5)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    qt5_wrap_ui(ui_meshViewerEdit_FORMS_HEADERS meshViewerEdit.ui)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Xml_EXECUTABLE_COMPILE_FLAGS}")
    add_executable(meshViewerEdit meshViewerEdit compClass/ViewerMesh ${ui_meshViewerEdit_FORMS_HEADERS})
    target_link_libraries(meshViewerEdit ${DGTAL_LIBRARIES}  ${DGtalToolsLibDependencies})
    qt5_use_modules(meshViewerEdit Widgets OpenGL Xml)
  endif (WITH_QT4)

  install(TARGETS meshViewerEdit RUNTIME DESTINATION bin
                              LIBRARY DESTINATION lib
                              ARCHIVE DESTINATION lib)
endif (WITH_VISU3D_QGLVIEWER)

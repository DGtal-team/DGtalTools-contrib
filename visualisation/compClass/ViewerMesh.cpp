
/**
 * @file ViewerMesh.cpp
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 *
 * @date 2011/01/03
 *
 * Implementation of methods defined in ViewerMesh.h
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////

#include "ViewerMesh.h"
#include "DGtal/io/DrawWithDisplay3DModifier.h"
#include "DGtal/images/ImageHelper.h"
#include "DGtal/images/ConstImageAdapter.h"
#include "DGtal/images/ImageSelector.h"
#include <sstream>      
#include <QKeyEvent>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace qglviewer;


///////////////////////////////////////////////////////////////////////////////
// class ViewerMesh
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Standard services - public :


template < typename Space, typename KSpace>
void
ViewerMesh< Space, KSpace>::init(){
   DGtal::Viewer3D<>::init();
   QGLViewer::setKeyDescription ( Qt::Key_D|Qt::MetaModifier, "Delete the current selected faces (highlighted in red)" );
   QGLViewer::setKeyDescription ( Qt::Key_D, "Change the current mode to Delete mode" );
   QGLViewer::setKeyDescription ( Qt::Key_C, "Change the current mode to Color mode" );
   QGLViewer::setKeyDescription ( Qt::Key_Z, "Change the current axis to Z for the current 2D image slice setting." );
   QGLViewer::setKeyDescription ( Qt::Key_Plus, "Increase by one the pen size" );
   QGLViewer::setKeyDescription ( Qt::Key_Minus, "Decrease by one the pen size" );
   QGLViewer::setKeyDescription ( Qt::Key_Minus, "Undo the last edition." );
   QGLViewer::setKeyDescription ( Qt::Key_S, "Save the current mesh." );
}




template < typename Space, typename KSpace>
QString
ViewerMesh< Space, KSpace>::helpString() const
{
  QString text ( "<h2> ViewerMesh</h2>" );
  text += "Use the mouse to move the camera around the object. ";
  text += "You can respectively revolve around, zoom and translate with the three mouse buttons. ";
  text += "Left and middle buttons pressed together rotate around the camera view direction axis<br><br>";
  text += "Pressing <b>Alt</b> and one of the function keys (<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
  text += "Simply press the function key again to restore it. Several keyFrames define a ";
  text += "camera path. Paths are saved when you quit the application and restored at next start.<br><br>";
  text += "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
  text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save a snapshot. ";
  text += "See the <b>Keyboard</b> tab in this window for a complete shortcut list.<br><br>";
  text += "Double clicks automates single click actions: A left button double click aligns the closer axis with the camera (if close enough). ";
  text += "A middle button double click fits the zoom of the camera and the right button re-centers the scene.<br><br>";
  text += "A left button double click while holding right button pressed defines the camera <i>Revolve Around Point</i>. ";
  text += "See the <b>Mouse</b> tab and the documentation web pages for details.<br><br>";
  text += "Press <b>Escape</b> to exit the viewer.";
  text += "<h3> Edition mode</h3>";
  text += "The viewer offers 2 modes of edition:";
  text += "<ul><li>Color mode (access with B key): by pressing SHIFT+mouse click ou can change the color of the selected faces.";
  text += "<li> Delete mode (access with D key): in this mode, you can select the faces that you want to delete and then you can press Meta+D to remove the selected faces.</ul> ";

  return text;
}





template < typename Space, typename KSpace>
void
ViewerMesh< Space, KSpace>::keyPressEvent ( QKeyEvent *e )
{
  
  bool handled = false;
  if( e->key() == Qt::Key_Plus){
    myPenSize += 1;
    std::stringstream ss;
    ss << "Pen size: " << myPenSize*myPenScale;
    (*this).displayMessage(QString(ss.str().c_str()), 100000);
    handled=true;
  }
   if( e->key() == Qt::Key_Minus){
    myPenSize -= 1;
    std::stringstream ss;
    ss << "Pen size: " << myPenSize*myPenScale;
    (*this).displayMessage(QString(ss.str().c_str()), 100000);
    handled=true;
  }
  if( e->key() == Qt::Key_C){
    (*this).displayMessage(QString("Color Mode"), 100000);
    myMode = COLOR_MODE;
    handled=true;
  }
  if( e->key() == Qt::Key_S){
    myMesh >> myOutMeshName;
    (*this).displayMessage(QString("SAVED"), 100000);
    handled=true;
  }
  if( e->key() == Qt::Key_U){
    (*this).displayMessage(QString("UNDO"), 100000);
    if(myUndoQueue.size()>0){
        myMesh = myUndoQueue.front();
        myUndoQueue.pop_front();
    }
    if(myUndoQueueSelected.size()>0){
      myVectFaceToDelete = myUndoQueueSelected.front();
      myUndoQueueSelected.pop_front();
    }
    if(myMode==ERASE_MODE){
        displaySelectionOnMesh();
    }else{
      DGtal::Viewer3D<Space, KSpace>::clear();
      DGtal::Viewer3D<Space, KSpace>::operator<<(myMesh);
      DGtal::Viewer3D<Space, KSpace>::updateList(false);
      DGtal::Viewer3D<Space, KSpace>::updateGL();
      
    }
    handled=true;
  }
  if( e->key() == Qt::Key_D){
    if (e->modifiers() & Qt::MetaModifier){
      deleteCurrents();
    }else{
      (*this).displayMessage(QString("Delete Mode"), 100000);
      myMode = ERASE_MODE;
    }
    handled=true;
  }
  
  if ( !handled )
    DGtal::Viewer3D<>::keyPressEvent ( e );
}  




template< typename Space, typename KSpace>
void
ViewerMesh<Space, KSpace>::postSelection ( const QPoint& point )
{
  bool found;
  Vec p  = DGtal::Viewer3D<Space, KSpace>::camera()->pointUnderPixel ( point, found ) ;
  if (found){
    if(myMode == ERASE_MODE){
      addToDelete(DGtal::Z3i::RealPoint(p.x, p.y, p.z));
    }else if (myMode == COLOR_MODE){
      deleteFacesFromDist(DGtal::Z3i::RealPoint(p.x, p.y, p.z));
    }
  }
}




template< typename Space, typename KSpace>
void
ViewerMesh<Space, KSpace>::deleteCurrents(){
  addCurrentMeshToQueue();
  myMesh.removeFaces(myVectFaceToDelete);
  myVectFaceToDelete.clear();
  DGtal::Viewer3D<Space, KSpace>::clear();
  DGtal::Viewer3D<Space, KSpace>::operator<<(myMesh);
  DGtal::Viewer3D<Space, KSpace>::updateList(false);
  DGtal::Viewer3D<Space, KSpace>::updateGL();

}



template< typename Space, typename KSpace>
void
ViewerMesh<Space, KSpace>::addToDelete(DGtal::Z3i::RealPoint p){
  myUndoQueueSelected.push_front(myVectFaceToDelete);
  for (unsigned int i = 0; i < myMesh.nbFaces(); i++) {
    DGtal::Z3i::RealPoint c = myMesh.getFaceBarycenter(i);
    if ((c-p).norm() <= myPenSize*myPenScale){
      myVectFaceToDelete.push_back(i);
    }
  }

  displaySelectionOnMesh();
}


template< typename Space, typename KSpace>
void
ViewerMesh<Space, KSpace>::deleteFacesFromDist(DGtal::Z3i::RealPoint p)
{
  addCurrentMeshToQueue();
  for (unsigned int i = 0; i < myMesh.nbFaces(); i++) {
    DGtal::Z3i::RealPoint c = myMesh.getFaceBarycenter(i);
    if ((c-p).norm() <= myPenSize*myPenScale){
      myMesh.setFaceColor(i, myPenColor);
    }
  }
  DGtal::Viewer3D<Space, KSpace>::clear();
  DGtal::Viewer3D<Space, KSpace>::operator<<(myMesh);
  DGtal::Viewer3D<Space, KSpace>::updateList(false);
  DGtal::Viewer3D<Space, KSpace>::updateGL();
}


template< typename Space, typename KSpace>
void
ViewerMesh<Space, KSpace>::addCurrentMeshToQueue(){
  myUndoQueue.push_front(myMesh);
  if (myUndoQueue.size()> MAXUNDO){
    myUndoQueue.pop_back();
  }  
}


template< typename Space, typename KSpace>
void
ViewerMesh<Space, KSpace>::displaySelectionOnMesh()
{
  RealMesh tmp = myMesh;
  
  for (unsigned int i = 0; i < myVectFaceToDelete.size(); i++) {
    tmp.setFaceColor(myVectFaceToDelete[i], DGtal::Color::Red);
  }
  
  DGtal::Viewer3D<Space, KSpace>::clear();
  DGtal::Viewer3D<Space, KSpace>::operator<<(tmp);
  DGtal::Viewer3D<Space, KSpace>::updateList(false);
  DGtal::Viewer3D<Space, KSpace>::updateGL();

}









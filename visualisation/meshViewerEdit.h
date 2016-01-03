/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
/**
 * @file meshViewerEdit.h
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 *
 * @date 2015/12/20
 *
 * An simple mesh viewer which allow basic edition (coloring, face removing).
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#ifndef Q_MOC_RUN 
#include "DGtal/io/viewers/Viewer3D.h"
#include "DGtal/images/ImageContainerBySTLVector.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/images/ConstImageAdapter.h"
#include "DGtal/io/colormaps/GradientColorMap.h"
#include "DGtal/io/colormaps/HueShadeColorMap.h"
#endif 

#include "compClass/ViewerMesh.cpp"



#include <QMainWindow>

namespace Ui {
  class MainWindow;

}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  

    
public:
  explicit MainWindow(ViewerMesh<> *viewer,   QWidget *parent = 0, Qt::WindowFlags flags=0);
  ~MainWindow();
  
public slots:

  void updatePenSize();
  void setSelectMode();
  void setColorMode();
  void undo();
  void save();
  void filterVisibleFace();

public slots:
  


private:
  Ui::MainWindow *ui;
  ViewerMesh<> *myViewer;  
};

#endif // MAINWINDOW_H

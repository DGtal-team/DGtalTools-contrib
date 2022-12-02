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
 * @file meshViewerEdit.cpp
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
#include <iostream>


#ifndef Q_MOC_RUN
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/viewers/Viewer3D.h"
#include "DGtal/io/DrawWithDisplay3DModifier.h"
#include "DGtal/io/readers/PointListReader.h"
#include "DGtal/io/readers/MeshReader.h"

#include "DGtal/io/Color.h"
#include "DGtal/io/colormaps/GradientColorMap.h"
#include "DGtal/io/readers/GenericReader.h"

#include "CLI11.hpp"
#endif

#include <QInputDialog>
#include "meshViewerEdit.h"
#include "ui_meshViewerEdit.h"


using namespace std;
using namespace DGtal;
using namespace Z3i;

/**
@code 
Usage: ./meshViewerEdit [OPTIONS] 1

Positionals:
  1 TEXT:FILE REQUIRED                  input file: mesh (off,obj).

Options:
  -h,--help                             Print this help message and exit
  -i,--input TEXT:FILE REQUIRED         input file: mesh (off,obj).
  -o,--outputFile TEXT=out.off          save output file automatically according the file format extension.
  -s,--scalePen FLOAT=1                 change the scale factor of the pen size (by default 1.0, real size: penSize*scale).
  -c,--penColor UINT ...                change the scale factor of the pen size (by default 1.0)
*/

///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(ViewerMesh<> *aViewer,
                       QWidget *parent, Qt::WindowFlags flags) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  myViewer(aViewer)
{
  ui->setupUi(this);
  ui->verticalLayout->addWidget(aViewer);  
  QObject::connect(ui->scaleSlider, SIGNAL(valueChanged(int)), this, SLOT(updatePenSize()));
  QObject::connect(ui->selectButton, SIGNAL(clicked()), this, SLOT(setSelectMode()));
  QObject::connect(ui->invButton, SIGNAL(clicked()), this, SLOT(invertSelection()));
  QObject::connect(ui->colorButton, SIGNAL(clicked()), this, SLOT(setColorMode()));
  QObject::connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(save()));
  QObject::connect(ui->undoButton, SIGNAL(clicked()), this, SLOT(undo()));
  QObject::connect(ui->filterButton, SIGNAL(clicked()), this, SLOT(filterVisibleFace()));
  updatePenSize();
}

void
MainWindow::invertSelection(){
  myViewer->invertSelection();
}
void 
MainWindow::setSelectMode(){
  myViewer->setSelectMode();
}

void 
MainWindow::setColorMode(){
  myViewer->setColorMode();
}

void 
MainWindow::undo(){
  myViewer->undo();
}

void 
MainWindow::save(){
  myViewer->save();
}


void 
MainWindow::filterVisibleFace()
{
  bool ok;
  double angle = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
                                       tr("max allowed angle (rad):"), 
                                           1.0,0.0,3.14, 3, &ok);
(*myViewer).filterVisibleFaces(angle);

}

void 
MainWindow::updatePenSize(){
  (*myViewer).myPenSize = ui->scaleSlider->value();
  stringstream s; s << ui->scaleSlider->value();
  ui->labelPenSize->setText(QString(s.str().c_str()));
}


MainWindow::~MainWindow()
{
  delete ui;
}

int main( int argc, char** argv )
{

  // parse command line CLI ----------------------------------------------
  CLI::App app;
  string inputFilename;
  string outputFilename {"out.off"};
  std::vector<unsigned int> colors;
  double scalePen {1.0};

  app.add_option("--input,-i,1", inputFilename, "input file: mesh (off,obj).")->required()->check(CLI::ExistingFile);
  app.add_option("--outputFile,-o,2", outputFilename, "save output file automatically according the file format extension.",true);
  auto scalePenOpt = app.add_option("--scalePen,-s", scalePen, "change the scale factor of the pen size (by default 1.0, real size: penSize*scale).", true);
  app.add_option("--penColor,-c", colors, "change the scale factor of the pen size (by default 1.0)");
  
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------
  
  Mesh<Z3i::RealPoint>  aMesh(true);
  aMesh << inputFilename;
  
  QApplication application(argc,argv);
 
  ViewerMesh<> *viewer = new ViewerMesh<> (aMesh, outputFilename );
  if (scalePenOpt->count()>0){
    viewer->myPenScale = scalePen;
  }
  if(scalePenOpt->count()>0){
    if (colors.size()!=4){
      trace.warning() << "you need to precise R,G,B, A values, taking default blue color..." << std::endl;
    }else{
      viewer->myPenColor = DGtal::Color(colors[0], colors[1], colors[2], colors[3]);
    }
  }
  
  MainWindow w(viewer, 0,0);
  w.setWindowTitle("Simple Mesh Edit");
  w.show();
  *viewer << aMesh;
  
  *viewer << Viewer3D<>::updateDisplay;
  return application.exec();
}


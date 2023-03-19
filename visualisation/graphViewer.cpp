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
 * @file graphViewer.cpp
 
 * @author Adrien Krähenbühl  (\c adrien.krahenbuhl@loria.fr)
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 *
 * @date 2016/02/29
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include <cstdlib>
#include <DGtal/io/viewers/Viewer3D.h>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/PointListReader.h>
#include <DGtal/io/readers/TableReader.h>
#include <DGtal/io/readers/MeshReader.h>
#include <DGtal/io/colormaps/HueShadeColorMap.h>

#include "CLI11.hpp"

/**
@code
Basic display graph.

Usage: ./graphViewer [OPTIONS]

Options:
  -h,--help                             Print this help message and exit
  -v,--inputVertex TEXT:FILE REQUIRED   input file containing the vertex list.
  -e,--inputEdge TEXT:FILE REQUIRED     input file containing the edge list.
  -a,--autoEdge                         generate edge list from vertex order.
  --cstSectionEdgeRad                   use a constant edge radius between two consecutive vertices.
  -r,--inputRadii TEXT                  input file containing the radius for each vertex.
  -b,--ballRadius FLOAT=1               radius of vertex balls.
  -m,--addMesh TEXT                     add mesh in the display.
  --meshColor UINT ...                  specify the color of mesh.
  --vertexColor UINT ...                specify the color of vertex.
  -s,--scaleRadius FLOAT=1              apply a scale factors on the radius input values
  --edgeColor UINT ...                  specify the color of edges.
  -c,--colormap                         display vertex colored by order in vertex file or by radius scale if the radius file is specidfied (-r).
  -d,--doSnapShotAndExit TEXT           save display snapshot into file. Notes that the camera setting is set by default according the last saved configuration (use SHIFT+Key_M to save current camera setting in the Viewer3D). If the camera setting was not saved it will use the default camera setting.

*/

template < typename Space = DGtal::Z3i::Space, typename KSpace = DGtal::Z3i::KSpace>
struct ViewerSnap: DGtal::Viewer3D <Space, KSpace>
{

  ViewerSnap(bool saveSnap): DGtal::Viewer3D<Space, KSpace>(), mySaveSnap(saveSnap){
  };

  virtual  void
  init(){
    DGtal::Viewer3D<>::init();
    if(mySaveSnap){
      QObject::connect(this, SIGNAL(drawFinished(bool)), this, SLOT(saveSnapshot(bool)));
    }
  };
  bool mySaveSnap;
};

using namespace std;
using namespace DGtal;

int main( int argc, char** argv )
{ 
  QApplication application(argc,argv);

  typedef ViewerSnap<> Viewer;

  // parse command line CLI-------------------------------------------------------
  CLI::App app;
  app.description("Basic display graph.\n");
  std::string nameFileVertex;  
  std::string nameFileEdge;
  double r {1.0};
  double scaleRadius {1.0};
  bool useRadiiFile {false};
  bool autoEdgeOpt {false};
  bool cstSectionEdgeRad {false};
  std::vector<unsigned int> vectColMesh;
  std::vector<unsigned int> vectColVertex;
  std::vector<unsigned int> vectColEdge;
  std::string nameFileRadii;
  std::string meshName;
  std::string name;

  app.add_option("--inputVertex,-v", nameFileVertex, "input file containing the vertex list.")->required()->check(CLI::ExistingFile);
  app.add_option("--inputEdge,-e", nameFileEdge, "input file containing the edge list.")->required()->check(CLI::ExistingFile);
  app.add_flag("--autoEdge,-a", autoEdgeOpt, "generate edge list from vertex order.");
  app.add_flag("--cstSectionEdgeRad",cstSectionEdgeRad,  "use a constant edge radius between two consecutive vertices.");
  auto inputRadiiOpt = app.add_option("--inputRadii,-r", nameFileRadii, "input file containing the radius for each vertex.");
  app.add_option("--ballRadius,-b", r, "radius of vertex balls.", true);
  auto addMeshOpt = app.add_option("--addMesh,-m", meshName, "add mesh in the display.");
  auto meshColorOpt = app.add_option("--meshColor", vectColMesh, "specify the color of mesh.");
  auto vertexColorOpt = app.add_option("--vertexColor", vectColVertex, "specify the color of vertex.");
  app.add_option("--scaleRadius,-s", scaleRadius, "apply a scale factors on the radius input values", true);
  auto edgeColorOpt = app.add_option("--edgeColor", vectColEdge, "specify the color of edges.");
  auto colormapOpt = app.add_flag("--colormap, -c","display vertex colored by order in vertex file or by radius scale if the radius file is specidfied (-r).");
  auto doSnapShotAndExitOpt = app.add_option("--doSnapShotAndExit,-d", name, "save display snapshot into file. Notes that the camera setting is set by default according the last saved configuration (use SHIFT+Key_M to save current camera setting in the Viewer3D). If the camera setting was not saved it will use the default camera setting.");
  
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------
  
  Viewer viewer(doSnapShotAndExitOpt->count());
  viewer.show();	
  if(doSnapShotAndExitOpt->count()>0){
    viewer.setSnapshotFileName(QString(name.c_str()));
  }

  DGtal::Color meshColor(240,240,240);
  DGtal::Color edgeColor(240,240,240);
  DGtal::Color vertexColor(240,240,240);

  if(inputRadiiOpt->count() > 0)
    useRadiiFile = true;

  // Structures to store vertex and edges read in input files
  std::vector<Z3i::RealPoint> vectVertex = PointListReader<Z3i::RealPoint>::getPointsFromFile(nameFileVertex);
  std::vector<Z2i::Point> vectEdges;
  if(!autoEdgeOpt)
  {
    vectEdges =  PointListReader<Z2i::Point>::getPointsFromFile(nameFileEdge);
  }
  else
  {
    for(unsigned int i=0; i<vectVertex.size()-1; i++)
    {
      vectEdges.push_back(Z2i::Point(i,i+1));
    }
  }
  std::vector<double> vectRadii( std::max(vectVertex.size(),vectEdges.size()), r );

  // read the mesh and ege colors
  if(meshColorOpt->count()>0){
    if(vectColMesh.size()!=4 ){
      trace.error() << "The color specification should contain R,G,B and Alpha values."<< std::endl;
    }
    meshColor.setRGBi(vectColMesh[0],vectColMesh[1],vectColMesh[2],vectColMesh[3]);
  }
  if(edgeColorOpt->count()>0){
    if( vectColEdge.size()!=4 ){
      trace.error() << "The color specification should contain R,G,B and Alpha values."<< std::endl;
    }
    edgeColor.setRGBi(vectColEdge[0],vectColEdge[1],vectColEdge[2],vectColEdge[3]);
  }
  if(vertexColorOpt->count()>0)
  {
    if( vectColVertex.size()!=4 )
    {
      trace.error() << "The color specification should contain R,G,B and Alpha values."<< std::endl;
    }
    vertexColor.setRGBi(vectColVertex[0],vectColVertex[1],vectColVertex[2],vectColVertex[3]);
  }
  
  // Create the color scale dpending on the specified radius file
  HueShadeColorMap<int> hueShade(0,vectVertex.size()-1);
  if ( useRadiiFile )
  {
    vectRadii = TableReader<double>::getColumnElementsFromFile(nameFileRadii,0);
    if ( vectRadii.size() != vectVertex.size() )
    {
      trace.info() << "Error: number of vertex ("
                   << vectVertex.size() << ") is not equal to the number of radii ("
                   << vectRadii.size() << ")." << std::endl;
      return 1;
    }
    hueShade = HueShadeColorMap<int>((*(std::min_element(vectRadii.begin(),
                                                         vectRadii.end())))*10000,
                                     (*(std::max_element(vectRadii.begin(),
                                                         vectRadii.end())))*10000);
    if (scaleRadius != 1.0){
      for(auto &v: vectRadii){
        v *= scaleRadius; 
      }
    }
  }


  // Add vertex to viewer as balls
  if ( colormapOpt->count() > 0)
  {
    Color currentColor;
    for ( int i=0 ; i<vectVertex.size() ; ++i )
    {
      
      currentColor = (useRadiiFile ? hueShade(vectRadii[i]*10000) : hueShade(i));
      viewer << CustomColors3D( currentColor, currentColor );
      viewer.addBall(vectVertex[i], vectRadii[i]);
    }
  }
  else
  {
    if(vertexColorOpt->count() > 0)
    {
        viewer << CustomColors3D( vertexColor, vertexColor );
    }
    
     for ( int i=0 ; i<vectVertex.size() ; ++i )
    {
      viewer.addBall(vectVertex[i], vectRadii[i]);
    }
  }

  // Add edges to viewer as lines
  std::vector<Z3i::RealPoint> vertex;
  std::vector<double> radii;

  if ( colormapOpt->count() > 0 )
  {
    for ( const auto& e: vectEdges )
    {
      Mesh<Z3i::RealPoint> aMesh;
      vertex = { vectVertex[e[0]], vectVertex[e[1]] };
      if (cstSectionEdgeRad) {
        radii = { std::min(vectRadii[e[0]], vectRadii[e[1]]), std::min(vectRadii[e[0]], vectRadii[e[1]]) };  
      }
      else {
        radii = { vectRadii[e[0]], vectRadii[e[1]] };
      }
      Mesh<Z3i::RealPoint>::createTubularMesh(aMesh, vertex, radii, 0.05);
      viewer << (useRadiiFile ? CustomColors3D( hueShade(radii[0]*10000),
                                                hueShade(radii[1]*10000) ) : CustomColors3D( hueShade(e[0]),
                                                                                             hueShade(e[1]) ));
      viewer << aMesh;
    }
  }
  else
  {
    Mesh<Z3i::RealPoint> aMesh;
    for ( const auto& e: vectEdges )
    {
      vertex = { vectVertex[e[0]], vectVertex[e[1]] };
      if (cstSectionEdgeRad) {
        radii = { std::min(vectRadii[e[0]], vectRadii[e[1]]), std::min(vectRadii[e[0]], vectRadii[e[1]]) };  
        }
        else {
          radii = { vectRadii[e[0]], vectRadii[e[1]] };
        }
      Mesh<Z3i::RealPoint>::createTubularMesh(aMesh, vertex, radii, 0.05);
    }
    viewer << CustomColors3D(DGtal::Color::Black, edgeColor);
    viewer << aMesh;
  }


  if(addMeshOpt->count() > 0){
    Mesh<Z3i::RealPoint> mesh;
    mesh << meshName ;
    viewer << CustomColors3D(DGtal::Color::Black, meshColor);
    viewer << mesh;
  }


  viewer << Viewer3D<>::updateDisplay;
  
 if(doSnapShotAndExitOpt->count() > 0){
    // Appy cleaning just save the last snap
    DGtal::trace.info() << "sorting surfel according camera position....";
    viewer.sortSurfelFromCamera();
    viewer.sortQuadFromCamera();
    viewer.sortTriangleFromCamera();
    viewer.updateList(false);    
    if(!viewer.restoreStateFromFile())
      {
        viewer.update();
      }    
    std::string extension = name.substr(name.find_last_of(".") + 1);
    std::string basename = name.substr(0, name.find_last_of("."));
    for(int i=0; i< viewer.snapshotCounter()-1; i++){
      std::stringstream s;
      s << basename << "-"<< setfill('0') << setw(4)<<  i << "." << extension;
      trace.info() << "erase temp file: " << s.str() << std::endl;
      remove(s.str().c_str());
    }
    std::stringstream s;
    s << basename << "-"<< setfill('0') << setw(4)<<  viewer.snapshotCounter()-1 << "." << extension;
    rename(s.str().c_str(), name.c_str());
    return 0;
  }


  return application.exec();
}
















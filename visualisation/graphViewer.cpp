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

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


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
namespace po = boost::program_options;

int main( int argc, char** argv )
{ 
  QApplication application(argc,argv);

  typedef ViewerSnap<> Viewer;
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
    ("help,h", "display this message")
    ("inputVertex,v", po::value<std::string>(), "input file containing the vertex list.")
    ("inputEdge,e", po::value<std::string>(), "input file containing the edge list.")
    ("autoEdge,a", "generate edge list from vertex order.")
    ("inputRadii,r", po::value<std::string>(), "input file containing the radius for each vertex.")
    ("ballRadius,b", po::value<double>()->default_value(1.0), "radius of vertex balls.")
    ("addMesh,m", po::value<std::string>(), "add mesh in the display.")
    ("meshColor", po::value<std::vector<unsigned int> >()->multitoken(), "specify the color mesh.")
    ("vertexColor", po::value<std::vector<unsigned int> >()->multitoken(), "specify the color of vertex.")
    ("edgeColor", po::value<std::vector<unsigned int> >()->multitoken(), "specify the color of edges.")
    ("colormap,c", "display vertex colored by order in vertex file or by radius scale if the radius file is specidfied (-r).")
    ("doSnapShotAndExit,d", po::value<std::string>(), "save display snapshot into file. Notes that the camera setting is set by default according the last saved configuration (use SHIFT+Key_M to save current camera setting in the Viewer3D). If the camera setting was not saved it will use the default camera setting." );

  bool parseOK = true;

  // Process options in arguments
  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
  }
  catch( const std::exception& ex )
  {
    trace.info() << "Error checking program options: " << ex.what() << std::endl;
    parseOK = false;
  }
  po::notify(vm);
  if( !parseOK || argc<=1 || vm.count("help") || !vm.count("inputVertex")  || (!vm.count("inputEdge")&& !vm.count("autoEdge") ))
  {
    trace.info() << "Basic display graph" << std::endl
                 << "Options:" << std::endl
		             << general_opt << std::endl;
    return 1;
  }

  Viewer viewer(vm.count("doSnapShotAndExit"));
  viewer.show();	
  if(vm.count("doSnapShotAndExit")){
    viewer.setSnapshotFileName(QString(vm["doSnapShotAndExit"].as<std::string>().c_str()));
  }

  DGtal::Color meshColor(240,240,240);
  DGtal::Color edgeColor(240,240,240);
  DGtal::Color vertexColor(240,240,240);

  std::string nameFileVertex = vm["inputVertex"].as<std::string>();

  double r = vm["ballRadius"].as<double>();
  bool useRadiiFile = vm.count("inputRadii");

  // Structures to store vertex and edges read in input files
  std::vector<Z3i::RealPoint> vectVertex = PointListReader<Z3i::RealPoint>::getPointsFromFile(nameFileVertex);
  std::vector<Z2i::Point> vectEdges;
  if(!vm.count("autoEdge"))
  {
    std::string nameFileEdge = vm["inputEdge"].as<std::string>();
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
  if(vm.count("meshColor")){
    std::vector<unsigned int> vectCol = vm["meshColor"].as<std::vector<unsigned int> >();
    if(vectCol.size()!=4 ){
      trace.error() << "The color specification should contain R,G,B and Alpha values."<< std::endl;
    }
    meshColor.setRGBi(vectCol[0],vectCol[1],vectCol[2],vectCol[3]);
  }
  if(vm.count("edgeColor")){
    std::vector<unsigned int> vectCol = vm["edgeColor"].as<std::vector<unsigned int> >();
    if( vectCol.size()!=4 ){
      trace.error() << "The color specification should contain R,G,B and Alpha values."<< std::endl;
    }
    edgeColor.setRGBi(vectCol[0],vectCol[1],vectCol[2],vectCol[3]);
  }
  if(vm.count("vertexColor"))
  {
    std::vector<unsigned int> vectCol = vm["vertexColor"].as<std::vector<unsigned int> >();
    if( vectCol.size()!=4 )
    {
      trace.error() << "The color specification should contain R,G,B and Alpha values."<< std::endl;
    }
    vertexColor.setRGBi(vectCol[0],vectCol[1],vectCol[2],vectCol[3]);
  }
  
  // Create the color scale dpending on the specified radius file
  HueShadeColorMap<int> hueShade(0,vectVertex.size()-1);
  if ( useRadiiFile )
  {
    std::string nameFileRadii = vm["inputRadii"].as<std::string>();
    vectRadii = TableReader<double>::getColumnElementsFromFile(nameFileRadii,0);
    if ( vectRadii.size() != vectVertex.size() )
    {
      trace.info() << "Error: number of vertex ("
                   << vectVertex.size() << ") is not equal to the number of radii ("
                   << vectRadii.size() << ")." << std::endl;
      return 1;
    }
    hueShade = HueShadeColorMap<int>((*(std::min_element(vectRadii.begin(),vectRadii.end())))*10000,(*(std::max_element(vectRadii.begin(),vectRadii.end())))*10000);
  }


  // Add vertex to viewer as balls
  if ( vm.count("colormap") )
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
    if(vm.count("vertexColor"))
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

  if ( vm.count("colormap") )
  {
    for ( const auto& e: vectEdges )
    {
      Mesh<Z3i::RealPoint> aMesh;
      vertex = { vectVertex[e[0]], vectVertex[e[1]] };
      radii = { vectRadii[e[0]], vectRadii[e[1]] };
      Mesh<Z3i::RealPoint>::createTubularMesh(aMesh, vertex, radii, 0.05);
      viewer << (useRadiiFile ? CustomColors3D( hueShade(radii[0]*10000), hueShade(radii[1]*10000) ) : CustomColors3D( hueShade(e[0]), hueShade(e[1]) ));
      viewer << aMesh;
    }
  }
  else
  {
    Mesh<Z3i::RealPoint> aMesh;
    for ( const auto& e: vectEdges )
    {
      vertex = { vectVertex[e[0]], vectVertex[e[1]] };
      radii = { vectRadii[e[0]], vectRadii[e[1]] };
      Mesh<Z3i::RealPoint>::createTubularMesh(aMesh, vertex, radii, 0.05);
    }
    viewer << CustomColors3D(DGtal::Color::Black, edgeColor);
    viewer << aMesh;
  }


  if(vm.count("addMesh")){
    std::string meshName = vm["addMesh"].as<std::string>();
    Mesh<Z3i::RealPoint> mesh;
    mesh << meshName ;
    viewer << CustomColors3D(DGtal::Color::Black, meshColor);
    viewer << mesh;
  }


  viewer << Viewer3D<>::updateDisplay;
  
  if(vm.count("doSnapShotAndExit")){
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
    std::string name = vm["doSnapShotAndExit"].as<std::string>();
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
















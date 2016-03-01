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
 
 * @author Bertrand Kerautret (\c kerautre@loria.fr)
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
#include <DGtal/geometry/surfaces/COBANaivePlaneComputer.h>
#include <DGtal/io/readers/PointListReader.h>
#include <DGtal/io/readers/TableReader.h>

// #include <DGtal/io/writers/MeshWriter.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


using namespace DGtal;
namespace po = boost::program_options;

int main( int argc, char** argv )
{
 
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
  ("help,h", "display this message")
  ("inputEdge,e", po::value<std::string>(), "input file containing the edge list.")
  ("inputVertex,v", po::value<std::string>(), "input file containing the vertex list.")
  ("inputRadii,r", po::value<std::string>(), "input file containing the radius for each edge.")
  ("ballRadius,b", po::value<double>()->default_value(2.0), "radius of edge balls.");

  bool parseOK = true;

  // Process options in arguments
  QApplication application(argc,argv);
  po::variables_map vm;
  try{
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
  }catch(const std::exception& ex){
    trace.info()<< "Error checking program options: "<< ex.what()<< std::endl;
    parseOK=false;
  }
  po::notify(vm);
  if( !parseOK || argc<=1 || vm.count("help") || !vm.count("inputEdge")  || !vm.count("inputVertex") )
  {
    trace.info()<< "Basic display graph" <<std::endl << "Options: "<<std::endl
		  << general_opt << "\n";
    return 1;
  }

  bool useRadiiFile = vm.count("inputRadii");

  std::string nameFileEdge = vm["inputEdge"].as<std::string>();
  std::string nameFileVertex = vm["inputVertex"].as<std::string>();
  double r = vm["ballRadius"].as<double>();

  // Structurex to store edges and vertex read in input files
  std::vector<Z3i::RealPoint> vectEdges = PointListReader<Z3i::RealPoint>::getPointsFromFile(nameFileEdge);
  std::vector<Z2i::Point> vectVertex = PointListReader<Z2i::Point>::getPointsFromFile(nameFileVertex);
  std::vector<double> vectRadii( vectEdges.size(), r );

  if ( useRadiiFile )
  {
    std::string nameFileRadii = vm["inputRadii"].as<std::string>();
    vectRadii = TableReader<double>::getColumnElementsFromFile(nameFileRadii,0);
    if ( vectRadii.size() != vectEdges.size() )
    {
      trace.info() << "Error: number of edges ("
                   << vectEdges.size() << ") is not equal to the number of radii ("
                   << vectRadii.size() << ")." << std::endl;
      return 1;
    }
  }

  typedef Viewer3D<> MyViewer;
  MyViewer viewer;

  // Add edges to viewer as balls
  viewer << CustomColors3D( Color::Blue, Color::Blue );
  for ( int i=0 ; i<vectEdges.size() ; ++i )
  {
    viewer.addBall(vectEdges[i], vectRadii[i]);
  }

  // Add vertex to viewer as lines
  viewer << CustomColors3D( Color::Blue,  Color::Blue );
  Mesh<Z3i::RealPoint> aMesh;
  for ( const auto& v: vectVertex )
  {
    const Z3i::RealPoint& e1 = vectEdges[v[0]];
    const Z3i::RealPoint& e2 = vectEdges[v[1]];
    viewer.addLine(e1, e2, 10);
    std::vector<Z3i::RealPoint> vertex = { e1, e2 };
    std::vector<double> radii = { vectRadii[v[0]], vectRadii[v[1]] };
    Mesh<Z3i::RealPoint>::createTubularMesh(aMesh, vertex, radii, 0.1, DGtal::Color::Green);
  }

  viewer << aMesh;

  // aMesh >> "toto.off";
  
  viewer << MyViewer::updateDisplay;
  viewer.show();

  return application.exec();
}
















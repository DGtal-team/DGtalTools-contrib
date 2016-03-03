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
#include <DGtal/io/colormaps/HueShadeColorMap.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


using namespace DGtal;
namespace po = boost::program_options;

int main( int argc, char** argv )
{ 
  QApplication application(argc,argv);

  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
    ("help,h", "display this message")
    ("inputVertex,v", po::value<std::string>(), "input file containing the vertex list.")
    ("inputEdge,e", po::value<std::string>(), "input file containing the edge list.")
    ("inputRadii,r", po::value<std::string>(), "input file containing the radius for each vertex.")
    ("ballRadius,b", po::value<double>()->default_value(1.0), "radius of vertex balls.")
    ("colormap,c", "display vertex colored by order in file.");

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
  if( !parseOK || argc<=1 || vm.count("help") || !vm.count("inputVertex")  || !vm.count("inputEdge") )
  {
    trace.info() << "Basic display graph" << std::endl
                 << "Options:" << std::endl
		             << general_opt << std::endl;
    return 1;
  }

  std::string nameFileVertex = vm["inputVertex"].as<std::string>();
  std::string nameFileEdge = vm["inputEdge"].as<std::string>();
  double r = vm["ballRadius"].as<double>();
  bool useRadiiFile = vm.count("inputRadii");

  // Structurex to store vertex and edges read in input files
  std::vector<Z3i::RealPoint> vectVertex = PointListReader<Z3i::RealPoint>::getPointsFromFile(nameFileVertex);
  std::vector<Z2i::Point> vectEdges = PointListReader<Z2i::Point>::getPointsFromFile(nameFileEdge);
  std::vector<double> vectRadii( vectVertex.size(), r );

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
  }

  typedef Viewer3D<> MyViewer;
  MyViewer viewer;


  // Add vertex to viewer as balls
  if ( vm.count("colormap") )
  {
    HueShadeColorMap<int> hueShade(0,vectVertex.size());
    Color currentColor;
    for ( int i=0 ; i<vectVertex.size() ; ++i )
    {
      currentColor = hueShade(i);
      viewer << CustomColors3D( currentColor, currentColor );
      viewer.addBall(vectVertex[i], vectRadii[i]);
    }
  }
  else
  {
    viewer << CustomColors3D( Color::Blue, Color::Blue );
     for ( int i=0 ; i<vectVertex.size() ; ++i )
    {
      viewer.addBall(vectVertex[i], vectRadii[i]);
    }
  }

  // Add edges to viewer as lines
  Mesh<Z3i::RealPoint> aMesh;
  std::vector<Z3i::RealPoint> vertex;
  std::vector<double> radii;
  for ( const auto& e: vectEdges )
  {
    vertex = { vectVertex[e[0]], vectVertex[e[1]] };
    radii = { vectRadii[e[0]], vectRadii[e[1]] };
    Mesh<Z3i::RealPoint>::createTubularMesh(aMesh, vertex, radii, 0.1);
  }

  viewer << aMesh;
  viewer << MyViewer::updateDisplay;
  viewer.show();

  return application.exec();
}
















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
 * @file
 * @ingroup geometry3d
 * @author Bertrand Kerautret (\c bertrand.kerautret@univ-lyon2.fr )
 * 
 *
 * @date 2023/04/20
 *
 * Source file of the tool graph2vol
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include <DGtal/io/readers/TableReader.h>
#include <DGtal/io/readers/PointListReader.h>
#include "DGtal/io/writers/GenericWriter.h"

#include "CLI11.hpp"

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page graph2vol graph2vol
 
 @brief  Description of the tool...

 @b Usage:   graph2vol [input]

 @b Allowed @b options @b are :
 
 @code
  -h [ --help ]           display this message
  -i [ --input ] arg      an input file...
  -p [ --parameter] arg   a double parameter...
 @endcode

 @b Example:

 @code
     graph2vol -i  $DGtal/examples/samples/....
 @endcode

 @image html resgraph2vol.png "Example of result. "

 @see
 @ref graph2vol.cpp

 */

typedef ImageContainerBySTLVector<Z3i::Domain, unsigned char> Image3D;


template<typename TPoint, typename TPointD>
inline
bool
projectOnStraightLine(const TPoint & ptA,
                      const TPoint & ptB,
                      const TPoint & ptC,
                      TPointD & ptProjected)
{
    if (ptA==ptC)
    {
        for(auto i=0; i<TPoint::dimension; i++){ ptProjected[i]=ptA[i];}
        return true;
    }
    if (ptB==ptC)
    {
        for(auto i=0; i<TPoint::dimension; i++){ ptProjected[i]=ptB[i];}
        return true ;
    }
    
    TPointD vAB = ptB - ptA;
    TPointD vABn = vAB / vAB.norm();
      
    TPointD vAC = ptC-ptA;
    double distPtA_Proj = vAC.dot(vABn);
    
    for(auto i=0; i<TPoint::dimension; i++){ ptProjected[i]= ptA[i]+vABn[i]*(distPtA_Proj);}
    bool res = false;
    for(auto i=0; i<TPoint::dimension; i++) { res = res || (ptA[i]<ptB[i] && ptProjected[i]<=ptB[i]); }
    return distPtA_Proj>=0 && res;

}



int main( int argc, char** argv )
{
  double gridSize = 1.0;
  int brdVol = 5;
  std::string nameFileVertex;
  std::string nameFileEdge;
  std::string nameFileRadii;

  std::string outputFileName;
  std::stringstream usage;
  usage << "Usage: " << argv[0] << " [input]\n"
        << "Typical use example:\n \t graph2vol -i ... \n";
  // parse command line using CLI-------------------------------------------------------
  CLI::App app;
  app.description("Your program description.\n" + usage.str() );
  app.add_option("--inputVertex,-v", nameFileVertex, "input file containing the vertex list.")->required()->check(CLI::ExistingFile);
  app.add_option("--inputEdge,-e", nameFileEdge, "input file containing the edge list.")->required()->check(CLI::ExistingFile);
  app.add_option("--inputRadii,-r", nameFileRadii, "input file containing the radius for each vertex.")
    ->required()->check(CLI::ExistingFile);
  app.add_option("--output,-o", outputFileName, "Output volumic filename")->required();

    
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  
  // Some nice processing  --------------------------------------------------
  
  // Structures to store vertex and edges read in input files
  std::vector<Z3i::RealPoint> vectVertex = PointListReader<Z3i::RealPoint>::getPointsFromFile(nameFileVertex);
  std::vector<Z2i::Point> vectEdges =  PointListReader<Z2i::Point>::getPointsFromFile(nameFileEdge);
  std::vector<double> vectRadii = TableReader<double>::getColumnElementsFromFile(nameFileRadii,0);
    
  trace.info() << "Starting " << argv[0]  <<std::endl;
  trace.info() << "Nb vertex read : " << vectVertex.size() << std::endl;
  trace.info() << "Nb edges read : " << vectEdges.size() << std::endl;
  trace.info() << "Nb radius read : " << vectRadii.size() << std::endl;

  // Compute the domain associated to the set vertices
  Z3i::Point lb = Z3i::Point(static_cast<int>(vectVertex.front()[0]),
                             static_cast<int>(vectVertex.front()[1]),
                             static_cast<int>(vectVertex.front()[2]));
  Z3i::Point ub = lb;

  for ( auto const &v: vectVertex ) {
        for(int i=0; i< 3; i++){
            if (v[i] < lb[i]) {
                lb[i] = static_cast<int>(v[i]);
            }
            if (v[i] > ub[i]) {
                ub[i] = static_cast<int>(v[i]);
            }
        }
    }
  trace.info() << "Bouding box found " << lb << " " << ub << std::endl;
  Z3i::Domain dom (lb - Z3i::Point::diagonal()*brdVol,
                   ub + Z3i::Point::diagonal()*brdVol);
  Image3D res (dom);
    
  for (auto const &p: dom){
      // Filling from cylinders
      for(auto const e: vectEdges){
            Z3i::RealPoint p0 = vectVertex[e[0]];
            Z3i::RealPoint p1 = vectVertex[e[1]];
            Z3i::RealPoint pr (p[0], p[1], p[2]);
            double r = vectRadii[e[0]];
            Z3i::RealPoint pprof;
            auto isProj = projectOnStraightLine(p0, p1, pr, pprof);
          if (isProj && (pr-pprof).norm() < r ) {
              res.setValue(p, 255);
          }
        }
    }
    
  res >> outputFileName;
  return 0;
}



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
 -h,--help                             Print this help message and exit
  -v,--inputVertex TEXT:FILE REQUIRED   input file containing the vertex list.
  -e,--inputEdge TEXT:FILE REQUIRED     input file containing the edge list.
  -r,--inputRadii TEXT:FILE REQUIRED    input file containing the radius for each vertex.
  -g,--gridSize FLOAT                   grid size.
  -o,--output TEXT REQUIRED             Output volumic filename
  --interpolRadius                      Interpolation of radius
 @endcode

 @b Example:

 @code
     graph2vol  -v res_vertex.dat -e res_edges.dat -r res_radius.dat
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
    TPointD vPA = ptA - ptProjected;
    TPointD vPB = ptB - ptProjected;
    return vPB.dot(vPA) <= 0 ;
}


void
compBB(std::vector<Z3i::RealPoint> vpts,
       Z3i::Point &lb, Z3i::Point &ub){
    for ( auto const &v: vpts ) {
        for(int i=0; i< 3; i++){
            if (v[i] < lb[i]) {
                lb[i] = static_cast<int>(v[i]);
            }
            if (v[i] > ub[i]) {
                ub[i] = static_cast<int>(v[i]);
            }
        }
    }
}

int main( int argc, char** argv )
{
  double gridSize = 1.0;
  bool interpolRadius = false;
  int brdVol = 2;
  std::string nameFileVertex;
  std::string nameFileEdge;
  std::string nameFileRadii;

  std::string outputFileName;
  std::stringstream usage;
  usage << "Usage: " << argv[0] << " [input]\n"
        << "Typical use example:\n \t graph2vol -v res_vertex.dat -e res_edges.dat -r res_radius.dat \n";
  // parse command line using CLI-------------------------------------------------------
  CLI::App app;
  app.description("Your program description.\n" + usage.str() );
  app.add_option("--inputVertex,-v", nameFileVertex, "input file containing the vertex list.")->required()->check(CLI::ExistingFile);
  app.add_option("--inputEdge,-e", nameFileEdge, "input file containing the edge list.")->required()->check(CLI::ExistingFile);
  app.add_option("--inputRadii,-r", nameFileRadii, "input file containing the radius for each vertex.")
    ->required()->check(CLI::ExistingFile);
  app.add_option("--gridSize,-g", gridSize, "grid size.");
  app.add_option("--output,-o", outputFileName, "Output volumic filename")->required();
  app.add_flag("--interpolRadius", interpolRadius, "Interpolation of radius");
    
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
    for ( auto  &v: vectVertex ) {
        for(int i=0; i< 3; i++) v[i] *= gridSize;
    }
    for ( auto  &v: vectRadii ) {
        v *= gridSize;
    }
  brdVol *= gridSize;
  compBB(vectVertex, lb, ub);
  trace.info() << "Bouding box found " << lb << " " << ub << std::endl;
  Z3i::Domain dom (lb - Z3i::Point::diagonal()*brdVol,
                   ub + Z3i::Point::diagonal()*brdVol);
    double rMax = *std::max_element(vectRadii.begin(), vectRadii.end());
    
  Image3D res (dom);
    
    for(auto const e: vectEdges){
        Z3i::RealPoint p0 = vectVertex[e[0]];
        Z3i::RealPoint p1 = vectVertex[e[1]];
        Z3i::Point pl, pu;
        compBB({p0, p1}, pl, pu);
        Z3i::Domain subDom (pl - Z3i::Point::diagonal()*brdVol,
                            pu + Z3i::Point::diagonal()*brdVol);
        for (const auto &p: subDom){
            Z3i::RealPoint pr (p[0], p[1], p[2]);
            double r = std::min(vectRadii[e[0]], vectRadii[e[1]]);
            double r0 =vectRadii[e[0]];
            double r1 =vectRadii[e[1]];
            double diffr = r1 - r0;
            Z3i::RealPoint pprof;
            auto isProj = projectOnStraightLine(p0, p1, pr, pprof);
            double dist0 = (pprof-p0).norm()/(p0-p1).norm();
            double rl = r0+diffr*dist0;
            if (interpolRadius){
                if (isProj && (pr-pprof).norm() <= rl ) {
                    res.setValue(p, (rl/rMax)*255);
                }
            }else{
                if (isProj && (pr-pprof).norm() <= r ) {
                    res.setValue(p, (r/rMax)*255);
                }
            }
        }
    }
    
    

  res >> outputFileName;
  return 0;
}



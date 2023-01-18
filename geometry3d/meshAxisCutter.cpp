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
 * @date 2023/01/18
 *
 * Source file of the tool meshAxisCutter
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/writers/MeshWriter.h"
#include "DGtal/shapes/Mesh.h"


#include "CLI11.hpp"

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page meshAxisCutter meshAxisCutter
 
 @brief  Cut the input mesh according one axis into sereral separate meshes.

 Usage: ./geometry3d/meshAxisCutter [OPTIONS] 1 2 [3]

 Positionals:
   1 TEXT:FILE REQUIRED                  Input file
   2 TEXT REQUIRED                       Output filename base
   3 UINT=2                              the number of parts
 @b Allowed @b options @b are :
 @code

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         Input file
   -o,--output TEXT REQUIRED             Output filename base
   -p,--nbParts UINT=2                   the number of parts
   -a,--axis UINT=3                      the axis to cut the mesh

 
  -h [ --help ]           display this message
  -i [ --input ] arg      an input file...
  -p [ --parameter] arg   a double parameter...
 @endcode

 @b Example:

 @code
     meshAxisCutter - $DGtal/examples/samples/bimbaPoly.obj result  5
 @endcode


 @see
 @ref meshAxisCutter.cpp

 */


int main( int argc, char** argv )
{

  unsigned int nbP {2};
  unsigned int axis {2};
  std::string inputFileName;
  std::string outputFileName;
  std::stringstream usage;
  usage << "Usage: " << argv[0] << " [input]\n"
        << "Typical use example:\n \t meshAxisCutter  $DGtal/examples/samples/bimbaPoly.obj result  5 \n";
  // parse command line using CLI-------------------------------------------------------
  CLI::App app;
  app.description("Cut the input mesh according one axis into sereral separate meshes.\n" + usage.str() );
  app.add_option("--input,-i,1", inputFileName, "Input file")->required()->check(CLI::ExistingFile);
  app.add_option("--output,-o,2", outputFileName, "Output filename base")->required();
  app.add_option("--nbParts,-p,3", nbP, "the number of parts", true);
  app.add_option("--axis,-a", axis, "the axis to cut the mesh", true);

  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  // Some nice processing  --------------------------------------------------
  Mesh<Z3i::RealPoint> inputMesh (true);
  inputMesh << inputFileName;
  std::string ext = inputFileName.substr(inputFileName.find_last_of(".") + 1);

  // Perform cutting
  auto bb = inputMesh.getBoundingBox();
  double minZ = bb.first[axis];
  double maxZ = bb.second[axis];
  double rangeZ = maxZ - minZ;
  double sliceHeight = rangeZ / nbP;

  vector<Mesh<PointVector<3,double> > > meshParts;
  // including input vertex for eacg mesh parts
  for (unsigned int i=0; i<=nbP; i++){
      meshParts.push_back(Mesh<PointVector<3,double>>(true));
      for (unsigned int j = 0; j< inputMesh.nbVertex(); j++){
          meshParts[i].addVertex(inputMesh.getVertex(j));
      }
  }
  // filtering each faces
  for (unsigned int i = 0; i< inputMesh.nbFaces(); i++){
       auto fc = inputMesh.getFaceBarycenter(i);
       unsigned int indexP = floor((fc[axis]-minZ)/sliceHeight);
      meshParts[indexP].addFace(inputMesh.getFace(i), inputMesh.getFaceColor(i));
      
  }
   
  // Write output meshes
  for(int i = 0; i <=nbP; i++) {
      stringstream ss;
      ss << outputFileName << "_" << i << "." << ext;
      string outputFile = ss.str();
      meshParts[i] >> outputFile;
   }

  return 0;
}



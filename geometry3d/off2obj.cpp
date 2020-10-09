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
 * @file off2obj.cpp
 * @ingroup geometry3d
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Lorraine, France
 *
 * @date 2017/04/04
 *
 * Source file of the tool off2obj
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>

#include "DGtal/base/Common.h"
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/writers/MeshWriter.h"
#include "DGtal/helpers/StdDefs.h"

#include "CLI11.hpp"


///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page off2obj off2obj
 
 @brief  Description of the tool...

 @b Usage:   off2obj [input]

 @b Allowed @b options @b are :

 @code
  
 Positionals:
   1 TEXT:FILE REQUIRED                  an input mesh file in .off format.

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         an input mesh file in .off format.
   -o,--output TEXT                      an output file (can generate .obj and .mtl if color option is selected)
   -n,--invertNormals BOOLEAN            invert the normals (face orientation).
   -c,--colors                           convert by taking into account the mesh colors (from each faces).

 
 @endcode

 @b Example: 

 @code
 off2obj $DGtal/examples/samples/tref.off converted.obj
 @endcode

 @see
 @ref off2obj.cpp

 */


int main( int argc, char** argv )
{
  
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  std::string inputFileName;
  std::string outputFileName {"result.obj"};
  bool invertNormals {false};
  bool colors {false};
  
  app.description("Converts a .off mesh into the .obj format.\n"
                  "Typical use example:\n \t off2obj -i file.off -o file.obj -c  \n");
  app.add_option("-i,--input,1", inputFileName, "an input mesh file in .off format." )
  ->required()
  ->check(CLI::ExistingFile);
  app.add_option("--output,-o,2", outputFileName, "an output file (can generate .obj and .mtl if color option is selected)");
  app.add_option("--invertNormals,-n", invertNormals, "invert the normals (face orientation).");
  app.add_flag("--colors,-c",colors, "convert by taking into account the mesh colors (from each faces).");

 
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  std::string basename = outputFileName.substr(0, outputFileName.find_last_of("."));
  std::stringstream outname; outname << basename << ".obj"; 
  // read input mesh
  DGtal::Mesh<DGtal::Z3i::RealPoint> aMesh(colors);

  MeshReader<DGtal::Z3i::RealPoint>::importOFFFile(inputFileName,
                                                   aMesh, invertNormals);
  
  ofstream fout;
  fout.open(outname.str().c_str());
  if(!colors)
    {
      MeshWriter<DGtal::Z3i::RealPoint>::export2OBJ(fout,aMesh);
    }
  else
    {
      ofstream foutmtl;
      std::stringstream outnamemtl; outnamemtl << basename << ".mtl";
      foutmtl.open(outnamemtl.str().c_str());
      MeshWriter<DGtal::Z3i::RealPoint>::export2OBJ_colors(fout, foutmtl, outnamemtl.str(),aMesh);
    }
  return 0;
}


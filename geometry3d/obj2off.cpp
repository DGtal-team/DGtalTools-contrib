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
 * @file obj2off.cpp
 * @ingroup geometry3d
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Lorraine, France
 *
 * @date 2017/04/04
 *
 * Source file of the tool obj2off
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
 @page obj2off obj2off
 
 @brief   Converts a .obj mesh into the .off format.

 @code
 Typical use example:
     obj2off -i file.obj -o file.off

 Usage: ./geometry3d/obj2off [OPTIONS] 1 [2]

 Positionals:
   1 TEXT:FILE REQUIRED                  an input mesh file in .obj format.
   2 TEXT                                an output file

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         an input mesh file in .obj format.
   -o,--output TEXT                      an output file

 
 @endcode

 @b Example: 

 @code
 obj2off $DGtal/examples/samples/spot.obj converted.off
 @endcode

 @see
 @ref obj2off.cpp

 */


int main( int argc, char** argv )
{
  
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  std::string inputFileName;
  std::string outputFileName {"result.off"};
  
  app.description("Converts a .obj mesh into the .off format.\n"
                  "Typical use example:\n \t obj2off -i file.obj -o file.off \n");
  app.add_option("-i,--input,1", inputFileName, "an input mesh file in .obj format." )
  ->required()
  ->check(CLI::ExistingFile);
  app.add_option("--output,-o,2", outputFileName, "an output file ");

 
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  // read input mesh
  DGtal::Mesh<DGtal::Z3i::RealPoint> aMesh(true);

  MeshReader<DGtal::Z3i::RealPoint>::importOBJFile(inputFileName, aMesh );
  ofstream fout;
  fout.open(outputFileName);
  MeshWriter<DGtal::Z3i::RealPoint>::export2OFF(fout, aMesh);
  return 0;
}


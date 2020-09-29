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
 * @file off2sdp.cpp
 * @ingroup geometry3d
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Lorraine, France
 *
 * @date 2017/03/05
 *
 * Source file of the tool off2sdp
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/shapes/Mesh.h"
#include "DGtal/io/readers/MeshReader.h"

#include "CLI11.hpp"

#include <iostream>
#include <fstream>


///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page off2sdp off2sdp
 
 @brief  Converts a mesh into a set of points (.sdp). It can extract the mesh vertices (by default) or the center of faces.  

 @b Usage:   off2sdp [input] [output]

 @b Allowed @b options @b are :
 
 @code
 
 Positionals:
   1 TEXT:FILE REQUIRED                  the input mesh filename (.off).

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         the input mesh filename (.off).
   -o,--output TEXT                      the output filename (.sdp).
   -f,--faceCenter                       export the face centers instead the mesh vertex.
   @endcode

 @b Example: 

 @code
 	 off2sdp $DGtal/examples/samples/tref.off  test.sdp
 @endcode

 
 @ref off2sdp.cpp

 */


int main( int argc, char** argv )
{
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  std::string inputFileName;
  std::string outputFileName {"result.sdp"};
  bool faceCenter {false};
  app.description("Converts a mesh into a set of points (.sdp)."
                  "It can extract the mesh vertices (by default) or the center of faces."
                  "Typical use example:\n \t off2sdp  $DGtal/examples/samples/tref.off  test.sdp  \n");
  
  app.add_option("-i,--input,1", inputFileName, "the input mesh filename (.off)." )
  ->required()
  ->check(CLI::ExistingFile);
  
  app.add_option("--output,-o,2",outputFileName, "the output filename (.sdp).", true);
  app.add_flag("--faceCenter,-f", faceCenter, "export the face centers instead the mesh vertex.");
  
  
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------


  
  
  // read input mesh
  DGtal::Mesh<DGtal::Z3i::RealPoint> aMesh;
  aMesh << inputFileName ;
  
  // export
  ofstream fout;
  fout.open(outputFileName.c_str());
  fout<< "# set of points extracted from the tools off2sdp with input file " << inputFileName << std::endl;
  if(!faceCenter)
    {
      for(unsigned int i=0; i<aMesh.nbVertex(); i++ )
        {
          DGtal::Z3i::RealPoint p = aMesh.getVertex(i);
          fout << p[0] << " " << p[1] << " " << p[2] << std::endl;
        }
    }
  else
    {
      for(unsigned int i=0; i<aMesh.nbFaces(); i++ )
        {
          DGtal::Z3i::RealPoint p = aMesh.getFaceBarycenter(i);
          fout << p[0] << " " << p[1] << " " << p[2] << std::endl;
        }
    }  
  fout.close();
  return 0;
}


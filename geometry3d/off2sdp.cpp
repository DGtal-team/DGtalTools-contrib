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

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


#include <iostream>
#include <fstream>


///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////
namespace po = boost::program_options;


/**
 @page off2sdp off2sdp
 
 @brief  Converts a mesh into a set of points (.sdp). It can extract the mesh vertex (by default) or the center of faces.  

 @b Usage:   off2sdp [input] [output]

 @b Allowed @b options @b are :
 
 @code
  -h [ --help ]         display this message
  -i [ --input ] arg    the input mesh filename (.off).
  -o [ --output ] arg   the output filename (.sdp).
  -f [ --faceCenter ]   export the face centers instead the mesh vertex.
 @endcode

 @b Example: 

 @code
 	 off2sdp -i $DGtal/examples/samples/tref.off -o test.sdp  
 @endcode

 
 @ref off2sdp.cpp

 */


int main( int argc, char** argv )
{
  // parse command line -------------------------------------------------------
  po::options_description general_opt("Allowed options are");
  general_opt.add_options()
    ("help,h", "display this message")
    ("input,i", po::value<std::string >(), "the input mesh filename (.off)." )
    ("output,o", po::value<std::string >(), "the output filename (.sdp)." )
    ("faceCenter,f", "export the face centers instead the mesh vertex." );

  bool parseOK=true;
  po::variables_map vm;
  try
    {
      po::store(po::parse_command_line(argc, argv, general_opt), vm);
    }
  catch(const std::exception& ex)
    {
      parseOK=false;
      trace.info()<< "Error checking program options: "<< ex.what()<< endl;
    }
  

  // check if min arguments are given and tools description ------------------
  po::notify(vm);
  bool canStart = true;
  if(! vm.count("input") || ! vm.count("output"))
    {
      trace.error() << " The input or output file name was not defined" << endl;
      canStart = false;
    }
  if( !parseOK || vm.count("help")||argc<=1 || !canStart)
    {
      trace.info() << "Usage: " << argv[0] << " [input] [output] \n"
                << "Converts a mesh into a set of points (.sdp). It can extract the mesh vertex (by default) or the center of faces.  \n"
                << general_opt << "\n"
                << "Typical use example:\n \t off2sdp -i $DGtal/examples/samples/tref.off -o test.sdp  \n";
      return 0;
    }  



  //  recover the  args ----------------------------------------------------
  string inputFileName = vm["input"].as<string>();
  string outputFileName = vm["output"].as<string>();
  bool faceCenter =  vm.count("faceCenter")==1;
  
  
  
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


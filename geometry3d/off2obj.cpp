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

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////
namespace po = boost::program_options;


/**
 @page off2obj off2obj
 
 @brief  Description of the tool...

 @b Usage:   off2obj [input]

 @b Allowed @b options @b are :

 @code
  -h [ --help ]         display this message
  -i [ --input ] arg    an input mesh file in .off format. 
  -o [ --output ] arg   an output file (can generate .obj and .mtl if color 
                        option is selected) 
  -c [ --colors ]       convert by taking into account the mesh colors (from 
                        each faces).
 @endcode

 @b Example: 

 @code
	 off2obj -i file.off -o file.obj -c  
 @endcode

 @see
 @ref off2obj.cpp

 */


int main( int argc, char** argv )
{
  // parse command line -------------------------------------------------------
  po::options_description general_opt("Allowed options are");
  general_opt.add_options()
    ("help,h", "display this message")
    ("input,i", po::value<std::string >(), "an input mesh file in .off format. " )
    ("output,o", po::value<std::string >(), "an output file (can generate .obj and .mtl if color option is selected) " )
    ("invertNormals,n", "invert the normals (face orientation).")
    ("colors,c", "convert by taking into account the mesh colors (from each faces)." );

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
  if( !parseOK || vm.count("help")||argc<=1)
    {
      std::cout << "Usage: " << argv[0] << " [input]\n"
                << "Converts a .off mesh into the .obj format.\n"
                << general_opt << "\n"
                << "Typical use example:\n \t off2obj -i file.off -o file.obj -c  \n";
      return 0;
    }  
  if(! vm.count("input") || ! vm.count("output"))
    {
      trace.error() << " The input/output file name was not defined" << endl;
      return 1;
    }



  //  recover the  args ----------------------------------------------------
  string inputFileName = vm["input"].as<string>();
  string outputFileName = vm["output"].as<string>();

  
  std::string basename = outputFileName.substr(0, outputFileName.find_last_of("."));
  std::stringstream outname; outname << basename << ".obj"; 
  // read input mesh
  DGtal::Mesh<DGtal::Z3i::RealPoint> aMesh;

  MeshReader<DGtal::Z3i::RealPoint>::importOFFFile(inputFileName,
                                                   aMesh, vm.count("invertNormals"));
  
  ofstream fout;
  fout.open(outname.str().c_str());
  if(!vm.count("colors"))
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


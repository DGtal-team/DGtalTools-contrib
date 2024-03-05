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
 * Laboratoire d'InfoRmatique en Image et Systemes d'information - LIRIS (CNRS, UMR 5205), CNRS, France
 *
 * @date 2024/03/05
 *
 * Source file of the tool meshTrunk2Pts
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/readers/TableReader.h"

#include "CLI11.hpp"

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page meshTrunk2Pts meshTrunk2Pts
 
 @brief  Description of the tool...
 
 @b Usage:   meshTrunk2Pts [input]
 
 @b Allowed @b options @b are :
 
 @code
 -h [ --help ]           display this message
 -i [ --input ] arg      an input file...
 -p [ --parameter] arg   a double parameter...
 @endcode
 
 @b Example:
 
 @code
 meshTrunk2Pts -i  $DGtal/examples/samples/....
 @endcode
 
 @image html resmeshTrunk2Pts.png "Example of result. "
 
 @see
 @ref meshTrunk2Pts.cpp
 
 */


int main( int argc, char** argv )
{
    typedef std::vector<double> CylCoordsCont;
    double parameter {1.0};
    std::string inputMeshFileName;
    std::string inputCLineFileName;
    
    std::string outputBaseName;
    std::stringstream usage;
    usage << "Usage: " << argv[0] << " [input]\n"
    << "Typical use example:\n \t meshTrunk2Pts -i ... \n";
    // parse command line using CLI-------------------------------------------------------
    CLI::App app;
    app.description("Transform an input mesh into points cloud simulating acquisition process like lidar Scan .\n" + usage.str() );
    app.add_option("--inputMesh,-i,1", inputMeshFileName, "Input file")
    ->required()->check(CLI::ExistingFile);
    app.add_option("--InputCCoords,-c,2", inputCLineFileName, "Input file containing cylinder coordinates")
    ->required()->check(CLI::ExistingFile);
    
    app.add_option("--outputBaseName,-o,3", outputBaseName, "Output SDP filename")->required();
    app.add_option("--parameter,-p", parameter, "a double parameter", true);
    
    app.get_formatter()->column_width(40);
    CLI11_PARSE(app, argc, argv);
    // END parse command line using CLI ----------------------------------------------
    
    
    // Reading input mesh   --------------------------------------------------
    DGtal::Mesh<DGtal::Z3i::RealPoint> aMesh;
    DGtal::Mesh<DGtal::Z3i::RealPoint> aCenterLine;
    std::vector<CylCoordsCont> cylCoordinates;
    
    trace.info() << "Starting " << argv[0]  << "with input: "
    <<  inputMeshFileName << " and output :" << outputBaseName
    << " param: " << parameter <<std::endl;
    
    trace.info() << "Reading input mesh...";
    aMesh << inputMeshFileName;
    trace.info() << " [done]" << std::endl;
    trace.info() << "Read mesh with " << aMesh.nbVertex() << std::endl;
    
    trace.info() << "Reading input cylinder coordinates... (R,theta,Z)";
    cylCoordinates = TableReader<double>::getLinesElementsFromFile(inputCLineFileName);
    trace.info() << " [done]" << std::endl;
    trace.info() << "Read tab with " << cylCoordinates.size() << std::endl;

    
    return 0;
}



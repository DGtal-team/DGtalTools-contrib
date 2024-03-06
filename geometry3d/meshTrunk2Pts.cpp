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
#include "DGtal/io/writers/MeshWriter.h"

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
    std::vector<double> basePoint;
    std::string outputBaseName;
    std::stringstream usage;
    double angleRange = 1.2;

    usage << "Usage: " << argv[0] << " [input]\n"
    << "Typical use example:\n \t meshTrunk2Pts -i ... \n";
    // parse command line using CLI-------------------------------------------------------
    CLI::App app;
    app.description("Transform an input mesh into points cloud simulating acquisition process like lidar Scan .\n" + usage.str() );
    app.add_option("--inputMesh,-i,1", inputMeshFileName, "Input file")
    ->required()->check(CLI::ExistingFile);
    app.add_option("--InputCCoords,-c,2", inputCLineFileName, "Input file containing cylinder coordinates")
    ->required()->check(CLI::ExistingFile);
    app.add_option("--basePoint,-b",basePoint , "trunk base coordinate point", true)
    ->expected(3);
    app.add_option("--angleRange,-a",angleRange , "angle range of accepted face orientations.");
    app.add_option("--outputBaseName,-o,3", outputBaseName, "Output SDP filename")->required();

    
    app.get_formatter()->column_width(40);
    CLI11_PARSE(app, argc, argv);
    // END parse command line using CLI ----------------------------------------------
    
    DGtal::Mesh<DGtal::Z3i::RealPoint> resultingMesh;

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
    Z3i::RealPoint ptBase;
    if (basePoint.size() >2){
        ptBase[0] = basePoint[0];
        ptBase[1] = basePoint[1];
        ptBase[2] = basePoint[2];
        trace.info() << "Input base point: " << ptBase << std::endl;
    }
    
    double baseRad = cylCoordinates[0][0];

    // prepare resulting mesh
    for (auto it = aMesh.vertexBegin(); it != aMesh.vertexEnd(); it++){
        resultingMesh.addVertex(*it);
    }
    
    
    // First sector extraction
    std::string extr1NamePts = outputBaseName+"_Extr1.pts";
    std::string extr1NameMesh = outputBaseName+"_Extr1.off";

    Z3i::RealPoint originExtr1 = ptBase;
    originExtr1[0] += 2.0*baseRad;
    Z3i::RealPoint aNormal = originExtr1 - ptBase;
    aNormal = aNormal.getNormalized();
    trace.info() << "Origin point from extraction simulation:"
                  << originExtr1 << std::endl;
    //a) filter faces from face normal vector
    for (auto it = aMesh.faceBegin();
         it!= aMesh.faceEnd(); it++){
        DGtal::Mesh<Z3i::RealPoint>::MeshFace aFace = *it;
        bool okOrientation = true;
        Z3i::RealPoint p0 = aMesh.getVertex(aFace.at(1));
        Z3i::RealPoint p1 = aMesh.getVertex(aFace.at(0));
        Z3i::RealPoint p2 = aMesh.getVertex(aFace.at(2));
        Z3i::RealPoint vectNormal = ((p1-p0).crossProduct(p2 - p0)).getNormalized();
        vectNormal /= vectNormal.norm();
        okOrientation = vectNormal.dot(aNormal) > cos(angleRange);
        if( okOrientation ){
            resultingMesh.addFace(aFace);
        }
    }
    trace.info() << "Cleaning isolated vertices from " << resultingMesh.nbVertex();
    resultingMesh.removeIsolatedVertices();
    trace.info() << "to " << resultingMesh.nbVertex() << " [done]";

    trace.info() << "Writing output mesh...";
    resultingMesh >> extr1NameMesh;
    trace.info() << "[done]." << std::endl;
    return 0;
}



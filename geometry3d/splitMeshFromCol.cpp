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
 * @date 2024/08/20
 *
 * Source file of the tool splitMeshFromCol
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/writers/MeshWriter.h"
#include "CLI11.hpp"

#include "sstream"

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page splitMeshFromCol splitMeshFromCol
 
 @brief  Description of the tool...
 
 @b Usage:   splitMeshFromCol [input]
 
 @b Allowed @b options @b are :
 
 @code
 -h [ --help ]           display this message
 -i [ --input ] arg      an input file...
 -p [ --parameter] arg   a double parameter...
 @endcode
 
 @b Example:
 
 @code
 splitMeshFromCol -i  $DGtal/examples/samples/....
 @endcode
 
 @image html ressplitMeshFromCol.png "Example of result. "
 
 @see
 @ref splitMeshFromCol.cpp
 
 */
typedef DGtal::Mesh<DGtal::Z3i::RealPoint> Mesh3DR;
typedef DGtal::Mesh<DGtal::Z3i::RealPoint>::Index Face;

void extractMeshFromCol(const Mesh3DR &aMesh, const DGtal::Color &colorRef,
                        const std::vector<Face> &indColFaces, Mesh3DR &resMesh){
    // Import all vertex from input in the resulting mesh:
    for (auto i = 0; i< aMesh.nbVertex(); i++){
        resMesh.addVertex(aMesh.getVertex(i));
    }
    for (auto i = 0; i< indColFaces.size(); i++){
            resMesh.addFace(aMesh.getFace(indColFaces[i]));
    }
    resMesh.removeIsolatedVertices();
}

int main( int argc, char** argv )
{
    
    double parameter {1.0};
    std::string inputFileName;
    std::string outputFileName;
    std::stringstream usage;
    usage << "Usage: " << argv[0] << " [input]\n"
    << "Typical use example:\n \t splitMeshFromCol -i ... \n";
    // parse command line using CLI-------------------------------------------------------
    CLI::App app;
    app.description("Your program description.\n" + usage.str() );
    app.add_option("--input,-i,1", inputFileName, "Input file")->required()->check(CLI::ExistingFile);
    app.add_option("--output,-o,2", outputFileName, "Output basename")->required();
    app.add_option("--parameter,-p", parameter, "a double parameter", true);
    
    app.get_formatter()->column_width(40);
    CLI11_PARSE(app, argc, argv);
    // END parse command line using CLI ----------------------------------------------
    
    
    // Some nice processing  --------------------------------------------------
    
    
    trace.info() << "Starting " << argv[0]  << "with input: " <<  inputFileName
    << " and output :" << outputFileName
    << " param: " << parameter <<std::endl;
    trace.info() << "Reading mesh...  ";
    DGtal::Mesh<DGtal::Z3i::RealPoint> aMesh(true);
    aMesh << inputFileName;
    trace.info() << "[done]" << std::endl;

    //Partion of mesh faces
    trace.info() << "Partionning colors of the mesh " ;
    std::map<DGtal::Color, std::vector<Face> > mapColorFaces;
    for (auto i = 0; i< aMesh.nbFaces(); i++){
        if ( mapColorFaces.count(aMesh.getFaceColor(i)) == 0){
            mapColorFaces[aMesh.getFaceColor(i)] = std::vector<Face>();
        }else{
            mapColorFaces[aMesh.getFaceColor(i)].push_back(i);
        }
    }
    trace.info() << "[done with " << mapColorFaces.size()<< " ] "  << std::endl;
    
    
    unsigned int n=0;
    for (auto col: mapColorFaces){
        DGtal::Mesh<DGtal::Z3i::RealPoint> aResMesh(true);
        extractMeshFromCol(aMesh, col.first, col.second, aResMesh);
        stringstream ss; ss<< outputFileName << "_"<< n << ".obj";
        trace.info() << "Writing output mesh  " << ss.str() ;
        aResMesh >> ss.str();
        trace.info() << "[done]" << std::endl;
        n++;
    }
    
    
    return 0;
}



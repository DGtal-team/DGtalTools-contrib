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
 * @date 2026/04/04
 *
 * Source file of the tool meshFaceOrderColor
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include <DGtal/shapes/Mesh.h>
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/writers/MeshWriter.h"
#include "DGtal/io/colormaps/HueShadeColorMap.h"
#include "CLI11.hpp"
#include <vector>
#include <algorithm>
#include <map>


///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page meshFaceOrderColor meshFaceOrderColor
 
 @brief  Colors a mesh based on face order and optionally sorts the faces
 lexicographically.Usage: ./geometry3d/meshFaceOrderColor [input]
 Typical use example:
 meshFaceOrderColor $DGtal/examples/samples/bunnyhead.obj resColor.obj -s

 @b Usage:   meshOrderColor [input]

 ./geometry3d/meshFaceOrderColor [OPTIONS] 1 2

 @code

 POSITIONALS:
   1 TEXT:FILE REQUIRED                  Input mesh
   2 TEXT REQUIRED                       Output mesh

 @b Allowed @b options @b are :

 OPTIONS:
   -h,        --help                     Print this help message and exit
   -i,        --input TEXT:FILE REQUIRED Input mesh
   -o,        --output TEXT REQUIRED     Output mesh
   -s,        --sort-lexicographic       sorts the faces lexicographically.

 

 @endcode

 @b Example:

 @code
       meshFaceOrderColor $DGtal/examples/samples/bunnyhead.obj resColor.obj -s

 @endcode

 @see
 @ref meshOrderColor.cpp

 */


typedef DGtal::Mesh<DGtal::Z3i::RealPoint> Mesh3D;
typedef DGtal::Z3i::RealPoint Point;

/**
 * Sort mesh faces from barycenter
 */
Mesh3D sortFaces(const Mesh3D& inputMesh) {
    Mesh3D resMesh;

    const size_t nbV = inputMesh.nbVertex();
    const size_t nbF = inputMesh.nbFaces();
    for (size_t i = 0; i < nbV; ++i) {
        resMesh.addVertex(inputMesh.getVertex(i));
    }
    // precompute barycenter
    std::vector<Point> barycenters(nbF);
    for (size_t i = 0; i < nbF; ++i) {
        barycenters[i] = inputMesh.getFaceBarycenter(i);
    }

    std::vector<size_t> faceIds(nbF);
    std::iota(faceIds.begin(), faceIds.end(), 0);
    // ----------- 4. Tri par barycentre -----------
    std::sort(faceIds.begin(), faceIds.end(),
        [&](size_t a, size_t b) {
            const auto& ca = barycenters[a];
            const auto& cb = barycenters[b];
            if (ca[0] != cb[0]) return ca[0] < cb[0];
            if (ca[1] != cb[1]) return ca[1] < cb[1];
            else return ca[2] < cb[2];
        });
    //add ordered faces to result
    for (auto fid : faceIds) {
        auto face = inputMesh.getFace(fid);
        resMesh.addFace(face);
    }
    return resMesh;
}

int main( int argc, char** argv )
{
    bool sortLexico {false};
    double parameter {1.0};
    std::string inputFileName;
    std::string outputFileName;
    std::stringstream usage;

    usage << "Usage: " << argv[0] << " [input]\n"
    << "Typical use example:\n \t  meshFaceOrderColor $DGtal/examples/samples/bunnyhead.obj resColor.obj -s\n";
    // parse command line using CLI-------------------------------------------------------
    CLI::App app;
    app.description("Colors a mesh based on face order and optionally sorts the faces lexicographically." + usage.str() );
    app.add_option("--input,-i,1", inputFileName, "Input mesh")->required()->check(CLI::ExistingFile);
    app.add_option("--output,-o,2", outputFileName, "Output mesh")->required();
    app.add_flag("--sort-lexicographic,-s", sortLexico, "sorts the faces lexicographically.");
    
    app.get_formatter()->column_width(40);
    CLI11_PARSE(app, argc, argv);
    // END parse command line using CLI ----------------------------------------------
    
    
    // Some nice processing  --------------------------------------------------
    typedef DGtal::Mesh<DGtal::Z3i::RealPoint> Mesh3D;
    Mesh3D inputMesh;
    Mesh3D outputMesh;
    
    trace.info() << "Starting " << argv[0]  << "with input: " <<  inputFileName
    << " and output :" << outputFileName
    << " param: " << parameter <<std::endl;
    trace.info() << "Reading the input mesh...";
    inputMesh << inputFileName;
    trace.info() << "[done]"<< endl;
    trace.info() << "Sorting faces...";
    if (sortLexico)
    {
        inputMesh = sortFaces(inputMesh);
    }
    trace.info() << "[done]"<< endl;
    outputMesh = inputMesh;
    HueShadeColorMap<unsigned long> hueMap(0, inputMesh.nbFaces());
    for (unsigned long i=0; i < inputMesh.nbFaces(); i++){
        outputMesh.setFaceColor(i, hueMap(i));
    }
    
    trace.info() << "Writing the output mesh...";
    outputMesh >> outputFileName;
    trace.info() << "[done]"<< endl;
    
    return 0;
}



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
#include "DGtal/io/readers/PointListReader.h"
#include "DGtal/io/readers/TableReader.h"
#include "DGtal/io/writers/MeshWriter.h"
#include <time.h>
#include <cstdlib>
#include <stdio.h>
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
double
gaussF(double x, double mu, double sigma){
    double max = exp((-(mu)*(mu))/(2.0*sigma*sigma))*(1.0/(sigma*sqrt(2*M_PI)));
    return (exp((-(x-mu)*(x-mu))/(2.0*sigma*sigma))*(1.0/(sigma*sqrt(2*M_PI))))/max;
}


struct PithSectionCenter {
    std::vector<Z3i::RealPoint> myPith;
    std::vector<Z3i::RealPoint> mySampledPith;
    double myMinZ, myMaxZ;
    const double sampleSize = 20.0;
    int myNbIntervals;
    PithSectionCenter(const std::vector<Z3i::RealPoint> &aPith): myPith(aPith){
        myMaxZ = (*std::max_element(aPith.begin(), aPith.end(), [](auto a, auto b){return a[2] < b[2];}))[2];
        myMinZ = (*std::min_element(aPith.begin(), aPith.end(), [](auto a, auto b){return a[2] < b[2];}))[2];
        myNbIntervals = (myMaxZ - myMinZ)/sampleSize;
        for (unsigned int i = 0; i < myNbIntervals; i++){
            mySampledPith.push_back(Z3i::RealPoint(0,0,0));
        }
        for (auto const &p: myPith){
            int i = (int) floor((p[2]-myMinZ)/sampleSize);
            mySampledPith[i] = p;
        }
        //check if all sample are presents
        unsigned int n = 0;
        for (unsigned int i = 0; i < myNbIntervals; i++){
            if (mySampledPith[i] != Z3i::RealPoint(0,0,0)){
                n++;
            }
        }
        if (n != mySampledPith.size()){
            trace.warning() << "all samples are not represented: " << n << "over " << myNbIntervals << std::endl;
        }
    }
    Z3i::RealPoint pithRepresentant(const Z3i::RealPoint &p) const {
        unsigned int i = (unsigned int) ceil((p[2]-myMinZ)/sampleSize);
        assert(i >= 0);
        i = std::min((unsigned int)(mySampledPith.size()-1), i);
        return mySampledPith[i];
    }
};


struct TrunkDeformator {
    double mySectorSize;
    double myMinZ, myMaxZ;
    int myNbSectors;
    std::vector<double> mySectorShift;
    const PithSectionCenter& mySectionCenter;
    
    TrunkDeformator(const PithSectionCenter &pSectCenter, double maxShift, double sectSize):
                    mySectionCenter(pSectCenter),
                    mySectorSize(sectSize){
        myNbSectors = (int)floor((2.0*M_PI) / mySectorSize);
        std::srand((unsigned int) std::time(NULL));
        for (unsigned int i = 0; i < myNbSectors; i++){
            double shift = rand()%((int)floor(2000.0*maxShift));
            shift /= 2000.0;
            mySectorShift.push_back(shift);
        }
    }
    Z3i::RealPoint deform(const Z3i::RealPoint &pt, const Z3i::RealPoint &ptCyl) const {
        Z3i::RealPoint res = pt;
        unsigned int sectInd = (unsigned int) floor(ptCyl[1]/mySectorSize);
        double posA = (((double) sectInd)*mySectorSize+mySectorSize/2.0)-ptCyl[1];
        double gCoef = gaussF(posA, 0, mySectorSize/4.0 );
        double ratioZ = (pt[2]-mySectionCenter.myMinZ)/(mySectionCenter.myMaxZ-mySectionCenter.myMinZ);
        double hShift = mySectorShift[sectInd]*ratioZ*gCoef*0.5;
        res = res  + (pt-mySectionCenter.pithRepresentant(pt)).getNormalized()*hShift;
        return res;
    }
};


int main( int argc, char** argv )
{
    double parameter {1.0};
    std::string inputMeshFileName;
    std::string inputCLineFileName;
    std::string inputPithFileName;
    
    std::vector<double> basePoint;
    std::string outputBaseName;
    std::stringstream usage;
    double normalAngleRange = 0.6;
    double posAngleRange = 3.0;
    double ampliMaxShift = 100.0;
    double sectSize = 0.3;

    usage << "Usage: " << argv[0] << " [input]\n"
    << "Typical use example:\n \t meshTrunk2Pts -i ... \n";
    // parse command line using CLI-------------------------------------------------------
    CLI::App app;
    app.description("Transform an input mesh into points cloud simulating acquisition process like lidar Scan .\n" + usage.str() );
    app.add_option("--inputMesh,-i,1", inputMeshFileName, "Input file")
    ->required()->check(CLI::ExistingFile);
    app.add_option("--InputCCoords,-c,2", inputCLineFileName, "Input file containing cylinder coordinates")
    ->required()->check(CLI::ExistingFile);
    app.add_option("--InputPithCoords,-p,3", inputPithFileName, "Input file containing pith coordinates")
    ->required()->check(CLI::ExistingFile);

    app.add_option("--basePoint,-b",basePoint , "trunk base coordinate point", true)
    ->expected(3);
    app.add_option("--normalAngleRange,-a",normalAngleRange , "angle range of accepted face orientations.");
    app.add_option("--posAngleRange,-r",posAngleRange , "position angle range of accepted mesh points.");
    app.add_option("--ampliMaxShift,-s",ampliMaxShift , "maximal amplitude of sector shift.");
    app.add_option("--sectSize,-S",sectSize , "sector size of the deformation.");

    app.add_option("--outputBaseName,-o,3", outputBaseName, "Output SDP filename")->required();

    
    app.get_formatter()->column_width(40);
    CLI11_PARSE(app, argc, argv);
    // END parse command line using CLI ----------------------------------------------
    
    DGtal::Mesh<DGtal::Z3i::RealPoint> resultingMesh;

    // Reading input mesh   --------------------------------------------------
    DGtal::Mesh<DGtal::Z3i::RealPoint> aMesh;

    DGtal::Mesh<DGtal::Z3i::RealPoint> aCenterLine;
    std::vector<DGtal::Z3i::RealPoint> cylCoordinates;
    std::vector<DGtal::Z3i::RealPoint> pith;
    
    trace.info() << "Starting " << argv[0]  << "with input: "
    <<  inputMeshFileName << " and output :" << outputBaseName
    << " param: " << parameter <<std::endl;
    
    trace.info() << "Reading input mesh...";
    aMesh << inputMeshFileName;
    trace.info() << " [done] (" << aMesh.nbVertex() << ")" << std::endl;
 
    trace.info() << "Reading input pith coordinates...";
    pith = PointListReader<DGtal::Z3i::RealPoint>::getPointsFromFile(inputPithFileName);
    trace.info() << " [done] (" << pith.size() << ")" <<  std::endl;
    PithSectionCenter pSct (pith);
 
    trace.info() << "Reading input cylinder coordinates... (R,theta,Z)";
    cylCoordinates = PointListReader<DGtal::Z3i::RealPoint>::getPointsFromFile(inputCLineFileName);
    trace.info() << " [done] (" << cylCoordinates.size() << ")" << std::endl;
    
    
    
    
    
    
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
        okOrientation = vectNormal.dot(aNormal) > cos(normalAngleRange);
        
        bool sectorCompatible = true;
        auto pB = (p0+p1+p2)/3.0;
        Z3i::RealPoint pC = pSct.pithRepresentant(pB);
        Z3i::RealPoint vectDir = pB - pC;
        vectDir /= vectDir.norm();
        sectorCompatible = vectDir.dot(aNormal) > cos(posAngleRange/2.0);

        if( okOrientation && sectorCompatible ){
            resultingMesh.addFace(aFace);
        }
    }
    //b) applying shift on sector
    TrunkDeformator tDef (pSct, ampliMaxShift, sectSize);
    for (unsigned int i = 0; i < resultingMesh.nbVertex(); i++){
        Z3i::RealPoint &pt = resultingMesh.getVertex(i);
        Z3i::RealPoint ptCyl = cylCoordinates[i];
        Z3i::RealPoint newP = tDef.deform(pt, ptCyl);
        pt[0] = newP[0]; pt[1] = newP[1]; pt[2] = newP[2];
        
    }
    
    trace.info() << "Cleaning isolated vertices from " << resultingMesh.nbVertex();
    resultingMesh.removeIsolatedVertices();
    trace.info() << "to " << resultingMesh.nbVertex() << " [done]";

    trace.info() << "Writing output mesh...";
    resultingMesh >> extr1NameMesh;
    trace.info() << "[done]." << std::endl;
    return 0;
}



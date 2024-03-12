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
 * Source file of the tool trunkMeshTransform
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
#include <iostream>
#include <fstream>

#include "CLI11.hpp"
///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page trunkMeshTransform trunkMeshTransform
 
 @brief Transform an input mesh into points cloud simulating acquisition process like lidar Scan .

 @b Usage:   trunkMeshTransform [input]
 Usage: ./geometry3d/trunkMeshTransform [input]

 Typical use example:
       trunkMeshTransform ../Samples/TrunkSample/chene1.off -c ../Samples/TrunkSample/chene1-cyl  -p ../Samples/TrunkSample/chene1_centerline.xyz  resTransform.off -s 200 1  --outputPoints resTransform.pts  -F 0.8 -P 5.0 --mainDir 0 -1 0

 Positionals:
   1 TEXT:FILE REQUIRED                  Input file
   2 TEXT:FILE REQUIRED                  Input file containing cylinder coordinates
   3 TEXT:FILE REQUIRED                  Input file containing pith coordinates
   3 TEXT                                Output mesh file name.

 
 @b Allowed @b options @b are :
   -h,--help                             Print this help message and exit
   -i,--inputMesh TEXT:FILE REQUIRED     Input file
   -c,--InputCCoords TEXT:FILE REQUIRED  Input file containing cylinder coordinates
   -p,--InputPithCoords TEXT:FILE REQUIRED
                                         Input file containing pith coordinates
   -F,--filterFaceNormal FLOAT           Filter mesh faces using their normal vector: the  accepted face orientations are defined the face normal and filtering direction (see option --filterDir).
   -P,--filterFacePosition FLOAT         Filter mesh faces using angle defined from the face barycenter position with its associated pith center (angle in radians) and the filtering direction (see option --filterDir).
   -s,--shiftFacePos [FLOAT,FLOAT]       Shift face position using maximal amplitude (first parameter value) of sector shift (sector size defined with the second parameter value).
   -m,--mainDir FLOAT x 3                Define the main direction to define the filtering angle based (see --filterFacePosition and --filterFaceNormal
   -o,--outputMesh TEXT                  Output mesh file name.
   --outputPoints TEXT                   Output pts file name
 @code
 -h [ --help ]           display this message
 -i [ --input ] arg      an input file...
 -p [ --parameter] arg   a double parameter...
 @endcode
 
 @b Example:
 
 @code
 trunkMeshTransform -i  $DGtal/examples/samples/....
 @endcode
 
 @image html restrunkMeshTransform.png "Example of result. "
 
 @see
 @ref trunkMeshTransform.cpp
 
 */

typedef DGtal::Mesh<DGtal::Z3i::RealPoint> Mesh3D;

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
        myMaxZ = (*std::max_element(aPith.begin(), aPith.end(),
                                    [](Z3i::RealPoint a, Z3i::RealPoint b){return a[2] < b[2];}))[2];
        myMinZ = (*std::min_element(aPith.begin(), aPith.end(),
                                    [](Z3i::RealPoint a, Z3i::RealPoint b){return a[2] < b[2];}))[2];
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


struct TrunkAngularSamplor {
    double myDistanceScan;
    double myAngularVSize; // the vertical angular scan resolution
    Mesh3D myMesh;
    double myAngularToleranceFactor = 0.1; // the angle factor sensibility to consider a face is intersected by a laser bean. For instance a factor of 1 will consider all faces where face barycenters exactly intersect the laser bean while a value of 0.1 will a marge of myAngularVSize/10.
    TrunkAngularSamplor(const Mesh3D &aMesh, const PithSectionCenter &pSectCenter,
                        double distanceScan, double angularTolFact=0.1,
                        double angularVSize = 0.1, bool estimateScanVRes = true):
                 myDistanceScan(distanceScan),
                        myMesh(aMesh), myAngularVSize(angularVSize){
        if (estimateScanVRes) {
            double dz = pSectCenter.myPith[0][2]-pSectCenter.myPith[1][2];
            double l = sqrt(dz*dz+myDistanceScan*myDistanceScan);
            myAngularVSize = abs(asin(dz/l));
            trace.info() << "estimated vertical scan angle size: " << myAngularVSize << std::endl;
        }
    }
    bool isScanned(Mesh3D::Index aFaceId){
            auto p = myMesh.getFaceBarycenter(aFaceId);
            double l = sqrt( (p[2])*(p[2]) + myDistanceScan*myDistanceScan);
            double a =   asin(p[2]/l);
            double rS =  ceil(a / myAngularVSize)*myAngularVSize-a;
            double rI =  a-floor(a / myAngularVSize)*myAngularVSize;
            
        return abs(rI) < myAngularVSize*myAngularToleranceFactor ||
               abs(rS) < myAngularVSize*myAngularToleranceFactor ;
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
    std::string outputMesh = "result.off";
    std::string outputPts = "result.pts";

    std::string outputBaseName {"resDeform"};
    std::stringstream usage;
    double normalAngleRange = 0.6;
    double posAngleRange = 3.0;
    double ampliMaxShift = 100.0;
    double sectSize = 0.3;
    std::pair<double, double> shiftFacePosParams {1.0, 1.0};
    Z3i::RealPoint mainDir {1.0,0.0,0.0};
    std::vector<double> mainDirV {1.0,0.0,0.0};
    usage << "Usage: " << argv[0] << " [input]\n"
    << "Typical use example:\n \t trunkMeshTransform ../Samples/TrunkSample/chene1.off -c ../Samples/TrunkSample/chene1-cyl  "
    <<"-p ../Samples/TrunkSample/chene1_centerline.xyz  resTransform.off -s 200 1  --outputPoints resTransform.pts  "
    << "-F 0.8 -P 5.0 --mainDir 0 -1 0 \n";
    // parse command line using CLI-------------------------------------------------------
    CLI::App app;
    app.description("Transform an input mesh into points cloud simulating acquisition process like lidar Scan .\n" + usage.str() );
    app.add_option("--inputMesh,-i,1", inputMeshFileName, "Input file")
    ->required()->check(CLI::ExistingFile);
    app.add_option("--InputCCoords,-c,2", inputCLineFileName, "Input file containing cylinder coordinates")
    ->required()->check(CLI::ExistingFile);
    app.add_option("--InputPithCoords,-p,3", inputPithFileName, "Input file containing pith coordinates")
    ->required()->check(CLI::ExistingFile);

    auto filterFaceNormal = app.add_option("--filterFaceNormal,-F",normalAngleRange , "Filter mesh faces using their normal vector: the  accepted face orientations are defined the face normal and filtering direction (see option --filterDir). ");
    auto filterFacePosition = app.add_option("--filterFacePosition,-P", posAngleRange , "Filter mesh faces using angle defined from the face barycenter position with its associated pith center (angle in radians) and the filtering direction (see option --filterDir).");
    auto shiftFacePos = app.add_option("--shiftFacePos,-s", shiftFacePosParams, "Shift face position using maximal amplitude (first parameter value) of sector shift (sector size defined with the second parameter value).")
    ->expected(1);
    auto mainDirOpt = app.add_option("--mainDir,-m", mainDirV, "Define the main direction to define the filtering angle based (see --filterFacePosition and --filterFaceNormal ")
    ->expected(3);
    auto outMesh = app.add_option("--outputMesh,-o,3", outputMesh, "Output mesh file name.");
    auto outPts = app.add_option("--outputPoints", outputPts, "Output pts file name");
    app.get_formatter()->column_width(40);
    CLI11_PARSE(app, argc, argv);
    // END parse command line using CLI ----------------------------------------------
    
    Mesh3D resultingMesh;

    // Reading input mesh   --------------------------------------------------
    Mesh3D aMesh;

    std::vector<DGtal::Z3i::RealPoint> cylCoordinates;
    std::vector<DGtal::Z3i::RealPoint> pith;
    
    trace.info() << "Starting " << argv[0]  << "with input: "
    <<  inputMeshFileName << " and output :" << outputBaseName
    << " param: " << parameter <<std::endl;
    
    trace.info() << "Reading input mesh...";
    aMesh << inputMeshFileName;
    trace.info() << " [done] (#vertices: " << aMesh.nbVertex() << ")" << std::endl;
 
    trace.info() << "Reading input pith coordinates...";
    pith = PointListReader<DGtal::Z3i::RealPoint>::getPointsFromFile(inputPithFileName);
    trace.info() << " [done] (#pith size:" << pith.size() << ")" <<  std::endl;
    PithSectionCenter pSct (pith);
 
    trace.info() << "Reading input cylinder coordinates... (R,theta,Z)";
    cylCoordinates = PointListReader<DGtal::Z3i::RealPoint>::getPointsFromFile(inputCLineFileName);
    trace.info() << " [done] (#vertices: " << cylCoordinates.size() << ")" << std::endl;
    
    
    double baseRad = cylCoordinates[0][0];
    mainDir[0] = mainDirV[0];
    mainDir[1] = mainDirV[1];
    mainDir[2] = mainDirV[2];
    
    // prepare resulting mesh
    for (auto it = aMesh.vertexBegin(); it != aMesh.vertexEnd(); it++){
        resultingMesh.addVertex(*it);
    }

    // First sector extraction
    mainDir = mainDir.getNormalized();

   
    //a) applying shift on sector
    if (shiftFacePos->count()>0){
        ampliMaxShift = shiftFacePosParams.first;
        sectSize = shiftFacePosParams.second;
        TrunkDeformator tDef (pSct, ampliMaxShift, sectSize);
        for (unsigned int i = 0; i < resultingMesh.nbVertex(); i++){
            Z3i::RealPoint &pt = resultingMesh.getVertex(i);
            Z3i::RealPoint ptCyl = cylCoordinates[i];
            Z3i::RealPoint newP = tDef.deform(pt, ptCyl);
            pt[0] = newP[0]; pt[1] = newP[1]; pt[2] = newP[2];
        }
    }
    TrunkAngularSamplor tSamplor (aMesh, pSct, 5000);
    //b) filter faces from face normal vector and c) applying sampling simulation
    for (unsigned int i = 0; i< aMesh.nbFaces(); i++){
        
        DGtal::Mesh<Z3i::RealPoint>::MeshFace aFace = aMesh.getFace(i);
        bool okOrientation = true;
        bool okSampling = true;

        Z3i::RealPoint p0 = aMesh.getVertex(aFace.at(1));
        Z3i::RealPoint p1 = aMesh.getVertex(aFace.at(0));
        Z3i::RealPoint p2 = aMesh.getVertex(aFace.at(2));
        okSampling = tSamplor.isScanned(i);

        if (filterFaceNormal -> count() > 0 && okSampling ){
               Z3i::RealPoint vectNormal = ((p1-p0).crossProduct(p2 - p0)).getNormalized();
            vectNormal /= vectNormal.norm();
            okOrientation = vectNormal.dot(mainDir) > cos(normalAngleRange/2.0);
        }
        bool sectorCompatible = true;
        if (filterFacePosition -> count() > 0 && okSampling){
            auto pB = (p0+p1+p2)/3.0;
            Z3i::RealPoint pC = pSct.pithRepresentant(pB);
            Z3i::RealPoint vectDir = pB - pC;
            vectDir /= vectDir.norm();
            sectorCompatible = vectDir.dot(mainDir) > cos(posAngleRange/2.0);
        }
        
        if( okOrientation && sectorCompatible && okSampling){
            resultingMesh.addFace(aFace);
        }
    }
    
   

    
    trace.info() << "Cleaning isolated vertices from " << resultingMesh.nbVertex();
    resultingMesh.removeIsolatedVertices();
    trace.info() << " to " << resultingMesh.nbVertex() << " [done]" << std::endl;

    if (outMesh->count() > 0 ){
        trace.info() << "Writing output mesh...";
        resultingMesh >> outputMesh;
        trace.info() << "[done]." << std::endl;
    }
    if (outPts->count() > 0 ){
        trace.info() << "Writing output points...";
        
        ofstream fout;
        fout.open(outputPts);
        for (auto it = resultingMesh.vertexBegin(); it != resultingMesh.vertexEnd(); it++){
            fout << (*it)[0] << " " << (*it)[1] << " " << (*it)[2] << std::endl;
        }
        fout.close();
        trace.info() << "[done]." << std::endl;
    }
    return 0;
}




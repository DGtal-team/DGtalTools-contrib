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
 * @file computeMeshDistance.cpp
 
 * @author Bertrand Kerautret (\c kerautre@loria.fr)
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 *
 * @date 2015/12/12
 *
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/writers/MeshWriter.h"
#include "DGtal/io/colormaps/GradientColorMap.h"
#include "DGtal/io/colormaps/HueShadeColorMap.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "DGtal/shapes/Mesh.h"
#include "DGtal/math/linalg/SimpleMatrix.h"
#include "DGtal/kernel/BasicPointFunctors.h"


using namespace DGtal;
namespace po = boost::program_options;

static const double approxSamePlane = 0.1;

template <typename TPoint>
static
bool sameSide(const TPoint &p1,const TPoint &p2, const TPoint &a,const TPoint &b)
{
  TPoint cp1 = (b-a).crossProduct(p1-a);
  TPoint cp2 = (b-a).crossProduct(p2-a);
  return cp1.dot(cp2) >= 0;
}



template<typename TPoint>
static
bool
isInsideFaceTriangle(const TPoint &p, const TPoint &q, const TPoint &r, const TPoint &aPoint ){
  if (sameSide(aPoint, p, q, r) && sameSide(aPoint, q, p, r) && sameSide(aPoint, r, p, q))
  {
    TPoint vc1 =  (p-q).crossProduct(p-r);
    return std::abs((p-aPoint).dot(vc1)) <= approxSamePlane;
  }
  return false;
}



template<typename TMesh>
static
bool
isInsideFace(const TMesh &aMesh,const typename TMesh::MeshFace &aFace, const typename TMesh::Point &aPoint){
  if (aFace.size() == 4 ){
    typename TMesh::Point p = aMesh.getVertex(aFace[0]);
    typename TMesh::Point q = aMesh.getVertex(aFace[1]);
    typename TMesh::Point r = aMesh.getVertex(aFace[2]);
    typename TMesh::Point s = aMesh.getVertex(aFace[3]);
    return  isInsideFaceTriangle(p,q,r, aPoint) || isInsideFaceTriangle(r, s, p, aPoint);
  } else if (aFace.size() == 3 ){
    typename TMesh::Point p = aMesh.getVertex(aFace[0]);
    typename TMesh::Point q = aMesh.getVertex(aFace[1]);
    typename TMesh::Point r = aMesh.getVertex(aFace[2]);
    return  isInsideFaceTriangle(p,q,r, aPoint);
  }
  else{
    trace.warning() << "face with more 4 vertex ... returning false.";
  }
  return  false;
}


template<typename TPoint>
static
TPoint
getProjectedPoint(const TPoint &normal, const TPoint &aPlanePt, const TPoint &p ){
  double d = -(normal[0]*aPlanePt[0]+normal[1]*aPlanePt[1]+normal[2]*aPlanePt[2]);
  double lambda = -(normal[0]*p[0] + normal[1]*p[1] + normal[2]*p[2] + d)/
  (normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);
  return (lambda*normal+p);
}


template<typename TPoint>
static
bool
lineProject(const TPoint & ptA, const TPoint &ptB, TPoint &p ){
  TPoint u = ptB-ptA;
  TPoint pSave = p;
  double dP = ((p-ptA).crossProduct(u).norm())/u.norm();
  double dAProj = std::sqrt(((ptA -p).norm() * (ptA -p).norm()) - dP*dP);
  p = ptA + (u/u.norm())*dAProj;
  return ((p - ptA).norm() <= u.norm()) && u.dot(pSave-ptA) > 0;
}


typedef typename Z3i::RealPoint RPoint;



int
main(int argc,char **argv)
{
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
  ("help,h", "display this message")
  ("input,i", po::value<std::string>(), "input file name of mesh A (reference shape) given as OFF format.")
  ("inputComp,c", po::value<std::string>(), "input file name of mesh B (compared shape) given as OFF format.")
   ("output,o", po::value<std::string>(),  "arg = file.dat : output file containing all the distances of each input mesh faces (faces of A)")
  ("outputMesh,m", po::value<std::string>(),  "arg = file.off : export the resulting distances represented with a color scale on the faces of the reference mesh A.")
  ("faceCenterDistance,f", "approximates the minimal distance by using the euclidean distance of the face centers (instead using the minimal distance given by projection).")
  ("squaredDistance,s", "computes squared distance.")
  ("saveNearestPoint,n", "save the nearest point obtained during the computation of the minimal distance (point of B).")
  ("maxScaleDistance", po::value<double>()->default_value(0.1), "set the default max value use to display the distance")
  ("exportDistanceEstimationType", "Export as face color the type of distance estimation used for each face (blue for projection, green for edge projection and white for euclidean distance.)")
  ("minScaleDistance", po::value<double>()->default_value(0.0), "set the default min value use to display the distance");
  
  bool parseOK=true;
  po::variables_map vm;
  try{
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
  }catch(const std::exception& ex){
    trace.info()<< "Error checking program options: "<< ex.what()<< std::endl;
    parseOK=false;
  }
  po::notify(vm);
  if(vm.count("help")||argc<=1 || !vm.count("input") || 
     !vm.count("inputComp") || !vm.count("output") ||  !parseOK  )
  {
    trace.info()<< "Computes for each face of a mesh A the minimal distance to another mesh B. For each face of A, the minimal distance to B is computed by a brut force scan and the result can be exported as a mesh where the distances are represented in color scale. The maximal value of all these distances is also given as std output. " <<std::endl << "Options: "<<std::endl
                << general_opt << "\n" <<
      "Example of use (from the DGtalTools-contrib directory:"<< "\n" <<
      "./build/geometry3d/computeMeshDistances -i Samples/fandisk.off -c Samples/box.off -o distances.dat -m res.off --maxScaleDistance 0.7 -n \n"
                << "Then you can also check the distance result by using the meshViewer tool from DGtalTools: \n" <<
      " meshViewer -i res.off \n"
                << "And you can also check the nearest points used to compute the minimal distance:\n"
                << "meshViewer -i res.off Samples/box.off -f distances.dat --vectorFieldIndex 0 1 2 4 5 6\n";
    
    return 0;
  }
  
  std::string inputMeshName = vm["input"].as<std::string>();
  std::string inputCompMeshName = vm["inputComp"].as<std::string>();

  
  DGtal::Mesh<Z3i::RealPoint> theMeshRef(true);
  DGtal::Mesh<Z3i::RealPoint> theMeshComp(true);
  DGtal::Mesh<Z3i::RealPoint> theNewMeshDistance (true);
  DGtal::Mesh<Z3i::RealPoint> projOkMesh (true);
  
  MeshReader<Z3i::RealPoint>::importOFFFile(inputMeshName, theMeshRef, false);
  MeshReader<Z3i::RealPoint>::importOFFFile(inputMeshName, theNewMeshDistance, false);
  MeshReader<Z3i::RealPoint>::importOFFFile(inputCompMeshName, theMeshComp, false);
  MeshReader<Z3i::RealPoint>::importOFFFile(inputMeshName, projOkMesh, false);
  
  double maxScaleDistance = vm["maxScaleDistance"].as<double>();
  double minScaleDistance = vm["minScaleDistance"].as<double>();
  bool useFaceCenterDistance =  vm.count("faceCenterDistance");
  bool saveNearestPoint = vm.count("saveNearestPoint");
  bool squaredDistance = vm.count("squaredDistance");
  
  trace.info()<< "reading the input Comp mesh ok: "<< theMeshComp.nbVertex() <<  std::endl;
  
  
  
  double maxOfMin = 0;
  std::vector<double> vectFaceDistances;
  std::vector<RPoint> vectNearestPt;
  // Brut force distance measure between reference mesh (A) and B:
  // for each face of A we search the face which minimizes the distance (by using face projection, edge projection or center point)
  
  int cptFace=0;
  for (unsigned int i = 0; i<theMeshRef.nbFaces(); i++){
    std::vector<unsigned int>  aFace = theMeshRef.getFace(i);
    cptFace++;
    trace.progressBar(cptFace, theMeshRef.nbFaces());
    double distanceMin = std::numeric_limits<double>::max();    
    RPoint cA = theMeshRef.getFaceBarycenter(i);
    
    enum ProjType {INSIDE, EDGE, CENTER};
    ProjType aProjType = INSIDE;
    vectNearestPt.push_back(cA);
    // compute the minimal distance of the point of A to one face of point B.
    for (unsigned int j=0; j < theMeshComp.nbFaces(); j++){
      // project center (cA) of a face of A into faces of B.
      std::vector<unsigned int>  aFaceB = theMeshComp.getFace(j);
      RPoint pB0 = theMeshComp.getVertex(aFaceB.at(0));
      RPoint pB1 = theMeshComp.getVertex(aFaceB.at(1));
      RPoint pB2 = theMeshComp.getVertex(aFaceB.at(2));
      RPoint cB =  theMeshComp.getFaceBarycenter(j);
      
      if (useFaceCenterDistance){
        double distance = (cB-cA).norm();
        if (distance < distanceMin){
          distanceMin = distance;
          if (saveNearestPoint) {vectNearestPt[i] = cB;}
        }
        continue;
      }
      
      RPoint normal = ((pB0-pB1).crossProduct(pB2 - pB1));
      RPoint proj = getProjectedPoint(normal, cB, cA);
      double distance = (proj-cA).norm();
      
      if(!isInsideFace(theMeshComp, aFaceB, proj)){
        // if the projection is outside the face, we approximate the distance with the projection on the face edges
        RPoint p = cA;
        bool lineProjOK1 = lineProject(pB0, pB1, p);
        if(lineProjOK1 && distanceMin > (p-cA).norm()){
          distanceMin = (p-cA).norm();
          aProjType = EDGE;
          if (saveNearestPoint) {vectNearestPt[i] = p;}
        }
        p= cA;
        bool lineProjOK2 = lineProject(pB1, pB2, p);
        if(lineProjOK2 && distanceMin > (p-cA).norm()){
          distanceMin = (p-cA).norm();
          aProjType = EDGE;
          if (saveNearestPoint) {vectNearestPt[i] = p;}
        }
        p= cA;
        bool lineProjOK3 = lineProject(pB2, pB0, p);
        if(lineProjOK3 && distanceMin > (p-cA).norm()){
          distanceMin = (p-cA).norm();
          aProjType = EDGE;
          if (saveNearestPoint) {vectNearestPt[i] = p;}
        }
        if (!lineProjOK1 && ! lineProjOK2 && ! lineProjOK3 && (cB - cA).norm() < distanceMin){
        //if the projection is outside the face, we approximate the distance with the center of face B
          distanceMin = (cB - cA).norm();
          aProjType = CENTER;
          if (saveNearestPoint) {vectNearestPt[i] = cB;}
       }
        
      }else{
        if (distance < distanceMin){
          aProjType = INSIDE;
          distanceMin = distance;
          if (saveNearestPoint) {vectNearestPt[i] = proj;}
        }
      }
    }

    if(distanceMin>maxOfMin){
      maxOfMin = distanceMin;
    }
    projOkMesh.setFaceColor(i, aProjType == INSIDE ? DGtal::Color::Blue:  aProjType == EDGE ?  DGtal::Color::Green:  DGtal::Color::White);
    
    vectFaceDistances.push_back(distanceMin);
  }
  
  std::ofstream outDistances;
  std::string outName = vm["output"].as<std::string>();
  outDistances.open(outName.c_str(), std::ofstream::out);
  std::string name  = argv[0];
  name = name.substr(name.find_last_of("/")+1);
  outDistances << "# resulting distances computed from the " << name << " program of the DGtalTools-contrib project." << std::endl;
  outDistances << "# minimal distance between the mesh " << inputMeshName << " to " << inputCompMeshName << std::endl;
  outDistances << "# format: faceCenter_x faceCenter_y faceCenter_z distanceMin";
 
  if (saveNearestPoint) {
    outDistances << " nearestPoint_x nearestPoint_y nearestPoint_z ";
  }
  outDistances << std::endl;


  DGtal::GradientColorMap<double, CMAP_JET>  gradientShade(minScaleDistance, maxScaleDistance );
  for (unsigned int i=0; i< theNewMeshDistance.nbFaces(); i++){
    RPoint center = theNewMeshDistance.getFaceBarycenter(i);
    theNewMeshDistance.setFaceColor(i, gradientShade(std::min((squaredDistance ? vectFaceDistances[i] :
                                                              1.0 )* vectFaceDistances[i], maxScaleDistance)));
    outDistances << center[0] << " " << center[1] << " " << center[2] << " " << vectFaceDistances[i];
    if (saveNearestPoint)
    {
      outDistances << " " << vectNearestPt[i][0] << " " << vectNearestPt[i][1] << " " << vectNearestPt[i][2];
    }
    outDistances<< std::endl;
  }
  outDistances.close();
  
  if (squaredDistance) {
    trace.info()<< "maximal of minimum squared distance : " << maxOfMin << std::endl;
  }else {
    trace.info()<< "maximal of minimum distance: " << maxOfMin << std::endl;
  }
  
  std::ofstream outMesh;
  if(vm.count("outputMesh")){
    std::string outputMeshName = vm["outputMesh"].as<std::string>();
    outMesh.open(outputMeshName.c_str(), std::ofstream::out);
    MeshWriter<Z3i::RealPoint>::export2OFF(outMesh, theNewMeshDistance,true);
    outMesh.close();
  }
  
  if(vm.count("exportDistanceEstimationType")){
    std::ofstream outApproxDistance;
    outApproxDistance.open("distanceEstimationType.off", std::ofstream::out);
    MeshWriter<Z3i::RealPoint>::export2OFF(outApproxDistance, projOkMesh,true);
    outApproxDistance.close();
  }
  
}

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


using namespace DGtal;
namespace po = boost::program_options;

typedef typename Z3i::RealPoint TPoint;

bool
isInsideFace(const TPoint &p0, const TPoint &p1, const TPoint &p2, const TPoint &q){
  return ((p1-p0).dot(q-p0) >= 0 && (p2-p1).dot(q-p1) >= 0 && (p0-p2).dot(q-p2) >= 0)||
  ((p1-p0).dot(q-p0) <= 0 && (p2-p1).dot(q-p1) <= 0 && (p0-p2).dot(q-p2) <= 0);
}


bool
lineProject(const TPoint & ptA, const TPoint &ptB, TPoint &p ){
  TPoint u = ptB-ptA;
  TPoint pSave = p;
  double dP = ((p-ptA).crossProduct(u).norm())/u.norm();
  double dAProj = std::sqrt(((ptA -p).norm() * (ptA -p).norm()) - dP*dP);
  p = ptA + (u/u.norm())*dAProj;
  return ((p - ptA).norm() <= u.norm()) && u.dot(pSave-ptA) > 0;
}


int
main(int argc,char **argv)
{
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
  ("help,h", "display this message")
  ("input,i", po::value<std::string>(), "input file name of mesh A (reference shape) given as OFF format.")
  ("inputComp,c", po::value<std::string>(), "input file name of mesh B (compared shape) given as OFF format.")
  ("output,o", po::value<std::string>(),  "arg = file.off : export the resulting distances represented with a color scale on the faces of the reference mesh A.")
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
  if(vm.count("help")||argc<=1|| !parseOK )
  {
    trace.info()<< "Computes for each face of a mesh A the minimal distance to another mesh B. For each face of A, the minimal distance to B is computed by a brut force scan and the result can be exported as a mesh where the distances are represented in color scale. The maximal value of all these distances is also given as std output. " <<std::endl << "Options: "<<std::endl
		  << general_opt << "\n";
    return 0;
  }
  
  std::string inputMeshName = vm["input"].as<std::string>();
  std::string inputCompMeshName = vm["inputComp"].as<std::string>();
  std::string outputMeshName = vm["output"].as<std::string>();
  
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
  std::vector<TPoint> vectNearestPt;
  // Brut force distance measure between reference mesh (A) and B:
  // for each face of A we search the face which minimizes the distance (by using face projection, edge projection or center point)
  
  int cptFace=0;
  for (unsigned int i = 0; i<theMeshRef.nbFaces(); i++){
    std::vector<unsigned int>  aFace = theMeshRef.getFace(i);
    cptFace++;
    trace.progressBar(cptFace, theMeshRef.nbFaces());
    double distanceMin = std::numeric_limits<double>::max();
    
    TPoint p0 = theMeshRef.getVertex(aFace.at(0));
    TPoint p1 = theMeshRef.getVertex(aFace.at(1));
    TPoint p2 = theMeshRef.getVertex(aFace.at(2));
    TPoint cA = (p0+p1+p2)/3.0;
    enum ProjType {INSIDE, EDGE, CENTER};
    ProjType aProjType = INSIDE;
    vectNearestPt.push_back(cA);
    // compute the minimal distance of the point of A to one face of point B.
    for (unsigned int j=0; j < theMeshComp.nbFaces(); j++){
      // project center (cA) of a face of A into faces of B.
      std::vector<unsigned int>  aFaceB = theMeshComp.getFace(j);
      TPoint pB0 = theMeshComp.getVertex(aFaceB.at(0));
      TPoint pB1 = theMeshComp.getVertex(aFaceB.at(1));
      TPoint pB2 = theMeshComp.getVertex(aFaceB.at(2));
      TPoint cB = (pB0+pB1+pB2)/3.0;
      
      if (useFaceCenterDistance){
        double distance = (cB-cA).norm();
        if (distance < distanceMin){
          distanceMin = distance;
          if (saveNearestPoint) {vectNearestPt[i] = cB;}
        }
        continue;
      }
      
      TPoint normal = (pB1-pB0).crossProduct(pB2 - pB0);
      double norm = normal.norm();
      if (norm == 0.0){
        continue;
      }
      double d = -(normal[0]*cB[0]+normal[1]*cB[1]+normal[2]*cB[2]);
      double lambda = -(normal[0]*cA[0] + normal[1]*cA[1] + normal[2]*cA[2] + d)/
      (normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);
      
      TPoint proj = (lambda*normal)+cA;
      double distance = std::abs(normal[0]*cA[0] + normal[1]*cA[1] + normal[2]*cA[2] + d);
     	distance /=  norm;
      if(!isInsideFace(pB0, pB1, pB2, proj)){
        // if the projection is outside the face, we approximate the distance with the projection on the face edges
        TPoint p = cA;
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
  outDistances.open("distances.dat", std::ofstream::out);
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
    TPoint center = theNewMeshDistance.getVertex(theNewMeshDistance.getFace(i)[0]);
    theNewMeshDistance.setFaceColor(i, gradientShade(std::min(squaredDistance ? vectFaceDistances[i] :
                                                              1.0 * vectFaceDistances[i], maxScaleDistance)));
    outDistances << center[0] << " " << center[1] << " " << center[2] << " " << vectFaceDistances[i];
    if (saveNearestPoint)
    {
      outDistances << " " << vectNearestPt[0][i] << " " << vectNearestPt[i][1] << " " << vectNearestPt[i][2];
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
  outMesh.open(outputMeshName.c_str(), std::ofstream::out);
  MeshWriter<Z3i::RealPoint>::export2OFF(outMesh, theNewMeshDistance,true);
  outMesh.close();
  if(vm.count("exportDistanceEstimationType")){
    std::ofstream outApproxDistance;
    outApproxDistance.open("distanceEstimationType.off", std::ofstream::out);
    MeshWriter<Z3i::RealPoint>::export2OFF(outApproxDistance, projOkMesh,true);
    outApproxDistance.close();
  }
  
}
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


int
main(int argc,char **argv)

{

  typedef typename Z3i::RealPoint TPoint;
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
    ("help,h", "display this message")
    ("input,i", po::value<std::string>(), "input file name of mesh vertex given as OFF format.")
    ("inputComp,c", po::value<std::string>(), "input file name of mesh vertex given as OFF format.")   
    ("output,o", po::value<std::string>(),  "arg = file.off : export the resulting mesh associated to the fiber extraction.")
    ("maxScaleError", po::value<double>()->default_value(0.1), "set the default max velue use to display the squared error");

  
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
      trace.info()<< "Compare error between two meshes (hausdorff) " <<std::endl << "Options: "<<std::endl
		  << general_opt << "\n";
      return 0;
    }
  

  std::string inputMeshName = vm["input"].as<std::string>();
  std::string inputCompMeshName = vm["inputComp"].as<std::string>();
  std::string outputMeshName = vm["output"].as<std::string>();  
 
  
  DGtal::Mesh<Z3i::RealPoint> theMeshRef(true);
  DGtal::Mesh<Z3i::RealPoint> theMeshComp(true);
  DGtal::Mesh<Z3i::RealPoint> theNewMeshError (true);
  MeshReader<Z3i::RealPoint>::importOFFFile(inputMeshName, theMeshRef, false);
  MeshReader<Z3i::RealPoint>::importOFFFile(inputMeshName, theNewMeshError, false);
  MeshReader<Z3i::RealPoint>::importOFFFile(inputCompMeshName, theMeshComp, false);  
  
  double maxScaleError = vm["maxScaleError"].as<double>();
  
  trace.info()<< "reading the input Comp mesh ok: "<< theMeshComp.nbVertex() <<  std::endl;


 
  double maxOfMin = 0;
  std::vector<double> vectFaceErros;
  // Brut force error measure between ref (input) mesh and comp
  int cptFace=0;
  for (unsigned int i = 0; i<theMeshRef.nbFaces(); i++){
    std::vector<unsigned int>  aFace = theMeshRef.getFace(i);
    cptFace++;
    trace.progressBar(cptFace, theMeshRef.nbFaces());
    double distanceMin = std::numeric_limits<double>::max();
    Z3i::RealPoint p0 = theMeshRef.getVertex(aFace.at(0));
    Z3i::RealPoint p1 = theMeshRef.getVertex(aFace.at(2));
    Z3i::RealPoint p2 = theMeshRef.getVertex(aFace.at(1));
    Z3i::RealPoint center = (p0+p1+p2)/3.0;
    TPoint normal = ((p1-p0).crossProduct(p2 - p0)).getNormalized();      
    for (unsigned int j=0; j < theMeshComp.nbFaces(); j++){
      std::vector<unsigned int>  aFaceC = theMeshComp.getFace(j);
      
      Z3i::RealPoint ps0 = theMeshComp.getVertex(aFaceC[0]);
      Z3i::RealPoint ps1 = theMeshComp.getVertex(aFaceC[2]);
      Z3i::RealPoint ps2 = theMeshComp.getVertex(aFaceC[1]);
      
      double val = normal[0]*ps0[0] + normal[1]*ps0[1] + normal[2]*ps0[2];
      double valP = normal[0]*center[0] + normal[1]*center[1] + normal[2]*center[2];
      
      double distance =  (val-valP)*(val-valP);
      if (distance < distanceMin   ){
        distanceMin = distance;
      }
    }
    
    if(distanceMin>maxOfMin){
      maxOfMin = distanceMin;
    }
    vectFaceErros.push_back(distanceMin);
  }
  
  
 std::ofstream outError;
 outError.open("error.dat", std::ofstream::out);
  
 DGtal::GradientColorMap<double, CMAP_JET>  gradientShade(0, maxScaleError );
 for (unsigned int i=0; i< theNewMeshError.nbFaces(); i++){
   theNewMeshError.setFaceColor(i, gradientShade(vectFaceErros[i]));
   outError << vectFaceErros[i] << std::endl;
  }

  
  trace.info()<< "Hausdorff error: " << maxOfMin << std::endl;
  
  std::ofstream outMesh;
  outMesh.open(outputMeshName, std::ofstream::out);
  MeshWriter<Z3i::RealPoint>::export2OFF(outMesh, theNewMeshError,true);
  

}

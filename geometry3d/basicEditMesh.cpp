#include <iostream>
#include <fstream>

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/writers/MeshWriter.h"

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
    ("output,o", po::value<std::string>(),  "arg = file.off : export the resulting mesh associated to the fiber extraction.")
    ("shrinkArea,s", po::value<std::vector<double> >() ->multitoken(),  "arg = <dist> <bounding box> apply a mesh shrinking on the defined area.")
    ("shrinkBallArea,b", po::value<std::vector<double> >() ->multitoken(),  "arg = <dist> <x> <y> <z> <radius> apply a mesh shrinking on the  area defined by a ball centered at x y z.")
    ("filterVisiblePart", po::value< double >() , "arg = angle nx ny nz: filter the mesh visible part (according the mesh part in the direction  nx, ny, nz and a maximal angle)." )
    ("nx,x", po::value< double >() , "arg = angle nx ny nz: filter the mesh visible part (according the mesh part in the direction  nx, ny, nz and a maximal angle)." )
    ("ny,y", po::value< double >() , "arg = angle nx ny nz: filter the mesh visible part (according the mesh part in the direction  nx, ny, nz and a maximal angle)." 
     )    ("nz,z", po::value< double >() , "arg = angle nx ny nz: filter the mesh visible part (according the mesh part in the direction  nx,k ny, nz and a maximal angle)." )
    ("scale", po::value< double >() , "change the scale factor" )

    ("filterFirstFaces", po::value<unsigned int >(), "arg= X : filters the X% of the first faces of the input mesh." )
    ("filterNbFaces", po::value<double >() ->multitoken(), "arg = X % limits the number of face by keeping only X percent of faces." );

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
      trace.info()<< "Basic edit mesh " <<std::endl << "Options: "<<std::endl
		  << general_opt << "\n";
      return 0;
    }
  

  std::string inputMeshName = vm["input"].as<std::string>();
  std::string outputMeshName = vm["output"].as<std::string>();
  TPoint aNormal;
  double theMaxAngle;
  unsigned int moduloLimitFace;
  TPoint ptUpper, ptLower;
  double distanceShrink;
  if (vm.count("shrinkArea")){
    std::vector<double> vectDistAndBBox =  vm["shrinkArea"].as<std::vector<double> > ();
    distanceShrink = vectDistAndBBox[0];
    ptLower[0] = vectDistAndBBox[1];
    ptLower[1] = vectDistAndBBox[2];
    ptLower[2] = vectDistAndBBox[3];
    ptUpper[0] = vectDistAndBBox[4];
    ptUpper[1] = vectDistAndBBox[5];
    ptUpper[2] = vectDistAndBBox[6];
  }

  
  TPoint ballCenter;
  double  radius;
  if (vm.count("shrinkBallArea")){
    std::vector<double> paramBallArea =  vm["shrinkBallArea"].as<std::vector<double> > ();
    distanceShrink = paramBallArea[0];
    radius = paramBallArea[4];
    ballCenter[0] = (int) paramBallArea[1];
    ballCenter[1] = (int) paramBallArea[2];
    ballCenter[2] = (int) paramBallArea[3];
  }

  
  if(vm.count("filterVisiblePart")){
    std::vector<double> vectNormalAndAngle;  
    theMaxAngle =vm["filterVisiblePart"].as<double>();
    aNormal[0] = vm["nx"].as<double>();
    aNormal[1] = vm["ny"].as<double>();
    aNormal[2] = vm["nz"].as<double>();
    aNormal /= aNormal.norm();
  }
  if(vm.count("filterNbFaces")){
    double percent = vm["filterNbFaces"].as<double>();
    moduloLimitFace = (int)(100.0/percent);
  } 
  

  DGtal::Mesh<Z3i::RealPoint> theMesh(true);
  MeshReader<Z3i::RealPoint>::importOFFFile(inputMeshName, theMesh, false);
  DGtal::Mesh<Z3i::RealPoint> theNewMesh(true);

  
  trace.info()<< "reading the input mesh ok: "<< theMesh.nbVertex() <<  std::endl;
  
  
  for (DGtal::Mesh<Z3i::RealPoint>::VertexStorage::const_iterator it = theMesh.vertexBegin();
       it != theMesh.vertexEnd(); it++){
    theNewMesh.addVertex(*it);
  }
  unsigned int numMaxFaces = theMesh.nbFaces();
  if (vm.count("filterFirstFaces"))
    {
      double percentFirst = vm["filterFirstFaces"].as<unsigned int>();
      numMaxFaces = (numMaxFaces/100.0)*percentFirst;
    }
  unsigned int num =0;
  for (DGtal::Mesh<Z3i::RealPoint>::FaceStorage::const_iterator it = theMesh.faceBegin();  
       it!= theMesh.faceEnd(); it++){
    num++;
    if(num>numMaxFaces){
      break;
    }
    DGtal::Mesh<Z3i::RealPoint>::MeshFace aFace = *it;
    bool okOrientation = true;
    TPoint p0 = theMesh.getVertex(aFace.at(1));
    TPoint p1 = theMesh.getVertex(aFace.at(0));
    TPoint p2 = theMesh.getVertex(aFace.at(2));
    TPoint vectNormal = ((p1-p0).crossProduct(p2 - p0)).getNormalized();    
    vectNormal /= vectNormal.norm();
    if (vm.count("filterVisiblePart")){
      okOrientation = vectNormal.dot(aNormal) > cos(theMaxAngle);
    }
    if( okOrientation && (!vm.count("filterNbFaces") || num%moduloLimitFace == 0 )){
      theNewMesh.addFace(aFace);
    }
    if( vm.count("shrinkBallArea")){
      TPoint ptCenter = (p0+p1+p2)/3.0;
      if((ptCenter-ballCenter).norm() <= radius){
        for(unsigned int i =0; i<3; i++){
          TPoint &aVertex = theNewMesh.getVertex(aFace.at(i));
          if(aVertex==theMesh.getVertex(aFace.at(i))){
            aVertex-=vectNormal*distanceShrink;
          }
        }        
      } 
    }

    if( vm.count("shrinkArea")){
      Z3i::Domain aDomain(ptLower, ptUpper);
      Z3i::Point ptCenter( (p0+p1+p2)/3.0, functors::Round<>());
      if(aDomain.isInside(ptCenter)){
        for(unsigned int i =0; i<3; i++){
          TPoint &aVertex = theNewMesh.getVertex(aFace.at(i));
          if(aVertex==theMesh.getVertex(aFace.at(i))){
            aVertex-=vectNormal*distanceShrink;
          }
        }
      }
    }
    
  }  

  if(vm.count("scale"))
    {
      double scale = vm["scale"].as<double>();
      for(unsigned int i =0; i<theNewMesh.nbVertex(); i++)
        {
          theNewMesh.getVertex(i) *= scale;
 
        }
    }
  trace.info()<< "nbFaces init: " << theNewMesh.nbFaces() << std::endl;
  trace.info()<< "New nbFaces: " << theMesh.nbFaces() << std::endl;  
  std::ofstream outMesh;
  outMesh.open(outputMeshName.c_str(), std::ofstream::out);
  MeshWriter<Z3i::RealPoint>::export2OFF(outMesh, theNewMesh);
  

}
